"""Selected gint RGB565 drawing operations and navigation keys."""
import display as _display
from keys import KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_EXE, KEY_MENU, KEY_EXIT, keydown
from display import show_screen as dupdate

DWIDTH = _display.WIDTH
DHEIGHT = _display.HEIGHT
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
