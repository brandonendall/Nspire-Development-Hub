# EndraTask Run004

Launch-and-exit verification build for the TI-Nspire CX II CAS on OS 6.4.0.74 with Ndless r2023. Run004 uses the pinned nKits Task Manager canvas, menu and shutdown sequence. It displays one action, **Return to TI Documents**. It has no task enumeration, USB requests, diagnostic export or report writes.

## Calculator test

Open Run004 once. When the **EndraTask Run004 | Exit check** screen appears, press Esc once. A pass means the TI Documents screen returns without a restart or freeze. If it restarts or freezes, stop this test. The cause of Run003's restart and the USB Busy error remain unverified. See the [restart notes](../restart/README.md) for the separate host-side USB tracing work.

## Build

Use Ndless SDK revision `e52415fb568e92cd36ed049067b3e9c2727b84af` and nKits revision `a040928502969b3cab393d890e23c9eaba45a30c`. Run `make NKITS=/path/to/nkits host-test all`. The host test verifies that menu selection and Esc follow the same shutdown sequence, that the graphics context is released before refreshing the TI screen, and that setup errors return through the intended paths. Native compilation and packaging verify the ARM executable. The actual screen and restart behavior still require a calculator test.

The nKits display code is GPLv3. The build package contains its license and corresponding source.
