# EndraPython project memory

## Run004 punctuation key, native TI text entry and Documents, 2026-11-22

Brandon reported that Run003 on his handheld could not access his keyboard popup or enter quotes and brackets as expected, could only browse the default scripts folder, and he wanted the green E removed. Run002 remains his explicitly confirmed working rollback. Source `f83d7f17f3b2c6c8856140573a4ecf13a4546f2c` on feature/endrapython-run001, [draft PR 47](https://github.com/brandonendall/Nspire-Development-Hub/pull/47). [Actions 45059581295](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/45059581295) passed. [Artifact 14768782801](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/45059581295/artifacts/14768782801) includes EndraPython_Run004.tns, 177942 bytes, SHA-256 `0143168af503a64c36ea43ae4f8d8bcbefde85b0e9a39908a42b67b410808603`. Downloaded archive SHA-256 `d754d7d83c487a766f565ee9a759f2c40b01a29edfa11064aaa7c53c662986c6`. The local ARM binary and all eight test screens match the CI results.

The physical ?▸ SDK key now opens a one-press marks picker in the editor, shell, find and naming fields. Enter inserts the selection. The dedicated Catalog key opens a native TI Request field through the SDK's dialog interface; press ?▸ or Ctrl+Catalog inside that field for the OS palette, then confirm. UTF-16 results are converted atomically to UTF-8. Direct Ctrl+parentheses and tap-then-key Ctrl are supported. Files > Menu browses Documents with nested folders, in-place edit/run/save, Esc to parent and Tab paging for over 96 entries. Header is text only.

Tests exercised every picker entry, cancel, key press edges and modifiers, multibyte selection and deletion, Unicode strings through execution and save/reopen, and selecting an external script with the same basename as a home script without changing the home copy. Displayed previews are EndraPython framebuffer captures. Physical TI dialog key routing, available symbol sets in that field, LCD restoration after native entry, on-device external-folder saves and Run004 acceptance still need confirmation. Next verify these before continuing multi-file project imports. Launch the retained Run002 for rollback and preserve scripts.

## Run003 IDE and module discovery, 2026-11-22

Brandon confirmed Run002 successful on the TI-Nspire CX II CAS. Retain Run002 and the existing scripts directory as the working rollback. He requested a fuller IDE and the commands and libraries in his Casio PythonUltra. The TI implementation is separate.

Run003 source `193c2803899dc75c3704b9acf0b26aa0147e154e` on feature/endrapython-run001, [draft PR 47](https://github.com/brandonendall/Nspire-Development-Hub/pull/47). [Actions 45056645849](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/45056645849) passed. [Artifact 14767647166](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/45056645849/artifacts/14767647166) contains EndraPython_Run003.tns, 165163 bytes, SHA-256 `e07749c74a257a9dc2f523047bc56e1a2080efd75f232a2af0f68b006cb86dc3`. Downloaded ZIP SHA-256 `ecf2a73a79bcd8b42deb5056782dff4dde75690bbeefcbea2b647143f8056fc6`. The CI executable matched the independent local ARM build byte for byte. Six framebuffer previews and their contact sheet matched pixel for pixel.

The redesigned workspace, editor, shell and console include selection, multiline clipboard operations, 12-step undo/redo, find-next and traceback line navigation. The shell Menu runs help('modules'). Four new imports (array, cmath, random, struct) bring the inventory to nine. Host checks exercised the bundled dice script, import calls, saves, clipboard/undo around saves, wrapped find and error navigation as well as Run002 regressions.

Physical Run003 acceptance is pending. Check LCD text clarity, modal navigation, modifier keys, existing-file save/reopen and error jumps. After feedback, work toward imports between saved scripts and multi-file projects. README records the reference package inventory and remaining port dependencies.

## Run002 storage fix, 2026-11-22

Brandon photographed Run001 on the CX II CAS with Hello_World.py.tns open, range(5) edited, and “Cannot create scripts folder”. The folder appeared on leaving and reopening. This confirms physical launch and editing; no successful persistence of that edit or execution result was reported.

Run001 unconditionally attempted mkdir on the root and accepted only success or EEXIST. A host reproduction with an existing root and mkdir returning EIO produced the exact photographed message. The calculator's actual errno was not measured. Run002 successfully runs the same edited example (10), reuses accessible directories without mkdir, and checks actual accessibility after first-use creation. Save lock ownership and overwrite protection remain in force. Regression tests cover the root appearing despite a failed create return, duplicate names, a denied create, a same-name blocking file, repeat save, and reopen. The five built-in modules are math, gc, collections, builtins and __main__. All five imports were checked on the compiled host port.

Source `641bdfc5aead129e25437aaf7ddc6dc270e64294` on `feature/endrapython-run001`, [draft PR 47](https://github.com/brandonendall/Nspire-Development-Hub/pull/47). [CI 45055316907](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/45055316907) passed. [Artifact 14767496881](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/45055316907/artifacts/14767496881). EndraPython_Run002.tns, 149421 bytes, SHA-256 `2e111f5ff3adec98da67de8d0d4ee941f3115edc037a0946f94f9c65844c57d1`. Extracted CI binary matched local ARM build byte for byte. All five UI frames matched the local renders. Brandon subsequently confirmed Run002 successful on his calculator. Confirm save/run/reopen on the existing example and retain the scripts folder. Run001 is available as an executable rollback with the known save defect.

## Run001 verified build, 2026-11-22

Branch `feature/endrapython-run001`, [draft PR 47](https://github.com/brandonendall/Nspire-Development-Hub/pull/47), source `ebec0a7effc16c6de24a70ab758492cb0c0b3556`. [Actions 45049865864](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/45049865864) passed host checks, native compilation, ARM/Zehn verification and artifact upload. [Artifact 14764633024](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/45049865864/artifacts/14764633024) contains the executable, checksum, two scripts and five 320×240 framebuffer previews plus a contact sheet.

| Evidence | Value |
| --- | --- |
| EndraPython_Run001.tns | 149,607 bytes, SHA-256 `08fc99875cea0cdf6f44ba6abbf04ed1150fc98a4b2da194428cfadffdccf4c6` |
| Downloaded ZIP | SHA-256 `2dfa7a9fe1957cf81459a3e81dc8e043a368721b0423b3f62ad693d1965244c4` matches the GitHub artifact digest |
| Zehn report | EndraPython 001, compressed, HW-W display supported, 1,078,848 bytes needed to load the image |
| Device | TI-Nspire CX II CAS 6.4.0.74/Ndless. Physical launch, LCD restoration, key mapping, file saves, interruption and timing **pending** |

Local host and CI suites exercised the example output and 45, save/reopen, changed-source execution, two-line syntax error tracebacks, REPL state and history, recovery from MemoryError and KeyboardInterrupt, 24 fresh reruns, ASCII/size/CRLF limits, external edits, failed saves and 100-line scrolling. Five previews matched pixel-for-pixel. The first local binary differed because make enumerated the MicroPython object files in a different order from the Docker build. Re-linking the local ARM objects in the order printed by Actions job 137292378939 reproduced the CI binary byte-for-byte. The executable distributed to Brandon is extracted from CI.

A separate x86-64 host probe measured 500 interpreter start/100-iteration script/stop cycles in 26.1 ms CPU time. With the configured 128 KiB heap, MicroPython reported 129,952 bytes free after initialization and 129,376 bytes after the script. Sampled stack depth at loop polling was 872 bytes. These host values do not measure calculator performance or worst-case stack use.

The README documents the runtime choice, controls, limits and build. WORK_HANDOFF.md contains transfer instructions and physical acceptance. This is the first executable candidate. Roll back by exiting and removing only EndraPython_Run001.tns. Preserve the scripts folder. Next: verify the first-launch and save/run/return flow on Brandon's calculator before expanding editing and clipboard support.
