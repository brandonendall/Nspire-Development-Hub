# UltraZIP 004

Ndless ZIP utility with a graphical file browser for TI-Nspire CX II CAS.
Run004 separates browsing from file actions and increases ZIP compression.

## Controls

| Key | Action |
|---|---|
| Enter or Right | Open the selected folder, or list a ZIP's contents |
| Menu | Open file actions; press Menu again to close |
| Left | Go to the parent folder |
| Up / Down | Move the selection or scroll ZIP contents |
| Esc | Close a menu or ZIP listing; exit from the browser |
| 1 / 2 / 3 | Extract ZIP / create ZIP / open selection |

Inside the actions menu, Enter runs the selected action. On an ordinary file,
Enter displays a hint to use Menu > Create ZIP; it does not open the menu.

## Compression and extraction

Create ZIP uses miniz's strongest ZIP setting, level 10, instead of Run003's
fastest setting, level 1. It creates a standard ZIP named `<file>.zip.tns` next
to the original, then reports the percentage saved including ZIP overhead.
The source file is retained. Stronger compression can take longer on hardware.
Already compressed files, random data and very small files may not shrink.

Creation supports one selected file. Folder packing and encrypted ZIPs remain
unsupported. Extraction creates a new `<archive>_unzipped` folder. Existing
output archives/files are refused, and unsafe archive paths are rejected.

## Validation

The host suite exercises the real application's key loop and compression code:
22 UI states plus exit, 66 Python checks, ZIP CRCs, byte-for-byte round trips
through both Python and the app, source preservation, existing-output refusal,
failed-create cleanup, and archive path rejection. ASan and UBSan are enabled.

Representative host fixtures (complete ZIP sizes, in bytes):

| Fixture | Original | Run003 level 1 | Run004 level 10 |
|---|---:|---:|---:|
| Repeated notes | 466,944 | 1,791 | 1,576 |
| App C source | 12,675 | 5,230 | 4,637 |
| Generated mixed binary | 1,048,576 | 7,680 | 7,105 |
| Random bytes | 262,144 | 262,331 | 262,331 |

These samples are not game ROM benchmarks. Physical calculator performance
and Run004 device compatibility still require testing. First open a disposable
folder with Enter, open actions with Menu, then create and extract a sample ZIP.

## Build and test

From the repository root:

```sh
(cd Projects/UltraZip && python3 tests/test_host.py)
(cd Projects/UltraZip && sh get_miniz.sh)
bash toolchain/build.sh Projects/UltraZip
```

Output: `Projects/UltraZip/UltraZIP_004.tns`.
The workflow `.github/workflows/ultrazip.yml` runs host checks, builds with the
existing pinned Ndless container, and uploads `UltraZIP_004` with a checksum.
If host LeakSanitizer cannot run under a debugger/ptrace container, set
`ASAN_OPTIONS=detect_leaks=0:halt_on_error=1`; memory access and UBSan checks
remain enabled. GitHub Actions uses leak detection by default.

Run003's original source restoration is documented in `IMPORT.md` and retained
in Git history. UltraCommander NX is a separate app under `Projects/UltimateCommander`.

