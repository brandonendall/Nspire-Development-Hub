# UltraZIP 003

Ndless ZIP utility with a native graphical file browser for TI-Nspire CX II CAS.
The original UltraZIP 003 application source has been restored without
changing its behavior. See IMPORT.md for source provenance.

The browser navigates folders, lists ZIP entries, creates a ZIP from the
selected file, and extracts an archive to a new output folder. Existing
destination files are refused. Encrypted ZIPs are unsupported. Test with
disposable files before using important documents.

## Build

From this repository's root:

```sh
(cd Projects/UltraZip && sh get_miniz.sh)
bash toolchain/build.sh Projects/UltraZip
```

Output: Projects/UltraZip/UltraZIP_003.tns.
The workflow .github/workflows/ultrazip.yml builds and uploads
UltraZIP_003_Restored_Run001 with a SHA-256 checksum.
A successful compile does not establish calculator compatibility.
