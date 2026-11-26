"""Calculator Pygame subset adapted from PythonUltra (MIT, see licenses)."""
import display as _screen
import keys as _keys
from ._geometry import Color, Rect, _color565, _color_from_565
from . import sprite, time, image, font, transform, math
from .math import Vector2, Vector3

__version__ = "0.1.4-endrapython"

NOEVENT, QUIT, KEYDOWN, KEYUP = 0, 0x100, 0x300, 0x301
K_UP, K_DOWN, K_LEFT, K_RIGHT = _keys.KEY_UP, _keys.KEY_DOWN, _keys.KEY_LEFT, _keys.KEY_RIGHT
K_RETURN, K_ESCAPE, K_MENU = _keys.KEY_OK, _keys.KEY_ESC, _keys.KEY_MENU
K_SPACE, K_BACKSPACE, K_LSHIFT, K_LCTRL = 32, 8, 512, 513
KMOD_NONE, KMOD_SHIFT, KMOD_CTRL = 0, 0x0003, 0x00c0
USEREVENT, NUMEVENTS = 0x8000, 0x10000
K_a = 97
K_b = 98
K_c = 99
K_d = 100
K_e = 101
K_f = 102
K_g = 103
K_h = 104
K_i = 105
K_j = 106
K_k = 107
K_l = 108
K_m = 109
K_n = 110
K_o = 111
K_p = 112
K_q = 113
K_r = 114
K_s = 115
K_t = 116
K_u = 117
K_v = 118
K_w = 119
K_x = 120
K_y = 121
K_z = 122
K_0 = 48
K_1 = 49
K_2 = 50
K_3 = 51
K_4 = 52
K_5 = 53
K_6 = 54
K_7 = 55
K_8 = 56
K_9 = 57

class Surface:
    def __init__(self, size, flags=0, depth=0, _display=False):
        self._w, self._h = map(int, size)
        if min(self._w, self._h) < 1:
            raise ValueError("Surface dimensions must be positive")
        self._flags = flags
        self._pixels = None if _display else bytearray(2 * self._w * self._h)
        self._clip = self.get_rect()
        self._colorkey = None

    def get_size(self):
        return self._w, self._h

    def get_width(self):
        return self._w

    def get_height(self):
        return self._h

    def get_flags(self):
        return self._flags

    def get_rect(self, **kwargs):
        rect = Rect(0, 0, self._w, self._h)
        for name, value in kwargs.items():
            setattr(rect, name, value)
        return rect

    def set_clip(self, rect=None):
        bounds = self.get_rect()
        self._clip = bounds if rect is None else bounds.clip(rect)

    def get_clip(self):
        return self._clip.copy()

    def set_colorkey(self, color):
        self._colorkey = None if color is None else _color565(color)

    def get_colorkey(self):
        return None if self._colorkey is None else _color_from_565(self._colorkey)

    def _put565(self, x, y, color):
        if not self._clip.collidepoint(x, y):
            return
        if self._pixels is None:
            _screen.set_pixel(x, y, _rgb(color))
        else:
            index = 2 * (y * self._w + x)
            self._pixels[index] = color >> 8
            self._pixels[index + 1] = color & 255

    def _get565(self, x, y):
        if not (0 <= x < self._w and 0 <= y < self._h):
            raise IndexError("pixel outside Surface")
        if self._pixels is None:
            return _color565(_screen.get_pixel(x, y))
        index = 2 * (y * self._w + x)
        return self._pixels[index] << 8 | self._pixels[index + 1]

    def set_at(self, pos, color):
        self._put565(int(pos[0]), int(pos[1]), _color565(color))

    def get_at(self, pos):
        return _color_from_565(self._get565(int(pos[0]), int(pos[1])))

    def fill(self, color, rect=None):
        area = self._clip.copy() if rect is None else self._clip.clip(rect)
        if not area:
            return area
        color = _color565(color)
        if self._pixels is None:
            _screen.fill_rect(area.x, area.y, area.w, area.h, _rgb(color))
        else:
            row = bytes((color >> 8, color & 255)) * area.w
            for y in range(area.top, area.bottom):
                offset = 2 * (y * self._w + area.x)
                self._pixels[offset:offset + len(row)] = row
        return area

    def blit(self, source, dest, area=None):
        if not isinstance(source, Surface):
            raise TypeError("expected a Surface")
        dx, dy = int(dest[0]), int(dest[1])
        src = source.get_rect() if area is None else Rect(area)
        crop = source.get_rect().clip(src)
        dx += crop.x - src.x
        dy += crop.y - src.y
        result = Rect(dx, dy, crop.w, crop.h).clip(self._clip)
        if not result:
            return result
        sx, sy = crop.x + result.x - dx, crop.y + result.y - dy
        if self._pixels is None and source._pixels is not None:
            _screen._blit565(source._pixels, source._w, sx, sy, result.x, result.y,
                             result.w, result.h, source._colorkey)
            return result
        # Copy overlapping areas in the direction that preserves source pixels.
        rows = range(result.h)
        cols = range(result.w)
        if source is self:
            if result.y > sy:
                rows = range(result.h - 1, -1, -1)
            if result.x > sx:
                cols = range(result.w - 1, -1, -1)
        for row in rows:
            for col in cols:
                color = source._get565(sx + col, sy + row)
                if color != source._colorkey:
                    self._put565(result.x + col, result.y + row, color)
        return result


