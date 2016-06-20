#pragma once

#define VIRTUAL_SCREEN
#define VIRTUAL_SCREEN_SIZE_APLITE  GSize(144, 168)
#define VIRTUAL_SCREEN_SIZE_BASALT  GSize(144, 168)
#define VIRTUAL_SCREEN_SIZE_CHALK   GSize(180, 180)
#define VIRTUAL_SCREEN_SIZE_DIORITE GSize(144, 168)
#define VIRTUAL_SCREEN_SIZE_EMERY   GSize(200, 228)

typedef enum {
  VIRTUAL_SCREEN_MODE_DIRECT,
  VIRTUAL_SCREEN_MODE_AUTO,
  VIRTUAL_SCREEN_MODE_CENTERED,
  VIRTUAL_SCREEN_MODE_TOP_LEFT,
  VIRTUAL_SCREEN_MODE_TOP_RIGHT,
  VIRTUAL_SCREEN_MODE_BOTTOM_LEFT,
  VIRTUAL_SCREEN_MODE_BOTTOM_RIGHT
} VirutalScreenMode;

Layer* virtual_screen_init(GRect screen_bounds, GSize virtual_size, VirutalScreenMode virtual_screen_mode);
void virtual_screen_deinit();
GPoint virtual_screen_get_offset();
