# EndraHome Run006 — isolated launcher proof (experimental)

Run005 has been **reported working on the TI-Nspire CX II CAS**. The user supplied a photograph of the seven-icon screen; several long captions overlap visually. This Run006 fixes caption spacing and then provides an *opt-in, one-target-only* Ndless application handoff test.

**Run006 has now been user-reported working on physical hardware** (November 14, 2026: “Everything works” after the complete test sequence). It remains a limited, test-only launcher—not a general-purpose launcher and not a verified GBA launcher. Previous Run001 freezes occurred in the original SDL launcher and during GBA launch, so do NOT test the GBA via this version.

## Preserve working baseline

- Run003: first working native GBA-style input/video.
- Run004: user-tested visual upgrade.
- Run005: user-tested 7-icon layout (keep the existing .tns).
- Run006 compiles the **unchanged** GBA-derived `../gba-ui/platform.c` and **unchanged** Run005 `../run005/logic7.c`. Only the captions/overlay + a gated launch path are new.

## Deliverables — must transfer BOTH
1. `EndraHome_Run006_LaunchProbe.tns`: menu + test launcher.
2. `EndraHome_Test006.tns`: tiny disposable program that only displays a success popup.

Place BOTH files in the SAME Documents folder (e.g. `/documents/EndraHome/`). Do not rename either. The launcher builds a sibling path from its own `argv[0]` and refuses root/traversal paths.

## Safety-first test order

1. **Open `EndraHome_Test006.tns` directly** from TI Documents. Verify the popup says SUCCESS and returns after Enter; stop if it freezes.
2. **Open `EndraHome_Run006_LaunchProbe.tns`**, immediately press Esc and confirm a clean return to Documents.
3. Reopen it. Verify all seven icon captions are readable, move between icons, open details and Menu, exit normally.
4. Only after steps 1–3 pass, select **Tools**, Enter (details), Enter (confirmation), Enter (run test). Esc cancels the confirmation.
5. The launcher restores the original LCD/timer/OS screen BEFORE calling `nl_exec(path,0,NULL)`. The test program should display a SUCCESS popup; closing it should return to Documents. If it freezes, stop and report the exact key and screen.

Every other tile stays preview-only. No GBA, no ZIP, no file manager launches yet. This build does **not** write configs, alter Ndless associations or install resident hooks.

**Root-cause note:** Ndless documents specify that when there are no additional arguments, `nl_exec(path,0,NULL)` is correct. Run001 used an incorrect extra `argv` entry as well as a different SDL display lifecycle. This is an architectural difference, *not* proof that it caused the earlier freezes.

## Build
- Host state/path tests: `make -C Projects/EndraHome/run006 test`
- Host previews: `make -C Projects/EndraHome/run006 preview`
- Native Ndless: `bash toolchain/build.sh Projects/EndraHome/run006`
- Workflow: `.github/workflows/endrahome-launch006.yml` outputs both binaries + SHA256 and a separate preview artifact.
- Source graphics use the bitmap font and GPL-covered GBA-derived architecture; no ROMs, user data or proprietary images are included.

Keep Run005 as fallback. Report success/failure separately for **direct target launch**, **Run006 navigation**, and **nested test launch**. A successful GitHub Actions job is NOT a physical device test.

## Hardware test update (2026-11-14)

The user reported **“Everything works”** after receiving the full direct-test, ESC, navigation and guarded Tools launch procedure. This is taken as a user-reported pass of Run006's controlled tiny Ndless companion handoff, with no individual step log. Run007 was created as a separate source directory so the tested Run006 binary remains unchanged.
