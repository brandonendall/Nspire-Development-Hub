# EndraDiagnostics Run008 — unified fast exit

Run006 was reported working on physical TI-Nspire CX II CAS with OS 6.4.0.74 but took roughly 10 seconds to exit. Run007 retained the same functioning battery monitor and added an A/B shutdown comparison: Esc skipped the final `refresh_osscr()` and exited quickly according to user testing; the 30-second automatic timeout still called the original refresh and took 7–8 seconds. Run007 Enter behavior has not been separately measured.

**Run008 changes precisely one behavior:** all three exit paths now restore LCD and timer registers using the Run007 experimental fast method, without an extra `refresh_osscr()` call.

- Esc = fast exit
- Enter = fast exit
- Automatic ~30-second expiry = fast exit

The battery percentage scanner, OS charging mode, display, validated RAM access, 1 Hz snapshots, and all screen initialization remain unchanged from Run007. The separate normal `eh_platform_stop()` routine still exists in the copied platform, but the Run008 app calls `eh_platform_stop_mode(0)` on every exit. No hardware ADC, battery-controller, filesystem, or OS memory writes are added.

The fast method does still restore native LCD base, control/timing registers and the previously saved timer settings. It omits only `refresh_osscr()`, which may leave a stale screen until the TI OS draws its next frame. **Physical Run008 exit behavior has NOT been tested.** Run001 hard-froze on this calculator previously; keep recovery available. A 30-second auto-exit is *not* a watchdog if an OS-memory read hangs.

GPL-derived battery-resolver attribution is preserved in Run004; gpSP-derived graphic routines attribution is preserved in Run002.
