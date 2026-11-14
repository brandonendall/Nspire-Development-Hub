# EndraHome — project status (2026-11-14)

Repository: `brandonendall/Nspire-Development-Hub`. Active source branch: `feature/endrahome-run001`, in [draft PR #10](https://github.com/brandonendall/Nspire-Development-Hub/pull/10); main contains the current Hydra handoff.

Hydra / collaborating AI: first read `AGENTS.md` and `handoff/HYDRA-START-HERE.md` at repository root; then read this README and the [device history](DEVICE-TEST-RESULTS.md). All verification assertions below distinguish physical calculator tests from CI builds.

## Run status

| Run | Key change | Hardware observation | Action |
| --- | --- | --- | --- |
| **001** | SDL launcher with `nl_exec` app launch | **FAIL:** hard freeze on Settings and attempted GBA launch | Do NOT use |
| **002** | Ndless `lcd_init/lcd_blit` diagnostic | **FAIL:** froze after Menu, possible exit-path failure | Do NOT use |
| **003** | UltraGBA/gpSP-derived native input/display engine | **PASS reported:** user photo of selected ZIP icon, “Works like a champ” | Keep as known-working baseline |
| **004** | Styled 4-icon Home, unchanged 003 platform/logic | **PASS reported:** photo on device and user says “It works” | Preserve; fails user's target *visual similarity*, not functional test |
| **005** | User-concept-inspired 7 bright icons, large cube brand, seven-item navigation | **PASS reported:** real-device photo + “It works”; tiny icon captions collide | Preserve as last user-tested 7-icon UI |
| **006** | Corrects captions, adds a *guarded single-test-app* handoff | **PENDING device test**; host tests passed, native build tracked in CI | Keep GBA launch disabled; test 2-binary probe only |

## Current experiment — Run006 (guarded handoff)

[Run006 source](run006/) includes a caption fix and a **controlled test of real Ndless app launching**. Only the Tools icon can open a dedicated tiny companion app, `EndraHome_Test006.tns`, after a separate Enter-confirmation screen. All actual apps, including GBA, remain preview-only. The launcher restores the native LCD and timer before calling `nl_exec(path,0,NULL)`, the correct API for zero extra arguments; Run001 previously used incorrect arguments as well as SDL. **The test is experimental and not yet validated on the calculator.** Transfer *both* binaries together and follow [the test sequence](run006/README.md) beginning with the companion app by itself.

The hardware platform and touchpad input are compiled unchanged from Run003; the seven-way navigation is compiled unchanged from Run005. No ROMs, saved data, hooks, or Ndless configuration files are touched.

## Previously tested — Run005

[Run005 source](run005/) and [Run005 CI](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43599465872) (successful build, **not** a physical calculator test). `EndraHome_Run005_PreviewMatch.tns` is uploaded as the Actions artifact.

Visual design directly references the user-supplied original mockup with the large 3D cube, centered `EndraHome` wordmark, muted subtitle, seven colored icon cards, cyan active border, and bottom Enter/Menu pill. It is programmed at the calculator's native **320×240 logical resolution**, so some small typography, anti-aliasing and light effects differ from the enlarged photographic concept. The screenshot is NOT embedded in our source.

Run005 compiles the **unaltered `gba-ui/platform.c`** from working Run003 for all hardware video, touchpad input, timer and return-to-OS logic. It uses a new **7-slot** pure page state handler (`run005/logic7.c`) and 100% software RGB565 renderer (`run005/render.c`).

**Important:** the seven icons are **UI placeholders, not working shortcuts**. Enter shows details; no external Ndless application is executed in Run005. No filesystem I/O, no `ndless.cfg.tns` modifications, no hooks, no SDL. Battery icon is decorative, not a live reading.

### Physical test order
1. Keep **both Run003 and Run004** installed as rollback.
2. Install Run005 separately; test immediate Esc exit to the TI browser first.
3. Relaunch; move across all 7 icons, Menu help, Enter details, Esc back, then clean exit.
4. If hard freeze occurs, stop and report the exact last key and screen.

Only after successfully testing Run005 will actual app launching be developed independently in a new run and verified with a small known-working nonemulator app before GBA.

## Files

- `src/`: failed Run001 SDL launcher, retained only for forensic comparison.
- `recovery/`: failed Run002 diagnostic, retained only for forensic comparison.
- `gba-ui/`: user-confirmed Run003 native platform and navigation baseline. **DO NOT modify in cosmetic changes.**
- `run004/`: last user-confirmed working polished UI with 4 icons.
- `run005/`: new 7-icon closer-to-concept UI, physical test pending.
- [GBA design analysis](GBA-ARCHITECTURE-ANALYSIS.md).
- [Physical test log](DEVICE-TEST-RESULTS.md).

The GitHub README and Hydra handoff are versioned project documentation and **are not automatically synchronized outside actual commits**.
