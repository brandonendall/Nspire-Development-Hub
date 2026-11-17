# EndraHome Run020 — Fullscreen canvas, original photo aspect ratio

User clarified: no image stretching or cover crop. The photo starts fully visible, centered, preserving aspect ratio, with unused canvas black and NO title/footer bars. Zoom magnifies from fit; arrows pan once image exceeds screen dimensions. ESC returns to Files. Keep Run019 fallback. Physical test pending.

Run024: Reclaims unused launcher space, draws type-specific file icons, shows file sizes on the right. Folder sizes are displayed as -- until an on-demand bounded calculation is implemented; no misleading directory totals. Run023 retained unchanged.

Run025 adds native Copy, Cut, Paste, Rename, New Folder, recursive Delete and Properties for files and folders. Folder copy streams 8KiB at a time with depth/item limits and no overwrite; host test streams a 33 MiB file. Confirm deletion; ESC cancels menu without losing clipboard. Folder sizes remain --.\n