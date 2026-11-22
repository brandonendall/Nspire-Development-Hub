# EndraPython project memory

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
