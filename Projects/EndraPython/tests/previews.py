from pathlib import Path
from PIL import Image

out = Path("build/previews")
out.mkdir(exist_ok=True)
frames = ["files", "editor", "console", "error", "repl", "tools", "browser", "symbols", "catalog", "catalog_modules", "focus_menu", "focus_settings", "input", "input_script", "import_error", "storage_result", "outline", "replace", "catalog_signatures", "numerics", "graphics", "graphics_return", "gint_drawing", "gint_keys", "pygame", "pygame_draw", "pygame_events", "pygame_vectors", "py3d_cube", "py3d_profile", "py3d_clipping", "pygame_exit", "file_manager", "file_menu", "pygame_assets", "workspace", "workspace_tools", "workspace_run"] + [f"help{i}" for i in range(5)]
for name in frames:
    with Image.open(f"build/{name}.ppm") as im:
        assert im.size == (320, 240)
        if name in ("repl", "console", "error"):
            assert len(im.crop((180, 220, 320, 240)).getcolors()) == 1, name
        im.save(out / f"{name}.png")
sheet = Image.new("RGB", (656, 504), "#273137")
for i, name in enumerate(["workspace", "focus_menu", "focus_settings", "catalog_signatures"]):
    with Image.open(out / f"{name}.png") as im:
        sheet.paste(im, (4 + (i % 2) * 328, 6 + (i // 2) * 250))
sheet.save(out / "EndraPython_Run028_Previews.png")
