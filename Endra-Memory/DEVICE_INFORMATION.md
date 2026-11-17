# Developer device and preservation notes

## Primary Nspire target
- Calculator: **TI-Nspire CX II CAS**, OS reported **6.4.0.74**, Ndless installed.
- OS previously 6.2.0.333. Some Ndless hooks (`nclock.hook`, `nsonic.hook`) and ndless=2022 were observed historically.
- **Do not change system hooks, reset/reinstall Ndless, delete root files, write `ndless.cfg.tns`, or alter the OS as part of ordinary projects.** One past utility hang required a hard reset/reinstall.
- Test a small/throwaway folder and keep rollback TNS builds; battery/storage safety and quick clean exit matter.
- For EndraHome, the permanent launcher config file is `EndraHome_Apps.cfg.tns` (Run010+); never overwrite user's existing configured copy.
- Historical EndraGBA data convention: `UltraGBA_Data/` alongside emulator; keep saved ROM-local files, GBZ compression support and RTC data intact.

## Android / transport
- Phone: Samsung Galaxy A16, Android 16; primary workflow is local Android paths, Termux, OTG USB, GitHub downloads. Avoid requiring desktop.
- USB paths can change after reconnect; `push` should discover device and obtain permission.
- APK signing identity matters: replacing installed packages with debug builds from other signing keys may require uninstall and may lose app data. Never casually recommend uninstalling EndraSpell or EndraClean.
- Avoid publishing unique device diagnostics, logs, credentials or private file contents in public memory documents.

## Other calculator target
- Casio fx-CG50. Older display/OS 3.7 and newer display/OS 3.81.0 have been discussed; compatibility should be checked per binary.
- PythonUltra is the portable MicroPython/pygame development effort for the fx-CG50; consult its roadmap for exact tested build evidence.

## Physical acceptance labels
- **CI PASS**: compiles/tests successfully in GitHub only.
- **USER DEVICE PASS**: user explicitly ran the precise build and confirmed relevant controls.
- **PARTIAL DEVICE PASS**: screenshots or test report prove only limited screens/flows.
- **FAILED DEVICE**: a freeze, error, regression, or mandatory reset was reported.
- **PENDING**: no reliable physical-device validation. Do not upgrade status by inference.
