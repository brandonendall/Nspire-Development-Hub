"""Importable Pygame event API using the shared calculator backend."""
from pygame import _event_backend

Event = _event_backend.Event
EventType = _event_backend.EventType
pump = _event_backend.pump
post = _event_backend.post
get = _event_backend.get
poll = _event_backend.poll
wait = _event_backend.wait
clear = _event_backend.clear
peek = _event_backend.peek
custom_type = _event_backend.custom_type
event_name = _event_backend.event_name
