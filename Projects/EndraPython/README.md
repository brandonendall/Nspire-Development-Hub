# EndraPython Run003

Standalone editor and embedded MicroPython for the TI-Nspire CX II CAS, targeting OS 6.4.0.74 with an existing Ndless installation. Brandon confirmed Run002 successful on his calculator. Run003 needs a handheld check.

Run003 keeps the Run002 save fix. It opens the existing scripts folder before saving. On first use it tries to create the folder, then checks that it can open it. Open your existing Hello_World.py.tns from Files to resume editing.

## First use

Open EndraPython from Documents. Files starts read-only. Select **+ New script** and press Enter again with a blank name to create and open the two-line Hello World example. A typed name creates an empty script. Bare names gain the .py.tns suffix. Ctrl+R saves changes and executes. Expected output is Hello from EndraPython followed by 45 on its own line. Esc returns to the same caret. Esc in Files exits to TI Documents.

| Control | Effect |
| --- | --- |
| Arrows / directional pad | Select a file or menu item, move the caret, scroll console history |
| Enter / center click | Open or confirm. In the editor, insert a newline and copy leading spaces |
| Shift + letter | Uppercase |
| Tab | Insert spaces up to the next four-column stop |
| Del | Delete the preceding character |
| Shift + minus / comma / apostrophe | Underscore / colon / double quote |
| Shift + left or right parenthesis | Square bracket |
| Menu | Open editor tools. In the shell, list compiled modules |
| Shift + arrow / Ctrl+A | Extend selection / select all |
| Ctrl+C / X / V | Copy / cut / paste with the application clipboard |
| Ctrl+Z / Ctrl+Y | Undo / redo, up to 12 edits |
| Ctrl+F | Find text. Confirm again with the previous query to find the next match |
| Enter after a script error | Jump to the failing source line when the traceback identifies one |
| Ctrl+S / Ctrl+R | Save / save and run. Ctrl+R also reruns from output |
| Esc | Back. With unsaved changes, Esc keeps editing, Enter discards, Ctrl+S saves and leaves |
| Hold Esc while running | Request KeyboardInterrupt at Python bytecode loop checkpoints |
| Python shell | Enter executes a single line. Up recalls the previous command. Esc ends the session |

The shell retains variables until exit. Each script run starts fresh. The console retains the last 96 lines. The 12-row, 40-column editor has a line-number gutter, a caret position indicator, horizontal scrolling and syntax colors. Find wraps to the beginning and selects the result. The clipboard supports multiline text within EndraPython. The symbol picker covers punctuation missing from the physical keys.

## Source files

The application lists up to 96 eligible files directly under /documents/EndraPython/. Python source is plain text, with .py.tns or .py filenames. The .py.tns form is recommended for transfer. On-device TI Documents visibility needs physical confirmation. Create writes only after Enter. Opening rejects binary, non-ASCII and over-8192-byte input. Saving checks the original bytes and reads back the staged output before renaming it. Edits retain uniform LF or CRLF style. Saving mixed endings normalizes them. Files with the same name cannot be created over existing scripts. On a filesystem that refuses replace-by-rename, the original is temporarily preserved in .saving/original.py.tns. If a save is interrupted and .saving remains, preserve its contents and inspect them before retrying. No automatic startup script.

## Runtime decision and capabilities

Upstream MicroPython v1.24.1, pinned at ecfdd5d6f9be971852003c2049600dc7b3e2a838, provides its own compiler, bytecode interpreter, tracing and garbage collection through its portable C embed port. It cross-links with the pinned ARM926EJ-S Ndless ABI and runs the same source in host tests. We chose it over a second interpreter such as PocketPy because the upstream embed interface already met the Run001 gates and has adjustable fixed-heap allocation. PocketPy has not been benchmarked. The Casio PythonUltra source informed the keyboard/editor design only.

| Area | Run003 scope |
| --- | --- |
| Language | Functions, loops, exceptions, lists, dictionaries, integer arithmetic including large integers, single-precision floats |
| Imports | Nine registered modules: array, builtins, __main__, cmath, collections, gc, math, random and struct. The shell command `help("modules")` lists the compiled inventory. `help()` gives controls and `help(math)` describes an imported module |
| Scripts and files | Editor handles file access. No Python open, os, or user modules exposed |
| Console | print, line-numbered exceptions, single-line REPL, previous-command recall |
| Resource limits | 128 KiB fixed Python heap and 16 KiB stack check. Cooperative Esc polling at VM loop checkpoints. Long operations inside C built-ins may delay interruption. These limits do not provide process isolation |
| Other modules | JSON, time, interactive input, display, keys, pygame, pip and desktop packages unavailable |

## PythonUltra parity and the IDE roadmap

The reference is Brandon's Casio PythonUltra at commit `865e4b5e94280c12a405b3752ec2bd2231236e77`. Run003 adds four upstream MicroPython modules and the editor tools above. Multi-file projects and imports between saved scripts are the next IDE milestone. Search and replace, go to line, and a function outline would make longer scripts easier to navigate.

The reference manifest includes pyeditor, pyfiles, pyterm, pyperm, zipfile, pythonultra, pygame, py3d and numpy. The TI port needs its own file, display, keyboard and timing interfaces before those packages can work here. Check dependencies and exercise each port on the handheld. Numerical extensions also need a memory and speed check on the target.

## Rebuild

From this folder run `sh prepare.sh`, then `make -j2 check` and `python3 tests/previews.py` (Pillow). Run `make -j2` with nspire-gcc, nspire-ld and genzehn on PATH. Alternatively use the repository workflow in .github/workflows/endrapython-run001.yml. It pins bensuperpc/ndless by digest. The generated MicroPython sources are build inputs and are recreated by prepare.sh. Host and native programs share the interpreter, renderer, editor and file operations. The files in build/previews are rendered from actual test states. Native main and the calculator display/input backend require the physical device.

## Attribution

EndraPython includes GPL-2.0-or-later display/input code derived from EndraHome Run047 (43aa6d5e3074b01736aeace777888e7bff6070a6), in turn derived from gpSP. Native keyboard scanning adapts the same branch. Distribute the complete program under GPL-2.0-or-later with source. The embedded MicroPython code is MIT licensed; the core license is included. The Run003 font atlas uses DejaVu Sans, Sans Bold and Sans Mono. The license travels with the source and build artifact. See licenses/.
