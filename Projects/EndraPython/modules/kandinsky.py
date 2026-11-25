"""Immediate-mode drawing for the TI screen."""
import display as _display
from display import color, get_pixel


def set_pixel(x, y, rgb):
    _display.set_pixel(x, y, rgb)
    _display.show_screen()


def fill_rect(x, y, width, height, rgb):
    _display.fill_rect(x, y, width, height, rgb)
    _display.show_screen()


def draw_string(text, x, y, rgb=(0, 0, 0)):
    _display.draw_string(x, y, text, rgb)
    _display.show_screen()
