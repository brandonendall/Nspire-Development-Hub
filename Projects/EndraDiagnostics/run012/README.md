# EndraDiagnostics Run012 — comprehensive one-session dashboard

Built in response to the owner's hardware-preservation policy: the TI-Nspire CX II CAS is not cheaply replaceable, and every cable insertion and physical test should earn its cost. **Run011 was user-reported to work great. Run012 is not yet hardware-tested.**

## Eight pages in one build
1. Run011 live OS battery percentage, charging-mode display, gauge and discovery.
2. Run011 processor identification and read-only ASIC ID.
3. Run011 cache controller: raw I/D fields and enable bits, no unverified sizing guesses.
4. Run011 MMU control, TTBR and domain access. Total/free RAM remain honestly unmeasured.
5. Run011 optional, strictly bounded 4 KiB malloc-owned integrity test (Up to trigger, never automatic).
6. Run011 OS signature resolver progress, capability/error flags and in-session sample telemetry.
7. **NEW** 32-sample in-RAM battery trend bar chart, coarse OS percentage with sample min/max. No assumption that one sample equals one second or that coarse OS percent is actual voltage. No log file writes.
8. **NEW** consolidated safety/availability page: firmware resolver completion, heap-test result and explicit statements about what the binary does *not* modify.

Run012 also corrects the page counts to 1–8, preserves Menu/Right next, Left previous, Esc/Enter fast exit, and keeps **no automatic exit**. The native GBA-style LCD/timer restore is copied byte-for-byte from physically validated Run011.

## Safety & device testing
No new OS RAM scanning, charger/ADC reads or writes, voltage speculation, CPU stress loops, flash/SD/NAND operations, filesystem writes, Ndless hooks or clock changes. The only heap write is the existing explicitly user-triggered 4 KiB test. A slow or missing OS battery response should never be replaced with fabricated figures. Existing battery metadata reading remains experimental and is not a recovery-safe operation if it hangs.

Do a single USB transfer, disconnect cable, verify pages 1–8, optionally run small page-5 test, then verify Esc fast exit and normal OS navigation. Avoid repeated insertions and do not run known-bad Run001. Keep Run011 as the stable rollback.

This implementation intentionally does not claim actual total/free physical RAM or calibrated battery health. Enhancements needing firmware-specific or hardware-level probes should have host validation before touching the only calculator.
