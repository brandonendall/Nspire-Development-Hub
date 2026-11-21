# EndraHome Run042

> **Run042, 2026-11-21 ET.** EndraHome adds a Files editor for existing plain-text TXT, Markdown and CSV files up to 4 KiB, including eligible .tns-wrapped names. Source `2527598a96b71d0e5e2945491282cdb40ac288a9` on `feature/endrahome-run001`. [Actions 44881971818](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44881971818) passed host and native tests/build. Native artifact **14698546393**, `EndraHome_Run042_Desktop.tns`, **445,170 bytes**, Zehn, SHA-256 `22a4adbbd70f71bd9cefb378ff95ed3130b5fa45dc736c9293699457baeb6746`. Preview artifact **14698122485**, 60 verified frames. Run042 calculator testing pending. Run041 remains the reported-working rollback, including JPEG viewing, zoom and trackpad pan. Transfer: `clear` then `push /storage/emulated/0/Download/EndraHome_Run042_Desktop.tns /upload`.

## Scope and verification

Selected the next functional priority recorded in Run041 notes: integrated plain-text editing. In Files, open a text file, enter the preview, then press Enter to edit. Use arrows and the physical keyboard, Enter for a newline, Menu to save, and Esc to leave. Esc prompts before discarding unsaved edits. Binary, truncated, non-ASCII, and system configuration files remain outside the editor. Existing JPEG and PNG viewer code was preserved. Native Settings registration and genuine standby are separate open work.

Save checks the original bytes, writes and validates a sibling stage, preserves a backup through installation and verification, and retains recovery files if rollback is needed. The host tests exercise the real Files entry path, CRLF preservation, discard/cancel, existing backup protection, external edits, successful writes, and the 4 KiB cap. The JPEG/PNG, shortcut, settings and file operations host regressions passed. Both artifact ZIP digests and the Zehn binary SHA-256 were verified. All 60 real 320×240 CI frames matched local renders byte for byte, and the new editor/confirmation screens were visually inspected. No Run042 handheld test yet.

Run041 rollback SHA-256 verified again: `cb352abafc67cf58e2551b5683fc9ff5c4dace1c0e9e5e580eec8ad05ef3ff50`.
