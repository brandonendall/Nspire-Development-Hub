# Endra Memory Hub — master index
_Last curated: 2026-11-17. Owner: brandonendall. This is a public-source, non-sensitive engineering knowledge base._

## Load this first (for ChatGPT, Fantasma, Hydra or collaborators)
1. Read this file **plus** all linked Endra-Memory documents relevant to the user's request.
2. Look up the **live** repository, default branch, open PRs (including drafts), the relevant active feature branch, current README/STATUS, and GitHub Actions. These notes are a **snapshot**, not a substitute for checking current code.
3. Prefer specific user-reported **physical-device results** over simulated tests, and label each separately. Never infer physical success from passing CI.
4. Confirm intended repo/branch and use a draft PR for changes; do not merge, force-update, or modify system hooks without explicit approval.
5. After a meaningful milestone, update this index and project-specific notes with version, branch, commit SHA, workflow run/artifact, CI status, real-device status, regressions, and next action.
6. Keep secrets, credentials, private correspondence, device identifiers, medical/legal/personal data, logs containing sensitive content, user ROMs, and signing keys **out** of this repository.

## What the user expects
- Endra is the unified name for the formerly Ultra-branded calculator utilities.
- Target primary calculator: TI-Nspire CX II CAS, OS 6.4.0.74 with Ndless. Also works on Casio fx-CG50 and Android. Preserve hardware, avoid risky resets/unstable OS modifications.
- User works primarily from an Android phone with Termux/OTG; make instructions copy/paste friendly and start Termux blocks with `clear`.
- **Always use the installed Termux `push` script** for ordinary Nspire uploads; do not substitute a hard-coded `termux-usb` path or `nlink push`. Exact EndraZIP example: `push "/storage/emulated/0/lab/Endra/eZip/EndraZIP_007.tns"`.
- Number outputs consistently (Run001, Run007, etc.); provide real `.tns` and APK downloads when available, with working links, not invented paths.
- Preserve confirmed working builds as rollback; avoid losing features between runs; honor UI appearance/screenshots and user feedback.
- Goal: modular tools that eventually work together, with strong functionality, speed, stability, low memory/CPU use, documented reproducible builds, and community distribution when ready.

## EndraHome build location

The [EndraHome project memory](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endrahome-run001/Projects/EndraHome/PROJECT_MEMORY.md) tracks the latest compiled build and the owner’s installed version. On 2026-11-19 these were Run029 Desktop and Run028 Dynamic Launcher, respectively. Both numbered code directories are on `feature/endrahome-run001`. [Archived builds](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endrahome-run001/Projects/EndraHome/builds).

## Memory modules
- [REPOSITORY_INVENTORY.md](REPOSITORY_INVENTORY.md): verified owner repos, platform, current source-of-truth caveats.
- [NSPIRE_PROJECTS.md](NSPIRE_PROJECTS.md): EndraZIP, EndraHome, EndraGBA, EndraCommander, Endra SDL, EndraDiagnostics.
- [ANDROID_PROJECTS.md](ANDROID_PROJECTS.md): EndraLink, EndraClean, EndraSpell, EndraCanasta, EndraMath; Android-vs-Nspire terminology.
- [TERMUX_COMMANDS.md](TERMUX_COMMANDS.md): user-preferred transfer and EndraTermux.
- [DEVICE_INFORMATION.md](DEVICE_INFORMATION.md): devices, constraints, safe testing.
- [DEVELOPMENT_RULES.md](DEVELOPMENT_RULES.md): PR/build/test/QA procedure.
- [BUILD_HISTORY.md](BUILD_HISTORY.md): version landmarks, latest observed feature branches and notable regressions.
- [FUTURE_FEATURES.md](FUTURE_FEATURES.md): approved aspirations vs built features.
- [KNOWN_BUGS.md](KNOWN_BUGS.md): do-not-regress and unresolved concerns.

## Established repository documents to read next
- [../PROJECT_MEMORY.md](../PROJECT_MEMORY.md), [../AGENTS.md](../AGENTS.md), [../handoff/HYDRA-START-HERE.md](../handoff/HYDRA-START-HERE.md).
- On every session search open PRs in [Nspire-Development-Hub](https://github.com/brandonendall/Nspire-Development-Hub/pulls), including the EndraHome and EndraZIP PR chains.
- Cross-repo inventory is not a claim of write access to every repository or an automatic external memory sync.

## Fast retrieval phrase
When user says **"Load Endra Memory"**, read this hub, then inspect the live relevant repos and feature PRs before coding. This convention works while the GitHub connector is available; the assistant does **not** automatically read an external repo in every chat.
