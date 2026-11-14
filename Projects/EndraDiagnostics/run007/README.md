# EndraDiagnostics Run007 — fast versus safe exit comparison

Run006 was confirmed as working by the user on physical TI-Nspire CX II CAS OS 6.4.0.74 but takes about ten seconds to return to TI OS after exiting. Run001 froze and required hard reset/reinstall of Ndless. Keep Run006 untouched.

## Source comparison

EndraDiagnostics Run006 uses `eh_platform_stop()`, which restores LCD registers and timer then calls `refresh_osscr()`. The fast-exiting UltraZip program instead calls `lcd_init(SCR_TYPE_INVALID)`. This comparison **does not** substitute `lcd_init()` for raw-LCD restoration: those are different display initialization paths. We only make one isolated change: skip `refresh_osscr()` *after* restoring LCD and timer, to determine whether that refresh contributes to the roughly 10-second exit latency.

## Test

- **Esc** uses experimental FAST exit. It restores LCD settings and timer but omits `refresh_osscr()`.
- **Enter** uses the original SAFE exit with `refresh_osscr()`. This matches Run006.
- Automatic ~30-second exit also uses SAFE path.
- Compare time to normal TI screen for Enter versus Esc. Test Enter first, then reopen and test Esc.
- If Esc returns to a blank/stale screen, **do not assume a freeze**; allow the normal TI OS to redraw. If no response, follow usual recovery procedure. Never rely on automatic exit as a watchdog.
- No new battery scanner/charger/ADC behavior. Same read-only Run004 resolver, approximately 1Hz updates, OS-level charge reporting. No direct writes to OS data; only existing display and timer register restore.
- The faster-exit hypothesis is unverified. Even if Esc is faster, the cause may be process teardown or task scheduling rather than graphics refresh. No proof of physical safety until tested.

This is a diagnostic A/B build, not a permanent performance fix. GPL provenance for the reused graphics and battery resolver is documented in earlier Run002 and Run004 READMEs.
