# EndraHome Run029 Desktop

Run029 builds on the installed Run028 Dynamic Launcher for TI-Nspire CX II CAS with Ndless. Its executable is `EndraHome_Run029_Desktop.tns`. Keep the executable alongside the existing `EndraHome_Apps.cfg.tns` to retain app assignments. Starting Run029 with no configuration shows Files.

## Desktop and controls

| Control | Action |
| --- | --- |
| Left or Right | Move to the neighboring tile in the same row. |
| Up or Down | Move to the nearest column in the row above or below. At the edge, the selection stays in place. |
| Enter on Files | Open the assigned external file manager, or built-in Endra Files when the slot is empty or the loader cannot open the app. A failed launch keeps the saved assignment. |
| Menu | Manage all seven shortcut slots, including empty ones. Select Files with Left/Right and use Assign / Change App to choose its file manager. Use Built-in Files clears this assignment. |
| Menu → Settings | Use Up/Down to choose Interface, Folders, Text or Restore defaults. Use Left/Right to preview each color. Enter saves the selections, or applies Restore defaults when selected. Esc cancels a preview. |
| Ctrl+On | Exit EndraHome through its normal display and timer cleanup from any launcher screen. To put the calculator in standby, use Ctrl+On again in TI OS. |

Files stays visible on the desktop. Every desktop tile, including Files, uses the navy-and-cyan EndraHome E icon. The blue E/cube beside the title uses the same mark. Six visible tiles form two centered rows of three. In the file browser, folders use the selected folder color and executable entries use the E icon. Full-width 8×8 captions wrap on the desktop. Long browser names end in an ellipsis before the size column. The same text color applies to all launcher-controlled screens. Status messages clear after three seconds. A clipboard indicator stays visible while an item is held.

Appearance saves separately in `EndraHome_Appearance.cfg.tns`. Run029 starts from the legacy interface theme if no appearance settings have been saved. Saving colors or restoring defaults does not write to the shortcut file. The launcher writes appearance preferences only on an explicit save.

## TI Home and standby

The CX II guide documents Home → 2 (Browse) and the OS Ctrl+On power shortcut. The documented Ndless interfaces provide key input, program launch and file associations. We found no supported API to add a ninth item to the native TI Settings menu or enter standby safely from this renderer on OS 6.4.0.74. The opt-in quick route is Home → 2 (Browse) → your EndraHome folder → Run029. As a personal shortcut, place the launcher and its existing configuration together in an easy-to-find Documents folder. [TI CX II handheld guide](https://education.ti.com/-/media/8E397FE4BC834CB988CD75EB8B2F7402) · [Ndless SDK](https://github.com/ndless-nspire/Ndless/blob/master/ndless-sdk/include/libndls.h) · [Ndless app interfaces](https://www.hackspire.org/Ndless_features_and_limitations/).

## Verification

From this directory, `make test preview` runs host tests and renders the launcher screens. The Run029 GitHub Actions workflow also invokes the repository’s pinned Ndless toolchain, checks the Zehn header and executable checksum, and uploads the native binary. Host tests exercise all 128 shortcut masks, Files fallback and saved assignments, settings persistence and interrupted writes, text colors, captions, keyboard navigation, timed messages, image decoding, clipboard operations and a 33 MiB folder copy. The loader return codes come from the [Ndless loader](https://github.com/ndless-nspire/Ndless/blob/master/ndless/src/resources/ploaderhook.c).

Physical Run029 verification is pending. When available, first open the launcher and use Esc to return to TI Documents. Then check Files, a saved text-color change after reopening, and navigation with your existing shortcuts. Run028 is preserved as the installed rollback. [Project memory](../PROJECT_MEMORY.md).

## Run029 universal badge build checkpoint (2026-11-19)

The desktop renderer now draws one identical cyan E badge for every configured application and the permanent Files tile, with a matching E badge in the upper-left header. The existing Run029 application logic, preferences, and navigation are unchanged. This source-path commit is intended to trigger the `EndraHome Run029 - Desktop` GitHub Actions push workflow (host tests followed by native Ndless build). Do not label the executable verified until CI succeeds. Keep Run028 as the working rollback.
