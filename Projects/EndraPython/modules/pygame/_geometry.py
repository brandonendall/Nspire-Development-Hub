"""Geometry and colors adapted from PythonUltra (MIT)."""

_NAMED_COLORS = {
    "black": (0, 0, 0),
    "white": (255, 255, 255),
    "red": (255, 0, 0),
    "green": (0, 255, 0),
    "blue": (0, 0, 255),
    "yellow": (255, 255, 0),
    "cyan": (0, 255, 255),
    "aqua": (0, 255, 255),
    "magenta": (255, 0, 255),
    "purple": (128, 0, 128),
    "orange": (255, 165, 0),
    "pink": (255, 192, 203),
    "brown": (165, 42, 42),
    "navy": (0, 0, 128),
    "lime": (0, 255, 0),
    "maroon": (128, 0, 0),
    "olive": (128, 128, 0),
    "teal": (0, 128, 128),
    "silver": (192, 192, 192),
    "gray": (128, 128, 128),
    "grey": (128, 128, 128),
    "darkgray": (64, 64, 64),
    "lightgray": (192, 192, 192),
    "transparent": (0, 0, 0, 0),
}


def _clamp8(value):
    value = int(value)
    if value < 0:
        return 0
    if value > 255:
        return 255
    return value


class Color:
    def __init__(self, *value):
        if len(value) == 1:
            value = value[0]
        if isinstance(value, Color):
            self.r, self.g, self.b, self.a = value.r, value.g, value.b, value.a
        elif isinstance(value, str):
            name = value.lower().replace(" ", "")
            if name.startswith("#") and len(name) in (7, 9):
                self.r = int(name[1:3], 16)
                self.g = int(name[3:5], 16)
                self.b = int(name[5:7], 16)
                self.a = int(name[7:9], 16) if len(name) == 9 else 255
            elif name in _NAMED_COLORS:
                parts = _NAMED_COLORS[name]
                self.r, self.g, self.b = parts[0], parts[1], parts[2]
                self.a = parts[3] if len(parts) == 4 else 255
            else:
                raise ValueError("unknown color name: " + value)
        elif isinstance(value, int):
            if 0 <= value <= 0xffff:
                self.r = ((value >> 11) & 31) * 255 // 31
                self.g = ((value >> 5) & 63) * 255 // 63
                self.b = (value & 31) * 255 // 31
                self.a = 255
            else:
                self.r = (value >> 16) & 255
                self.g = (value >> 8) & 255
                self.b = value & 255
                self.a = (value >> 24) & 255 if value > 0xffffff else 255
        else:
            parts = tuple(value)
            if len(parts) not in (3, 4):
                raise ValueError("color needs 3 or 4 components")
            self.r, self.g, self.b = map(_clamp8, parts[:3])
            self.a = _clamp8(parts[3]) if len(parts) == 4 else 255

    def __len__(self):
        return 4

    def __iter__(self):
        return iter((self.r, self.g, self.b, self.a))

    def __getitem__(self, index):
        return (self.r, self.g, self.b, self.a)[index]

    def __repr__(self):
        return "Color(%d, %d, %d, %d)" % (self.r, self.g, self.b, self.a)

    def __eq__(self, other):
        try:
            other = Color(other)
            return tuple(self) == tuple(other)
        except (TypeError, ValueError):
            return False

    def normalize(self):
        return (self.r / 255, self.g / 255, self.b / 255, self.a / 255)

    def correct_gamma(self, gamma):
        return Color(
            int(255 * (self.r / 255) ** gamma),
            int(255 * (self.g / 255) ** gamma),
            int(255 * (self.b / 255) ** gamma),
            self.a,
        )


def _color565(value):
    if isinstance(value, int) and 0 <= value <= 0xffff:
        return value
    color = value if isinstance(value, Color) else Color(value)
    return ((color.r & 0xf8) << 8) | ((color.g & 0xfc) << 3) | (color.b >> 3)


def _color_from_565(value):
    return Color(value & 0xffff)


