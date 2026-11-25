"""Generate an original 24-bit BMP test fixture without external assets."""
from pathlib import Path
import struct
import sys

side = 47
stride = (side * 3 + 3) & ~3
rows = bytearray()
for y in range(side - 1, -1, -1):
    for x in range(side):
        distance = abs(x - 23) + abs(y - 23)
        if distance > 22:
            red, green, blue = 255, 0, 255
        elif distance > 18:
            red, green, blue = 84, 160, 150
        elif distance > 8:
            red, green, blue = 70, 222, 184
        else:
            red, green, blue = 255, 230, 153
        rows.extend((blue, green, red))
    rows.extend(bytes(stride - side * 3))

header = struct.pack("<2sIHHI", b"BM", 54 + len(rows), 0, 0, 54)
info = struct.pack("<IiiHHIIiiII", 40, side, side, 1, 24, 0, len(rows), 0, 0, 0, 0)
Path(sys.argv[1]).write_bytes(header + info + rows)
