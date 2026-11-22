#!/bin/bash
set -eu -o pipefail
project=$(cd "$(dirname "$0")" && pwd)
make -j4 -C mupdf OS=Linux build=release OUT=build/host SYS_OPENSSL_CFLAGS= SYS_OPENSSL_LIBS= \
  XCFLAGS="-Os -DNOCJK -fcommon" build/host/libmupdf.a build/host/libjpeg.a build/host/libjbig2dec.a build/host/libopenjpeg.a
cc -Imupdf/include "$project/render_test.c" -o render-test -Lmupdf/build/host \
  -lmupdf -ljbig2dec -ljpeg -lopenjpeg -lfreetype -lz -lm
python3 "$project/test_fixture.py"
./render-test proof.pdf.tns
