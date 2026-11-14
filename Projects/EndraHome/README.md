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
| **006** | Corrects captions, adds guarded tiny Ndless test-program handoff | **PASS user report:** “Everything works” after full Run006 direct-launch, navigation and nested-launch test sequence | Preserve as latest reported working handoff |
| **007** | Same input/graphics; adds opt-in **one real ZIP app** handoff (plus Tools regression) | **PENDING physical test**; [CI workflow](https://github.com/brandonendall/Nspire-Development-Hub/actions/workflows/endrahome-zip007.yml) | Test ZIP only after direct ZIP and Tools tests succeed |

## Current experiment — Run007 (one real ZIP application)

[Run007 source](run007/) tests one additional application using the exact Run006 handoff ordering after the user reported all Run006 testing working. All other icons except Tools remain preview-only. Specifically, EndraGBA does **not** launch. To try ZIP, place a **copy** of a ZIP program that already works when opened directly from TI Documents next to the Run007 launcher, named exactly `EndraHome_ZIP007.tns`. See [run007/README.md](run007/README.md) for the cautious step-by-step process. Do not test the ZIP app on irreplaceable files.

Run007 is a **new device experiment, not a verified hardware release**. Its source compiles the unmodified Run003 input/display platform and Run005 navigation, and keeps Run006's corrected captions. Run006 is preserved.

## Run006 — user-reported working launcher handoff

[Run006 source](run006/) includes a caption fix and a **controlled test of real Ndless app launching**. Only the Tools icon can open a dedicated tiny companion app, `EndraHome_Test006.tns`, after a separate Enter-confirmation screen. All actual apps, including GBA, remain preview-only. The launcher restores the native LCD and timer before calling `nl_exec(path,0,NULL)`, the correct API for zero extra arguments; Run001 previously used incorrect arguments as well as SDL. **User reported on November 14, 2026: “Everything works” following the complete Run006 test sequence.** This includes a general confirmation of the guarded launcher test, though no detailed per-key log was supplied. Transfer *both* binaries together and follow [the test sequence](run006/README.md) beginning with the companion app by itself.

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

Run006 completed the first reported successful tiny companion-program handoff. Run007 tests one independently working ZIP application. EndraGBA remains disabled until independent testing warrants its own isolated run.

## Files

- `src/`: failed Run001 SDL launcher, retained only for forensic comparison.
- `recovery/`: failed Run002 diagnostic, retained only for forensic comparison.
- `gba-ui/`: user-confirmed Run003 native platform and navigation baseline. **DO NOT modify in cosmetic changes.**
- `run004/`: last user-confirmed working polished UI with 4 icons.
- `run005/`: user-reported working seven-icon UI.
- `run006/`: user-reported working guarded tiny-program launch, safe rollback.
- `run007/`: untested experiment launching one explicitly supplied existing ZIP .tns.
- [GBA design analysis](GBA-ARCHITECTURE-ANALYSIS.md).
- [Physical test log](DEVICE-TEST-RESULTS.md).

The GitHub README and Hydra handoff are versioned project documentation and **are not automatically synchronized outside actual commits**.
