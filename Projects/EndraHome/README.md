# EndraHome - Run001

A native, dark-themed SDL home launcher for TI-Nspire CX II CAS and Ndless.
This is a standalone program, not an operating system replacement.

Run001 has four dock shortcuts: EndraGBA, Files, ZIP and Apps.
Left/Right or Up/Down selects, Enter or touchpad click launches,
Menu shows configuration help, Esc exits.

Every icon starts disabled. To use the dock, copy shortcuts.example.cfg.tns
to /documents/EndraHome/shortcuts.cfg.tns on your calculator.
Edit that text file on your phone to set each actual .tns executable path.

Example (replace these with real locations before trying):
0=EndraGBA|/documents/EndraGBA/EndraGBA.tns
1=Files|/documents/UltraCommander/UltraCommanderNX_Run001.tns
2=ZIP|/documents/UltraZip/UltraZIP_003.tns
3=Apps|

The launcher accepts only explicit absolute paths inside /documents,
rejects dot-dot traversal, and does not scan folders or modify any
system configuration. It does not install hooks or run in the background.

Build with make in an Ndless SDK environment or run:
bash toolchain/build.sh Projects/EndraHome
The executable is EndraHome_Run001.tns.
Host parser tests: make test.

CAUTION: A GitHub Actions build is not the same as a successful physical
calculator test. First test the screen and Esc exit, then launch ONE known
working Ndless app and confirm it returns without locking the calculator.
OS 6.4.0.74 and Ndless compatibility must be verified on the device.
