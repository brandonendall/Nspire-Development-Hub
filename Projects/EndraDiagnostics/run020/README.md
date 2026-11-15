# EndraDiagnostics Run020 — immediate first frame

Run019-derived experimental build. Paint the HUD before heap test and before bounded read-only Documents enumeration, repainting after each operation. This reduces the initial black-screen interval, but the Documents scan is still synchronous and may briefly block interaction. Preserve native CPU and RAM N/A rather than invent utilization. No filesystem writes; Esc/Enter exits; keep Run019 as tested fallback. Physical testing pending.
