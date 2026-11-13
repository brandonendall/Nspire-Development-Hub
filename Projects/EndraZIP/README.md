# EndraZIP 006 — Recursive Folder ZIP

Ndless ZIP manager for the TI-Nspire CX II CAS. Run006 builds on the tested
EndraZIP 005 browsing and compression controls and adds **whole-folder ZIPs**.

## How to ZIP a complete folder

1. Highlight a folder in the browser (do **not** open it).
2. Press **Menu**; move to **ZIP FILE / FOLDER** and press **Enter**.
3. EndraZIP writes `<folder>.zip.tns` next to that folder.
4. The archive includes the named root folder, all files, nested folders, hidden
   files and empty subfolders. The originals stay untouched.

For example, selecting `/documents/Projects` creates
`/documents/Projects.zip.tns`, containing `Projects/` and everything below it.
You can also select one ordinary file and ZIP it as before.

## Controls

| Key | Action |
|---|---|
| Enter / Right | Open a folder or list the selected ZIP's contents |
| Menu | Open actions (Menu again closes) |
| Left | Parent folder |
| Up / Down | Move selection / scroll |
| Esc | Close listing or menu; exit the program |
| 1 / 2 / 3 | Extract selected ZIP / create ZIP / open selection |

The menu offers **Open / Browse**, **Extract ZIP**, **ZIP FILE / FOLDER**,
and **Cancel**. Enter on an ordinary file does not automatically open a menu.

## Safety and constraints

- Uses standard ZIP/Deflate with miniz level 10 for files. Actual savings vary.
- Archives are named `*.zip.tns` for calculator file compatibility.
- Never overwrites an existing archive or extracted file.
- Archives are written beside the selected item, **not inside** the folder
  being compressed, preventing self-inclusion.
- Extraction creates `<archive>_unzipped` and checks ZIP paths to reject
  traversal, absolute paths, backslashes and encrypted members.
- Deep directory nesting is bounded at 24 levels, with at most 8192 files;
  a failed attempt deletes its incomplete ZIP. If a source folder exceeds the
  calculator's 512-byte path buffer, the operation also fails safely.
- Symbolic links and device/special files are not supported as archive members.
  The TI-Nspire document filesystem normally contains ordinary files/folders.
- Recursive compression runs synchronously. For big folders, allow it time to
  finish rather than repeatedly pressing keys or resetting the calculator.

## Checks

The Linux host test compiles the actual program with ASan/UBSan and checks
file and folder round trips, nested and empty directories, hidden files,
archive CRCs, the menu's folder ZIP command, malicious extraction paths,
non-overwrite behavior, and failed archive cleanup. These host checks cannot
replace testing on a physical TI-Nspire.

## Build

From the repository root:

```sh
(cd Projects/EndraZIP && sh get_miniz.sh)
(cd Projects/EndraZIP && python3 tests/test_host.py)
bash toolchain/build.sh Projects/EndraZIP
```

Output: `Projects/EndraZIP/EndraZIP_006.tns`.
Workflow: `.github/workflows/endrazip.yml` uploads the TNS build and
SHA-256 checksum when compilation and host tests succeed.

The working Run005 implementation and earlier versions are preserved in Git
history. Do not delete your known-working `EndraZIP_005.tns` until Run006
has been checked on the calculator.
