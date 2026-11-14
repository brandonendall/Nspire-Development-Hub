# UltraGBA frontend analysis for EndraHome

Reference source: `vendor/upstream-gpsp/`, snapshot from `andymcca/lr-gpsp-nspire` at commit `2ddeb23b565cab84e20278bb73ed76f3d91b25f5`. The currently working UltraGBA Run007 applies a patch on top of that upstream via `Projects/UltraGBA/run007_patch.py`; its native menu/input/display foundation originates from this reference. Treat confirmed hardware success as user observations, not automated proof.

## How the GBA controls work

- **Key scanning:** `nspire_input.c:keys_poll` reads four 32-bit keypad registers (`0x900E0010` + offsets). It also calls `touchpad_scan()` to translate arrow/click events into key bits, and computes `new_keys = ~old & current` on every poll.
- **Actions:** `nspire_keys.c` assigns names and default bindings; `nspire_input.c:get_gui_input` maps Enter/click to `CURSOR_SELECT`, Esc to `CURSOR_BACK`, Menu to `CURSOR_EXIT`, and arrows to movements. It repeats held directional keys after about 400 ms at ~15 Hz.
- **Page navigation:** `gui.c` handles those abstract `CURSOR_*` events in a switch, updates the menu selection, enters submenus, and returns/backtracks. The key reader is **separate** from the page/view logic.
- **Drawing:** `nspire_video_ui.c` draws rectangles and text to a 320x240 logical framebuffer. `flip_screen()` turns menus for the CX II portrait LCD when needed, then calls `nspire_flip()`.
- **Display lifecycle:** `nspire/nspire.c` stores the OS LCD state, initializes three aligned framebuffers, swaps display buffers at refresh, and puts the original LCD registers and timer back in `nspire_restore`. It avoids drawing into the buffer currently being scanned.
- **Boot/exit:** `nspire_host_main.c` calls `nspire_init`, `init_video`, `video_resolution_large`; the quit path calls `nspire_restore`.

## Why Run001/Run002 freezing matters

Run001 (SDL + direct Ndless key polling and child `nl_exec`) froze in its help/settings screen, then again upon trying to launch GBA. Run002 (no SDL, no launching) showed native screen/help and a decreasing countdown; user reports a full freeze after going to Menu. In the Run002 screenshot the countdown shows 05s remaining, suggesting that the UI loop and timer ran **at least part of the time** after entry into the help screen. This does **not** establish that Menu caused the freeze: the run may have reached the automatic exit/cleanup path.

Run002 called `lcd_init(SCR_TYPE_INVALID)` and `refresh_osscr` during exit; Run001 called `SDL_Quit()`. Both differ from the GBA's save/restore of native LCD registers. Both older EndraHome builds also lacked the GBA's `--240x320-support=true` Zehn metadata. These are plausible, **unproven** differences worth isolating.

## Run003 controlled experiment

Run003 tests only (1) GBA-like LCD state management, (2) GBA-like input edges/arrow repeat, and (3) a custom EndraHome visual renderer and page state machine. It **does not** test the app-launch mechanism, ZIP, file browsing or system hooks. A working Run003 would narrow the issue to later functionality; it would not by itself establish that `nl_exec` can safely launch an emulator.

Do not rebuild Run001 as the main launcher without device tests. Use one isolated experimental version and record all results.
