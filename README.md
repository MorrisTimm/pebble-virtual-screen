# pebble-virtual-screen
A virtual screen implementation for the Pebble smartwatch.

Use this library to test the layout for large screens (e.g. Pebble Time 2) on smaller screens (e.g. Pebble Time).

There are several modes available:

`VIRTUAL_SCREEN_MODE_DIRECT`<br>
The large screen is centered on the position the watch has when the app starts. You can scroll the screen around by moving the watch.

`VIRTUAL_SCREEN_MODE_AUTO`<br>
The large screen is autmatically moved to the next corner every second. Use this for testing in the emulator.

`VIRTUAL_SCREEN_MODE_`{`CENTERED`,`TOP_LEFT`,`TOP_RIGHT`,`BOTTOM_LEFT`,`BOTTOM_RIGHT`}<br>
The large screen remains in a fixed position.

To use a virtual screen you just have to use the `Layer` returned by `virtual_screen_init` instead of the root layer you normally use.
If you use `graphics_capture_frame_buffer` to modify the framebuffer you also have to manually apply the offset returned by `virtual_screen_get_offset`.

You can use the `VIRTUAL_SCREEN` define to determine whether or not the virtual screen is used. The define is set if `pebble-virtual-screen.h` is included.

This is how `VIRTUAL_SCREEN_MODE_AUTO` looks in CloudPebble:<br>
![BCD minimalist](http://i.imgur.com/T1zf06b.gif)

Usage example:

    #include <pebble-virtual-screen/pebble-virtual-screen.h>

    static void my_window_load(Window *window) {
      Layer* root_layer = window_get_root_layer(window);
      GRect root_layer_bounds = layer_get_bounds(root_layer);

    #ifdef VIRTUAL_SCREEN
      Layer* real_root = root_layer;
      root_layer = virtual_screen_init(root_layer_bounds, VIRTUAL_SCREEN_SIZE_EMERY, VIRTUAL_SCREEN_MODE_AUTO);
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
