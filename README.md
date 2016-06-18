# pebble-virtual-screen
A virtual screen implementation for the Pebble smartwatch.

Use this library to test the layout for large screens (e.g. Pebble Time 2) on smaller screens (e.g. Pebble Time).

There are two modes available:

1) direct<br>
The large screen is centered on the position the watch has when the app starts. You can scroll the screen around by moving the watch.

2) auto<br>
The large screen is autmatically moved to the next corner every second.

To use a virtual screen you just have to use the `Layer` returned by `virtual_screen_init` instead of the root layer you normally use.
If you use `graphics_capture_frame_buffer` to modify the framebuffer you also have to manually apply the offset returned by `virtual_screen_get_offset`.

You can use the `VIRTUAL_SCREEN` define to determine whether or not the virtual screen is used. The define is set if `pebble-virtual-screen.h` is included.

Usage example:

    #define EMERY_TEST
    #ifdef EMERY_TEST
    #include <pebble-virtual-screen/pebble-virtual-screen.h>
    #define VIRTUAL_SCREEN_SIZE_W 200
    #define VIRTUAL_SCREEN_SIZE_H 228
    #define VIRTUAL_SCREEN_MODE VIRTUAL_SCREEN_MODE_AUTO
    #endif // EMERY_TEST

    static void my_window_load(Window *window) {
      Layer* root_layer = window_get_root_layer(window);
      GRect root_layer_bounds = layer_get_bounds(root_layer);

    #ifdef VIRTUAL_SCREEN
      Layer* real_root = root_layer;
      root_layer = virtual_screen_init(root_layer_bounds, GSize(VIRTUAL_SCREEN_SIZE_W, VIRTUAL_SCREEN_SIZE_H), VIRTUAL_SCREEN_MODE);
      root_layer_bounds = layer_get_bounds(root_layer);
    #endif // VIRTUAL_SCREEN

      background_layer = layer_create(root_layer_bounds);
      layer_set_update_proc(background_layer, background_layer_draw);
      layer_add_child(root_layer, background_layer);
      
      time_layer = layer_create(GRectZero);
      layer_set_update_proc(time_layer, time_layer_draw);
      layer_add_child(root_layer, time_layer);

      date_layer = layer_create(GRectZero);
      layer_set_update_proc(date_layer, date_layer_draw);
      layer_add_child(root_layer, date_layer);

    #ifdef VIRTUAL_SCREEN
      layer_add_child(real_root, root_layer);
    #endif // VIRTUAL_SCREEN
    }
    
