# EndraCommander — SDL Restart Run002

**Independent clean restart**, not a modification of EndraCommander NX Run006.

## Reason

A TI-Nspire CX II CAS running OS 6.4.0.74 repeatedly hard-locked in the previous custom LCD/UI implementation, including after ESC dismissed a protected-root warning and while showing a CFG text preview. One photograph showed Run006 in text preview with A015 H006 K260 S0. Root scanning had already been disabled, so the cause is not established. Previous stack, screen and viewer code is not used by this application.

The user chose the **SDL File Browser direction**. Run001 uses the official Ndless SDK's bundled **nSDL** library and is original code written from scratch, informed by the 2015 Gameblabla SDL File Browser. The uploaded old \`filebrowser_nspire.zip\` contains only a binary and README, so this project is **not a literal fork** of its unavailable source. Its separately posted \`filebrowser-src.zip\` could later be examined for licensed code with proper attribution; none was copied into this new Run001.

## Run002 addition: bounded read-only text/CFG viewer

Run001 was physically tested on CX II CAS: startup, folder navigation, returning to Documents and ESC exit worked without freezing. Run002 changes only one feature: ENTER on a plain text-like file (`.txt`, `.cfg`, `.ini`, `.log`, `.md`, or these extensions followed by `.tns`) opens a **read-only SDL preview**.

The implementation is new, does not link the previous unstable NX viewer, and reads only the first 2048 bytes using a single bounded `fread` then `fclose`. All page navigation is in memory, with maximum 96 lines and 38 characters per line. NUL/binary content is rejected. ESC leaves the preview; UP/DOWN page; no editing, writes, fseek/ftell, hooks, or OS-root access.

**Calculator test:** First repeat normal folder navigation and ESC exit. If stable, use a *small disposable text file*, not an important system CFG file, to test preview opening and closing. Stop after any full calculator lock; Run002 is not yet hardware verified. Save the working Run001 for rollback.

## First milestone (Run001 baseline)

- Native nSDL screen and event loop, using \`SDL_SetVideoMode\`, \`SDL_PollEvent\`, \`SDL_Flip\`, and nSDL font primitives.
- **Only** \`/documents\` and its subdirectories; no access to OS root or system folders.
- **Read-only** list of up to 64 entries in each directory; directory names first, basic sorting.
- UP/DOWN selection, ENTER/RIGHT open folder, LEFT or ESC go back, ESC at Documents exits, MENU opens help.
- **Intentionally absent at Run001**: all file preview, launching, ZIP, USB hooks, writes, settings, and dual-pane rendering. Run002 adds only bounded read-only text/CFG preview.
- No old EndraCommander NX files or implementation units linked.
- No persistent OS hooks, CPU changes, log writes, or startup services.

## Testing — the actual calculator is still the authority

The GitHub CI can confirm compilation and path-guard unit tests **only**. It cannot verify nSDL's behavior on the user's OS version. Because prior test builds caused hard resets, back up important files and stop if any complete lockup occurs. On the calculator, first confirm the splash/list draws and ESC exits with USB disconnected. Run001 has been verified stable for basic navigation. Run002 still requires separate on-device testing. Never reproduce crashes deliberately.

Once Run002 has proven stable on device, consider dual panes, then copy/move, and ZIP only later. Each phase is an independent measured change.

## Build

From \`Projects/EndraCommanderSDL\`, run \`bash scripts/build-sdk.sh\` (pinned Ndless r2023 SDK, commit \`c659d1691295dd5db79ab171fff680638ca6a4bf\`). Result: \`dist/EndraSDL_Run002.tns\`.

## License and reference

Original Endra SDL source is provided for project use under MIT; nSDL and SDL retain their own LGPL-2.1-or-later terms. Copyright for Gameblabla's 2015 SDL File Browser remains with its author. This package does not include or modify that browser's source.
