# UltraCommander NX Run001 source restoration

Restored from UltraCommanderNX_Run001_Complete.zip, preserved in the complete
Nspire backup dated November 13, 2026.

All 42 original source-package files, including the Makefile, C sources,
tests, test fixtures, dependency licenses, runtime source archive, and
original validation records, are retained byte for byte.

The hub's project folder is Projects/UltimateCommander. The existing
application and binary names remain UltraCommander NX and
UltraCommanderNX_Run001.tns. This import does not rebrand the application.

The original validation/ directory reports checks performed for the
November 12 package. The restored source is tested and built again by
.github/workflows/ultracommander.yml; new results are recorded under handoff.

The original source uses its pinned Ndless SDK build script with GNU Arm
GCC. It is kept intact rather than substituting the shared nspire-gcc
wrapper for its different Makefile interface.

Physical calculator testing remains pending.
