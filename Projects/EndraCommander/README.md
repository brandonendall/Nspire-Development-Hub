# EndraCommander NX — Run 005 (root and preview safety diagnostics)

Endra-branded Ndless file manager build for Brandon's TI-Nspire CX II CAS.
Target device: OS 6.4.0.74 with working Ndless. Built using Ndless r2023.
Run005 builds on Run004's read-only diagnostic mode; it does not change calculator files automatically.
The new build requires its own calculator test.

## Run005 full-calculator freeze investigation

On CX II CAS OS 6.4.0.74, the owner reported hard locks requiring physical resets after (1) navigating from filesystem root toward Documents and (2) using Text Viewer. Because both freeze the entire calculator, do not repeatedly test those pathways on earlier builds.

Targeted safeguards:
- `nx_scan()` now refuses paths outside `/documents` **before** trying `opendir` or child `stat`. At Documents, LEFT produces a safe error rather than browsing OS root.
- Text/hex preview uses a **single bounded 4 KiB read into heap memory**. Subsequent paging stays within that memory; `fseek` and `ftell` were removed from the preview loop. The old on-stack 1024-element offsets table is replaced with a small static array. Large text files show only their initial 4 KiB.
- Existing A/H/K key and idle heartbeat instrumentation remains active; Run004's read-only lock remains on at launch.
- Root access is temporarily disabled for safety. It has not been removed from the long-term plan, and file-management functions within Documents are unchanged in code (but locked by default).
- Automated host tests/compilation cannot reproduce a calculator OS hard lock. The freeze is not yet confirmed fixed.

Safe test procedure: USB disconnected; check heartbeat while idle; browse only inside Documents; test a **small disposable** text file (not a ROM or system file). Stop testing immediately if a complete hard lock occurs. If possible preserve any newly generated exception log and a photo with the A/H/K counters.

## Run004 freeze tracing (retained)

Run003 lasted through more actions but still froze on a physical CX II CAS. The cause is **not established**. Run004 keeps Run003's avoidance of frequent `localtime()` conversion and nested Ndless launching, and adds visible, in-memory freeze breadcrumbs on every screen:

- `A###`: key/action count (modulo 1000).
- `H###`: idle heartbeat, expected to increment about every 1.5 seconds while idle.
- `K###`: last key code (e.g. 256=Up, 257=Down, 258=Left, 259=Right, 260=Enter, 261=Esc, 262=Tab, 263=Del, 264=Menu, 65/97=A).
- The current screen is redrawn once as each key is received, **before** executing that action. A photo of the frozen screen therefore identifies the key last received in most situations.
- **Read-only mode is on by default**: no copy, move, rename, mkdir, deletion, ZIP create/extract, workspace save or theme-config writes. Use `Q` or the last Menu item to toggle this. Enabling writes requires confirmation; test on disposable files only.
- No background tasks, file-based logging, settings migration, runtime patches or CPU changes.

**Test:** with USB disconnected, observe whether `H` increments while no keys are pressed. Test folder scrolling, Enter, Left, Tab, Menu, and Esc with read-only mode on. If it hangs, send a photo including `A/H/K` and say whether `H` still changes and whether ESC exits. If read-only mode is stable, repeat *only on disposable files* after unlocking writes with Q. Do not assume the freeze is fixed without a physical test.

## Install and first test

1. Transfer `EndraCommanderNX_Run005.tns` into a Documents folder using your
   existing calculator transfer app. No additional app data files are required.
2. With Ndless active, open the file from the calculator's document browser.
3. First check that both panes appear, arrows navigate, Tab switches panes,
   Menu opens the action list, and Esc returns to the TI browser.
4. For file-operation tests, transfer the supplied `test_files/Left` sample
   files into a new test folder and create an empty `Right` folder beside it.
   Browse Left in one pane and Right in the other. Use these disposable samples
   before trying operations on important documents.

The executable does not install a resident hook or require the old file
managers to be installed. It does not change the CPU clock. Theme/workspace
settings retain `/documents/UltraCommanderNX/settings.cfg.tns` for compatibility
with existing workspaces and themes.

## What this build incorporates

| Reference | Features in this fresh implementation |
|---|---|
| Ndless Commander v0.6 | Two panes, sorting, rename, folders, text/hex previews |
| SDL File Browser | Ndless app launch/return, files and empty-folder deletion |
| nSonic 5.32 | Folder-only selection and saved workspace folder |
| EndraZIP direction | Create/list/extract ZIPs; staged extraction and path checks |

These are feature adaptations in new code, not a literal merger of the three
original codebases. The supplied nSonic refresh-address table ends at OS 5.3;
its native TI browser mount/hide hook is NOT ported or enabled on OS 6.4 in
Run 001. A saved workspace is a shortcut inside EndraCommander NX, not a
system-wide document-root change.

## Run003 freeze-isolation changes retained

