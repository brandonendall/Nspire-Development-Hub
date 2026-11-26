# EndraPython Run033

Standalone editor and embedded MicroPython for the TI-Nspire CX II CAS, targeting OS 6.4.0.74 with an existing Ndless installation. Brandon confirmed Run007 imports, traceback reporting and navigation to the helper’s second line on hardware. Run008 passed the JSON counter test on the CX II CAS. Run009 expands the workbench.

The editor keeps the Run002 save fix. It opens the existing scripts folder before saving. On first use it tries to create the folder, then checks that it can open it. Open your existing Hello_World.py.tns from Files to resume editing.

## Commands, help and catalog

Press **Menu** from Files, Editor, Shell or Output. Use the arrows to highlight **Help**, then press **Enter** to see controls for the current screen. Left and Right cycle through Files, Editing and running, Selection and clipboard, Python shell, and Script output. Esc returns directly to the same work, including a partially typed shell command.

In the Python editor or shell, press the physical **Catalog** key or choose **Menu > Command catalog**. Both open the built-in command index. Use the unmodified Catalog key. Start with Common for frequently used builtins, or choose a module to browse public names. Up/Down moves, Enter or Right opens a module, Left or Esc goes back. Type a prefix to jump and use Del to shorten it. The selected command displays its syntax when available. Enter inserts the qualified name and an opening parenthesis for callables. Tab inserts the selected module's import statement. Pygame includes public names in its bundled draw, event, image, and other namespaces.

**Menu** opens the options for the current screen. Use the arrows to select an option, Enter to open it, and Esc to go back. **Settings** controls auto-indent, guides and line numbers for the current session. Tab advances to two-space stops.

The built-in input([prompt]) reads a line in the IDE and returns a string in scripts and the shell. Enter accepts an empty or filled reply, Esc raises KeyboardInterrupt, and the punctuation key opens the existing symbol picker. Each reply holds up to 511 UTF-8 bytes.

## Symbols and folders

The physical **?▸** key opens Marks and symbols directly in the editor, shell, Find field or filename. Select a character with the arrows and press Enter. It includes every mark listed in the TI guide plus programming punctuation and brackets. The same popup is an editor Menu item. Ctrl with the parenthesis keys enters the corresponding square bracket; a tapped Ctrl applies to the next key. The original Shift shortcuts still work.

For TI's own keyboard interface, select **TI symbols** from the menu to open a native TI text field. Use **?▸** inside it for TI punctuation or **Ctrl+Catalog** for the broader TI symbol palette. Confirm the field to insert its text. Esc cancels. Technical notes: the Ndless SDK exports a TI Request text dialog and EndraPython reads its UTF-16 result, converts it to UTF-8, and inserts the whole result as one undoable edit. This two-stage route requires hardware verification. EndraPython stores actual characters, including mathematical symbols. Executable expressions must follow MicroPython syntax. Uncovered font characters use a replacement glyph while retaining the original text.

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

## Files startup

The app starts in the EndraPython folder. Enter runs the selected Python script and Right opens it for editing. Either key opens folders and plain text files. The Files menu includes New Python script, New text file, Python shell and Browse Documents. Text files use `.txt` or `.txt.tns` and use the same editor save checks as scripts.

## Pygame on the calculator

`import pygame as pg` loads a small 2D compatibility layer adapted from PythonUltra. It includes `Surface`, `Rect`, `Color`, clipping, RGB565 color-key blitting, line and rectangle drawing, arrow and Enter key state, sampled key events, `time.Clock`, sprite groups and rectangle collisions. Drawing uses the existing 320 × 240 display. Keep offscreen surfaces small for the 128 KiB Python heap. The screen surface itself uses the native canvas. This stage covers 2D drawing and input. Run015 adds image loading and bitmap fonts. The next graphics milestones are Gint drawing, Gint keyboard events, and Pygame parity with PythonUltra, followed by py3d.

