# EndraHome roadmap

| Stage | Work | Calculator check |
|---|---|---|
| Run051 – structured files | Direct JSON reading, section navigation beyond 4 KiB, creation and bounded editing, other wrapped text extensions | Open a real save/config file, reach its final key, edit and reopen a small copy |
| Run052 and Run053 – finding files and values | Search filenames in the current folder and find text inside an open JSON document | Locate a known filename and jump to a known key in a large document |
| After search – launcher and Files testing | Exercise app launch and return, file operations, clock recovery, and saved settings on device | Return to the same workflow and verify saved data after relaunch |
| Run054 – app artwork | Opt-in app-supplied PNG icons with existing icon as fallback | Test apps with, without, and with invalid artwork |
| Final packaging | Integrate the tested PDF engine into the desktop after testing | Transfer one EndraHome .tns and open a PDF directly |

Keep the side-by-side nPDF.tns during development so only the desktop needs transferring. Future stages are proposals, not shipped features.
