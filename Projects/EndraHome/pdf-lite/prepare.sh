#!/bin/bash
set -eu -o pipefail
# Run in an empty build directory. All source archives are pinned.
fetch() {
  mkdir -p "$3"
  curl -fsSL --retry 3 "https://codeload.github.com/$1/tar.gz/$2" | tar -xz -C "$3" --strip-components=1
}
fetch Legimet/nPDF ba05f334b6500170c6cb74099d4cdfc050a69e36 .
fetch ArtifexSoftware/jbig2dec 7496658740e6dc94ce794e22eaea85c697f1341a mupdf/thirdparty/jbig2dec
fetch ArtifexSoftware/thirdparty-libjpeg 219d59dcfd0e6ce8a3d8c5510e29237f0b5078ed mupdf/thirdparty/jpeg
fetch Legimet/openjpeg 119c52ee28f510b5e302eb679993f1cf40bfbf31 mupdf/thirdparty/openjpeg
patch -p1 < "$(dirname "$0")/reader.patch"
make -C mupdf generate build=release XCFLAGS=-fcommon