## Pygame assets

Use `pygame.image.load(path)` with an uncompressed 24- or 32-bit BMP or a binary P6 PPM. Image paths are relative to the project. File objects must be opened in binary mode and support seek and tell. For calculator transfer, keep the data file’s final `.tns` suffix. Decoding streams one row at a time and limits each image to 16,384 pixels. The surface uses RGB565. Use `Surface.set_colorkey` for transparency.

`pygame.font.Font(None, size)` selects the nearest built-in bitmap face from three size ranges. Use `size(text)` to measure a single line and `render(text, antialias, color[, background])` to create a surface. Solid backgrounds support antialiasing, and transparent text uses a color key.

## Python indentation and editor

Enter preserves the current indentation. A Python block header such as `def`, `for`, or `if` adds two spaces, including when a comment follows the colon. The rule ignores colons inside strings, dictionaries, slices and comments. Plain text carries forward existing whitespace. The editor shows indentation guides and key hints for run and save.

## First use

Open EndraPython from Documents. Choose Menu > New Python script, type a filename and press Enter to create a blank script. Ctrl+R saves and runs it. Esc returns from output to the same caret. Esc in Files exits to TI Documents.

| Control | Effect |
| --- | --- |
| Arrows / directional pad | Select a file or menu item, move the caret, scroll console history |
| Enter / center click | Run a highlighted script, open a folder or confirm. In the editor, carry indentation forward and add two spaces after a Python block header |
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

| Area | Run033 scope |
| --- | --- |
| Language | Functions, loops, exceptions, lists, dictionaries, integer arithmetic including large integers, single-precision floats |
| Imports | Registered modules include array, builtins, __main__, cmath, collections, gc, io, json, math, os, random and struct. The shell command `help("modules")` lists the compiled inventory. `help()` gives controls and `help(math)` describes an imported module |
| Scripts and files | Sibling source imports from the selected folder. Project-relative text and binary file I/O, directory operations and JSON |
| Console | print, line-numbered exceptions, single-line REPL, previous-command recall |
| Resource limits | 128 KiB fixed Python heap and 16 KiB stack check. Cooperative Esc polling at VM loop checkpoints. Long operations inside C built-ins may delay interruption. These limits do not provide process isolation |
| Other modules | display, keys, time and selected compatibility helpers. a compact Pygame subset supports 2D scripts |

## Calculator numerics

`import numpy as np` loads the bundled PythonUltra numerical module. It supports small arrays and matrices, reshaping and transpose, vector dot products and normalization, two-dimensional matrix multiplication and `matrix.I` for inversion. The command catalog lists the available names. Use `np.matmul(a, b)` for multiplication of arrays and `a * b` for PythonUltra matrix objects.

This port uses the existing 128 KiB MicroPython heap and single-precision floats. Matrix capacity depends on the live arrays, element values, and operation. The Stage 5 test script reports the completed square products and free heap after collection.

## Script graphics, input, and timing

Use `import display as d` for the 320 × 240 RGB565 canvas. Draw with `set_pixel`, `fill_rect`, and `draw_string`, then call `show_screen()`. Colors are RGB triples with channels from 0 to 255. `get_pixel` returns the color stored on the canvas, rounded to RGB565. Drawing clips to the display. The canvas starts white per fresh Python session.

The `keys` module exposes held Up, Down, Left, Right, Enter, Menu and Esc states via `keydown`. `time.ticks_ms()` and `time.ticks_diff(a,b)` measure elapsed time with wrap handling. `time.sleep_ms(ms)` and `time.sleep(seconds)` pause while keeping Esc cancellation available. Script completion returns to the regular output view.

Run012 bounds Python sleep using the existing free-running timer. A stalled counter raises RuntimeError. The frame probe exercises one show and one delay before the separate animation test.
Run033 adds the interactive 3D test, a full-screen depth-tested scene with four overlapping cubes. Arrows rotate, plus and minus zoom, Enter toggles automatic rotation, and Esc returns to the console. The demo also ends after at most 1200 frames. `keys.active()` supplies one snapshot of currently held script keys for simultaneous input.

