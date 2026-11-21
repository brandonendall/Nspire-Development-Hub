#!/usr/bin/env python3
"""Pack the approved Run038 handoff artwork for the native renderer.

Run with Pillow and the extracted work package as the only argument.
"""
from pathlib import Path
from struct import pack
from sys import argv

from PIL import Image, ImageChops, ImageDraw

SIZE = 48
HEADER_SIZE = (156, 29)
WALLPAPERS = ("01_Cyber_Wave", "02_Neon_City", "03_Cyber_Lab",
              "04_Reactor", "05_Tech_Corridor")

# Trace the actual emblem silhouettes on the 850-wide reference layout.
# Interior holes are transparent in the open rings and orbital styles.
ICONS = [
    ([(27,128),(99,98),(151,120),(152,206),(59,233),(27,205)], []),
    ([(190,168),(197,139),(215,115),(239,101),(261,96),(287,102),
      (305,112),(321,133),(332,159),(333,180),(320,208),(304,226),
      (283,236),(259,239),(232,233),(213,217),(198,195)],
     [[(218,167),(222,144),(237,130),(254,123),(275,124),(297,135),
       (308,154),(311,174),(303,193),(288,210),(265,216),(241,213),
       (228,200),(220,183)]]),
    ([(360,117),(391,108),(426,94),(459,107),(492,117),(490,152),
      (482,179),(468,203),(448,223),(426,240),(402,223),(383,204),
      (369,180),(362,151)], []),
    ([(524,153),(537,150),(543,133),(536,125),(554,109),(565,116),
      (580,111),(583,96),(605,96),(609,111),(623,116),(633,108),
      (652,124),(645,134),(652,149),(665,154),(665,178),(652,182),
      (645,197),(651,207),(634,226),(623,218),(610,222),(606,238),
      (582,238),(578,222),(563,217),(553,226),(535,210),(543,198),
      (536,182),(524,179)],
     [[(554,165),(558,146),(570,133),(590,126),(611,129),(627,142),
       (635,160),(634,179),(624,197),(607,206),(588,208),(569,199),
       (557,185)]]),
    ([(698,131),(762,94),(826,129),(826,200),(763,241),(698,203)], []),
    ([(16,413),(24,390),(49,361),(80,341),(109,330),(139,328),
      (152,334),(161,346),(158,371),(144,398),(126,420),(101,443),
      (74,456),(46,464),(27,457),(17,445)],
     [[(33,422),(40,399),(58,379),(85,359),(115,346),(136,346),
       (146,355),(144,373),(132,394),(110,418),(83,437),(57,445),
       (42,442)]]),
    ([(190,368),(259,322),(330,350),(328,370),(316,378),(330,383),
      (330,399),(316,402),(330,409),(329,425),(268,468),(190,438),
      (190,417),(201,410),(190,403),(190,385),(202,378)], []),
    ([(426,320),(454,344),(496,390),(426,469),(357,391),(397,346)], []),
    ([(525,358),(544,358),(544,346),(556,346),(556,329),(629,329),
      (629,346),(644,346),(644,358),(662,358),(662,434),(643,434),
      (643,445),(630,445),(630,463),(555,463),(555,445),(543,445),
      (543,434),(524,434)], []),
    ([(694,380),(700,359),(720,343),(739,333),(763,331),(755,318),
      (787,338),(790,330),(807,347),(814,356),(830,367),(831,378),
      (822,387),(814,390),(817,382),(805,377),(809,392),(799,398),
      (800,386),(784,377),(763,373),(744,382),(733,398),(732,417),
      (738,434),(751,446),(771,451),(790,446),(811,430),(834,408),
      (833,427),(823,451),(802,467),(779,473),(754,471),(732,464),
      (715,451),(705,435),(696,416),(691,397)], []),
]

