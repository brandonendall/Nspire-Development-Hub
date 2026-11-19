# EndraTask Run001

Read-only diagnostics for the TI-Nspire CX II CAS on OS 6.4.0.74 with Ndless r2023, the revision Brandon confirmed. The dashboard shows a local Documents open check, the last valid stage from an optional imported EndraTermux Build 010 report, and a scrollable task snapshot. The calculator's local check opens and closes the Documents directory without enumerating it. The app does not poll the task list or USB while idle.

## One-session calculator check

1. Once the normal transfer path is available, copy `EndraTask_Run001.tns` to Documents and open it. Check that the dashboard appears. Note the Documents result and whether tasks are listed or marked unavailable.
2. Scroll with Up/Down, then press R once to refresh. Note any errors and press Esc. Verify the TI Documents screen returns.
3. If the file service is still busy, keep the existing calculator tools installed. The Android-only Build 010 `probe-path <known-absolute-calculator-path>` runs independently. Use its terminal output to record the last `[V2 PROBE] stage=` line. If you already have a saved report on the calculator, Run001 reads it from `/documents/EndraTask_USB.txt.tns`. A report is optional, and the dashboard identifies it as imported data. There is no live USB feed.

“Opened /documents” establishes local directory accessibility. A task name or wait state does not identify the owner of a USB or filesystem lock. No recovery action runs from this app.

## Build and source

Use the Ndless SDK at `e52415fb568e92cd36ed049067b3e9c2727b84af` and nKits at `a040928502969b3cab393d890e23c9eaba45a30c` with the workflow in this repository. Set `NKITS` to the path of that checkout or the upstream subset from the source archive, then run `make host-test all`. The patch applies to a generated build copy of nKits' native task backend. It removes non-task discovery and the mutation entry point, and caps successful snapshots at 64 tasks. Original firmware signature validation remains in place.

This build includes nKits code by Ziyang-Bai under GPLv3. TaskManager credits compu and Excale for the original. The build artifact includes the license and corresponding app sources.
