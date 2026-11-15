# EndraDiagnostics Run010 — manual-only exit

Requested on physical TI-Nspire CX II CAS (OS 6.4.0.74): **remove the automatic 30-second timeout**. The user reported that the existing manual exit appears fast. This build isolates the requested change; it does not add RAM stress testing or any other uncertain system behavior.

- The dashboard stays open **indefinitely** until Esc or Enter is pressed.
- Esc and Enter both invoke the same fast-exit procedure confirmed in Run008. Run009 photos confirmed CPU and MMU pages, with fast exit reported tentatively.
- The 30-second exit limit, exit progress bar, and 30-second instruction were removed.
- Menu/Right cycles 3 pages; Left goes to previous page.
- Live battery updates, OS charging-mode display, and read-only CPU/MMU register reads are unchanged from Run009.
- This is **not a persistent background service**; battery monitoring stops when exiting the application.
- Platform source and GPL-licensed vendored battery resolver are unchanged from previous runs.
- Hardware test still pending; because battery metadata reads OS memory, hangs are possible and Esc cannot recover from a native freeze. Do not rerun the known-bad Run001.
