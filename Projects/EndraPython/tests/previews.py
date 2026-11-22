from pathlib import Path
from PIL import Image, ImageDraw

out = Path("build/previews")
out.mkdir(exist_ok=True)
frames = ["files", "editor", "console", "error", "repl"]
for name in frames:
    with Image.open(f"build/{name}.ppm") as im:
        assert im.size == (320, 240)
        im.save(out / f"{name}.png")
sheet = Image.new("RGB", (656, 504), "#273137")
for i, name in enumerate(["files", "editor", "error", "repl"]):
    with Image.open(out / f"{name}.png") as im:
        sheet.paste(im, (4 + (i % 2) * 328, 6 + (i // 2) * 250))
sheet.save(out / "EndraPython_Run001_Previews.png")
