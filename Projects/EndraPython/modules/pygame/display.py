"""Importable Pygame display API using the shared calculator backend."""
from pygame import _display_backend

set_mode = _display_backend.set_mode
get_surface = _display_backend.get_surface
flip = _display_backend.flip
update = _display_backend.update
set_caption = _display_backend.set_caption
