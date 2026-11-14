# EndraHome Run012 — Quick Launch (hardware test pending)

User requested single-press launching without confirmation screens. On the home screen, arrows select, **Enter immediately launches** configured shortcuts, **Menu opens information** for the selected app, and Esc exits. Within Info, Enter launches with one press, Menu shows config/help, Esc returns home. Unconfigured entries show a setup hint and never launch.

Preserves the **physically tested Run011 child return**: native LCD/timer state is restored before nl_exec(path,0,NULL), and on normal child return Run012 reinitializes EndraHome UI, remembers selected icon and does not automatically relaunch. No refresh_osscr; retain working fast exit. Older run011/run010 remain untouched.

Config remains the permanent read-only EndraHome_Apps.cfg.tns, same contents as Run010/Run011. Never replace the user's configured file with this blank template.

Safe device test: (1) Keep Run011 as rollback. (2) Start Run012 and press Esc; check normal fast exit. (3) Start Run012, select Tools, press Enter **once**; EndraHome_Test006.tns should appear immediately, without confirmation. (4) Dismiss tiny app message; EndraHome should return with Tools selected. (5) Menu from home shows Tools info, Menu again config help, Esc backs out. Do not use GBA or other large apps before tiny test is proven. Stop on any freeze or visual corruption.

Build host: make -C Projects/EndraHome/run012 test ; preview: make -C Projects/EndraHome/run012 preview ; native: bash toolchain/build.sh Projects/EndraHome/run012 . Device result is pending.
