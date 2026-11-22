#!/bin/sh
set -eu
pin=ecfdd5d6f9be971852003c2049600dc7b3e2a838
src=${MICROPYTHON_SRC:-"$PWD/.deps/micropython"}
if [ ! -d "$src/.git" ]; then
    git clone --depth 1 --branch v1.24.1 https://github.com/micropython/micropython.git "$src"
fi
test "$(git -C "$src" rev-parse HEAD)" = "$pin"
make -f embed.mk MICROPYTHON_TOP="$src"
cp "$src/extmod/modrandom.c" micropython_embed/extmod/
# Two 18-character columns fit our on-calculator console without wrapping.
sed -i 's/#define NUM_COLUMNS (4)/#define NUM_COLUMNS (2)/' micropython_embed/py/builtinhelp.c
mkdir -p micropython_embed/shared/readline
cp "$src/shared/readline/readline.h" micropython_embed/shared/readline/
