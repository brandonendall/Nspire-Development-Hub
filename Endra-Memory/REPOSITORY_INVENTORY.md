# Endra-owned repositories and source locations
_Verified by listing owner account `brandonendall` on 2026-11-17. These are GitHub repos, **not** a list of shipped applications._

| Repository | Scope | Default branch and important distinction |
| --- | --- | --- |
| [Nspire-Development-Hub](https://github.com/brandonendall/Nspire-Development-Hub) | Ndless TI-Nspire programs, shared toolchain, handoffs | `main` includes old Ultra source and project handoffs. Most recent Endra implementations live in **open feature PRs**, not main. |
| [EndraLink](https://github.com/brandonendall/EndraLink) | Android USB transfer/management app, fx-CG50 and prospective Nspire | `main` README may lag experimental versions built in external fork/feature branches. |
| [EndraTermux](https://github.com/brandonendall/EndraTermux) | Termux install scripts, push, Rust NLink source/binaries | `main`, Run004 recovered source/archive; README still says binary absent, while tree contains `bin/aarch64/nlink` and `source/nlink-termux/`. Recheck before advising installation. |
| [EndraClean](https://github.com/brandonendall/EndraClean) | Android user-app cache cleaner | `main` README describes Run039 downloadable APK; historical checked-in sources may differ from latest binary. |
| [EndraSpell](https://github.com/brandonendall/EndraSpell) | Android spelling/worksheet learning | `main` README distinguishes approved Build034 and signed Build041 UI candidate. Some complete source checkpoints/private signing materials are **not** public. |
| [EndraCanasta](https://github.com/brandonendall/EndraCanasta) | Cross-platform Canasta game | Flutter Prototype 0.1. |
| [EndraMath](https://github.com/brandonendall/EndraMath) | Adaptive mathematics learning | Planning/prototype; README states no tested APK. |
| [PythonUltra](https://github.com/brandonendall/PythonUltra) | Casio fx-CG50 MicroPython app | Detailed historical roadmap and independent repo migration from the earlier PythonExtra-CG50 work. |

### Important namespace/identity rules
- `EndraZIP` / `EndraZip` here is the **TI-Nspire Ndless archive manager** (`Projects/EndraZIP/` on feature branches), not an Android APK. Some speech-to-text calls it **Android zip** or **Andrew Zip**. Do not silently treat these as separate repositories or Android apps.
- `EndraHome` is an **Ndless app launcher**, despite a similar sounding Android home concept.
- `EndraTermux` is a **phone-side command-line distribution**. `push` is the shell command, `nlink` is its underlying utility.
- Earlier work under `OffCamera-Civilman/EndraLink` / `OffCamera-Civilman/PythonExtra-CG50` and earlier Ultra names may be useful history, but the owner wants **brandonendall** as authoritative for new development.
- Additional experiments mentioned in conversations (e.g. UltraDex, Ultra Jam, prospective integrated home/image/PDF features) are **not confirmed by the eight-repo inventory**; record them as historical ideas unless actual source is located.
- For every repo inspect tags/releases, all relevant branches/PRs, and current Actions, and do not assume the README equals the newest artifact.
