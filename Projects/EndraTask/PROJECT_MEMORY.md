# EndraTask project memory

Updated November 19, 2026. Start here when resuming EndraTask in the Nspire Development Hub. Active branch `feature/endratask-run001`. Latest build: **Run002**. Read the build history and device checklist below.

## Build history

| Build | Source and verification | Download |
| --- | --- | --- |
| Run002 | [Source and device checklist](run002/README.md) at `2f228cd4acdd4c3c5ea18098b3cd4e43da4e8801`. Host tests and the native ARM build passed with the pinned Ndless r2023 SDK. Rebuilding from the packaged source reproduced the delivered TNS byte for byte. [GitHub Actions run 44605910040](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44605910040) passed at `125b53f5967dc0ea15e77c018af2f69d1b2cddcf` and published [artifact 14569718743](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44605910040/artifacts/14569718743). SHA-256 `242d20e1a6e80e10930b06f4afba5edebb0a65423b0bc658da2cddb4bc8d9dd8`. Calculator launch, task refresh, export and retrieval are pending. | [Run002 TNS](https://chatgpt.com/library?file_id=libfile_bf771b1640a48191917ac62f90966b39&account_id=personal) · [TNS, source and checksums](https://chatgpt.com/library?file_id=libfile_3c54fa22ea6c8191a73b3e6eedea3c3f&account_id=personal) |
| Run001 | [Source](run001/README.md) at `38eca6486d05c3b2472c5d5e51d1036d0e7eb8ff`. Host tests and native ARM compilation passed off-device. No physical test recorded. | [Run001 TNS](https://chatgpt.com/library?file_id=libfile_77b60499e66081918b39456fd935713e&account_id=personal) |

Run002 adds a per-snapshot task ID, task state, priority, stack allocation and manual report export to `/documents/EndraTask_Run002_Report.txt.tns`. The task ID is a transient Nucleus object address. CPU utilization, total task memory, open handles and lock-owner attribution are unavailable. Local and remote termination remain proposals. Preserve the Run001 binary as a read-only baseline.

The November 19 EndraTask/EndraTermux handoff reports that Smart Push Run011 transferred a file with `nlink-v2` to a custom `/upload` destination after one “No such device” failure. Treat that as one successful transfer, with reliability and the cause of the initial failure still unverified. Use normal EndraTermux file retrieval for the Run002 export. Preserve the working transport.

## Goal

Build a lightweight task and file-service diagnostics utility for Brandon's TI-Nspire CX II CAS on OS 6.4.0.74. Gather useful evidence about the recurring USB “file system busy” condition without repeatedly transferring builds, stressing the calculator, or modifying its system state. Batch related features into one release and do as much verification as possible off-device.

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
