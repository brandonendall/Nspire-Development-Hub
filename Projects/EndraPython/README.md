# EndraPython Run012

Standalone editor and embedded MicroPython for the TI-Nspire CX II CAS, targeting OS 6.4.0.74 with an existing Ndless installation. Brandon confirmed Run007 imports, traceback reporting and navigation to the helper’s second line on hardware. Run008 passed the JSON counter test on the CX II CAS. Run009 expands the workbench.

The editor keeps the Run002 save fix. It opens the existing scripts folder before saving. On first use it tries to create the folder, then checks that it can open it. Open your existing Hello_World.py.tns from Files to resume editing.

## Commands, help and catalog

Press **Menu** from Files, Editor, Shell or Output. **Help** is highlighted. Press **Enter** to see controls for the current screen. Left and Right cycle through Files, Editing and running, Selection and clipboard, Python shell, and Script output. Esc returns directly to the same work, including a partially typed shell command.

In the editor or shell choose **Menu > Command catalog**. Common opens with print and input. Use Left/Right to switch to builtins or a compiled module, Up/Down to select a member, or type a name prefix to jump. Del shortens the prefix. Enter inserts the name and an opening parenthesis for callables. Import a module before using its qualified entries. The module and builtins groups read public names directly from the embedded runtime. This follows PythonUltra’s module/member grouping and uses the TI build's own inventory.

The built-in input([prompt]) reads a line in the IDE and returns a string in scripts and the shell. Enter accepts an empty or filled reply, Esc raises KeyboardInterrupt, and the punctuation key opens the existing symbol picker. Each reply holds up to 511 UTF-8 bytes.

## Symbols and folders

The physical **?▸** key opens Marks and symbols directly in the editor, shell, Find field or filename. Select a character with the arrows and press Enter. It includes every mark listed in the TI guide plus programming punctuation and brackets. The same popup is an editor Menu item. Ctrl with the parenthesis keys enters the corresponding square bracket; a tapped Ctrl applies to the next key. The original Shift shortcuts still work.

For TI's own keyboard interface, press **Catalog (book)** to open a native TI text field. Use **?▸** inside it for TI punctuation or **Ctrl+Catalog** for the broader TI symbol palette. Confirm the field to insert its text. Esc cancels. Technical notes: the Ndless SDK exports a TI Request text dialog and EndraPython reads its UTF-16 result, converts it to UTF-8, and inserts the whole result as one undoable edit. This two-stage route requires hardware verification. EndraPython stores actual characters, including mathematical symbols. Executable expressions must follow MicroPython syntax. Uncovered font characters use a replacement glyph while retaining the original text.

In Files, **Menu > Browse Documents** opens Documents. Enter opens a folder or runs the highlighted Python script. Right opens a highlighted script for editing. Esc goes up one folder and returns to the usual EndraPython view from Documents. Save and Run use the opened file's own directory. New script also uses the displayed directory. Tab cycles 96-entry pages in large folders. Browsing reads directory listings only.

## Multi-file projects

Create `main.py.tns` and `helper.py.tns` in the same folder. In the main script use `import helper` or `from helper import f`. Imports also work between helpers. The current folder supplies shell imports. Ctrl+R starts a fresh interpreter and reads saved helpers again. A shell session caches imports until you exit it. When both filenames exist, `helper.py.tns` takes precedence over `helper.py`. Each source file uses the editor’s 8 KiB limit and UTF-8 rules. Run007 handles sibling files. Package directories are outside this milestone.

On a script error, Enter opens the innermost source file and line shown in the traceback. Esc from Output returns to the script you ran.

## Project data files

Open text files with `open(name)`, `open(name, "w")` or `open(name, "a")`. Add `b` for binary reads and writes. Opening with `w` replaces existing contents. The stream supports `with`, `read`, `readline`, iteration, `write`, `seek`, `tell`, `flush` and `close`. The `json` module supports `load`, `dump`, `loads` and `dumps`.

At startup the script’s folder is its virtual `/`. `os.getcwd()`, `os.listdir([path])`, `os.mkdir(path)` and `os.chdir(path)` work inside that folder. Relative data paths start at the virtual working directory. Each run resets it to the project root. Sibling imports continue to resolve from the script folder. The editor’s Files view lists Python sources. Use `os.listdir()` to see data files. The shell uses its selected folder in the same way.

## Workbench

Open the editor’s **Menu** to choose **Go to line**, **Outline**, or **Replace all**. The outline lists function and class declarations. Highlight a declaration and press Enter to jump there. Replace all uses literal, case-sensitive matching. Leave the replacement blank to delete matches. Ctrl+Z undoes the entire operation. Ctrl+S saves your changes.

In the Python shell, type `:help` to list commands. `:ls [path]` lists files, `:pwd` prints the working directory, `:cd path` changes it, and `:cat file` displays up to 4 KiB. Enter the rest of the line as the path, including spaces. `:edit file` opens a script and `:python file` runs it. These two commands leave the shell session. `:clear` clears the display. Terminal paths are relative to the active project root or working directory, like Python file operations.

The command catalog shows signatures for common module calls. Press Tab in a module’s group to insert its import statement at the cursor.

