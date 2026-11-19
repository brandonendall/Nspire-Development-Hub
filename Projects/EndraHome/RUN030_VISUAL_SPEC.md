# Run030 premium graphics acceptance criteria

User approved visual direction: polished metallic 3D E cube and subdued gray/dark header; compact professional title, not Run029's chunky font. Reference mockup is concept art, not a pixel-accurate or already-implemented screen. Physical display is 320x240 RGB565; prioritize legibility and smooth rendering.

## Required visual changes
- Header: 3D shaded E cube, theme-derived highlight/shadow colors, clean title and smaller Run 030 indicator. Preserve Run028's gray panel feel.
- Desktop: distinct controller, archive, terminal, folder, image, tools, star icons for existing seven slots. Do not add an eighth slot or fake Settings shortcut without actual wiring.
- Use lightweight framebuffer primitives; evaluate SDL only if already linked and beneficial. Avoid continuous animation or CPU-heavy effects.
- Theme: red, green, blue, yellow, orange, black, white, gray, cyan, purple, gold, pink, emerald and variants; text, folders, glyphs, accents, borders and 3D cube update. Preserve independent interface/folder/text controls and contrast.
- Captions: no orphan final letter in Favorites; labels fit 320x240. Bottom bar compact Enter: Open and Menu: Apps.
- Preserve Run029 functions, configuration, Files fallback, controls and Ctrl+On cleanup. Keep Run028 and Run029 binaries as rollback.

## Verification
Host screenshots across themes; overflow and theme tests; native Ndless compile; artifact checksum; separate CI vs hardware status. Deliver CamelCase TNS and lowercase /upload destination while root USB remains busy.
