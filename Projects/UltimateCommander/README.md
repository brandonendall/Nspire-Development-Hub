# UltraCommander NX — Run 001

First standalone Ndless file manager build for Brandon's TI-Nspire CX II CAS.
Target device: OS 6.4.0.74 with working Ndless. Built using Ndless r2023.
This executable has compiled successfully and passed host checks; it has not
yet been run on a physical calculator or in an Nspire emulator.

## Install and first test

1. Transfer `UltraCommanderNX_Run001.tns` into a Documents folder using your
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
settings are saved in `/documents/UltraCommanderNX/settings.cfg.tns` when used.

## What this build incorporates

| Reference | Features in this fresh implementation |
|---|---|
| Ndless Commander v0.6 | Two panes, sorting, rename, folders, text/hex previews |
| SDL File Browser | Ndless app launch/return, files and empty-folder deletion |
| nSonic 5.32 | Folder-only selection and saved workspace folder |
| UltraZIP direction | Create/list/extract ZIPs; staged extraction and path checks |

These are feature adaptations in new code, not a literal merger of the three
original codebases. The supplied nSonic refresh-address table ends at OS 5.3;
its native TI browser mount/hide hook is NOT ported or enabled on OS 6.4 in
Run 001. A saved workspace is a shortcut inside UltraCommander NX, not a
system-wide document-root change.

## Controls

| Key | Action |
|---|---|
| Up / Down | Select an item; page in text/hex views |
| Tab | Switch panes |
| Left / Right | Parent folder / enter selected folder |
| Enter | Open folder, preview file, or confirm launching a recognized Ndless app |
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
| Menu / Esc | Full action menu / close view or quit |

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
  implemented yet. App launch depends on the selected app's own compatibility.
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

Output: `dist/UltraCommanderNX_Run001.tns`. The build compiles ARM machine code,
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
