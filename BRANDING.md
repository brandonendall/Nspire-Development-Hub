# Endra product family

The owner selected **Endra** as the shared product name on November 13, 2026.
Use Endra in new first-party product names, app titles, executable names,
release packages and current documentation. Keep the product suffix.

| Previous name | Current name | First branded build |
|---|---|---|
| UltraGBA | EndraGBA | `EndraGBA_Run008_Branding.tns` |
| UltraZIP | EndraZIP | `EndraZIP_005.tns` |
| UltraCommander NX / Ultimate Commander | EndraCommander NX | `EndraCommanderNX_Run002.tns` |

EndraLink already follows the family name. Other first-party projects should
use the same Ultra-to-Endra rule when their sources are brought into this hub.

## Compatibility and provenance

The GBA baseline scripts and historical build records keep their original
names. Third-party project names, authors and licenses retain their attribution.
EndraGBA keeps the `UltraGBA_Data` and `UltraGBA_Tests` directories next to the
emulator and per-game saves next to the ROM. EndraCommander NX keeps the
`UltraCommanderNX/settings.cfg.tns` location. Place the new GBA executable in
the existing emulator folder to retain its global settings and custom clock.
The rename does not move, rename or delete calculator files.

Game-file associations depend on the executable filename. See
[EndraGBA setup](Projects/EndraGBA/README.md) for the two matching entries.
Automatic changes to Ndless configuration remain disabled.

## Baselines

- EndraGBA builds from the vendored upstream snapshot and original Run007 patch.
- EndraZIP carries forward UltraZIP 004 controls and level-10 compression from
  commit `7931edca4c1edf4a02226e0e864157efd5c8a58a`.
- EndraCommander NX carries forward UltraCommander NX Run001 functionality.

Host checks and calculator compilation verify the build. Physical calculator
testing is a separate step; branding does not establish that native ROM
dispatch or any previously unresolved hardware behavior has been repaired.
