# EndraHome Run036 — selected Hero Launcher + Neon File Browser
Updated 2026-11-20

## Owner-approved visual choices
User inspected four image concepts and explicitly selected **EndraHome Hero Launcher icons** plus **Neon File Browser**. This supersedes the plain Run035 boxed desktop and solid browser. Keep the exact original physically verified 320×240 Run031 RGB565 electric-blue branding as wallpaper; its upper illustrated metallic E and "EndraHome / TI-Nspire Edition" banner must remain visible and no new desktop header or bottom hints should be drawn.

On desktop, remove **all full app cards** and filled tile backgrounds. Draw only a refined floating metallic/ice-blue **3D angular E**, with wallpaper visible inside and surrounding the icon. Label sits beneath each. Highlight **only the selected name** by a subtle dark glass pill and cyan underline, not the whole icon/tile. Preserve configured application count, Files-only default with no Add App placeholder, standard arrow navigation and Menu assignments.

Files Browser should closely reflect the **Neon File Browser** concept: dark high-readability translucent file-list panel, cyan name/size columns and path band, type-colored file icons, compact floating cyan-framed File Actions with eight existing commands plus small symbols, readable selected-row glow and narrow lower legend (legend only inside Files, NOT desktop). None of the redesign implies every operation is hardware tested.

## Risk limits
Implement purely in current native RGB565 CPU framebuffer: `render.c`, `render.h`, `picker_render.c`, `files_render.c`, host visual tests and build metadata. Do not edit `main.c`, `platform.c`, `config_save.c`, `registry.c`, keyboard hardware handling, OS hooks, power registers or filesystem operation code. Do not change existing user configs. Preserve Run034 and Run035 as rollback. Preview + host tests + native CI before a SINGLE deliberate physical test.

## Second user request — native TI Settings
Owner reiterated Home > 5 > 9 (or next available slot) **EndraHome** direct launcher, like existing nKits ThemeEditor. Investigated live upstream `Ziyang-Bai/nKits` ThemeEditor menu injection and `tornadus/nTheme`. Both rewrite or redirect a native OS Settings menu pointer and require resident callbacks; coexistence is not proven. This is **NOT completed in Run036**; see `NATIVE_SETTINGS_SHORTCUT_RESEARCH.md` for verified technical constraints and safe follow-up. No resident OS writes in Run036. Ctrl+Home true OS standby still pending.

## Build status
Run036 source work not yet CI verified as of this text. After CI record commit, run ID, artifact SHA256 and actual physical test separately. Verified transfer syntax: `clear` then `push /storage/emulated/0/Download/EndraHome_Run036_Desktop.tns /upload`.
