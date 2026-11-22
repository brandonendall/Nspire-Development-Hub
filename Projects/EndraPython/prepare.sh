#!/bin/sh
set -eu
pin=ecfdd5d6f9be971852003c2049600dc7b3e2a838
src=${MICROPYTHON_SRC:-"$PWD/.deps/micropython"}
if [ ! -d "$src/.git" ]; then
    git clone --depth 1 --branch v1.24.1 https://github.com/micropython/micropython.git "$src"
fi
test "$(git -C "$src" rev-parse HEAD)" = "$pin"
make -f embed.mk MICROPYTHON_TOP="$src"
