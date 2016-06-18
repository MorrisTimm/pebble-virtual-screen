#pragma once

#define VIRTUAL_SCREEN

typedef enum {
  VIRTUAL_SCREEN_MODE_DIRECT,
  VIRTUAL_SCREEN_MODE_AUTO
} VirutalScreenMode;

Layer* virtual_screen_init(GRect screen_bounds, GSize virtual_size, VirutalScreenMode virtual_screen_mode);
void virtual_screen_deinit();
GPoint virtual_screen_get_offset();
