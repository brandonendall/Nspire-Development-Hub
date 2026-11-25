"""Calculator Pygame subset adapted from PythonUltra (MIT, see licenses)."""
import display as _screen
import keys as _keys
from ._geometry import Color, Rect, _color565, _color_from_565
from . import sprite, time

__version__ = "0.1.1-endrapython"

NOEVENT, QUIT, KEYDOWN, KEYUP = 0, 0x100, 0x300, 0x301
K_UP, K_DOWN, K_LEFT, K_RIGHT = _keys.KEY_UP, _keys.KEY_DOWN, _keys.KEY_LEFT, _keys.KEY_RIGHT
K_RETURN = K_SPACE = _keys.KEY_OK
K_ESCAPE = _keys.KEY_ESC
_KEYS = (K_UP, K_DOWN, K_LEFT, K_RIGHT, K_RETURN, K_ESCAPE)

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


display = _Display()


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


draw = _Draw()


class _Pressed:
    def __getitem__(self, value):
        return value in _KEYS and _keys.keydown(value)


class _Key:
    def get_pressed(self):
        return _Pressed()


key = _Key()


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

    def __init__(self):
        self._held = {}
        self._queue = []

    def pump(self):
        for value in _KEYS:
            down = _keys.keydown(value)
            if down != self._held.get(value, False):
                self._held[value] = down
                self.post(Event(KEYDOWN if down else KEYUP, key=value))
                if value == K_ESCAPE and down:
                    self.post(Event(QUIT))

    def post(self, item):
        if not isinstance(item, Event):
            raise TypeError("expected an Event")
        if len(self._queue) >= 32:
            self._queue.pop(0)
        self._queue.append(item)
        return True

    def get(self, types=None):
        self.pump()
        if types is None:
            result, self._queue = self._queue, []
            return result
        if isinstance(types, int):
            types = (types,)
        result = [item for item in self._queue if item.type in types]
        self._queue = [item for item in self._queue if item.type not in types]
        return result

    def poll(self):
        self.pump()
        return self._queue.pop(0) if self._queue else Event(NOEVENT)

    def clear(self):
        self.pump()
        self._queue = []


event = _Event()
_initialized = False


def init():
    global _initialized
    _initialized = True
    return 3, 0


def quit():
    global _initialized
    _initialized = False
    display.surface = None
    event._held.clear()
    event._queue = []


def get_init():
    return _initialized
