"""Importable Pygame key API using the shared calculator backend."""
from pygame import _key_backend

get_pressed = _key_backend.get_pressed
get_mods = _key_backend.get_mods
name = _key_backend.name
set_repeat = _key_backend.set_repeat
get_repeat = _key_backend.get_repeat
