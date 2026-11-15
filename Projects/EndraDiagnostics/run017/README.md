# Endra System Monitor Run017 — Native Gauge Dashboard

The owner specifically chose a modern tachometer-style CPU/RAM/battery/storage dashboard preview and requested an ACTUAL native Ndless build. This run upgrades the genuine 320x240 framebuffer UI, not just a new image.

### Main dashboard
- Four 270-degree segmented circular gauges drawn in software with fixed-point trigonometry precomputed offline: **CPU load, RAM used, battery level, storage used**. The outer ring is inactive for missing measurements, with **N/A** in its center. No fictional 24% CPU, 61% RAM or storage percentage from the concept artwork.
- The only live percentage currently validated on TI-Nspire CX II CAS OS 6.4.0.74 is coarse OS **battery percentage** from existing read-only OS resolver. Battery gauge colors accordingly. The CPU identifier is actual CP15 read and shown below its unavailable utilization gauge.
- Clean dark theme, cyan card borders, pixel-safe 8x8 text. All drawing uses the **same known-working Run016 framebuffer** with a bounded Bresenham dial renderer. No hardware screen mode, timing or register changes.

### Five detail pages
1. Overview with four truthful gauges.
2. Processor identity/revision/cache state and explicit unavailability of CPU utilization/MHz.
3. RAM/MMU with optional owner-triggered 4 KiB malloc-owned two-pattern integrity test, not a free-RAM calculation.
4. Documents scanner: owner-triggered read-only, bounded 384 directory entries, 48 folders, depth 4, 48 optional read-only seek-to-end checks. **Fixes untrustworthy `%llu` rendering:** previous Run016 showed `KNOWN FILE BYTES: 0` but its overview showed 84,468 KB from the same 64-bit accumulator. Run017 converts known bytes to KB then formats via 32-bit `%lu`. It labels `SIZED` and `UNKNOWN` counts rather than total disk usage.
5. Battery gauge bar, charge state, coarse sample count.
6. Read-only battery resolver telemetry, simple min/max/mean and explicit unavailable storage free.

### Safety and validity
- Run016 hardware platform source is copied **byte-identical**; fast Esc/Enter exit and no automatic exit are preserved.
- BatteryNative resolver and CP15 reads unchanged. No new MMIO, charger writes, CPU stress or arbitrary RAM probing.
- Filesystem scan stays strictly optional, read-only and resource-limited. Its size calls can still be unsupported by the virtual Documents filesystem.
- Host checks validate ring geometry, percentage range, unavailable behavior, format no longer uses `%llu`, page count and preserved platform.
- No file uploads required beyond the ONE Run017 TNS. Run016 is retained as last photographed fallback. Run017 has not been physically tested.

### Single-session acceptance
Upload Run017 once, disconnect cable. The main view should render 4 circular gauges: CPU=N/A, RAM=N/A, Battery=current OS % (once discovered), Storage=N/A. Navigate CPU/RAM/Battery, optionally run the bounded Documents scan on page 4 and look for `KNOWN DOCUMENTS: ... KB`. Esc/Enter fast exit remains unchanged; user reports only failures.

Do not interpret colorful draft mockup CPU/RAM/storage percentages as real readings.
