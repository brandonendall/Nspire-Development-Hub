# EndraGBA Run012 — Loading NightMode

Owner: Brandon / Endall (`brandonendall`). Target: TI-Nspire CX II CAS,
OS 6.4.0.74 / Ndless. Active branch:
`feature/endragba-run012-loading-nightmode`.

## Verified build

Source `551687aa3cdb062365299582250f1163ee620981`; Actions **44874060224**
SUCCESS; artifact **14694973380**. Native TNS **480198 bytes**, Zehn v1.
SHA-256 `ec855a1cd498da024cf07a1705901f2492669d72e7e741cf306a72baf58bf9db`
verified against the bundled checksum; ZIP digest also matches GitHub. All nine
CI preview PNGs exactly match locally checked renders. Draft PR #46; physical
testing remains pending. Full details are in PROJECT_MEMORY.md.

## Change

Run011's menus and ROM library use the approved black/gray theme, but its
separate `nspire_loading.c` renderer retained blue/cyan colors. Run012 changes
its six palette constants: black background, silver-gray text, charcoal track,
light-gray progress fill and readable light-gray error text. The same renderer
covers loading, state status, failed load and stopped-game screens.

`run012_loading_nightmode.py` invokes the complete Run011 patch chain. It checks
that only `nspire_loading.c`, GUI version labels and Makefile metadata differ
from that output. Every non-palette loading byte and every other source file is
unchanged: progress math, timing, key handling, ROM-local saves/states, custom
RTC, GBZ paging, Run010 browser layout, Run011 menu colors, Run009 FastExit,
LCD/timer restoration and UltraGBA_Data/UltraGBA_Tests compatibility remain.
No firmware, OS hooks, Ndless associations, gameplay colors or device writes
are added.

## Build and host validation

Workflow: `.github/workflows/endragba-run012-loading-nightmode.yml`.
Pinned SDK: `bensuperpc/ndless@sha256:fb11585d8f6aa43e69692911b713b41958fcf83aa465729210a2be5906e04093`.

From a clean checkout, copy `vendor/upstream-gpsp` to `upstream-gpsp`, then run:

```sh
python3 Projects/EndraGBA/run012_loading_nightmode.py
python3 Projects/EndraGBA/verify_run012_loading.py
bash toolchain/build.sh upstream-gpsp
```

The host check compiles the actual patched loading C with its existing native
text/rectangle routines and font; only hardware presentation and keypad polling
are stubbed. AddressSanitizer and UndefinedBehaviorSanitizer check nine 320x240
frames: startup; 0%, 25%, 100%; state status; failure; stopped; long filename;
unknown total. Checks cover grayscale colors, text/frame bounds, exact bar
widths, callback cleanup and one presentation per request. These are host
renders, **not calculator screenshots**. Local scratch runs require
`ASAN_OPTIONS=detect_leaks=0` because LeakSanitizer cannot run under ptrace; CI
uses the normal sanitizer defaults. No gameplay or physical hardware tests
are claimed.

The workflow packages `EndraGBA_Run012_LoadingNightMode.tns`, SHA256SUMS.txt,
SOURCE_COMMIT.txt and the host previews/results. Exact successful run, artifact,
size and checksum are recorded in PROJECT_MEMORY.md after retrieval.

## Rollback and hardware check

Keep Run011 alongside Run012 and use the same existing calculator folder/data
layout. Run011 source is `f899c4ff9bddb6d5eac70be49570d18b45f89da2`, branch head
`67721cb257e5ea201dfa99a7eeca29c1b9c7c885` includes delivery documentation.
Actions `44862054240`, artifact `14690386825`,
`EndraGBA_Run011_NightMode.tns`, 480480 bytes,
SHA-256 `33ddfd84b70b917540df199a390bf3a68d28b4c6e9d484d004f78bb9cf7883bd`.
Rollback artifact was retrieved and independently verified during Run012 work.
Retain Run010 and the owner-confirmed Run009 FastExit baseline too.

Latest owner report in the Run012 handoff: Run011 looks good except the blue
loading screen. This supersedes the earlier Run011 memory's blanket physical
test pending label; detailed save/RTC and timed-exit testing remain unrecorded.

Run012 hardware testing is pending. After saving the download, transfer:

```sh
clear
push /storage/emulated/0/Download/EndraGBA_Run012_LoadingNightMode.tns /upload
```

If moved into the owner's eGBA directory, use that exact location instead:

```sh
clear
push /storage/emulated/0/lab/Endra/eGBA/EndraGBA_Run012_LoadingNightMode.tns /upload
```

Discover the current USB node if needed; never assume a fixed ID. Keep `/upload`.
Open Run012, load an already-working GBZ title, check black/gray loading and
progress/MB counter, unchanged library/menu, fast exit and existing save/RTC
behavior. Preserve prior executable and user files; stop on an anomaly. Do not
manufacture corrupt ROMs, resets or filesystem changes for error-screen tests.
