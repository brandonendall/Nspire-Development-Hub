# EndraDiagnostics Run005 — battery signature discovery status

Hardware: TI-Nspire CX II CAS / Ndless / OS 6.4.0.74 target.

**Observed:** Run001 hard-froze and required hard-reset + Ndless reinstall. Run003 was user-confirmed fully working (graphics, keys, exits). Run004 photographed running and scanning at 327,680 / 1,048,576 words with OS battery level unavailable (`--`). **Run004 exit and full scan result have NOT yet been reported.**

Run005 changes **one thing:** speeds up the same existing bounded read-only scan (16,384 words per five platform loop cycles instead of ten), so it should reach the end of its 1,048,576-word, 4 MiB search earlier. It displays signature candidate counts (P percent, C charge, A ADC, V converter), capability/error flags and distinguishes incomplete vs completed unsupported scan. It does **not** add any battery register access, charging hardware access, or writes to OS memory.

If `OS BATTERY: --` persists after `SCAN COMPLETE`, the current resolver does not recognize this firmware. Do not infer an actual battery percentage, voltage or battery health. Scanned candidate counts alone are not valid telemetry. The reported percentage, when available, is a coarse OS level, not a precision gauge.

Same tested Run003 graphics/ESC/autoexit platform; autoexit at ~12 seconds is not a watchdog if native code freezes. **Run005 has not been tested on real hardware and carries the same undocumented OS-memory-read risk as Run004.** Preserve Ndless recovery.

Source uses existing Run004 GPLv3-derived nMedia battery resolver; attribution in Run004 README and its vendored files. No new reverse-engineered OS services are called.