E_SHAPES = {
    1: [(244,140),(300,140),(286,153),(260,153),(256,164),(293,165),
        (281,176),(253,176),(250,183),(292,183),(281,196),(229,196)],
    3: [(576,144),(627,144),(613,155),(590,155),(587,164),(621,165),
        (612,175),(582,175),(578,182),(620,182),(609,192),(564,192)],
    5: [(65,362),(136,361),(116,376),(81,378),(78,387),(124,387),
        (108,401),(73,401),(69,413),(118,411),(101,427),(47,427)],
    9: [(754,383),(801,383),(790,396),(766,396),(763,404),(794,404),
        (784,415),(759,415),(755,423),(796,423),(784,434),(739,434)],
}


def indexed(rgb):
    reduced = rgb.quantize(colors=256, method=Image.Quantize.MEDIANCUT,
                           dither=Image.Dither.NONE)
    palette = reduced.getpalette()
    colors = [(r >> 3) << 11 | (g >> 2) << 5 | (b >> 3)
              for r, g, b in zip(palette[::3], palette[1::3], palette[2::3])]
    return pack("<256H", *colors) + reduced.tobytes()


def trace(sheet, contour, holes, emblem=None, fade_dark=False):
    def scaled(points):
        return [(round(x * sheet.width / 850), round(y * sheet.height / 567))
                for x, y in points]

    shape = scaled(contour)
    mask = Image.new("L", sheet.size)
    draw = ImageDraw.Draw(mask)
    draw.polygon(shape, fill=255)
    for hole in holes:
        draw.polygon(scaled(hole), fill=0)
    if emblem:
        draw.polygon(scaled(emblem), fill=255)

    # Fade only the outer dark details of the pin grid and dragon.
    # Their shapes have tiny gaps that a 48-pixel silhouette cannot trace cleanly.
    if fade_dark:
        brightness = ImageChops.lighter(sheet.getchannel("R"), sheet.getchannel("B"))
        mask = ImageChops.multiply(mask, brightness.point(
            lambda x: max(0, min(255, (x - 32) * 8))))

    x1, y1, x2, y2 = mask.getbbox()
    cx, cy = (x1 + x2) // 2, (y1 + y2) // 2
    radius = (max(x2 - x1, y2 - y1) + 1) // 2 + 6
    crop = (cx - radius, cy - radius, cx + radius, cy + radius)
    sprite = sheet.crop(crop)
    sprite.putalpha(mask.crop(crop))
    return sprite.resize((SIZE, SIZE), Image.Resampling.LANCZOS)


def masked(image):
    rgb = image.convert("RGB")
    alpha = image.getchannel("A")
    return indexed(rgb) + alpha.tobytes()


def main(package, output):
    wall = bytearray()
    for name in WALLPAPERS:
        with Image.open(package / "Native_320x240" / f"{name}.png") as original:
            assert original.size == (320, 240)
            wall.extend(indexed(original.convert("RGB")))

    with Image.open(package / "Reference" / "Run031_original.png") as original:
        header = original.convert("RGBA").crop((4, 1, 206, 40))
    alpha = Image.new("L", header.size, 255)
    edge = ImageDraw.Draw(alpha)
    for x in range(10):
        edge.line((x, 0, x, header.height), fill=x * 25)
        right = header.width - 1 - x
        edge.line((right, 0, right, header.height), fill=x * 25)
    for y in range(4):
        edge.line((0, y, header.width, y), fill=y * 63)
        bottom = header.height - 1 - y
        edge.line((0, bottom, header.width, bottom), fill=y * 63)
    header.putalpha(alpha)
    header = header.resize(HEADER_SIZE, Image.Resampling.LANCZOS)

    with Image.open(package / "Reference" / "Endra_Universal_Icon_Concepts.png") as original:
        sheet = original.convert("RGB")
    sprites = b"".join(masked(trace(sheet, contour, holes, E_SHAPES.get(i), fade_dark=i in (8, 9)))
                       for i, (contour, holes) in enumerate(ICONS))
    packed_header = masked(header)
    output.write_bytes(wall + sprites + packed_header)
    print(f"{len(wall)} wallpaper bytes, {len(sprites)} icon bytes, "
          f"{len(packed_header)} header bytes")


if __name__ == "__main__":
    main(Path(argv[1]), Path(__file__).with_name("run038_art.bin"))
