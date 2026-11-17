# EndraHome — Endra Packages Roadmap

Status: **Future feature / design proposal** (not implemented).
Recorded: 2026-11-17.

## Vision
Give every Endra application a recognizable custom icon in **EndraHome**, similar to the icons bundled with Casio fx-CG50 G3A applications. Preserve EndraHome's existing dark gray/cyan visual identity, fast launch and low memory footprint.

## Important platform distinction
TI-Nspire's native Documents browser does **not** support custom Ndless `.tns` launcher icons in the same way Casio's G3A menu does. EndraHome can render custom icons **inside its own launcher**; this roadmap does not promise native Documents icons.

## Proposed application package (illustrative, subject to design)
```text
EndraZIP/
├── EndraZIP.tns
├── icon.png
└── app.cfg
```
Manifest metadata should eventually include application name, version, executable path, icon path, and optional category/description.

## Planned milestones
1. **Icon specification** — Define supported PNG format, transparency, dimensions (e.g. 32×32 or 48×48), safe fallback icon, and memory/size budgets.
2. **Manifest parser** — Read validated, bounded `app.cfg` metadata. Reject malformed paths, oversized metadata and unsafe traversal.
3. **Icon rendering** — Display per-app icons in EndraHome, preserve the current color palette, cache optimized decoded icons, and never repeatedly decode full-size PNGs per frame.
4. **Application discovery** — Scan a designated Endra apps folder within strict item/depth limits and populate the adaptive launcher without requiring a new EndraHome build.
5. **Package installation** — Add an optional, reversible installer with previews, collision handling, validation, rollback/recovery, and explicit confirmation before replacing files.
6. **Ecosystem rollout** — Provide icons/manifests for EndraGBA, EndraZIP, EndraDiagnostics, EndraCmd and future Endra apps.

## Acceptance criteria
- Installing a recognized package adds a correctly labeled, custom-icon launcher entry.
- Two, four, five, or seven installed apps center and space correctly; no stale icon gaps.
- Missing/corrupt icon or manifest falls back safely; no crash or hard reset.
- Launch and return-home remain fast; no repeated PNG decode on each redraw.
- Existing manually assigned shortcuts still work; no forced migration.
- Native TI-Nspire Documents browser behavior remains unchanged.

## Priority
After Run026 file-management stability and adaptive layout are verified on device. Build incrementally with host tests and physical-device validation before shipping package installation.
