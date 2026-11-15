# EndraDiagnostics Run013 — display, keypad and battery analytics

Run012 is **user-confirmed working**, including 8 screens, 4KiB heap PASS, OS signature scan, battery history and **fast exit**. The owner emphasizes reducing TI-Nspire CX II CAS USB connector wear: bundle several benign features into every hardware test. Run013 retains Run012's original platform code byte-for-byte, including fast manual exit and no automatic timeout.

## Eleven screens, one transfer
Pages 1–8: exactly the same battery/CPU/cache/MMU/heap/resolver/history/safety features as Run012, with page counts updated.
9. **LCD visual check:** red, green, blue, white and black patches. Checks appearance by eye; does not claim automatic pixel-defect detection or modify LCD timing.
10. **Keypad event monitor:** counts UP/DOWN/LEFT/RIGHT/MENU events reported through existing platform driver and displays latest event. Keys still perform navigation; ENTER/ESC always exits fast. Deliberately does not attempt invasive hardware key scanning or interfere with escape.
11. **Battery summary analytics:** min/max, mean coarse OS percent, sample count and first/last in the existing bounded 32-sample in-RAM history. Does not infer battery voltage, remaining time, health, charge cycles or actual capacity.

Additional validation added to GitHub Actions: checks the exact platform code matches Run012, no auto-exit, unchanged original OS battery resolver, original 4KiB heap allocation limit and page count. Build the real .tns in pinned Ndless SDK.

## Device test: single cable connection
Transfer one .tns, unplug, inspect all 11 screens; verify known CPU and MMU values, optional 4KiB check only if desired, examine colors and key counts, and exit with Esc. Run012 remains untouched as rollback.

## Risks
The pre-existing validated OS memory battery resolver is experimental and can hang if the firmware changes. The new LCD and keypad diagnostic pages only render pixels/count app key events in user-owned state. This build neither writes system RAM nor manipulates clocks, chargers, ADC, flash, USB, hooks or the NAND filesystem. CI success does not equal physical stability; Run013 needs real hardware confirmation.
