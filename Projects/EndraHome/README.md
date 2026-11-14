# EndraHome — development status

**WARNING — Run001 is NOT safe for further calculator testing.**
On November 14, 2026, the TI-Nspire CX II CAS repeatedly froze while Run001 was active: once on its shortcut settings screen, and once immediately after attempting to launch a GBA app through it. A hardware reset was required each time. **Do not relaunch Run001** even though its GitHub Actions compilation and host tests passed.

See [device test results](DEVICE-TEST-RESULTS.md) for observed behavior and open hypotheses.

## Recommended next build: Run002 Safe diagnostic

The new separate diagnostic build is at [recovery/](recovery/). It uses Ndless native framebuffer drawing (no SDL), provides arrows/Menu/ESC input tests, and **does not launch any applications** or access files. It exits automatically after approximately 30 seconds if its loop remains responsive; auto-exit is not a hardware watchdog.

GitHub Actions workflow: `.github/workflows/endrahome-safe002.yml`.
Binary artifact: `EndraHome_Run002_Safe.tns`.

## Original Run001 prototype (archived for diagnosis)

Run001 attempted a 320x240 SDL/nSDL home screen with four configurable icons: EndraGBA, Files, ZIP and Apps. It used `nl_exec` to launch selected apps from `/documents`, and used direct Ndless keyboard polling inside an SDL-based UI.

Its source and CI are retained for diagnosis **but it is not approved for use on the physical calculator** until the root cause has been isolated and fixed. Do not configure or install the Run001 launcher again.