Run032 reduces allocations in mesh preparation, specializes affine transforms, and reuses projected depth at shared vertices. General 4 × 4 transforms keep homogeneous division through the existing path. Run the unchanged Run031 benchmark on both builds to measure the effect on the calculator.

Run031 adds a hardware benchmark for painter and depth modes using the same one- and four-cube scenes. Each line reports preparation, clear and raster, and screen update totals in milliseconds for 24 frames. The script reports `display.BUILD_RUN` so photos identify the IDE build.

Run030 adds optional per-pixel depth testing for filled triangles. Set `Renderer(depth=True)`, call `clear()` once per frame, then draw all opaque meshes and call `present()`. Use one camera and depth renderer per frame because the canvas and depth buffer are shared. `close()` releases the buffer early. Ending the Python session also releases it. The buffer allocates 150 KiB of native memory on the first clear, outside the 128 KiB MicroPython heap. If allocation raises `MemoryError`, scripts can switch to the default painter renderer. Depth mode uses 16-bit reciprocal depth with more precision near the camera. It supports opaque fill. Painter mode retains wireframe outlines.

Run029 separates frame preparation and rasterization through `Renderer.prepare_mesh()` and `draw_prepared()`. Its benchmark runs in Run028 and later with identical timed workloads and no scripted pause. It reports preparation, clear-and-raster time, display update time, and free heap after collection for one and four cubes.

Run028 clips triangles against the near plane before projection. A face that crosses the plane keeps its visible portion, which is triangulated and depth-sorted with the rest of the scene. Fully hidden faces are discarded. Painter sorting still works best with small, non-intersecting meshes.

Run027 reuses projected shared vertices within each mesh draw and skips normal-length calculations for culled faces. The profile reports render-and-present time separately from the scripted pause. Run the same profile on Run026 and Run027 to measure the change on the handheld.

Run026 adds PythonUltra's software `py3d` renderer using Gint's native triangle rasterizer. It supports 4 × 4 transforms, perspective projection, back-face culling, lighting, depth sorting, and optional wireframe outlines. The default viewport is 320 × 240. Run026 skips faces crossing the near plane and uses painter sorting, so it is best suited to small meshes. The bundled cube test reports elapsed frame time and free Python heap. The existing Pygame `display`, `draw`, `event`, and `key` APIs are also importable as submodules.

Run025 ports PythonUltra's calculator-focused `Vector2` and `Vector3`. Import them from `pygame.math` or access them as `pygame.Vector2` and `pygame.Vector3`. The supported operations include arithmetic, dot and cross products, magnitude, normalization, distance, and interpolation. `Vector2` adds planar angles and rotation. Calculations use the port's floating-point precision.

Run024 expands Pygame input with the mapped CX II keypad, modifier state, key names, optional repeat, and `event.wait`, `peek`, filtered `get` and `clear`, and `custom_type`. Keyboard events include `key`, `mod`, `unicode`, `scancode`, and `repeat`. Poll or pump regularly to capture short taps. A physical Space key sends `K_SPACE` and Enter sends `K_RETURN`; the held state for `K_SPACE` also accepts Enter for older scripts. Esc continues to interrupt scripts, including blocking waits. See `tests/Run024_Pygame_Events.py.tns` for the full-screen handheld check.

Run023 expands Pygame with circle, ellipse, polygon, and connected-line drawing for display and off-screen surfaces. `pygame.transform` supports scale, scale_by, flip, rotate, and rotozoom. Like PythonUltra, smoothscale currently uses nearest-neighbor sampling. Transforms preserve RGB565 color keys. See `tests/Run023_Pygame_Draw.py.tns` for the full-screen handheld check.

