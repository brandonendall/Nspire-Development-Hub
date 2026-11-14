# EndraHome Run011 — Return Home (experimental)

User confirmed that Run010 launches the small test application immediately and exits without the previous ~10-second delay. Run011 retains the no-refresh video/timer restoration and adds a session loop around the documented Ndless nl_exec(path,0,NULL) call.

When the child returns normally from nl_exec, Run011 reinitializes EndraHome's native GBA-derived framebuffer/input, resets confirmation and launch-request state, and displays Home again, preserving the selected tile. It never relaunches without a new Enter confirmation. This does not intercept resets, crashes, or apps that terminate the parent or never return to the callsite. Not a resident hook or OS replacement. Large emulators remain untested.

CONFIG: EndraHome_Apps.cfg.tns is the permanent UNNUMBERED config, unchanged from Run010. Place it alongside the new launcher. Do not replace your configured existing file with the bundled blank template. Keep Run010 installed as a rollback.

SAFE HARDWARE TEST: (1) Launch Run011 and press ESC to confirm it exits quickly to TI Documents. (2) Relaunch Run011 and use Tools to launch only the previously working EndraHome_Test006.tns. (3) Dismiss the test success dialog. Expected result: EndraHome Home reappears with Tools still selected. (4) Press ESC to exit Home. Stop immediately on frozen, garbled or blank LCD and revert to Run010. No GBA test yet.

Build commands: make -C Projects/EndraHome/run011 test ; make -C Projects/EndraHome/run011 preview ; bash toolchain/build.sh Projects/EndraHome/run011 . Native and device results must be tracked separately.
