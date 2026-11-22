#!/bin/bash
set -eu -o pipefail
flags="-Os -DNOCJK -fcommon -ffunction-sections -fdata-sections"
make -j4 -C mupdf OS=ti-nspire build=release OUT=build/arm AR=arm-none-eabi-ar XCFLAGS="$flags" \
  build/arm/libmupdf.a build/arm/libjbig2dec.a build/arm/libjpeg.a build/arm/libopenjpeg.a
nspire-g++ -std=gnu++11 -marm $flags -Imupdf/include -Wl,--gc-sections *.cpp -o reader.elf \
  -Lmupdf/build/arm -lmupdf -lfreetype -ljbig2dec -ljpeg -lopenjpeg -lz -lm
genzehn --input reader.elf --output EndraPDF_Lite_Run001.tns --compress --name "EndraPDF Lite" --author "Legimet / EndraHome"
test "$(head -c4 EndraPDF_Lite_Run001.tns)" = Zehn
test "$(wc -c < EndraPDF_Lite_Run001.tns)" -lt 1935892
sha256sum EndraPDF_Lite_Run001.tns > SHA256SUMS.txt
