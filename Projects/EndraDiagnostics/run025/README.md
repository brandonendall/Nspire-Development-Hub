# Run025 — measured live performance prototype

## Source investigation
- Ndless SDK `SDL_timer.h` declares `SDL_GetTicks` and describes timer resolution; useful for elapsed-time benchmarking, not OS CPU utilization.
- Ndless SDK `libndls.h` declares CPU speed controls; changing clocks is deliberately out of scope.
- Ndless SDK `syscall-list.h` has allocation/free, but no verified total/free system RAM syscall was identified.
- Ndless SDK tree includes `OS_cascx2-6.4.0.74.idc`, confirming OS-specific symbol mapping resources exist; no unverified syscall is invoked.
- Existing tested EndraDiagnostics platform.c already owns a 32768 Hz timer. Run025 exposes a read-only tick getter without reinitializing hardware.

## Measurement design
- CPU dial now means **relative speed of an 8192-iteration bounded integer benchmark** measured every ~1 second. First measurement is baseline; 100 means at least as fast as baseline. Lower means slower benchmark; **NOT CPU load**.
- RAM dial means **completion of two application-owned memory integrity checks**, repeated every ~5 seconds; **NOT system RAM used/free**. Stable 100 is expected when both checks pass; no fake fluctuations.
- No new hardware register writes. Keep fast first paint, keyboard exit, battery, read-only bounded Docs scan and PART status.
- Benchmark sample time in timer ticks appears on CPU tab. CPU LOAD:N/A and RAM USED/FREE:N/A remain explicit.
- Run024 remains physical-tested fallback. CI compilation does not equal physical validation.

## Future research
Investigate SDK's OS-specific 6.4.0.74 syscall mappings for verified idle time and RAM statistics, without arbitrary memory probing. Review external nClock and Ndless file managers when original source provenance is confirmed.
