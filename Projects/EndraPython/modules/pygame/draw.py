"""Importable Pygame draw API using the shared calculator backend."""
from pygame import _draw_backend

line = _draw_backend.line
lines = _draw_backend.lines
rect = _draw_backend.rect
circle = _draw_backend.circle
ellipse = _draw_backend.ellipse
polygon = _draw_backend.polygon
