# EndraDiagnostics Run004 — experimental OS battery monitoring

Run003 was confirmed on real TI-Nspire CX II CAS with working display, input, ESC, autoexit. Run001 hard-froze; do not rerun Run001.

**Run004 adds a single new diagnostic:** A read-only attempt to monitor the *coarse OS-reported* battery level through validated RAM metadata. It draws a fresh reading about once per second while running. It does **not** read the battery ADC, query hardware voltage, change charger controls or generate a fake percentage. If OS 6.4.0.74 is not recognized by the signature validator, it shows "-- / NOT SUPPORTED ON THIS OS".

Run004 uses GPLv3 code derived from GigaZelensky/Nspire-MediaPlayer-Ndless (battery_native.c, battery_native.h, portable_resolver.h, arm926_ram_span.h) at https://github.com/GigaZelensky/Nspire-MediaPlayer-Ndless. Source under vendor/ must retain GPLv3 attribution. The implementation leaves the original model-neutral resolver unchanged but removes the upstream platform wrapper and supplies a small read-only validated view adapter. It temporarily masks interrupts to safely read current OS RAM. It does not call reverse-engineered OS functions or write to MMIO; however, **reading undocumented OS memory may still freeze incompatible firmware**, so do not assume it is as safe as Run003.

ESC exits and bounded ~12s auto-exit are retained. If firmware validation fails, output "--" rather than inventing a reading. Sample is an OS coarse status (may only change in steps), not a calibrated continuous battery percentage, and does not establish charging state.

Physical calculator compatibility **NOT tested**. Keep Ndless reinstall ready. Do not install persistent Ndless on account of this experiment.