- The owner confirmed Run002 renders on CX II CAS but subsequently freezes; the triggering key/action is not yet known.
- Removed the per-selection `localtime()` conversion from the main render path. Date metadata is still collected for date sorting, but not formatted on each keypress.
- Temporarily disabled `nl_exec` from inside the file manager. Selecting a recognized Ndless executable shows a message rather than nesting another program. This is a safety diagnostic, not a permanent feature removal.
- Changed the onscreen footer and version so Run004 can be identified on hardware.
- **No modifications to original files or settings format**. Run002 remains available for comparison.
- A successful build cannot verify the freeze is resolved; confirm with a device test. First navigate up/down, Enter into directories, Tab and Menu with no USB connected. Then report which exact key or elapsed time makes the UI unresponsive.

## Controls

| Key | Action |
|---|---|
| Up / Down | Select an item; page in text/hex views |
| Tab | Switch panes |
| Left / Right | Parent folder / enter selected folder |
| Enter | Open folder or preview file; app launch disabled in diagnostics |
| Space / A | Mark item / toggle all shown items |
| C / M | Copy / move selected or marked items into the other pane |
| R / N | Rename / new folder |
| Del | Confirm permanent deletion of files or empty folders |
| V / H | Text / hex view; H toggles the two while viewing |
| Z | List ZIP contents |
| P / X | Create ZIP / extract ZIP into the other pane |
| F / S / T | Folders-only / filter current folder / cycle name-size-date sorting |
| W / B | Save selected folder or current folder as workspace / jump to workspace |
| G / D | Go to typed absolute path / Documents |
| Y / I | Dark-light theme / device information |
| Menu / Esc | Full action menu / close view or quit |\n| Q | Toggle diagnostic read-only lock (requires confirmation to unlock) |

Name entry: Shift produces capitals; Shift-minus gives `_`; divide gives `/`;
Del erases. Enter accepts and Esc cancels. Confirmations default to No.
All letters work without Shift for action shortcuts.

## Files and limits

- PNG/JPG/BMP image preview fits the screen; maximum 2,097,152 decoded pixels.
  Text and hex previews stream from disk. Text displays ASCII; Markdown is
  shown as plain text. Non-ASCII filenames are retained but display as `?`.
- `.txt.tns`, `.md.tns`, `.png.tns`, `.zip.tns` and similar suffixed files are
  recognized. Normal TI document files do not open as TI documents from this
  app; unrecognized files get a hex preview. ROM launch associations are not
  implemented yet. Run004 temporarily disables nested app launching while diagnosing freezes. Exit to Documents to launch another application.
- Reads can browse the filesystem. Writes are restricted to entries inside
  `/documents`; the Documents root itself is protected. Existing destinations
  are refused. Folder copy is recursive; folder deletion is empty-only.
- Batch operations commit one item at a time. If an item fails or is cancelled,
  earlier completed items remain completed. Esc cancellation is checked during
  copy/extraction and between files during ZIP creation. ZIP compression of
  one large file can take time before responding.
- ZIP supports unencrypted stored/deflated entries. Limits: 64 MiB per file,
  128 MiB expanded total, 8,192 entries. Path traversal and symlink ZIP entries
  are rejected. Extraction first uses a new `.nxpart` folder, then renames it;
  original archives and existing destinations are retained.
- ZIP creation processes the single highlighted item, including its children
  if it is a folder. Space-marked multi-item ZIP creation is not implemented.
  ZIP timestamps and original filesystem attributes are not preserved.
- Directory listings cap at 4,096 entries; folder copy nesting caps at 32.
  Copy does not preserve timestamps/permissions. Search is a current-folder
  filename filter, not recursive search. No free-space/battery gauge yet.
- Abrupt power loss can leave `.nxpart` files/folders; automatic cleanup only
  covers failures handled while the app is still running. Do not use USB file
  transfers concurrently with file operations in this first device test.

## Build from source

On Debian/Ubuntu, install the packages named at the top of
`scripts/build-sdk.sh`, then run:

```sh
bash scripts/build-sdk.sh
make test
```

The script clones the pinned official Ndless revision into `.toolchain/Ndless`
and builds only the SDK components needed by this C app. It does not install
or flash Ndless onto a calculator. It can also take an existing checkout path
at the exact pinned revision. A working cross-compiler/newlib installation is
required; stock Debian GNU Arm GCC was used for this build.

With a built SDK already available:

```sh
make NDLESS_SDK=/absolute/path/Ndless/ndless-sdk
```

Output: `dist/EndraCommanderNX_Run005.tns`. The build compiles ARM machine code,
processes the ELF using genzehn, and prepends the official Zehn loader.
`make preview` builds a host-only renderer. Example:

```sh
./build/preview "$PWD/test_files/Left" build/browser.ppm
```

Host preview images are generated by the same drawing code. They are not
calculator screenshots and do not verify hardware compatibility. Core tests
use AddressSanitizer and UndefinedBehaviorSanitizer; leak detection is disabled
in the target test command because the build environment does not support it.
See `validation/` for the exact checks and build results shipped with Run 001.