Run022 adds Gint key waits and events. `getkey()` waits for a press or arrow repeat. `getkey_opt(options, timeout_ms)` returns an event with `type == KEYEV_NONE` on timeout. `pollevent()` samples immediately and returns the oldest queued event. Poll regularly in interactive code to capture short taps. Events have `type`, `key`, `time` in milliseconds, `mod`, `shift`, `ctrl`, and `alpha` (false on TI). Use Gint constants for navigation, Enter, Menu, digits, modifiers, and supported punctuation. Use lowercase ASCII codes, such as `ord("a")`, for letters. Read `keypressed()` and `keyreleased()` after consuming events, and clear their flags with `cleareventflips()`. Esc remains the script interrupt. Gint options support Shift as a modifier and arrow/all-key repeat. Casio OS menu and power actions are outside the TI key API.

Run021 adds native RGB565 Gint drawing with `dline`, `dhline`, `dvline`, `dtriangle`, `dpoly`, `dcircle` and `dellipse`. The functions draw into the existing script canvas. Call `dupdate()` to present the full 320 × 240 script canvas. The test script controls any title or border shown in graphics mode. Polygons take a flat list or tuple of 3 to 128 coordinate pairs and use even-odd fill. Circle, ellipse and polygon fill or outline accept `C_NONE` for transparency. The rasterizer clips to the visible canvas and uses the Casio calling conventions, with EndraPython's own pixel rasterization. See `tests/Run021_Gint_Drawing.py.tns` for a handheld smoke test.

Run017 removed the bundled `casioplot` adapter. Use `display` for native drawing. The current graphics libraries include `ion.keydown` with navigation constants, immediate-mode `kandinsky` pixel, rectangle and text functions, and selected `gint` RGB565 functions (`dclear`, `dpixel`, `drect`, `dtext`, `dupdate`). Use the calculator-provided constants rather than numeric key codes. Run011_Graphics_Test.py.tns exercises drawing, held arrows, release, Esc and rerun.

## PythonUltra parity and the IDE roadmap

The reference is Brandon's Casio PythonUltra at commit `865e4b5e94280c12a405b3752ec2bd2231236e77`. Run003 adds four upstream MicroPython modules and the editor tools above. Run007 adds multi-file projects. Run008 adds persistent project data. Run009 adds workbench tools and passed on Brandon’s calculator. Run010 integrates the numerical module. Run011 adds drawing, key holds, timing and compatibility adapters.

The reference manifest includes pyeditor, pyfiles, pyterm, pyperm, zipfile, pythonultra, pygame, py3d and numpy. The TI port needs its own file, display, keyboard and timing interfaces before those packages can work here. Check dependencies and exercise each port on the handheld. Numerical extensions also need a memory and speed check on the target.

## Rebuild

From this folder run `sh prepare.sh`, then `make -j2 check` and `python3 tests/previews.py` (Pillow). Run `make -j2` with nspire-gcc, nspire-ld and genzehn on PATH. Alternatively use the repository workflow in .github/workflows/endrapython-run001.yml. It pins bensuperpc/ndless by digest. The generated MicroPython sources are build inputs and are recreated by prepare.sh. Host and native programs share the interpreter, renderer, editor and file operations. The files in build/previews are rendered from actual test states. Native main and the calculator display/input backend require the physical device.

## Attribution

EndraPython includes GPL-2.0-or-later display/input code derived from EndraHome Run047 (43aa6d5e3074b01736aeace777888e7bff6070a6), in turn derived from gpSP. Native keyboard scanning adapts the same branch. Distribute the complete program under GPL-2.0-or-later with source. The embedded MicroPython code is MIT licensed; the core license is included. The Run003 font atlas uses DejaVu Sans, Sans Bold and Sans Mono. The license travels with the source and build artifact. See licenses/.

Run013 uses a fixed decimal scaling table on the single-precision port to address the handheld literal error observed in Run010. The handheld check prints Decimal checks passed before running the numerical regression.