def _rgb(color):
    r, g, b = color >> 11, color >> 5 & 63, color & 31
    return (r << 3 | r >> 2, g << 2 | g >> 4, b << 3 | b >> 2)


class _Display:
    def __init__(self):
        self.surface = None

    def set_mode(self, size=(0, 0), flags=0, depth=0):
        native = (_screen.WIDTH, _screen.HEIGHT)
        size = native if tuple(size) == (0, 0) else tuple(size)
        if size != native:
            raise ValueError("display mode is 320 x 240")
        self.surface = Surface(size, flags, depth, _display=True)
        return self.surface

    def get_surface(self):
        return self.surface

    def set_caption(self, title):
        pass

    def flip(self):
        if self.surface is None:
            raise RuntimeError("display mode is unset")
        _screen.show_screen()

    def update(self, rect=None):
        self.flip()


_display_backend = _Display()
display = _display_backend


class _Draw:
    def rect(self, surface, color, rect, width=0):
        rect = Rect(rect)
        if not rect or width < 0:
            return Rect(rect.x, rect.y, 0, 0)
        if width == 0 or 2 * width >= min(rect.w, rect.h):
            return surface.fill(color, rect)
        surface.fill(color, (rect.x, rect.y, rect.w, width))
        surface.fill(color, (rect.x, rect.bottom - width, rect.w, width))
        surface.fill(color, (rect.x, rect.y + width, width, rect.h - 2 * width))
        surface.fill(color, (rect.right - width, rect.y + width, width, rect.h - 2 * width))
        return rect.clip(surface.get_clip())

    def line(self, surface, color, start, end, width=1):
        x0, y0 = map(int, start)
        x1, y1 = map(int, end)
        if not all(-4096 <= value <= 4096 for value in (x0, y0, x1, y1)):
            raise ValueError("line coordinates out of range")
        if width < 1:
            return Rect(x0, y0, 0, 0)
        if width > 320:
            raise ValueError("line width out of range")
        dx, dy = abs(x1 - x0), -abs(y1 - y0)
        result = Rect(min(x0, x1), min(y0, y1), dx + 1, 1 - dy)
        offset = width // 2
        across_y = dx >= -dy
        result = result.move(0, -offset) if across_y else result.move(-offset, 0)
        if across_y:
            result.h += width - 1
        else:
            result.w += width - 1
        sx, sy = (1 if x0 < x1 else -1), (1 if y0 < y1 else -1)
        err = dx + dy
        color = _color565(color)
        while True:
            for i in range(width):
                surface._put565(x0 if across_y else x0 + i - offset,
                                y0 + i - offset if across_y else y0, color)
            if x0 == x1 and y0 == y1:
                break
            twice = 2 * err
            if twice >= dy:
                err += dy
                x0 += sx
            if twice <= dx:
                err += dx
                y0 += sy
        return result.clip(surface.get_clip())


    def lines(self, surface, color, closed, points, width=1):
        points = [tuple(map(int, point)) for point in points]
        if len(points) < 2:
            raise ValueError("lines require at least two points")
        result = Rect(points[0][0], points[0][1], 0, 0)
        for index in range(1, len(points)):
            part = self.line(surface, color, points[index - 1], points[index], width)
            if part:
                result = result.union(part) if result else part
        if closed:
            part = self.line(surface, color, points[-1], points[0], width)
            if part:
                result = result.union(part) if result else part
        return result

    def ellipse(self, surface, color, rect, width=0):
        rect = Rect(rect)
        return self._ellipse(surface, _color565(color), rect, int(width))

    def circle(self, surface, color, center, radius, width=0, draw_top_right=None,
               draw_top_left=None, draw_bottom_left=None, draw_bottom_right=None):
        cx, cy = map(int, center)
        radius = int(radius)
        if radius < 0:
            raise ValueError("radius must be nonnegative")
        rect = Rect(cx - radius, cy - radius, 2 * radius + 1, 2 * radius + 1)
        corners = (draw_top_left, draw_top_right, draw_bottom_left, draw_bottom_right)
        if all(value is None for value in corners):
            corners = None
        return self._ellipse(surface, _color565(color), rect, int(width), corners)

    def _ellipse(self, surface, color, rect, width, corners=None):
        area = rect.clip(surface.get_clip())
        if not area or width < 0:
            return Rect(rect.x, rect.y, 0, 0)
        inner = rect.inflate(-2 * width, -2 * width) if width else None
        if inner is not None and (inner.w <= 0 or inner.h <= 0):
            inner = None
        result = Rect(rect.x, rect.y, 0, 0)
        for y in range(area.top, area.bottom):
            left, right = _ellipse_span(rect, y)
            if inner is not None and inner.top <= y < inner.bottom:
                inner_left, inner_right = _ellipse_span(inner, y)
                spans = ((left, inner_left), (inner_right, right))
            else:
                spans = ((left, right),)
            if corners is None:
                windows = ((area.left, area.right),)
            else:
                cx, cy = rect.center
                upper = y <= cy
                lower = y >= cy
                windows = []
                if (upper and corners[0]) or (lower and corners[2]):
                    windows.append((area.left, min(cx + 1, area.right)))
                if (upper and corners[1]) or (lower and corners[3]):
                    windows.append((max(cx, area.left), area.right))
            for low, high in spans:
                for clip_left, clip_right in windows:
                    start, end = max(low, clip_left), min(high, clip_right)
                    if start < end:
                        part = surface.fill(color, (start, y, end - start, 1))
                        result = result.union(part) if result else part
        return result

    def polygon(self, surface, color, points, width=0):
        points = [tuple(map(int, point)) for point in points]
        if not 3 <= len(points) <= 128:
            raise ValueError("polygon requires 3 to 128 points")
        if not all(-4096 <= coordinate <= 4096 for point in points for coordinate in point):
            raise ValueError("polygon coordinates out of range")
        width = int(width)
        if width < 0:
            return Rect(points[0][0], points[0][1], 0, 0)
        if width:
            return self.lines(surface, color, True, points, width)
        color = _color565(color)
        ys = [point[1] for point in points]
        clip = surface.get_clip()
        result = self.lines(surface, color, True, points)
        for y in range(max(clip.top, min(ys)), min(clip.bottom, max(ys))):
            intersections = []
            x1, y1 = points[-1]
            for x2, y2 in points:
                if y1 <= y < y2 or y2 <= y < y1:
                    intersections.append(x1 + (y - y1) * (x2 - x1) // (y2 - y1))
                x1, y1 = x2, y2
            intersections.sort()
            for i in range(0, len(intersections) - 1, 2):
                left, right = intersections[i], intersections[i + 1]
                part = surface.fill(color, (left, y, right - left + 1, 1))
                if part:
                    result = result.union(part) if result else part
        return result


def _ellipse_span(rect, y):
    w2, h2 = (rect.w - 1) ** 2, (rect.h - 1) ** 2
    if not h2:
        return rect.left, rect.right
    cx, cy = rect.left + rect.right - 1, rect.top + rect.bottom - 1
    limit = w2 * (h2 - (2 * y - cy) ** 2)
    lo, hi = rect.left, cx // 2
    while lo < hi:
        x = (lo + hi) // 2
        if (2 * x - cx) ** 2 * h2 <= limit:
            hi = x
        else:
            lo = x + 1
    return lo, cx - lo + 1

_draw_backend = _Draw()
draw = _draw_backend


def _key_snapshot():
    down = _keys._active()
    return down + (K_ESCAPE,) if _keys.keydown(K_ESCAPE) else down


def _modifiers(down):
    return (KMOD_SHIFT if K_LSHIFT in down else 0) | (KMOD_CTRL if K_LCTRL in down else 0)


class _Pressed:
    def __getitem__(self, value):
        down = _key_snapshot()
        return value in down or (value == K_SPACE and K_RETURN in down)


class _Key:
    def __init__(self):
        self._repeat = (0, 0)

    def get_pressed(self):
        return _Pressed()

    def get_mods(self):
        return _modifiers(_key_snapshot())

    def name(self, keycode):
        names = {
            K_UP: "up", K_DOWN: "down", K_LEFT: "left", K_RIGHT: "right",
            K_RETURN: "return", K_ESCAPE: "escape", K_SPACE: "space",
            K_BACKSPACE: "backspace", K_LSHIFT: "left shift", K_LCTRL: "left ctrl",
            K_MENU: "menu",
        }
        if isinstance(keycode, int) and 33 <= keycode <= 126:
            return chr(keycode)
        return names.get(keycode, "unknown key")

    def set_repeat(self, delay=0, interval=0):
        delay, interval = int(delay), int(interval)
        if not (0 <= delay < (1 << 29) and 0 <= interval < (1 << 29)):
            raise ValueError("repeat timing out of range")
        self._repeat = (delay, interval or delay) if delay else (0, 0)

    def get_repeat(self):
        return self._repeat


_key_backend = _Key()
key = _key_backend


class Event:
    def __init__(self, event_type, attributes=None, **kwargs):
        self.type = int(event_type)
        self.dict = {}
        if attributes:
            self.dict.update(attributes)
        self.dict.update(kwargs)
        for name, value in self.dict.items():
            setattr(self, name, value)

    def __repr__(self):
        return "<Event(%d %r)>" % (self.type, self.dict)


class _Event:
    Event = Event
    EventType = Event

    def __init__(self):
        self._held = {}
        self._queue = []
        self._next_user_type = USEREVENT

    def _key_event(self, kind, code, mods, repeat=False):
        value = chr(code) if 32 <= code <= 126 else "\r" if code == K_RETURN else ""
        if mods & KMOD_SHIFT:
            value = value.upper()
        self.post(Event(kind, key=code, mod=mods, unicode=value,
                        scancode=code, repeat=repeat))

    def pump(self):
        down = _key_snapshot()
        mods = _modifiers(down)
        stamp = time._ticks_ms()
        for code in list(self._held):
            if code not in down:
                del self._held[code]
                self._key_event(KEYUP, code, mods)
        delay, interval = key.get_repeat()
        for code in down:
            if code not in self._held:
                self._held[code] = (stamp, False)
                self._key_event(KEYDOWN, code, mods)
                if code == K_ESCAPE:
                    self.post(Event(QUIT))
            elif delay and code not in (K_ESCAPE, K_LSHIFT, K_LCTRL):
                last, repeated = self._held[code]
                if time._ticks_diff(stamp, last) >= (interval if repeated else delay):
                    self._held[code] = (stamp, True)
                    self._key_event(KEYDOWN, code, mods, True)

    def post(self, item):
        if not isinstance(item, Event):
            raise TypeError("expected an Event")
        if len(self._queue) >= 32:
            self._queue.pop(0)
        self._queue.append(item)
        return True

    def get(self, types=None, pump=True, exclude=None, *, eventtype=None):
        if eventtype is not None:
            if types is not None:
                raise TypeError("specify one event type filter")
            types = eventtype
        if pump:
            self.pump()
        if types is None and exclude is None:
            result, self._queue = self._queue, []
            return result
        if isinstance(types, int):
            types = (types,)
        if isinstance(exclude, int):
            exclude = (exclude,)
        result, remaining = [], []
        for item in self._queue:
            take = (types is None or item.type in types) and (exclude is None or item.type not in exclude)
            (result if take else remaining).append(item)
        self._queue = remaining
        return result

    def poll(self):
        self.pump()
        return self._queue.pop(0) if self._queue else Event(NOEVENT)

    def wait(self, timeout=0):
        timeout = int(timeout)
        if not 0 <= timeout < (1 << 29):
            raise ValueError("event timeout out of range")
        start = time._ticks_ms()
        while True:
            item = self.poll()
            if item.type != NOEVENT:
                return item
            remaining = timeout - time._ticks_diff(time._ticks_ms(), start) if timeout else 8
            if remaining <= 0:
                return item
            time._sleep_ms(min(remaining, 8))

    def peek(self, eventtype=None, pump=True):
        if pump:
            self.pump()
        if eventtype is None:
            return bool(self._queue)
        if isinstance(eventtype, int):
            eventtype = (eventtype,)
        return any(item.type in eventtype for item in self._queue)

    def clear(self, eventtype=None, pump=True):
        self.get(eventtype, pump=pump)

    def custom_type(self):
        if self._next_user_type >= NUMEVENTS:
            raise ValueError("no user event types available")
        value = self._next_user_type
        self._next_user_type += 1
        return value

    def event_name(self, event_type):
        return {NOEVENT: "NoEvent", QUIT: "Quit", KEYDOWN: "KeyDown",
                KEYUP: "KeyUp"}.get(event_type, "UserEvent" if USEREVENT <= event_type < NUMEVENTS else "Unknown")


_event_backend = _Event()
event = _event_backend
_initialized = False


def init():
    global _initialized
    font.init()
    _initialized = True
    return 3, 0


def quit():
    global _initialized
    font.quit()
    _initialized = False
    _display_backend.surface = None
    key.set_repeat()
    _event_backend._held.clear()
    _event_backend._queue = []


def get_init():
    return _initialized
