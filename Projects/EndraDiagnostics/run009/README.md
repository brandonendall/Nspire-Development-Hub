# EndraDiagnostics Run009 — Hardware information dashboard

## Physical testing baseline
Run008's unified fast exit after approximately 30 seconds was user-confirmed **super fast**. Run009 preserves that exact LCD/timer restoration logic: `eh_platform_stop_mode(0)`. Battery telemetry is unchanged. No new battery/OS RAM resolver writes or memory stress tests are introduced.

## New: CPU and MMU dashboard
- Three pages: battery monitor, CPU identity, memory/MMU registers.
- MENU or RIGHT cycles to next page; LEFT cycles to previous. ESC or ENTER fast-exits. The existing 30-second auto-exit also uses fast stop.
- CPU page reads ARM926 CP15 Main ID Register and cache type register (both read-only); reports implementer, architecture nibble, part and revision. The CPU name is shown only if implementer 0x41 and part 0x926 match.
- Memory page reads CP15 MMU system control, translation table base and domain access control (all read-only). It **does not** infer installed physical RAM or free heap capacity from these registers. Those are labeled `NOT MEASURED` until a distinct validated memory API/test exists.
- Not a CPU speed measurement. No allocation/stress test or OS memory probing added.

## Safety
CP15 register reads on the user's known ARM926EJ-S CPU are expected to be read-only and non-destructive, but Run009 is NOT YET hardware-tested. The pre-existing experimental OS battery resolver also reads undocumented mapped RAM. Don't rerun original Run001, which froze the calculator and required Ndless reinstall.

Run009 compiles Run008's platform code **unchanged** into this new run. New page management is isolated in `main.c`. Its original fast-exit path is retained for all exits.

GPL code provenance is recorded in Run002 (gpSP graphics) and Run004 (nMedia battery resolver).
