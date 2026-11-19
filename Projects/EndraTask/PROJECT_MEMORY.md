# EndraTask project memory

Updated November 19, 2026. Start here when resuming EndraTask in the Nspire Development Hub. Active branch `feature/endratask-run001`. Run004 is the first nKits-based restart build and isolates launch and exit.

## Restart status

Brandon rejected Run003's local file-presence dashboard and reported a calculator restart when pressing Back to exit. The photo shows successful local metadata results for Documents and Documents/upload. Local metadata did not exercise the USB file service. Run002 and Run003 are retired from device testing; preserve the builds as historical evidence.

[Restart notes and host-tested traces](restart/README.md). The EndraTermux Build 010 host transport returns Busy for an incomplete CX II handshake, a failed stream write, a missing acknowledgment or an already-connected local service handle. This source finding does not identify which condition the device hit on the reported failures. The next bring-up must start from nKits Task Manager's validated lifecycle, with a clean exit check and no process-control or filesystem-write actions. Use manual task information only to support independently observed USB/file-service behavior. Run004 now packages the nKits startup and exit sequence as a minimal native TNS. Host verification passed; physical exit and the USB Busy source remain unverified.

## Build history

| Build | Source and verification | Download |
| --- | --- | --- |
| Run004 | [nKits-based launch-and-exit check](run004/README.md). Host lifecycle tests, native ARM compilation and byte-identical clean rebuild passed. Source commit `bbe53008df9a4ef6b0ddad5cf76eeff4009579ec`. [GitHub Actions run 44632856200](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44632856200) passed and produced [artifact 14583081514](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44632856200/artifacts/14583081514). SHA-256 `c7e9fa8fc85df67e9fc1a9cfa00b41363ca1b98bb3a3448e63d25dc54f94a9dc`. Calculator launch and Esc exit are pending. | [Run004 TNS](https://chatgpt.com/library?file_id=libfile_1c593cf36d3c8191ae3441cb7b7d263c&account_id=personal) |
| Run003 | [Read-only path dashboard and device checklist](run003/README.md). Source commit `2a5b9d24f635f717ca0492a3cb3154066868b94e`. [GitHub Actions run 44616085823](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44616085823) passed and produced [artifact 14575355227](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44616085823/artifacts/14575355227). Host tests and native ARM build passed off-device with pinned Ndless r2023; a clean build from the packaged source inputs matched the delivered TNS byte for byte. SHA-256 `b8e728cda8eec0d4a8f77d1d468a391799eab8d1f540c7f2804206f143361ff5`. Device test showed the local metadata dashboard and a reported calculator restart on Back. Retired. | [Run003 TNS](https://chatgpt.com/library?file_id=libfile_c0431627bb448191a29a993d4008d483&account_id=personal) |
| Run002 | [Source and device checklist](run002/README.md) at `2f228cd4acdd4c3c5ea18098b3cd4e43da4e8801`. Host tests and the native ARM build passed with the pinned Ndless r2023 SDK. Rebuilding from the packaged source reproduced the delivered TNS byte for byte. [GitHub Actions run 44605910040](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44605910040) passed at `125b53f5967dc0ea15e77c018af2f69d1b2cddcf` and published [artifact 14569718743](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44605910040/artifacts/14569718743). SHA-256 `242d20e1a6e80e10930b06f4afba5edebb0a65423b0bc658da2cddb4bc8d9dd8`. Run002 opened on the calculator, and displayed the Export confirmation after E. The calculator froze after Enter confirmed the save. Task refresh, report creation and retrieval are unverified. | [Run002 TNS](https://chatgpt.com/library?file_id=libfile_bf771b1640a48191917ac62f90966b39&account_id=personal) · [TNS, source and checksums](https://chatgpt.com/library?file_id=libfile_3c54fa22ea6c8191a73b3e6eedea3c3f&account_id=personal) |
| Run001 | [Source](run001/README.md) at `38eca6486d05c3b2472c5d5e51d1036d0e7eb8ff`. Host tests and native ARM compilation passed off-device. No physical test recorded. | [Run001 TNS](https://chatgpt.com/library?file_id=libfile_77b60499e66081918b39456fd935713e&account_id=personal) |

Run002 adds a per-snapshot task ID, task state, priority, stack allocation and manual report export to `/documents/EndraTask_Run002_Report.txt.tns`. The task ID is a transient Nucleus object address. CPU utilization, total task memory, open handles and lock-owner attribution are unavailable. Local and remote termination remain proposals. Preserve the Run001 binary as a read-only baseline.

The November 19 EndraTask/EndraTermux handoff reports that Smart Push Run011 transferred a file with `nlink-v2` to a custom `/upload` destination after one “No such device” failure. Treat that as one successful transfer, with reliability and the cause of the initial failure still unverified. Use normal EndraTermux file retrieval for the Run002 export. Preserve the working transport.

### Run002 transfer via Smart Push Run012

On November 19, 2026, Brandon supplied a Smart Push Run012 log for the 68,348-byte `EndraTask_Run002.tns` upload to `/upload/EndraTask_Run002.tns`. The TI-Nspire session and file service opened, the file data phase returned `ret=0`, and Smart Push reported `Upload command completed`. This confirms a successful transfer at the command level. The subsequent on-calculator test is recorded below. The transfer alone does not identify the cause of earlier busy errors.

### Run002 calculator freeze when confirming Export

On November 19, 2026, Brandon clarified that **E** opened the Export save confirmation normally. The calculator froze after he pressed **Enter** to confirm, with no visible save result. He recovered with the physical rear reset button. The confirmed path calls `wait_no_key_pressed()`, then synchronously opens `/documents/EndraTask_Run002_Report.txt.tns` in write mode, writes and closes the file, and redraws with the result. The exact blocking operation and whether a file was created are unverified. Investigate the Enter key-release wait and the open/write/close calls. A diagnostic build can show progress before each stage and avoid the blocking key-release wait. Pause further Run002 export attempts on the calculator.

## Build delivery and Termux transfer

When Brandon asks for a new EndraTask build and the build is ready:

1. Verify the actual `.tns` matches the requested run. Preserve the exact filename.
2. Make the actual `.tns` available as a direct download in the same chat reply, using a sandbox link to the file in the workspace. The download should work from the chat without opening the Library, visiting GitHub, or extracting a ZIP.
3. Include a copy-ready Termux code block immediately below the download. Put `clear` first, then `push` with the phone's source path and the exact lowercase calculator destination `/upload`. Use the actual filename and default to the phone's Download folder unless Brandon specifies a different location.

```bash
clear
push /storage/emulated/0/Download/EndraTask_Run002.tns /upload
```

## Goal

Build a lightweight task and file-service diagnostics utility for Brandon's TI-Nspire CX II CAS on OS 6.4.0.74. Gather useful evidence about the recurring USB “file system busy” condition without repeatedly transferring builds, stressing the calculator, or modifying its system state. Batch related features into one release and do as much verification as possible off-device.

## Historical Run003 diagnostic scope

Run003 makes local path probes the main screen and keeps results in memory. It does not include the nKits task-list backend or the Run002 report write path. Compare the same resolved target with the phone's Build 010 probe separately. Brandon confirmed the Run002 task list largely duplicates nKits Task Manager. EndraTask's primary job is to identify the affected folder or file, the failing operation, and what can safely be canceled. Keep the task snapshot as optional context. Run002's local `opendir("/documents")` checks one directory and does not identify a lock or its owner.

1. Make the probe results the main screen. Compare calculator-local access with EndraTermux's single-path metadata operation for the same resolved target and a known-good control. Start with the folder used for the successful Run012 transfer and the path involved in the failing operation. Verify phone-side and calculator-local path mapping before comparing them.
2. Record the operation, path, elapsed time and raw result. Distinguish a returned busy error from a missing path, transport error or timeout. Reuse the existing read-only Build 010 direct-path probe for known existing files. A metadata success establishes only that operation; it cannot establish transfer or write availability. Avoid full-tree enumeration and automatic retries.
3. Run one check at a time, show the stage before a filesystem call, and keep report writing out of the first diagnostic pass after the Run002 freeze. Use a timeout on the phone-side diagnostic where possible. A timeout leaves calculator-side state unknown and does not prove a request was canceled.
4. Leave owner attribution unavailable until evidence ties a handle, task or service request to the specific failure. Prefer canceling a verified request through a supported interface. A task name or wait state is insufficient grounds to terminate a task. Keep any future recovery action separate, explicit and confirmed.

## Reuse these sources

| Source | How EndraTask should use it |
| --- | --- |
| [Uploaded nkits.zip](https://chatgpt.com/library?file_id=libfile_750c69d88e8c8191bd879a538bba4b81&account_id=personal) | Existing calculator builds and UI references. The inspected archive contains `nkits/taskmanager.tns`, `nkits/nusb.tns`, `nkits/nusb-diag.tns`, and usage guides. Build from upstream C source. |
| [nKits TaskManager at a040928](https://github.com/Ziyang-Bai/nKits/tree/a040928502969b3cab393d890e23c9eaba45a30c/TaskManager) | Reuse task enumeration, object naming and status handling where applicable. Review `src/native.c`, `src/tasks.c`, and the host tests first. Keep the read-only path and preserve firmware validation. nKits is GPLv3. Preserve notices and provide corresponding source if distributing a derived binary. |
| [nKits nUSB at a040928](https://github.com/Ziyang-Bai/nKits/tree/a040928502969b3cab393d890e23c9eaba45a30c/nUSB) | Supports calculator-hosted USB flash storage. Reuse ideas such as stage reporting and cancellation. Its diagnostics source also contains write tests, so keep it out of the read-only phone-link workflow. |
| [EndraTermux USB Recovery V2 Build 010](https://github.com/brandonendall/EndraTermux/commit/872d31a39b2200272379190a9c980e1f68d0b176) | Reuse the Android-side link diagnostics and Build 010's direct-path metadata probe, which avoids enumerating the root directory. Keep `push` and the existing NLink install intact. Build 010's device outcome is unconfirmed. |
| [Shared Ndless tooling](../../toolchain/README.md) | Reuse the hub's native build workflow pattern and pin the toolchain used for CI. The nKits source uses Ndless r2023. |

## Hardware evidence and compatibility

- Earlier Android diagnostics reached the TI-Nspire session and file-service connection, then stalled during directory-list activity. The exact failure point and whether any task owns a lock remain unverified.
- The rear reset and removal of the empty calculator-side `/documents/USB` directory did not resolve the busy response. Treat that folder and the observed directory entry position as clues only.
- The pinned Task Manager backend checks for Ndless r2023 or newer and validates firmware signatures. Brandon confirmed the installed revision is **Ndless r2023** on November 19, 2026. On a mismatch, show “Task inspection unavailable” and keep the ordinary file and UI checks available. Keep the guard intact.

## Run001 acceptance criteria

1. Add a foreground screen with a task snapshot and a separate local Documents accessibility result. Use at most one task snapshot at launch and refresh on an explicit keypress. Inspect bounded data and release buffers between snapshots. Show “Unavailable” when data cannot be read safely.
2. Display the exact stage reached by the Android-side diagnostic separately from calculator-local file access. An on-device file open only establishes local accessibility. A successful USB session only establishes that the link reached that stage. Use the existing direct-path probe for one known path and keep observed results distinct from inferred causes.
3. Keep the first release read-only. Avoid resident hooks, repeated idle scans, stress tests, automatic reset/replug loops, task termination, service restarts and system writes. Host tests exercise the report parser and verify that the upstream backend patch removes non-task discovery and mutation and bounds task snapshots. Native firmware compatibility and the unsupported-firmware screen require device verification. Verify native compilation and exit behavior before a device session.
4. Package the checked `.tns` and a short acceptance checklist together. Plan one transfer and one device session once the file service can transfer the build. Leave the previous tools available for rollback.

## Update this file after each meaningful change

Record run number, source path, commit, artifact ID, CI result, physical result, regressions, and the next action. Update the [hub memory index](../../PROJECT_MEMORY.md) so the next session can find the active branch. Follow the [owner's hardware-preservation policy](https://github.com/brandonendall/Nspire-Development-Hub/pull/23) for batched verification.
