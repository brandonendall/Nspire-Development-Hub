# UltraZIP 003 source restoration

Imported from the verified Nspire_Repo_Complete_Backup_Run001_2026-11-13.zip.
Application sources and Makefile match the original repository's main commit
47e9b6d9da2fe6e6df80a16c4d2e1d9b6ae84b1a. No application behavior was changed.

This project retains the new hub's Projects/UltraZip directory name. The
executable remains UltraZIP_003.tns. Older build metadata still says
"UltraZIP 001" internally; this restoration preserves it.

miniz 3.0.2 is vendored at commit
293d4db1b7d0ffee9756d035b9ac6f7431ef8492 with its license and source files.
Run sh get_miniz.sh to generate miniz_export.h before compiling.

The source's earlier README is retained as README.original.md.
The new workflow uses the Ndless container digest recorded by this hub's
successful November 13 smoke and UltraGBA builds.

Physical calculator testing of the restored build remains pending.
