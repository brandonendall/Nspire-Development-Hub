# EndraTask restart

The original mission is a lightweight on-calculator process and USB/file-service diagnostic that complements EndraTermux and EndraLink. The primary question is why the normal Documents transfer or enumeration fails. Attribute a failure to a specific operation and owner only when the evidence supports it. A confirmed, user-selected cancellation or termination is a later capability and requires a supported operation, positive target identity and protection for essential tasks.

## Retired calculator builds

Run002 froze when Enter confirmed report export. In Run003, the photo shows that the calculator answered local metadata requests for Documents and Documents/upload. Brandon reported that pressing Back to exit restarted the calculator. The exact key and reset mechanism are unverified. Keep both runs for history and stop device testing of those builds.

## nKits baseline

Use the proven-on-device nKits package as the comparison baseline. The inspected upload contains `nkits/taskmanager.tns`. The pinned upstream source is [nKits a040928](https://github.com/Ziyang-Bai/nKits/tree/a040928502969b3cab393d890e23c9eaba45a30c/TaskManager). Its Task Manager validates the firmware dispatch signatures before reading task objects, uses the stock menu and restores the OS display on exit. It exposes tasks, pools, events and queues. Those records do not associate a path or USB request with a filesystem lock owner. Run003 and nKits both use `refresh_osscr()`, so the reset cannot be attributed to that call alone.

Start the next calculator prototype from nKits' lifecycle. Preserve the guards and test clean launch and exit before device-side diagnostics are added. Make snapshots manual and use validated task metadata only as supporting evidence. Keep file writes, persistent hooks, process control and guessed lock ownership out of this phase.

## Instrument the host first

The [Build 010 transport](https://github.com/brandonendall/EndraTermux/tree/872d31a39b2200272379190a9c980e1f68d0b176/source/nlink-termux) has a generic `Busy` return in the CX II transport for incomplete handshake, failed bulk write or missing stream acknowledgment. It also returns `Busy` if its local service handle is already marked connected. An unsuccessful service disconnect leaves that local flag set. These cases can look like a filesystem lock from the client. Actor and root cause for Brandon's original incident remain unknown.

The [tracing patch](usb-busy-origin.patch) adds distinct `[ENDRATASK USB]` markers and raw libusb results without changing timeouts, retries, return values, request bytes or hardware controls. Existing Build 010 directory diagnostics provide the enumeration stage and entry number. The Run012 success on `/upload` demonstrates one working transfer; a successful upload and a failing Documents-root enumeration exercise different operations.

After packaging an isolated Android diagnostic, compare one read-only USB Documents-root enumeration with the matching read-only operation against the working Upload folder, in separate sessions. Record the exact library stage where the first failure occurs. Keep the installed transport and application builds as the controls. Stop after the first failure in each diagnostic session and retain the trace. A verified firmware observation is needed before designing the on-calculator owner view.

## Host verification

Apply `usb-busy-origin.patch` to a clean EndraTermux checkout at commit `872d31a39b2200272379190a9c980e1f68d0b176`. With the libusb 1.0.29 headers installed, the two checked-in tests can be compiled on a Linux host. The tests supply their own USB and service stubs. They assert that the return codes, retry counts and cleanup semantics are preserved while distinct trace markers are emitted.

```bash
src=source/nlink-termux/libnspire-sys-debug/libnspire/src
tests=/absolute/path/to/Projects/EndraTask/restart
usb_headers=/absolute/path/to/libusb/libusb
c++ -std=c++11 -I "$usb_headers" -I "$src" "$tests/test_usb_busy.cpp" "$src/cx2.cpp" -o /tmp/endratask-test-usb
cc -std=gnu11 -I "$usb_headers" -I "$src" "$tests/test_service_busy.c" "$src/service.c" -o /tmp/endratask-test-service
/tmp/endratask-test-usb
/tmp/endratask-test-service
```

Both Linux host tests and trace checks passed on November 19, 2026. Android packaging, USB-connected execution and nKits-based EndraTask calculator testing are pending.
