# EndraDiagnostics Run006 — bounded live battery view

Run005 was physically photographed completing a full 4 MiB, read-only OS code signature scan, detecting a coarse `OS BATTERY: 100%` with four unique signature matches and no resolver errors. Run005 exit success is **not independently confirmed**; Run003 graphics, input and exit were user-confirmed working.

Run006 continues with the **same unchanged** Run004 nMedia-derived GPLv3 battery signature scanner, OS mapping checker, and Run002/Run003 tested framebuffer/keypad routines. No new memory-read targets or firmware modifications have been introduced.

## Added
- A visual battery gauge representing the validated *coarse OS status*, not physical battery health or fine-grained state of charge.
- Read-only OS charge-task *mode* labels when resolver supplies authenticated `BATTERY_NATIVE_CHARGE_MODE` (1 = discharge, 2–5 = OS charging, 6 = checking). This is **not independently verified external power**; charge current and voltage are not measured. For other modes, displays unknown.
- Updates validated snapshots about once a second after initial discovery (initial scan still in bounded slices).
- Esc exit, plus an approximately 30-second bounded monitoring interval. The time limit cannot interrupt a hung OS or bad memory read.

## Safety
This remains an experimental Ndless binary on TI-Nspire CX II CAS OS 6.4.0.74. Reading undocumented OS RAM risks freezing the calculator and requiring a hard reset/reinstall of Ndless. Run001 did freeze; do not rerun. Run006 makes no ADC conversions, voltage claims, writes, filesystem changes, battery charging commands, clock changes, or hooks.

If the OS signature is not detected, displays `--` / `NOT AVAILABLE`, no fabricated data.

GPLv3-derived code provenance: GigaZelensky/Nspire-MediaPlayer-Ndless, via unchanged Run004 `vendor/battery_native.c` and associated files. Run006 is a separate diagnostic version and does not alter known-working Run005.
