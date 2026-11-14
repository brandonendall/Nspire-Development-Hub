# EndraDiagnostics Run001

Experimental native Ndless app for TI-Nspire CX II CAS OS 6.4.0.74.

- 1: prints CPU *reference facts* from prior CPUID photo, not a live probe.
- 2: tests only a 256 KiB malloc region.
- 3: runs a loop benchmark, not clock-frequency detection.
- 0: exits.

**Risk:** This application has NOT been tested on physical hardware. Test cautiously with a recovery path available. No changes are made to Ndless configuration, hooks, OS, ROMs, or flash.

Build: `make -C Projects/EndraDiagnostics/run001` in Ndless SDK. CI uploads `EndraDiagnostics_Run001.tns`.
