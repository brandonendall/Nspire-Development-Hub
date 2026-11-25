"""Stream uncompressed BMP and P6 PPM rows into a calculator Surface."""
from struct import unpack

_WHITESPACE = b" \t\r\n\v\f"
_MAX_PIXELS = 16384


def _read(handle, length):
    data = handle.read(length)
    if not isinstance(data, bytes) or len(data) != length:
        raise ValueError("truncated binary image")
    return data


def _surface(width, height):
    from . import Surface
    if width <= 0 or height <= 0 or width * height > _MAX_PIXELS:
        raise ValueError("image must contain 1 to 16384 pixels")
    return Surface((width, height))


def _row(surface, y, raw, step, bgr=False, maximum=255):
    width = surface.get_width()
    dest = 2 * y * width
    for x in range(width):
        i = x * step
        r, g, b = raw[i], raw[i + 1], raw[i + 2]
        if bgr:
            r, b = b, r
        if maximum != 255:
            if max(r, g, b) > maximum:
                raise ValueError("PPM sample exceeds maximum")
            r, g, b = r * 255 // maximum, g * 255 // maximum, b * 255 // maximum
        value = (r & 248) << 8 | (g & 252) << 3 | b >> 3
        surface._pixels[dest] = value >> 8
        surface._pixels[dest+1] = value & 255
        dest += 2


def _bmp(handle, start):
    header = _read(handle, 52)
    offset, dib, width, signed_height, planes, depth, compression = unpack(
        "<IIiiHHI", header[8:32])
    if dib < 40 or offset < 14 + dib or planes != 1 or depth not in (24, 32) or compression:
        raise ValueError("BMP must be uncompressed 24-bit or 32-bit")
    height = abs(signed_height)
    surface = _surface(width, height)
    step = depth // 8
    stride = (width * step + 3) & ~3
    handle.seek(start + offset)
    rows = range(height - 1, -1, -1) if signed_height > 0 else range(height)
    for y in rows:
        _row(surface, y, _read(handle, stride), step, True)
    return surface


def _token(handle):
    # Bound header scanning even on a malformed file with no tokens.
    token = bytearray()
    comment = False
    for _ in range(4096):
        ch = _read(handle, 1)
        if comment:
            if ch in b"\r\n":
                comment = False
            continue
        if ch in _WHITESPACE:
            if token:
                return int(bytes(token))
        elif ch == b"#" and not token:
            comment = True
        elif ch in b"0123456789" and len(token) < 8:
            token.extend(ch)
        else:
            break
    raise ValueError("invalid PPM header")


def _load(handle):
    start = handle.tell()
    kind = _read(handle, 2)
    if kind == b"BM":
        return _bmp(handle, start)
    if kind != b"P6" or _read(handle, 1) not in _WHITESPACE:
        raise ValueError("image.load expects BMP or binary P6 PPM")
    width, height, maximum = _token(handle), _token(handle), _token(handle)
    if not 1 <= maximum <= 255:
        raise ValueError("PPM maximum must be 1 to 255")
    surface = _surface(width, height)
    for y in range(height):
        _row(surface, y, _read(handle, width * 3), 3, maximum=maximum)
    return surface


def load(source, namehint=""):
    if hasattr(source, "read"):
        return _load(source)
    with open(source, "rb") as handle:
        return _load(handle)


def get_extended():
    return False
