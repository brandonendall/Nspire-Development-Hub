"""Small RGB565 surface transforms for the calculator."""
from math import ceil, cos, pi, sin


def _destination(surface, size, dest_surface=None):
    from . import Surface
    width, height = size
    if width < 1 or height < 1:
        raise ValueError("transform dimensions must be positive")
    if dest_surface is None:
        dest_surface = Surface(size, surface.get_flags())
    elif not isinstance(dest_surface, Surface) or dest_surface.get_size() != size:
        raise ValueError("destination must be a Surface with the requested size")
    dest_surface._colorkey = surface._colorkey
    return dest_surface


def scale(surface, size, dest_surface=None):
    width, height = map(int, size)
    result = _destination(surface, (width, height), dest_surface)
    if result is surface:
        return result
    sw, sh = surface.get_size()
    for y in range(height):
        sy = y * sh // height
        for x in range(width):
            result._put565(x, y, surface._get565(x * sw // width, sy))
    return result


# PythonUltra also implements smoothscale as nearest-neighbor scaling.
smoothscale = scale


def scale_by(surface, factor, dest_surface=None):
    fx, fy = factor if isinstance(factor, (tuple, list)) else (factor, factor)
    if fx <= 0 or fy <= 0:
        raise ValueError("scale factors must be positive")
    sw, sh = surface.get_size()
    return scale(surface, (max(1, int(sw * fx)), max(1, int(sh * fy))), dest_surface)


def flip(surface, flip_x, flip_y):
    width, height = surface.get_size()
    result = _destination(surface, (width, height))
    for y in range(height):
        sy = height - 1 - y if flip_y else y
        for x in range(width):
            sx = width - 1 - x if flip_x else x
            result._put565(x, y, surface._get565(sx, sy))
    return result


def rotate(surface, angle):
    sw, sh = surface.get_size()
    angle = angle % 360
    if angle in (0, 90, 180, 270):
        sine = (0, 1, 0, -1)[int(angle) // 90]
        cosine = (1, 0, -1, 0)[int(angle) // 90]
    else:
        radians = angle * pi / 180
        sine, cosine = sin(radians), cos(radians)
    width = int(ceil(abs(sw * cosine) + abs(sh * sine)))
    height = int(ceil(abs(sw * sine) + abs(sh * cosine)))
    result = _destination(surface, (width, height))
    pad = surface._colorkey
    result.fill(surface._get565(0, 0) if pad is None else pad)
    scx, scy = (sw - 1) / 2, (sh - 1) / 2
    tcx, tcy = (width - 1) / 2, (height - 1) / 2
    for y in range(height):
        dy = y - tcy
        for x in range(width):
            dx = x - tcx
            sx = dx * cosine - dy * sine + scx + 0.5
            sy = dx * sine + dy * cosine + scy + 0.5
            if 0 <= sx < sw and 0 <= sy < sh:
                result._put565(x, y, surface._get565(int(sx), int(sy)))
    return result


def rotozoom(surface, angle, scale_factor):
    if scale_factor <= 0:
        raise ValueError("scale factor must be positive")
    rotated = rotate(surface, angle)
    return rotated if scale_factor == 1 else scale_by(rotated, scale_factor)
