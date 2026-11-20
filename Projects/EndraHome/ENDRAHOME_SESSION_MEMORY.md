# EndraHome verified session memory — 2026-11-20

## Mandatory pre-build procedure
1. Read this file and applicable repository project memory/roadmap, existing source, and last successful build logs before making changes.
2. Preserve verified user instructions over older ChatGPT summaries. If commands conflict, inspect the actual script and ask only if necessary.
3. Build with GitHub Actions; proactively inspect run results, fetch the artifact, verify .tns Zehn header/checksum, deliver direct .tns. Do not make user find run IDs unless tools genuinely cannot retrieve them.
4. Never claim calculator runtime behavior tested until user confirms it. Preserve hardware: no unnecessary resets, destructive file operations, or repeated hardware experiments.

## Verified transfer command (CURRENT; user confirmed 2026-11-20)
Termux: `clear` then `push /storage/emulated/0/Download/<file>.tns /upload`.
The trailing `/upload` argument is REQUIRED for this user's current workflow and has worked for days. Do NOT omit it on the basis of an older memory summary. Do not confuse with nlink's own upload subcommand or historical scripts that targeted calculator root (/). Confirm the local source path for each build.

## Graphics milestone
EndraHome Run031 Graphics Demo, GitHub Actions run #3, run ID 44804296431, artifact EndraHome_Run031_GraphicsDemo (artifact ID 14662028363). Native .tns compiled and verified by workflow. User's calculator photo confirms the selected electric-blue EndraHome artwork displays successfully on TI-Nspire CX II CAS. Standalone read-only demo; do not mistake it for completed interactive launcher.

## UI direction — binding
- Use EndraZIP file browser and pop-up menu as the immediate visual/interaction reference. User loves its styling, menu, spacing and file browsing; EndraHome Files should resemble it closely, with EndraHome-specific icon/image tweaks and a much more extensive contextual menu.
- EndraZIP uses native Ndless C RGB565 framebuffer rendering, lcd_blit and custom bitmap fonts (not SDL), per previous project discussion. Verify actual code before porting.
- Prior approved visual direction: dark charcoal background, cyan highlight/selection, gold folder icons, green ZIP icons, white file icons, right-aligned file sizes, polished pop-up menus. Keep performance lightweight.
- EndraHome main launcher should show only user-selected apps, adaptively arranged 1–7 uniform icons; don't force fixed GBA/ZIP/terminal/files dock icons. Enter launches directly; Menu offers details/config.
- File browser is default picker and full file manager; roadmap includes copy/paste, move, rename, delete, text/image/PDF viewers, and configurable alternate apps. Avoid duplicative pages or unnecessary status counters.
- Prioritize polishing EndraHome before Endra Packages; future app bundles with icons remain roadmap, not a reason to delay core UI.

## Repository / workflow
Repository brandonendall/Nspire-Development-Hub; current feature branch feature/endrahome-run001. Graphics demo source Projects/EndraHome/run029; workflow .github/workflows/endrahome-graphics-demo.yml.
Keep a dated, explicit distinction between planned, source committed, CI passed, downloaded/verified, and hardware tested. Never claim a background build is underway without triggering/checking it.
