# EndraHome Run019 — bounded larger JPEG/PNG support

Derived from Run018, preserving all launcher, Files, configuration, text preview, zoom/pan and ESC behavior. The user confirmed Run018 oversized-image error returns safely to Files.

Run019 increases the *decode* ceiling from 1MP to 2MP (max dimension 2048), then downsamples the decoded RGB buffer to at most 1MP of retained RGB565 pixels. This is a bounded interim improvement, NOT streaming/partial JPEG decode; a 2MP RGB decode may temporarily use >6MB plus decoder overhead. Images above 2MP are still rejected. Test small image first, then the previously rejected image; revert to Run018 if freeze. No user files or settings are changed. Physical test pending.
