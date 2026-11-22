# EndraPython Run006

Standalone editor and embedded MicroPython for the TI-Nspire CX II CAS, targeting OS 6.4.0.74 with an existing Ndless installation. Brandon confirmed Run004 working and its physical punctuation key on his calculator. Run006 needs a handheld check of the catalog, two-space Tab and interactive input.

The editor keeps the Run002 save fix. It opens the existing scripts folder before saving. On first use it tries to create the folder, then checks that it can open it. Open your existing Hello_World.py.tns from Files to resume editing.

## Commands, help and catalog

Press **Menu** from Files, Editor, Shell or Output. **Help** is highlighted. Press **Enter** to see controls for the current screen. Left and Right cycle through Files, Editing and running, Selection and clipboard, Python shell, and Script output. Esc returns directly to the same work, including a partially typed shell command.

In the editor or shell choose **Menu > Command catalog**. Common opens with print and input. Use Left/Right to switch to builtins or a compiled module, Up/Down to select a member, or type a name prefix to jump. Del shortens the prefix. Enter inserts the name and an opening parenthesis for callables. Import a module before using its qualified entries. The module and builtins groups read public names directly from the embedded runtime. This follows PythonUltra’s module/member grouping and uses the TI build's own inventory.

The built-in input([prompt]) reads a line in the IDE and returns a string in scripts and the shell. Enter accepts an empty or filled reply, Esc raises KeyboardInterrupt, and the punctuation key opens the existing symbol picker. Each reply holds up to 511 UTF-8 bytes.

## Symbols and folders

The physical **?▸** key opens Marks and symbols directly in the editor, shell, Find field or filename. Select a character with the arrows and press Enter. It includes every mark listed in the TI guide plus programming punctuation and brackets. The same popup is an editor Menu item. Ctrl with the parenthesis keys enters the corresponding square bracket; a tapped Ctrl applies to the next key. The original Shift shortcuts still work.

For TI's own keyboard interface, press **Catalog (book)** to open a native TI text field. Use **?▸** inside it for TI punctuation or **Ctrl+Catalog** for the broader TI symbol palette. Confirm the field to insert its text. Esc cancels. Technical notes: the Ndless SDK exports a TI Request text dialog and EndraPython reads its UTF-16 result, converts it to UTF-8, and inserts the whole result as one undoable edit. This two-stage route requires hardware verification. EndraPython stores actual characters, including mathematical symbols. Executable expressions must follow MicroPython syntax. Uncovered font characters use a replacement glyph while retaining the original text.

In Files, **Menu > Browse Documents** opens Documents. Enter opens a folder or a Python script. Esc goes up one folder and returns to the usual EndraPython view from Documents. Save and Run use the opened file's own directory. New script also uses the displayed directory. Tab cycles 96-entry pages in large folders. Browsing reads directory listings only.

## First use

Open EndraPython from Documents. Choose New script, type a filename and press Enter to create a blank script. Ctrl+R saves and runs it. Esc returns from output to the same caret. Esc in Files exits to TI Documents.

| Control | Effect |
| --- | --- |
| Arrows / directional pad | Select a file or menu item, move the caret, scroll console history |
| Enter / center click | Open or confirm. In the editor, insert a newline and copy leading spaces |
| Shift + letter | Uppercase |
| Tab | Insert spaces up to the next two-column stop |
| Del | Delete the preceding character |
| Shift + minus / comma / apostrophe | Underscore / colon / double quote |
| Shift + left or right parenthesis | Square bracket |
| Menu | Open commands and help for the current screen. In the shell choose List modules to print the inventory |
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

Files starts in /documents/EndraPython/ and can browse nested folders under Documents. Python source is plain text, with .py.tns or .py filenames. The .py.tns form is recommended for transfer. On-device TI Documents visibility needs physical confirmation. Create writes only after Enter. Opening rejects binary, invalid UTF-8 and over-8192-byte input. Saving checks the original bytes and reads back the staged output before renaming it. Edits retain uniform LF or CRLF style. Saving mixed endings normalizes them. Files with the same name cannot be created over existing scripts. On a filesystem that refuses replace-by-rename, the original is temporarily preserved in .saving/original.py.tns. If a save is interrupted and .saving remains, preserve its contents and inspect them before retrying. No automatic startup script.

## Runtime decision and capabilities

Upstream MicroPython v1.24.1, pinned at ecfdd5d6f9be971852003c2049600dc7b3e2a838, provides its own compiler, bytecode interpreter, tracing and garbage collection through its portable C embed port. It cross-links with the pinned ARM926EJ-S Ndless ABI and runs the same source in host tests. We chose it over a second interpreter such as PocketPy because the upstream embed interface already met the Run001 gates and has adjustable fixed-heap allocation. PocketPy has not been benchmarked. The Casio PythonUltra source informed the keyboard/editor design only.

| Area | Run006 scope |
| --- | --- |
| Language | Functions, loops, exceptions, lists, dictionaries, integer arithmetic including large integers, single-precision floats |
| Imports | Nine registered modules: array, builtins, __main__, cmath, collections, gc, math, random and struct. The shell command `help("modules")` lists the compiled inventory. `help()` gives controls and `help(math)` describes an imported module |
| Scripts and files | Editor handles file access. No Python open, os, or user modules exposed |
| Console | print, line-numbered exceptions, single-line REPL, previous-command recall |
| Resource limits | 128 KiB fixed Python heap and 16 KiB stack check. Cooperative Esc polling at VM loop checkpoints. Long operations inside C built-ins may delay interruption. These limits do not provide process isolation |
| Other modules | JSON, time, display, keys, pygame, pip and desktop packages unavailable |

## PythonUltra parity and the IDE roadmap

The reference is Brandon's Casio PythonUltra at commit `865e4b5e94280c12a405b3752ec2bd2231236e77`. Run003 adds four upstream MicroPython modules and the editor tools above. Multi-file projects and imports between saved scripts are the next IDE milestone. Search and replace, go to line, and a function outline would make longer scripts easier to navigate.

The reference manifest includes pyeditor, pyfiles, pyterm, pyperm, zipfile, pythonultra, pygame, py3d and numpy. The TI port needs its own file, display, keyboard and timing interfaces before those packages can work here. Check dependencies and exercise each port on the handheld. Numerical extensions also need a memory and speed check on the target.

## Rebuild

From this folder run `sh prepare.sh`, then `make -j2 check` and `python3 tests/previews.py` (Pillow). Run `make -j2` with nspire-gcc, nspire-ld and genzehn on PATH. Alternatively use the repository workflow in .github/workflows/endrapython-run001.yml. It pins bensuperpc/ndless by digest. The generated MicroPython sources are build inputs and are recreated by prepare.sh. Host and native programs share the interpreter, renderer, editor and file operations. The files in build/previews are rendered from actual test states. Native main and the calculator display/input backend require the physical device.

## Attribution

EndraPython includes GPL-2.0-or-later display/input code derived from EndraHome Run047 (43aa6d5e3074b01736aeace777888e7bff6070a6), in turn derived from gpSP. Native keyboard scanning adapts the same branch. Distribute the complete program under GPL-2.0-or-later with source. The embedded MicroPython code is MIT licensed; the core license is included. The Run003 font atlas uses DejaVu Sans, Sans Bold and Sans Mono. The license travels with the source and build artifact. See licenses/.
