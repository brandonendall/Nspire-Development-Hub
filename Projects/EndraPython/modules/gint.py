"""Gint RGB565 drawing and sampled keypad events."""
import display as _display
from keys import KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_EXE, KEY_MENU, KEY_EXIT
from keys import _active, keydown as _navigation_keydown
from time import ticks_ms as _ticks_ms, ticks_diff as _ticks_diff, sleep_ms as _sleep_ms
from display import show_screen as dupdate
from display import _gint_line as dline, _gint_triangle as dtriangle, _gint_poly as dpoly
from display import _gint_ellipse as dellipse, _gint_circle as dcircle

DWIDTH = _display.WIDTH
DHEIGHT = _display.HEIGHT
C_NONE = -1
C_BLACK = 0
C_WHITE = 65535


def _color(value):
    r, g, b = value >> 11 & 31, value >> 5 & 63, value & 31
    return ((r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2))


def dclear(rgb):
    _display.clear_screen(_color(rgb))


def dpixel(x, y, rgb):
    _display.set_pixel(x, y, _color(rgb))


def drect(x1, y1, x2, y2, rgb):
    _display.fill_rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1, _color(rgb))


def dtext(x, y, rgb, text):
    _display.draw_string(x, y, text, _color(rgb))


def dhline(y, rgb):
    dline(0, y, DWIDTH - 1, y, rgb)


def dvline(x, rgb):
    dline(x, 0, x, DHEIGHT - 1, rgb)


# Event codes follow the Casio API. The keypad codes are EndraPython's.

KEYEV_NONE, KEYEV_DOWN, KEYEV_UP, KEYEV_HOLD = 0, 1, 2, 3
KEY_SHIFT, KEY_CTRL = 512, 513
KEY_0, KEY_1, KEY_2, KEY_3, KEY_4 = 48, 49, 50, 51, 52
KEY_5, KEY_6, KEY_7, KEY_8, KEY_9 = 53, 54, 55, 56, 57
KEY_SPACE, KEY_DEL, KEY_DOT, KEY_COMMA = 32, 8, 46, 44
KEY_LEFTP, KEY_RIGHTP, KEY_ADD, KEY_SUB = 40, 41, 43, 45
KEY_MUL, KEY_DIV, KEY_EQUALS = 42, 47, 61
GETKEY_NONE = 0
GETKEY_MOD_SHIFT = 1
GETKEY_REP_ARROWS = 2
GETKEY_REP_ALL = 4
GETKEY_DEFAULT = GETKEY_MOD_SHIFT | GETKEY_REP_ARROWS

_arrows = (KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT)
_held = _active()
_repeat = {}
_queue = []
_pressed = {}
_released = {}


class _KeyEvent:
    def __init__(self, kind, key, stamp, shift=False, ctrl=False):
        self.type = kind
        self.key = key
        self.time = stamp
        self.shift = shift
        self.ctrl = ctrl
        self.alpha = False
        self.mod = shift or ctrl


def keydown(key):
    if key == KEY_EXIT:
        return _navigation_keydown(key)
    return key in _active()


def keydown_all(*keys):
    down = _active()
    return all(_navigation_keydown(k) if k == KEY_EXIT else k in down for k in keys)


def keydown_any(*keys):
    down = _active()
    return any(_navigation_keydown(k) if k == KEY_EXIT else k in down for k in keys)


def _enqueue(kind, key, stamp, down):
    if len(_queue) == 64:
        _queue.pop(0)
    _queue.append(_KeyEvent(kind, key, stamp, KEY_SHIFT in down, KEY_CTRL in down))


def _sample(options):
    global _held
    down = _active()
    stamp = _ticks_ms()
    for key in _held:
        if key not in down:
            _repeat.pop(key, None)
            _enqueue(KEYEV_UP, key, stamp, down)
    for key in down:
        if key not in _held:
            _repeat[key] = (stamp, 400)
            _enqueue(KEYEV_DOWN, key, stamp, down)
        elif key not in (KEY_SHIFT, KEY_CTRL) and (
            options & GETKEY_REP_ALL or options & GETKEY_REP_ARROWS and key in _arrows
        ):
            last, delay = _repeat.get(key, (stamp, 400))
            if _ticks_diff(stamp, last) >= delay:
                _repeat[key] = (stamp, 40)
                _enqueue(KEYEV_HOLD, key, stamp, down)
    _held = down


def _next_event(options):
    _sample(options)
    if not _queue:
        return _KeyEvent(KEYEV_NONE, 0, _ticks_ms())
    event = _queue.pop(0)
    if event.type == KEYEV_DOWN:
        _pressed[event.key] = True
    elif event.type == KEYEV_UP:
        _released[event.key] = True
    return event


def pollevent():
    return _next_event(GETKEY_DEFAULT)


def clearevents():
    _sample(GETKEY_NONE)
    _queue.clear()


def cleareventflips():
    _pressed.clear()
    _released.clear()


def keypressed(key):
    return key in _pressed


def keyreleased(key):
    return key in _released


def getkey_opt(options, timeout_ms):
    if not isinstance(options, int):
        raise TypeError("options must be an integer")
    if timeout_ms is not None:
        if not isinstance(timeout_ms, int):
            raise TypeError("timeout must be an integer or None")
        if not 0 <= timeout_ms < (1 << 29):
            raise ValueError("timeout out of range")
    start = _ticks_ms()
    while True:
        event = _next_event(options)
        if event.type == KEYEV_DOWN or event.type == KEYEV_HOLD:
            if (event.key != KEY_SHIFT or not options & GETKEY_MOD_SHIFT) and (
                event.type != KEYEV_HOLD or options & GETKEY_REP_ALL or
                options & GETKEY_REP_ARROWS and event.key in _arrows
            ):
                return event
        if _queue:
            continue
        if timeout_ms is not None:
            remaining = timeout_ms - _ticks_diff(_ticks_ms(), start)
            if remaining <= 0:
                return _KeyEvent(KEYEV_NONE, 0, _ticks_ms())
        else:
            remaining = 8
        _sleep_ms(min(remaining, 8))


def getkey():
    return getkey_opt(GETKEY_DEFAULT, None)
