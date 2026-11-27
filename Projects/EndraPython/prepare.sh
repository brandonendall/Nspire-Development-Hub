#!/bin/sh
set -eu
pin=ecfdd5d6f9be971852003c2049600dc7b3e2a838
src=${MICROPYTHON_SRC:-"$PWD/.deps/micropython"}
if [ ! -d "$src/.git" ]; then
    git clone --depth 1 --branch v1.24.1 https://github.com/micropython/micropython.git "$src"
fi
test "$(git -C "$src" rev-parse HEAD)" = "$pin"
make -f embed.mk MICROPYTHON_TOP="$src"
cp "$src/extmod/modrandom.c" "$src/extmod/modjson.c" micropython_embed/extmod/
# Two 18-character columns fit our on-calculator console without wrapping.
sed -i 's/#define NUM_COLUMNS (4)/#define NUM_COLUMNS (2)/' micropython_embed/py/builtinhelp.c
mkdir -p micropython_embed/shared/readline
cp "$src/shared/readline/readline.h" micropython_embed/shared/readline/
# Freeze the source with the pinned MicroPython tool so help('modules') can list it.
PYTHONPATH="$src/tools" python3 - <<'PY'
from pathlib import Path
from makemanifest import generate_frozen_str_content

names = ("numpy", "ion", "kandinsky", "gint", "csv", "statistics")
paths = [f"{name}.py" for name in names]
paths += [f"pygame/{name}.py" for name in ("__init__", "_geometry", "sprite", "time", "image", "font", "transform", "math", "draw", "display", "event", "key")]
paths.append("py3d/__init__.py")
output = generate_frozen_str_content([(f"modules/{path}", path) for path in paths])
output += b'const char mp_frozen_names[] = MP_FROZEN_STR_NAMES "\\0";\n'
Path("micropython_embed/port/frozen_modules.c").write_bytes(output)
PY
# In v1.24.1 the compact string-only build also needs to count string entries.
sed -i 's/^    #if MICROPY_MODULE_FROZEN_STR \&\& MICROPY_MODULE_FROZEN_MPY$/    #if MICROPY_MODULE_FROZEN_STR/' micropython_embed/py/frozenmod.c
# Use fixed decimal scaling on the single-precision calculator port.
patch -s -d micropython_embed -p1 < patches/decimal-float.patch
patch -s -d micropython_embed -p1 < patches/study-builtins.patch
