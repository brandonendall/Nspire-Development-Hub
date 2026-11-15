# EndraHome — built-in Files and customizable file-manager roadmap

## Owner request (2026-11-15)
The user loves the *existing native Documents file picker* used when assigning a shortcut in Run014/015/016. They want **the same browser design to become EndraHome's default Files application**, accessible by pressing Enter on the Files icon. Unlike the assignment picker, the full Files view must list **all file types**, not only .tns applications. They should be free to override Files with a different external .tns file manager or editor using the existing Assign/Change App mechanism. They also want to work gradually toward a complete file manager: open/view (eventually text editing), rename, copy, move, delete, and other essentials. Do not sacrifice EndraHome's instant launch, return-home or shutdown performance.

## Core design
- Default slot 3 (label **Files**, index 3) is a **built-in provider**, not a hard-coded .tns path. When slot 3 has **no external assigned path**, Home → Files → Enter opens a native full-files browser based on the already-tested `Eh13Picker` interaction and dark picker renderer. A configured external .tns on slot 3 overrides the built-in provider, and Home → Files → Enter does the existing immediate `nl_exec`. If user selects *Use Built-in Files* in the manager, remove only that external shortcut path and restore the built-in provider; do not delete any files or existing saved settings.
- **Keep the Assign/Change App picker separate and unchanged**: this picker continues to filter folders + .tns, returns a chosen .tns path to the assignment workflow and never performs file operations. Do NOT add delete/rename actions to the picker; that could affect EndraHome's config safety.
- New **Files browser** shares drawing/navigation and directory-traversal helpers but uses a view-mode parameter to include *all file types* (and possibly file size/type), with a distinct state machine and a separate files actions menu. Preserve sorting folders before files, breadcrumb path, Back parent-directory navigation, resource bounds, and root confinement. Consider internal/hidden config protection so casual browsing cannot delete critical EndraHome config or Ndless hooks.
- Set **Documents root** as initial location. The user currently launches EndraHome from the TI Documents filesystem. Do not provide root filesystem access without a separately reviewed safety policy. Ensure entries with unusual filenames are bounded and cannot escape the Documents root.
- For a file, Enter should initially show a file information/details screen (name, extension, size, path), or launch only recognized/tested types. Do not call nl_exec for arbitrary files. The eventual in-app text viewer/editor should be introduced and tested separately.
- Application manager options should allow *Assign External App* and *Use Built-in Files* for Files; normal customizability of other 6 slots stays intact. Labels remain editable using Run016 physical QWERTY rename.
- Future operations: **read-only browsing** first; then safe text viewer / file-type actions, then **copy**, **move**, **rename**, and **delete** only after isolated filesystem tests and recovery design. Filesystem operations must be opt-in (explicit selection), preserve data on failure, and be guarded from acting on OS files or special Ndless hooks. Require confirmation for irreversible delete, protect config and save-game files, and never conflate *Remove Shortcut* with delete-on-disk. Check same-name conflicts and avoid overwrites.
- Keep `EndraHome_Apps.cfg.tns` stable and unnumbered; no migration that overwrites user config. Keep all previous Runs014–016 as rollback. Use the physically-tested Run010 framebuffer + no-`refresh_osscr` shutdown, Run011 return-home and Run016 native QWERTY input.

## Suggested sequence
**Run017:** Default built-in Files *read-only*: all file types, browse Documents, folder navigation, open file detail screen, Esc. Verify immediate return to launcher, no file modifications or freeze. External override through existing Assign path, and restore built-in default through a new manager choice, without editing other shortcuts.

**Run018+:** Native text viewer (including .txt/.cfg/.tns text-like files only when content is safe; don't render binary garbage), details/size, operations menu; staged copy/move/rename/delete with explicit confirmation, conflict detection, and crash-safe behavior. QWERTY editing may later be reused for renames.

## Separate future design request
Integrate a real clock in the **dark EndraHome header** via a verified RTC/nClock-safe source, never the pictured ugly white OS overlay. Keep clock work independent from file browser tasks and avoid hook changes.

## Device and UI acceptance tests
1. No config edit: slot Files defaults to built-in browsing when blank and Home Enter opens the browser, not the Assign picker.
2. Native Files can browse a mixed folder with directories, .tns, .cfg, .txt, zip archives, and unknown formats without mutation.
3. Esc returns to Home promptly; existing Tools shortcut still opens and returns automatically.
4. Assign external file browser .tns to Files and it opens with one Enter; restore built-in Files and former external .tns still exists unmodified.
5. Existing QWERTY Rename, Assign picker, Remove Shortcut (never delete file), settings and config persistence continue working.
6. No crash, no firmware/OS hooks, no silent file deletion or overwrite.
