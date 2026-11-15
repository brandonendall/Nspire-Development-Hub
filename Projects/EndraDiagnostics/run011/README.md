# EndraDiagnostics Run011 — multi-feature, minimal-USB-test bundle

**Hardware preservation is a project-wide priority.** User cannot casually replace the TI-Nspire CX II CAS, wants fewer USB connection cycles and more meaningful features per build. This Run011 bundles six useful diagnostic pages in one new binary and retains verified Run008 fast manual exit.

## Pages
1. Live OS battery percentage, graphical gauge, validated OS charging task and scan status.
2. CPU ARM ID, CPU revision and read-only ASIC hardware ID.
3. CP15 cache type ID, raw I/D descriptor fields, CPU cache enable flags. Raw descriptor fields deliberately avoid unsupported capacity estimates.
4. MMU status, control, table base and domain registers. Total/free physical RAM still explicitly **NOT MEASURED**.
5. **Optional** conservative user-heap integrity check: press UP to allocate just 4096 bytes, write two known patterns into *only that allocation*, verify and free; displays PASS, ALLOC FAILED or MISMATCH. This is **not** a device-wide RAM diagnosis, hardware stress test or estimate of available RAM. Does not execute unless requested from page 5.
6. OS battery resolver detail: scanned words, candidate counts, validated capabilities/errors, session duration estimate, up to 32 coarse battery samples with first/latest readings, stored in RAM only (no file writes).

Controls: Menu/Right next, Left previous, Esc/Enter immediate fast exit, Up on the 5th page initiates small optional heap test. **No automatic exit**, no mandatory 30-second timer. This is a foreground-only tool; exiting ends monitoring.

The same GPLv3-derived nMedia resolver, validated MMU mapping, screen platform and proven Run008 fast-exit routine are used unchanged. No battery ADC conversion, charger register writes, firmware modification, filesystem writes or large-scale RAM probing were introduced.

## Safe testing sequence (one USB hookup)
Transfer Run011 once, unplug USB, launch and use Menu/Right to inspect all six pages. Verify battery/CPU/MMU values match prior photos. Optionally press Up once on page 5 for 4 KiB heap test. Stay open beyond 30 seconds, then Esc to exit; verify normal calculator operation. If any crash/freeze occurs, stop testing and preserve Run008/Run010 as recovery baseline.

**Run011 compiled status is separate from hardware verification.** Running on the calculator has not yet been confirmed.
