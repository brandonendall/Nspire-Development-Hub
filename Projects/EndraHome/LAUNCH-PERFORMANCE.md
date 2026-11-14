# EndraHome launch-speed evidence

Updated 2026-11-14; TI-Nspire CX II CAS. This file separates **measured end-to-end timing** from hypothesized causes.

| Version | Target | Trigger → observed endpoint | Measured device duration | Result |
| --- | --- | --- | --- | --- |
| Run006 | Small `EndraHome_Test006.tns` | Confirmed Tools shortcut → success popup | Not timed | User reported “Everything works.” |
| **Run008** | Same small `EndraHome_Test006.tns` | **Final Enter on “LAUNCH APPLICATION?” → success popup** | **10.04 seconds** | User photos verify successful execution; delay is unsatisfactory. |
| **Run009** | Same small target (context of current test) | Final Enter → success popup, assuming same timing procedure | **9.94 seconds shown on stopwatch** | Photo supplied after Run009 test instructions; timing appears 0.10 seconds (≈1.0%) faster than Run008. Need confirmation of same measurement endpoints. |
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


## New stopwatch photo — 2026-11-14

User supplied a photograph of a Timex stopwatch with **9.94 s** visible, immediately after the Run009 trial instructions. Context strongly suggests this is the Run009 final-Enter-to-test-popup time, but the photograph shows **only the stopwatch**, not Run009 or the success popup. Accordingly, record **9.94 s as the photographed candidate Run009 measurement**, with the measurement protocol and exact build attribution not independently visible. Compared with Run008's user-reported 10.04 s, that is **0.10 s (100 ms) faster, about 1.0%**—far smaller than the overall ≈10-second delay and well within the range where run-to-run variation may matter. Do not claim a meaningful speed fix or root cause. Still need a direct-from-Documents target timing and ideally repeated runs.

## Run010 — suspected refresh overhead (2026-11-14)

User clarification: **"It takes our Endra home about 10 seconds to close out."** We must not assume `nl_exec` itself accounts for ~10 seconds. Run009's platform exit includes `refresh_osscr()` and its main includes another call after child execution; Ndless docs reserve this for document-browser updates after file create/delete. Because Home never creates/deletes files, Run010 tests whether **skipping unnecessary browser refreshes after preserving hardware register/timer restoration** accelerates exit. This is a hypothesis, not the verified cause. Run010 uses `EndraHome_Apps010.cfg.tns` and is currently native-built/physical-test pending. **Measure Esc-only return to usable TI Documents separately from final Enter → child popup.** If screen restore misbehaves, stop and revert to known working Run009.

## Shared config filename decision (2026-11-14)

Owner's final preference is a permanent unnumbered file: **`EndraHome_Apps.cfg.tns`** for Run010 and future versions. Older binaries still use their earlier numbered configs. This naming change is separate from the Run010 no-browser-refresh performance experiment; measure its exit time before claiming a speed improvement.
