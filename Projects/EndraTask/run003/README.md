# EndraTask Run003

Read-only path diagnosis for the TI-Nspire CX II CAS with OS 6.4.0.74 and Ndless r2023. Run003 tests selected paths one operation at a time. All results stay in memory until exit. There are no startup filesystem probes, directory scans, report writes, automatic retries or task controls.

## One calculator session

1. Upload `EndraTask_Run003.tns` once and launch it. Press Enter once on **Documents / META** for the calculator-local control.
2. Use Up/Down to select a target and Left/Right to choose **META** or **ACCESS**. Press Enter once to run the selected check. META queries metadata. ACCESS opens and closes a directory without listing it, or opens a file read-only and tries to read one byte. Start with the two Upload rows to check the local path mapping. When one resolves, check the corresponding Run002 file. The USB folder and previous export file are optional targets.
3. Note the path, step, result, raw return code and errno. If a call does not return, note the last displayed step and stop the test instead of repeatedly probing. The calculator's filesystem calls are synchronous. Esc exits from the dashboard when the call has returned.

A returned BUSY label requires the reported errno to equal EBUSY. Some SDK operations return a failure without an errno, shown as FAILED with errno 0. An on-screen Running message identifies the unfinished step. Elapsed time uses the device clock with one-second resolution. Results reflect the time each operation ran.

## USB comparison

Use the existing EndraTermux Build 010 read-only `probe-path` command separately for the known uploaded file at `/upload/EndraTask_Run002.tns`. Record its last `[V2 PROBE] stage=` line. Compare it only with the calculator-local path that resolves to that file. Each result covers its own operation. A successful metadata or read check does not establish write availability. Run003 does not identify a lock owner or cancel requests.

## Build and source

Use Ndless revision `e52415fb568e92cd36ed049067b3e9c2727b84af` and nKits revision `a040928502969b3cab393d890e23c9eaba45a30c`. Run `make NKITS=/path/to/nkits host-test all`. The build includes GPLv3 nKits drawing code by Ziyang-Bai. The source package includes the used upstream code and license.
