# EndraHome Run047 Desktop

Files > Menu > New File creates a named file in the current folder. A plain name like Diary becomes Diary.txt.tns and opens in the editor; press Menu to save. Create ZIP packages the selected file or entire folder beside it as .zip.tns. Enter on a .zip or .zip.tns browses its folders and file sizes without extraction. Extract ZIP writes the contents into a new sibling folder ending in _unzipped and refuses an existing destination.

Menu > Appearance > Icon color previews eight independent icon colors for any app icon shape. Match theme follows the interface color. Enter saves the choice.

Use Menu > Appearance > Set clock. Left/right selects hour, minute or second. Up/down adjusts it, including AM/PM when the hour crosses noon or midnight. Enter synchronizes the selected time to the current RTC second and saves the EndraHome time offset in the shortcut configuration. Esc cancels.

The desktop clock shows hours, minutes, seconds and AM/PM in pure white with a dark glyph outline. Low-contrast labels use a glyph shadow. Rename Shortcut uses an inline caret. Open Endra Files is the last desktop menu entry.

In Files, open a plain-text TXT, Markdown or CSV file up to 4 KiB. From its
text preview press Enter to edit. Use the arrows and QWERTY keys to edit,
Enter for a newline, Menu to save, and Esc to leave. Esc prompts before
discarding unsaved edits. Legacy previews remain available for other text formats.

Use **Menu > Add App** from any desktop tile. Select an Ndless executable and
the new shortcut appears selected on the desktop. To change a shortcut, select
it and use **Menu > Reassign Selected**. Files stays built-in and opens at
`/documents`. Old Files links remain in the configuration for rollback.
A missing or non-app target is reported before leaving the desktop.

Open **Menu > Appearance**. Use Up/Down to choose a setting and Left/Right to preview.
Wallpaper, App Icon, Interface, Folders and Text are independent. The preview shows
the selected wallpaper and a native 48×48 launcher icon. Enter saves all pending choices.
Esc cancels. Enter on Restore defaults previews the defaults, then Enter saves them.

Interface has eight choices: Blue (default), Red, Green, Yellow, Orange, Purple,
Black and White. Endra Files uses distinct file-type glyphs based on the suffix.
An ordinary .tns uses the TI document glyph. Glyphs do not change open or launch behavior.

Keep the executable with `EndraHome_Apps.cfg.tns`. Run047 reads prior appearance
settings and maps legacy interface colors to their closest retained choice. Save
stores appearance and app shortcuts in `EndraHome_Apps.cfg.tns`. Older appearance
files are read as a fallback and are left untouched for older builds. The first
successful save writes the imported settings alongside the shortcuts. Desktop
names retain their configured color, with a thin ring marking the selected name.

The five supplied 320×240 wallpapers use 8-bit indices into RGB565 palettes.
The ten supplied emblems use traced silhouettes and 8-bit transparency.
Both the theme and wallpaper palette are applied directly in the existing framebuffer.
See `make_run038_art.py` to repack the supplied handoff artwork.

Native shortcut registration and Ctrl+Home OS standby are pending investigation.

```sh
clear
push /storage/emulated/0/Download/EndraHome_Run047_Desktop.tns /upload
```

Historical source notes follow.

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