## First use

Open EndraPython from Documents. Choose New script, type a filename and press Enter to create a blank script. Ctrl+R saves and runs it. Esc returns from output to the same caret. Esc in Files exits to TI Documents.

| Control | Effect |
| --- | --- |
| Arrows / directional pad | Select a file or menu item, move the caret, scroll console history |
| Enter / center click | Run a highlighted script, open a folder or confirm. In the editor, insert a newline and copy leading spaces |
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
| Right Arrow on a script in Files | Open the highlighted script for editing |
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

| Area | Run012 scope |
| --- | --- |
| Language | Functions, loops, exceptions, lists, dictionaries, integer arithmetic including large integers, single-precision floats |
| Imports | Registered modules include array, builtins, __main__, cmath, collections, gc, io, json, math, os, random and struct. The shell command `help("modules")` lists the compiled inventory. `help()` gives controls and `help(math)` describes an imported module |
| Scripts and files | Sibling source imports from the selected folder. Project-relative text and binary file I/O, directory operations and JSON |
| Console | print, line-numbered exceptions, single-line REPL, previous-command recall |
| Resource limits | 128 KiB fixed Python heap and 16 KiB stack check. Cooperative Esc polling at VM loop checkpoints. Long operations inside C built-ins may delay interruption. These limits do not provide process isolation |
| Other modules | display, keys, time and selected compatibility helpers. Pygame, pip and desktop extension modules are future work |

## Calculator numerics

`import numpy as np` loads the bundled PythonUltra numerical module. It supports small arrays and matrices, reshaping and transpose, vector dot products and normalization, two-dimensional matrix multiplication and `matrix.I` for inversion. The command catalog lists the available names. Use `np.matmul(a, b)` for multiplication of arrays and `a * b` for PythonUltra matrix objects.

This port uses the existing 128 KiB MicroPython heap and single-precision floats. Matrix capacity depends on the live arrays, element values, and operation. The Stage 5 test script reports the completed square products and free heap after collection.

## Script graphics, input, and timing

Use `import display as d` for the 320 × 240 RGB565 canvas. Draw with `set_pixel`, `fill_rect`, and `draw_string`, then call `show_screen()`. Colors are RGB triples with channels from 0 to 255. `get_pixel` returns the color stored on the canvas, rounded to RGB565. Drawing clips to the display. The canvas starts white per fresh Python session.

The `keys` module exposes held Up, Down, Left, Right, Enter, Menu and Esc states via `keydown`. `time.ticks_ms()` and `time.ticks_diff(a,b)` measure elapsed time with wrap handling. `time.sleep_ms(ms)` and `time.sleep(seconds)` pause while keeping Esc cancellation available. Script completion returns to the regular output view.

Run012 bounds Python sleep using the existing free-running timer. A stalled counter raises RuntimeError. The frame probe exercises one show and one delay before the separate animation test.
Stage 6 includes a subset of the Casio `casioplot` drawing functions, `ion.keydown` with navigation constants, immediate-mode `kandinsky` pixel, rectangle and text functions, and selected `gint` RGB565 functions (`dclear`, `dpixel`, `drect`, `dtext`, `dupdate`). Use the calculator-provided constants rather than numeric key codes. Run011_Graphics_Test.py.tns exercises drawing, held arrows, release, Esc and rerun.

## PythonUltra parity and the IDE roadmap

The reference is Brandon's Casio PythonUltra at commit `865e4b5e94280c12a405b3752ec2bd2231236e77`. Run003 adds four upstream MicroPython modules and the editor tools above. Run007 adds multi-file projects. Run008 adds persistent project data. Run009 adds workbench tools and passed on Brandon’s calculator. Run010 integrates the numerical module. Run011 adds drawing, key holds, timing and compatibility adapters.

The reference manifest includes pyeditor, pyfiles, pyterm, pyperm, zipfile, pythonultra, pygame, py3d and numpy. The TI port needs its own file, display, keyboard and timing interfaces before those packages can work here. Check dependencies and exercise each port on the handheld. Numerical extensions also need a memory and speed check on the target.

## Rebuild

From this folder run `sh prepare.sh`, then `make -j2 check` and `python3 tests/previews.py` (Pillow). Run `make -j2` with nspire-gcc, nspire-ld and genzehn on PATH. Alternatively use the repository workflow in .github/workflows/endrapython-run001.yml. It pins bensuperpc/ndless by digest. The generated MicroPython sources are build inputs and are recreated by prepare.sh. Host and native programs share the interpreter, renderer, editor and file operations. The files in build/previews are rendered from actual test states. Native main and the calculator display/input backend require the physical device.

## Attribution

EndraPython includes GPL-2.0-or-later display/input code derived from EndraHome Run047 (43aa6d5e3074b01736aeace777888e7bff6070a6), in turn derived from gpSP. Native keyboard scanning adapts the same branch. Distribute the complete program under GPL-2.0-or-later with source. The embedded MicroPython code is MIT licensed; the core license is included. The Run003 font atlas uses DejaVu Sans, Sans Bold and Sans Mono. The license travels with the source and build artifact. See licenses/.
