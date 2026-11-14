# EndraHome launch-speed evidence

Updated 2026-11-14; TI-Nspire CX II CAS. This file separates **measured end-to-end timing** from hypothesized causes.

| Version | Target | Trigger → observed endpoint | Measured device duration | Result |
| --- | --- | --- | --- | --- |
| Run006 | Small `EndraHome_Test006.tns` | Confirmed Tools shortcut → success popup | Not timed | User reported “Everything works.” |
| **Run008** | Same small `EndraHome_Test006.tns` | **Final Enter on “LAUNCH APPLICATION?” → success popup** | **10.04 seconds** | User photos verify successful execution; delay is unsatisfactory. |
| Run009 | Same small target (planned) | Final Enter → success popup | **Not yet tested** | Fast-path experiment: remove redundant preflight open and fixed 180-ms sleep. |
| Direct TI Documents | Same target (planned) | Open directly → success popup | **Not yet tested** | Compare to estimate Ndless loader/OS time. |

## Code-level observations

Run008: final Enter sets `request` in `run007/ui.c`, the loop exits, `eh_platform_stop()` restores LCD + timer + TI Documents screen, a preflight `fopen/fclose` checks the target, `msleep(180)` pauses, then `nl_exec(path,0,NULL)` starts child process.

Run009: it retains input, request handling, config validation, and **the exact native display/timer/OS restoration sequence**, but eliminates the redundant preflight open and 180ms sleep. Loader errors are still shown after `nl_exec` returns an error.

A fixed 180ms pause is **only 1.8% of 10.04 seconds**, so this alone cannot fix the full delay. Disk operations and OS/loader startup may account for more, but **no phase-by-phase device timing is available**. Never claim a root cause without controlled measurements.

## Rules for next experiment

- Keep run numbers aligned: Run009 reads only `EndraHome_Apps009.cfg.tns`; future RunNNN requires `EndraHome_AppsNNN.cfg.tns`.
- Time the same target on the same calculator; start at **final confirmation Enter** and stop at **success popup appearance**.
- Also time launching the target directly from TI Documents. Repeat timings if convenient.
- Do not bypass the verified video restoration to save a second; the original launcher repeatedly hard-froze when it mixed graphics and child launching.
- Do not alter hardware clocks, Ndless hooks, root files, or ROM association to chase loading speed.

