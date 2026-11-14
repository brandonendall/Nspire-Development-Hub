# EndraDiagnostics Run002 — display and exit probe

**Run001 failed on the physical TI-Nspire CX II CAS:** black/garbled display, no response to exit attempts, hard reset required, Ndless reinstall required. **Do not rerun Run001.**

This is a new, isolated interface-only diagnostic. It uses the exact `platform.c`, `platform.h`, `logic.h` from the successful EndraHome Run003 GBA-style display/keypad baseline on `feature/endrahome-run001`. The graphics platform originates from the gpSP Nspire port (GPL-2.0-or-later); see EndraHome Run003 README for attribution. Run002 adds only a static information screen and exit handling.

* Displays previously observed CPU/cache reference values; does **not** probe CPU or detect OS.
* No malloc, benchmark, console input/output, file writes, hooks or app launches.
* Escape exits. A bounded ~12-second polling loop also exits automatically, provided the platform input/sleep routines continue running.
* Native compilation does not prove stability on the real calculator. **Physical compatibility untested.**
* Test the normal display and ESC/timeout return; stop if any instability occurs. A hard reset requires Ndless reinstall on this calculator.

Build with `make -C Projects/EndraDiagnostics/run002` in pinned Ndless SDK container. Do not overwrite Run001 artifact or existing projects.
