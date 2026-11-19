# EndraTask Run002

Target: TI-Nspire CX II CAS, OS 6.4.0.74, Ndless r2023. The 320×240 dashboard reuses the bounded nKits task backend and the Run001 local Documents and optional Build 010 report checks. It shows task name, state, priority, stack allocation and a hexadecimal snapshot ID. The ID is a Nucleus task object address that can be reused by the OS. It is unsuitable as a persistent PID or a process-control argument. CPU usage, total task memory, open file handles and lock ownership are unavailable through the interfaces validated for this build.

## One-session check

1. Transfer `EndraTask_Run002.tns` once and launch it. Note the local Documents result, task rows or any unavailable message.
2. Scroll with Up/Down and refresh once with R. Press E and then Esc to verify cancellation. To save the report, press E and Enter. This creates or replaces `/documents/EndraTask_Run002_Report.txt.tns`. A failed write can leave a partial report.
3. Press Esc to exit. Retrieve the report with the existing EndraTermux `nlink-v2 download` workflow, using the exact calculator path above. Check that the report opens and contains the displayed task and diagnostic results. This device and retrieval check is pending.

The confirmation controls only report export. It has no process-control action. Run002 uses no resident hooks, polling of task or USB status, resets, service restarts or repeated enumeration. Diagnosing the “file system busy” response still requires correlating the local results with the Android-side probe. A task wait state gives no verified lock ownership. The optional USB field reflects a manually imported Build 010 log, so it can be stale.

## Future process control

The handheld build needs a verified way to classify protected tasks and revalidate task identity before considering local termination. Graceful shutdown depends on the target having a supported request mechanism. A separate opt-in and authenticated channel, with read-only requests validated first, would be required for remote commands. The handoff's list, info, kill, memory, usb and filesystem commands are proposals.

## Build

Use the pinned nKits and Ndless revisions in [Run001](../run001/README.md) and the Run002 workflow. From this directory, run `make NKITS=/path/to/nkits host-test all`. The resulting file is `EndraTask_Run002.tns`. The build uses the upstream nKits GPLv3 task backend and drawing code. The packaged source includes the upstream license, C sources and the read-only patch.
