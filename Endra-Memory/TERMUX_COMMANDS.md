# Termux / Endra Push — owner-preferred usage
_Last checked against `brandonendall/EndraTermux` README and `scripts/install-push.sh`: 2026-11-17._

## Default upload command — USE THIS
```bash
clear
push "/storage/emulated/0/lab/Endra/eZip/EndraZIP_007.tns"
```
- The example file path was supplied by the user. Replace only the filename/path for a new build, keeping `clear` first.
- **`push` is a Bash wrapper**, installed in `$PREFIX/bin/push`; it is NOT an `nlink` subcommand. Do not instruct `nlink push`.
- The wrapper stages source files in `$HOME/nlink-termux`, scans current USB devices via Termux:API, probes candidate devices, and calls `nlink upload` to the calculator root. **Do not hardcode /dev/bus/usb/001/002** in everyday commands; path may change.
- `push` without arguments can prompt for a file path (if installed), but use the direct absolute path when user supplies one.
- Keep the user workflow phone-only; avoid requiring a PC for routine transfers.

## Source/installation
Repository: https://github.com/brandonendall/EndraTermux
```bash
clear
pkg update -y
pkg install -y git python termux-api
git clone https://github.com/brandonendall/EndraTermux.git
cd EndraTermux
bash install.sh
```
Needs Termux + matching Termux:API; actual installation depends on current source, appropriate ARM64 binary and permissions. The GitHub tree presently includes `bin/push`, `bin/aarch64/nlink`, `source/nlink-termux`, `EndraTermux_Run004_Source_And_Binary.zip`, and scripts, whereas the README has older "not yet published" wording: **reconcile before claiming full out-of-box install is verified**.

## Advanced troubleshooting only
```bash
clear
termux-usb -l
command -v push
command -v nlink
nlink --help
```
User previously confirmed that Termux USB file-descriptor form `termux-usb -r -E -e 'nlink <subcommand> <args>' /dev/bus/usb/001/002` works when device path is current and Android grants permission. Treat this as low-level diagnostic fallback, **not** the normal copy/paste upload answer.

## Transfer/data caution
- Ndless files end `.tns`; plain text guides/configs on calculator often end `.txt.tns` or `.cfg.tns`.
- Do not overwrite working calculator files or relocate data trees without user consent.
- The Rust utility historically included `upload`, `download`, `ls`, `copy`, `move`, `mkdir`, `rmdir`, `upload-os`; check the installed binary's actual help for current commands.