class Rect:
    def __init__(self, *args):
        if len(args) == 1:
            value = args[0]
            if isinstance(value, Rect):
                values = (value.x, value.y, value.w, value.h)
            else:
                values = tuple(value)
        elif len(args) == 2:
            values = tuple(args[0]) + tuple(args[1])
        else:
            values = args
        if len(values) != 4:
            raise TypeError("Rect needs (x, y, width, height)")
        self.x, self.y, self.w, self.h = map(int, values)

    def __len__(self):
        return 4

    def __iter__(self):
        return iter((self.x, self.y, self.w, self.h))

    def __getitem__(self, index):
        return (self.x, self.y, self.w, self.h)[index]

    def __setitem__(self, index, value):
        values = [self.x, self.y, self.w, self.h]
        values[index] = int(value)
        self.x, self.y, self.w, self.h = values

    def __repr__(self):
        return "<rect(%d, %d, %d, %d)>" % (self.x, self.y, self.w, self.h)

    def __eq__(self, other):
        try:
            return tuple(self) == tuple(Rect(other))
        except (TypeError, ValueError):
            return False

    def __bool__(self):
        return self.w != 0 and self.h != 0

    def copy(self):
        return Rect(self)

    def update(self, *args):
        other = Rect(*args)
        self.x, self.y, self.w, self.h = other.x, other.y, other.w, other.h

    @property
    def width(self):
        return self.w

    @width.setter
    def width(self, value):
        self.w = int(value)

    @property
    def height(self):
        return self.h

    @height.setter
    def height(self, value):
        self.h = int(value)

    @property
    def left(self):
        return self.x

    @left.setter
    def left(self, value):
        self.x = int(value)

    @property
    def right(self):
        return self.x + self.w

    @right.setter
    def right(self, value):
        self.x = int(value) - self.w

    @property
    def top(self):
        return self.y

    @top.setter
    def top(self, value):
        self.y = int(value)

    @property
    def bottom(self):
        return self.y + self.h

    @bottom.setter
    def bottom(self, value):
        self.y = int(value) - self.h

    @property
    def centerx(self):
        return self.x + self.w // 2

    @centerx.setter
    def centerx(self, value):
        self.x = int(value) - self.w // 2

    @property
    def centery(self):
        return self.y + self.h // 2

    @centery.setter
    def centery(self, value):
        self.y = int(value) - self.h // 2

    @property
    def center(self):
        return (self.centerx, self.centery)

    @center.setter
    def center(self, value):
        self.centerx, self.centery = value

    @property
    def size(self):
        return (self.w, self.h)

    @size.setter
    def size(self, value):
        self.w, self.h = map(int, value)

    @property
    def topleft(self):
        return (self.left, self.top)

    @topleft.setter
    def topleft(self, value):
        self.left, self.top = value

    @property
    def topright(self):
        return (self.right, self.top)

    @topright.setter
    def topright(self, value):
        self.right, self.top = value

    @property
    def bottomleft(self):
        return (self.left, self.bottom)

    @bottomleft.setter
    def bottomleft(self, value):
        self.left, self.bottom = value

    @property
    def bottomright(self):
        return (self.right, self.bottom)

    @bottomright.setter
    def bottomright(self, value):
        self.right, self.bottom = value

    @property
    def midtop(self):
        return (self.centerx, self.top)

    @midtop.setter
    def midtop(self, value):
        self.centerx, self.top = value

    @property
    def midbottom(self):
        return (self.centerx, self.bottom)

    @midbottom.setter
    def midbottom(self, value):
        self.centerx, self.bottom = value

    @property
    def midleft(self):
        return (self.left, self.centery)

    @midleft.setter
    def midleft(self, value):
        self.left, self.centery = value

    @property
    def midright(self):
        return (self.right, self.centery)

    @midright.setter
    def midright(self, value):
        self.right, self.centery = value

    def move(self, *offset):
        if len(offset) == 1:
            offset = offset[0]
        return Rect(self.x + int(offset[0]), self.y + int(offset[1]), self.w, self.h)

    def move_ip(self, *offset):
        if len(offset) == 1:
            offset = offset[0]
        self.x += int(offset[0])
        self.y += int(offset[1])

    def inflate(self, x, y):
        result = self.copy()
        result.inflate_ip(x, y)
        return result

    def inflate_ip(self, x, y):
        old_center = self.center
        self.w += int(x)
        self.h += int(y)
        self.center = old_center

    def normalize(self):
        if self.w < 0:
            self.x += self.w
            self.w = -self.w
        if self.h < 0:
            self.y += self.h
            self.h = -self.h

    def clamp(self, other):
        result = self.copy()
        result.clamp_ip(other)
        return result

    def clamp_ip(self, other):
        other = Rect(other)
        if self.w >= other.w:
            self.centerx = other.centerx
        else:
            if self.left < other.left:
                self.left = other.left
            if self.right > other.right:
                self.right = other.right
        if self.h >= other.h:
            self.centery = other.centery
        else:
            if self.top < other.top:
                self.top = other.top
            if self.bottom > other.bottom:
                self.bottom = other.bottom

    def clip(self, other):
        other = Rect(other)
        left = max(self.left, other.left)
        top = max(self.top, other.top)
        right = min(self.right, other.right)
        bottom = min(self.bottom, other.bottom)
        if right <= left or bottom <= top:
            return Rect(left, top, 0, 0)
        return Rect(left, top, right - left, bottom - top)

    def union(self, other):
        other = Rect(other)
        left = min(self.left, other.left)
        top = min(self.top, other.top)
        right = max(self.right, other.right)
        bottom = max(self.bottom, other.bottom)
        return Rect(left, top, right - left, bottom - top)

    def union_ip(self, other):
        result = self.union(other)
        self.update(result)

    def unionall(self, rects):
        result = self.copy()
        for item in rects:
            result = result.union(item)
        return result

    def contains(self, other):
        other = Rect(other)
        return (self.left <= other.left and self.top <= other.top and
                self.right >= other.right and self.bottom >= other.bottom)

    def collidepoint(self, *point):
        if len(point) == 1:
            point = point[0]
        x, y = point
        return self.left <= x < self.right and self.top <= y < self.bottom

    def colliderect(self, other):
        other = Rect(other)
        return (self.left < other.right and self.right > other.left and
                self.top < other.bottom and self.bottom > other.top)

    def collidelist(self, rects):
        for index, other in enumerate(rects):
            if self.colliderect(other):
                return index
        return -1

    def collidelistall(self, rects):
        return [i for i, other in enumerate(rects) if self.colliderect(other)]
