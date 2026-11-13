# Sources, dependencies and notices

Original application: `src/`, `tests/`, `scripts/`, Makefile and documentation,
MIT license in `LICENSE`. No source from the three reference browsers was
copied into the executable. Their original applications are not redistributed
as part of this package.

## Design references inspected

- **Ndless Commander**: Dav8616's v0.6 fork of Hoffa's project,
  https://github.com/Dav8616/NdlessCommander . Source archive blob inspected:
  `f236dc7fe38b22e9ee8867c66186b2654ea76ed0`. This is where the Commander
  reference came from. No explicit license was found in the inspected archive.
- **SDL File Browser**: Gameblabla's `filebrowser-src.zip` supplied for this
  project. Its source header and COPYING used different GPL versions; it was
  used as a behavior reference, not copied.
- **nSonic 5.32**: supplied `nSonic_5.32.zip`, including `source/nSonic.c`.
  The native refresh hooks in that source do not include an OS 6.4 address.
  No nSonic hook or binary is installed by UltraCommander NX.

## Code used

- **miniz 3.0.2**, MIT, https://github.com/richgel999/miniz , commit
  `293d4db1b7d0ffee9756d035b9ac6f7431ef8492`. License and copyright notices
  retained in `vendor/miniz/`. Local changes: `miniz_export.h` supplies an empty
  `MINIZ_EXPORT`; `mz_zip_file_write_func` returns immediately for zero bytes
  so optional null comments do not reach `fwrite` (UBSan guard). Built with
  `MINIZ_NO_TIME`; ZIP modification times are not preserved.
- **stb_image v2.31**, Sean Barrett and contributors, MIT/public-domain dual
  license at the end of `vendor/stb_image.h`. Retrieved from
  https://github.com/nothings/stb/blob/master/stb_image.h . The vendored file is
  the exact build input. No source edits; only PNG/JPEG/BMP are enabled.
- **font8x8_basic.h**, Daniel Hepper / IBM VGA origins, public domain per
  retained header, https://github.com/dhepper/font8x8 . Vendored unchanged.
- **Ndless SDK r2023**, https://github.com/ndless-nspire/Ndless , pinned commit
  `c659d1691295dd5db79ab171fff680638ca6a4bf`. Runtime libndls/libsyscalls and Zehn
  loader have their original license notices; MPL 1.1 text is included in
  `licenses/`. SDK include/system directories carry public-domain notices.
  The exact corresponding runtime/tool sources are bundled in
  `vendor/ndless-runtime-source.tar.gz`. No SDK source edits were made.
  SDK build uses `PATH_MAX=512` for stock newlib compilation.
- **nspire-io headers**, pinned submodule
  `d084a245286c073d161e1ed602788946aff79aee`, needed to compile the SDK syscall
  wrapper. Its notice is bundled with the SDK source snapshot. Its console
  implementation is not linked into this app.
- **GNU Arm toolchain / newlib**: executable built with GCC 14.2.1, binutils
  2.44 and newlib 4.5.0 from Debian packages. Newlib and GCC runtime notices are
  copied in `licenses/`; these components retain their respective licenses and
  runtime exceptions. The compiler binaries are not included.

Build verification and SHA-256 manifest identify the exact distributed files.
