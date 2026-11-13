# EndraCommander — SDL Restart Run001

**Independent clean restart**, not a modification of EndraCommander NX Run006.

## Reason

A TI-Nspire CX II CAS running OS 6.4.0.74 repeatedly hard-locked in the previous custom LCD/UI implementation, including after ESC dismissed a protected-root warning and while showing a CFG text preview. One photograph showed Run006 in text preview with A015 H006 K260 S0. Root scanning had already been disabled, so the cause is not established. Previous stack, screen and viewer code is not used by this application.

The user chose the **SDL File Browser direction**. Run001 uses the official Ndless SDK's bundled **nSDL** library and is original code written from scratch, informed by the 2015 Gameblabla SDL File Browser. The uploaded old \`filebrowser_nspire.zip\` contains only a binary and README, so this project is **not a literal fork** of its unavailable source. Its separately posted \`filebrowser-src.zip\` could later be examined for licensed code with proper attribution; none was copied into this new Run001.

## First milestone

- Native nSDL screen and event loop, using \`SDL_SetVideoMode\`, \`SDL_PollEvent\`, \`SDL_Flip\`, and nSDL font primitives.
- **Only** \`/documents\` and its subdirectories; no access to OS root or system folders.
- **Read-only** list of up to 64 entries in each directory; directory names first, basic sorting.
- UP/DOWN selection, ENTER/RIGHT open folder, LEFT or ESC go back, ESC at Documents exits, MENU opens help.
- **Intentionally absent**: CFG/text/hex viewer, file opening, executable launch, ZIP functions, USB hooks, file mutations, settings, advanced controls and the old two-pane rendering.
- No old EndraCommander NX files or implementation units linked.
- No persistent OS hooks, CPU changes, log writes, or startup services.

## Testing — the actual calculator is still the authority

The GitHub CI can confirm compilation and path-guard unit tests **only**. It cannot verify nSDL's behavior on the user's OS version. Because prior test builds caused hard resets, back up important files and stop if any complete lockup occurs. On the calculator, first confirm the splash/list draws and ESC exits with USB disconnected. Only after this is stable should folder navigation be tested. Never reproduce crashes deliberately.

If stability is established, next phase: directory open/return, then a separate very small *read-only* text preview, then dual panes, then copy/move; ZIP only later. Each phase is an independent measured change.

## Build

From \`Projects/EndraCommanderSDL\`, run \`bash scripts/build-sdk.sh\` (pinned Ndless r2023 SDK, commit \`c659d1691295dd5db79ab171fff680638ca6a4bf\`). Result: \`dist/EndraSDL_Run001.tns\`.

## License and reference

Original Endra SDL source is provided for project use under MIT; nSDL and SDL retain their own LGPL-2.1-or-later terms. Copyright for Gameblabla's 2015 SDL File Browser remains with its author. This package does not include or modify that browser's source.
