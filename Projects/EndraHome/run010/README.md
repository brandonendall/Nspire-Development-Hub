# EndraHome Run010 — no unnecessary document-browser refresh

**Experimental, not yet tested on calculator.** User clarified that approximately **10 seconds is spent closing EndraHome itself**, not necessarily loading the tiny child app. That changes our investigation from generic Ndless startup to *the EndraHome exit path*. Run008 took 10.04 seconds final-confirmation-to-target; a following stopwatch photo showed 9.94 seconds in the Run009 context. These were end-to-end measurements, not separately measured phase times.

## Why try this?

Reviewing the actual Run009 code found **two calls to `refresh_osscr()`**:
- Inside `gba-ui/platform.c:eh_platform_stop()`, just after restoring the original TI LCD registers and timer.
- Again at the end of the Run009 `main()` after `nl_exec` returns.

Official Ndless documentation describes `refresh_osscr()` as necessary to make document-browser file additions/deletions visible. Community developers have also warned it is **slow with many Documents files**. EndraHome only reads shortcut configuration, does not create/delete files, so a full document-browser refresh appears unnecessary. This is a **hypothesis, not a confirmed root cause**.

## Exact code change

- **Only launch/exit change**: the Run010 copy of the known-working GBA-derived native `platform.c` omits the `refresh_osscr()` call but preserves **all LCD register writes, timer restore and started-state cleanup unchanged**. The trailing unconditional `refresh_osscr()` in `main.c` is omitted too.
- Everything else is copied from Run009, including `nl_exec(path,0,NULL)`, seven shortcut navigation, confirmation, UI, path restrictions and config parser. Existing Run003–Run009 files remain untouched.
- **Permanent config filename**: `EndraHome_Apps.cfg.tns`. Starting with Run010, all future builds use the same filename. The Menu displays this name. Runs007–009 remain historical binaries and expect their old numbered files.

## Device test: isolate the exit delay first

1. **Keep working Run006/008/009 installed.** Make a **copy** of your configured `EndraHome_Apps009.cfg.tns` (or 007 if that's where you edited the shortcuts). Rename that copy exactly `EndraHome_Apps.cfg.tns`, placed alongside `EndraHome_Run010_NoRefresh.tns` inside Documents. Keep the older numbered file; older builds still need it.
2. **Before attempting any app launch**, open Run010 and immediately press **Esc** on the Home page. Time the interval until the TI Documents display is usable.
3. If a blank/garbled/stale TI Documents screen, hard freeze, or broken keyboard occurs, **stop using Run010**, reset the calculator if necessary, and return to Run009. Removing the refresh is a controlled experiment, not a guaranteed safe optimization.
4. Only if Esc returns correctly, reopen Run010. Verify Menu reads `EndraHome_Apps.cfg.tns` and Home shows `1 / 7 READY` for your configured Tools test path.
5. Time **Tools → Enter (details) → Enter (confirm) → Enter (launch)** until the **EndraHome_Test006.tns** success dialog appears. Compare Run010 with original ~10-second times using the **same target**.
6. If it is still slow, time opening the test app directly from TI Documents to distinguish Ndless startup from EndraHome. Do not remove any LCD/timer restoration or touch system hooks to chase performance.

**Important:** This build is NOT a file manager and does not refresh the TI document list after other apps create/delete files. Such apps may refresh it themselves; if not, re-opening TI Documents should update the list. If you notice missing file-list updates, return to the prior working build.

See `Projects/EndraHome/LAUNCH-PERFORMANCE.md` for the historical measurements.

## Permanent name policy — supersedes earlier numbered configs

Owner's final preference: **NO build number on the config file**. Run010 and all later builds expect exactly `EndraHome_Apps.cfg.tns`. The executable still keeps RunNNN in its filename. A single user-edited config can be reused in future builds when placed beside the launcher. EndraHome never edits the file and never searches old numbered config filenames.

### Fixed Help filename

Run009's device photo showed `EndraHome_Apps007.cfg.tns` on its Help screen even though Run009's loader expected Apps009. This was caused by the imported Run007 renderer header pulling in an old config constant before the new header. Run010 now has its own `render.h` which includes the local registry header first. Host tests verify the stable config name, and the in-app Help screen uses `EH7_CONFIG_FILE`. Verify on hardware that the correct name is displayed.
