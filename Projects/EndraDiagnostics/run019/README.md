# Endra System Monitor Run019 — Automatic diagnostics, ACTUAL interactive tabs

User physically tested Run018 on the TI-Nspire CX II CAS OS 6.4.0.74. User explicitly confirmed fast exit, CPU/RAM N/A dials, battery 100%, working manual Docs scan (146 files /42 folders; 84622KB) and requested (1) checks run automatically, (2) bottom tabs actually change information, (3) CPU/RAM gauges eventually show real usage.

## Single-page HUD changes in this build
- One-screen tachometer HUD remains; 6 tabs change center-detail panel + relevant lower cards **visibly**, with real CPU CP15 ID/cache, memory heap-test outcome, document count/size stats, OS battery, and validated resolver counters. NO secondary pages.
- **One-time automatically run small 4 KiB heap integrity test on startup.** This is a known-tested user-owned memory allocation; does NOT measure system free/used RAM.
- **One-time automatically run existing bounded READ-ONLY Documents scan on startup.** Uses exactly the same previously tested Run018 function (at most 384 directory entries, 48 folders, depth 4 and 48 read-only size checks). Displays scanned-document KB and file/folder counts immediately instead of requesting an Up key press. Does not scan repeatedly, does not write files, or claim total/free storage.
- Original tested Run018 270-degree dial renderer and fast manual exit platform are unchanged. Esc or Enter exits immediately once in the main event loop.
- No auto-exit or timeout.
- **CPU utilization and OS RAM usage are STILL N/A.** The Ndless SDK documents CPU core/cache info but no firmware-verified OS CPU-utilization/free RAM counters were found. The OS Handheld Status displays storage capacity/free, but no stable Ndless API for it is verified on OS 6.4.0.74. Never fill gauges with fictional numbers from the concept art. 0-to-100 dial colored segments represent scale, not usage. No needle until real measurements exist.

## UI/behavior
HOME: CPU/RAM N/A dials, center CPU properties, battery and scanned Docs stats.
CPU tab: CPU ID/revision, cache info in Documents-size card location.
RAM tab: app-owned 4 KB test result and test count, RAM used/free still N/A.
DOCS tab: number of files, unknown sizes, scanned known KB.
BATT tab: coarse OS percent and samples, charging status.
INFO tab: scanned words, capabilities, errors and read-only status.
Right/Menu forward and Left back select within same full dashboard; Up no longer needed. Esc/Enter preserves proven native fast exit.

## Hardware wear / remaining risk
Only one USB transfer requested. Startup scan may take more time than Run018 startup: platform input cannot be processed during bounded scan, so the first frame is delayed by the scan. Same already-tested read-only scanner and 4 KB heap check as prior run, not new hardware probing. Physical Run019 verification pending; preserve working Run018.
