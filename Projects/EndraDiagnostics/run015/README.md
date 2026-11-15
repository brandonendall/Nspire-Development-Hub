# EndraDiagnostics Run015 — practical system monitor

The owner wants a **useful daily hardware monitor** rather than redundant LCD/grayscale/keypad pages: CPU, RAM, battery and storage at a glance. The user's TI-Nspire CX II CAS on OS 6.4.0.74 is difficult to replace; one physical transfer is preferred, hardware preservation is mandatory.

## What Run015 really reports
Page 1: a 6-page main dashboard, with real ARM926EJ-S CPU identification, validated live coarse OS battery percent, and read-only scanned Documents file size if the scan has been requested. RAM/free-storage percentages are **UNAVAILABLE**, not invented.
Page 2: CPU ID, processor revision, I/D cache activation from already-read CP15 registers. CPU MHz and utilization are explicitly unmeasured (no CPU stress probing).
Page 3: RAM monitor with transparent unavailable total/free/used values, plus existing optional 4 KiB allocator-owned two-pattern check on UP; does not touch arbitrary physical/system memory.
Page 4: **NEW read-only documents file accounting**. UP manually triggers a stat()/opendir()/readdir() scan starting at Ndless get_documents_dir(). Strict cap of 256 directory entries, max 16 visited folders, depth < 3, 256-byte folder paths. Results report file/folder counts and file-byte tally of files actually examined; partial or inaccessible trees are labeled PARTIAL/UNAVAILABLE. These bytes are **not total installed storage, actual disk used/free, or a utilization percentage.** It does not write, open file contents, or follow outside the Documents virtual root intentionally; however it uses stat rather than lstat and does not claim hardened symlink confinement.
Page 5: live OS battery percentage bar and charge-task mode, using same Run014 battery resolver without additional ADC/charger accesses.
Page 6: read-only OS battery resolver status and in-session 32-reading min/max/average.

## Invariants
- Run014 known-good platform.c copied byte-for-byte: fast manual Esc/Enter exit, no auto exit, original LCD/timer restore.
- Ndless r2023 API get_documents_dir() is in its public syscall declarations. Directory stat()/opendir()/readdir() already used in UltraZip in this repository, but **the new Documents scan has not been tested on physical hardware**.
- Existing proven Run014 battery resolver remains untouched. Existing optional 4 KB user allocation pattern remains bounded and manual-only.
- No ADC or charger writes, no unknown OS memory/heap boundary probing, no filesystem writes, no clock or hook changes. OS free RAM/total storage should not be reported until authoritative safely callable OS APIs are validated for 6.4.0.74.
- Successful native build does not guarantee physical compatibility, especially for the optional Documents scan.

## One-connection acceptance
Download the single TNS, upload once, disconnect USB, check CPU and battery display. On RAM page optionally trigger the small heap test. On Storage page UP triggers **read-only** bounded document accounting; skip it if the user prefers avoiding any filesystem access. Check that no data changed, Esc exits normally. Keep Run014 unchanged as the stable rollback.

We intentionally removed color/grayscale/keypad pages from the main interface rather than duplicating them. Their source and binaries remain in earlier runs.
