# EndraDiagnostics Run003 — live keypad and exit validation

**Do not run Run001**: physical calculator hard-froze and user had to hard reset and reinstall Ndless.

Run002 was photographed rendering correctly on real TI-Nspire CX II CAS OS 6.4.0.74. **Esc and auto-exit results were not reported**, so exit reliability is still unknown. Do not overstate Run002 success.

Run003 uses the **unchanged** Run002 `platform.c` from the earlier physical-device-tested EndraHome Run003 baseline. It adds one small group of behavior: interactive keypad event display. The test shows the last recognized direction/Enter/Menu input.

Press **Esc** to exit, or it attempts automatic exit after ~12 seconds if the program remains responsive. Because the shared platform manages timer and LCD registers, no exit path is guaranteed safe on every device.

No CPU benchmarks, malloc test, battery access, file writes, Ndless configuration changes, hardware overclock, app launching or direct register reads in run003 main. CPU/battery details remain out of scope until stable physical exit is confirmed.

Source provenance: upstream gpSP Nspire display/input GPL-2.0-or-later, see EndraHome Run003 README.

Native CI success != physical test. First verify display, spontaneous return, Esc exit without hard-reset. Keep a way to reinstall Ndless if necessary.
