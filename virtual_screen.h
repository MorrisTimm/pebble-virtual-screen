#pragma once
#include <pebble.h>

#define VIRTUAL_SCREEN

Layer* virtual_screen_init(GRect screen_bounds, GSize virtual_size);
void virtual_screen_deinit();
GPoint virtual_screen_get_offset();
