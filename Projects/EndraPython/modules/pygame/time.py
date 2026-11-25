"""Frame timing adapted from PythonUltra (MIT)."""
import time as _pytime

_ticks_ms = _pytime.ticks_ms
_ticks_diff = _pytime.ticks_diff
_sleep_ms = _pytime.sleep_ms

_start_ticks = _ticks_ms()


class Clock:
    def __init__(self):
        self._last = _ticks_ms()
        self._time = 0
        self._raw = 0
        self._fps = 0.0
        self._fps_start = self._last
        self._fps_frames = 0

    def tick(self, framerate=0):
        now = _ticks_ms()
        self._raw = max(0, _ticks_diff(now, self._last))
        if framerate:
            target = max(1, 1000 // int(framerate))
            if self._raw < target:
                _sleep_ms(target - self._raw)
                now = _ticks_ms()
        self._time = max(0, _ticks_diff(now, self._last))
        self._last = now
        self._fps_frames += 1
        sample = _ticks_diff(now, self._fps_start)
        if sample >= 1000:
            self._fps = self._fps_frames * 1000.0 / sample
            self._fps_frames = 0
            self._fps_start = now
        return self._time

    tick_busy_loop = tick

    def get_time(self):
        return self._time

    def get_rawtime(self):
        return self._raw

    def get_fps(self):
        return self._fps


def get_ticks():
    return max(0, _ticks_diff(_ticks_ms(), _start_ticks))


def delay(milliseconds):
    start = _ticks_ms()
    _sleep_ms(max(0, int(milliseconds)))
    return max(0, _ticks_diff(_ticks_ms(), start))


wait = delay
