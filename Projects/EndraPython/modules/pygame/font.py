"""Rasterize the calculator's built-in bitmap fonts to RGB565 surfaces."""
import display as _screen
from ._geometry import _color565

_active = False


def init():
    global _active
    _active = True


def quit():
    global _active
    _active = False


def get_init():
    return _active


def get_default_font():
    return "endra-default"


def get_fonts():
    return [get_default_font()]


class Font:
    def __init__(self, file, size):
        if file not in (None, get_default_font()):
            raise ValueError("only the built-in font is available")
        size = int(size)
        if size < 1:
            raise ValueError("font size must be positive")
        self._face = 0 if size <= 13 else 1 if size <= 16 else 2

    def size(self, text):
        return _screen._text_size(text, self._face)

    def get_height(self):
        return self.size("")[1]

    get_linesize = get_height

    def render(self, text, antialias, color, background=None):
        from . import Surface
        width, height = self.size(text)
        if width > 640:
            raise ValueError("font line is wider than 640 pixels")
        result = Surface((max(1, width), height))
        foreground = _color565(color)
        transparent = background is None
        if transparent:
            key = 65535 if foreground == 0 else 0
            result.fill(key)
            result.set_colorkey(key)
        else:
            result.fill(background)
        _screen._rasterize_text(result._pixels, max(1, width), height, text,
                                self._face, foreground, antialias, transparent)
        return result


def SysFont(name, size, bold=False, italic=False):
    return Font(None, size)
