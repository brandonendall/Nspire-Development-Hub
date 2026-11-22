#!/bin/bash
set -eu -o pipefail
project=$(cd "$(dirname "$0")" && pwd)
make -j4 -C mupdf OS=Linux build=release OUT=build/release SYS_OPENSSL_CFLAGS= SYS_OPENSSL_LIBS= \
  XCFLAGS="-Os -DNOCJK -fno-strict-aliasing -fcommon" build/release/libmupdf.a build/release/libjpeg.a build/release/libjbig2dec.a build/release/libopenjpeg.a
cc -Imupdf/include "$project/render_test.c" -o render-test -Lmupdf/build/release \
  -lmupdf -ljbig2dec -ljpeg -lopenjpeg -lfreetype -lz -lm
python3 "$project/test_fixture.py"
./render-test proof.pdf.tns
