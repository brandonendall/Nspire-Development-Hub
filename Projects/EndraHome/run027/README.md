# EndraHome Run027 — Launcher grid and safe file columns

Based on Run026. Launcher icons now use a centered four-across top row and three-across second row, with labels and indicators tied to each tile. Removed obsolete READY/config text that rendered behind icons. File browser restricts filenames to their column, and size formatting avoids unsupported Ndless 64-bit printf specifiers that printed `(null)` on the calculator. All seven slots remain available for assigning apps. Copy/move/delete functionality retained; test on disposable files first.
