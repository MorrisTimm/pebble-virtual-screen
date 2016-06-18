#include <pebble.h>
#include "pebble-virtual-screen.h"

#define VIRTUAL_SCREEN_UPDATE_SENSITIVITY 5
#define VIRTUAL_SCREEN_UPDATE_TIMEOUT_MS 50
#define VIRTUAL_SCREEN_UPDATE_THRESHOLD 100

#define VIRTUAL_SCREEN_UPDATE_AUTO_TIMEOUT_MS 1000
#define VIRTUAL_SCREEN_UPDATE_AUTO_ANIM_SPEED 500

#define ABS(X, Y) (X) > (Y) ? (X)-(Y) : (Y)-(X)

static Layer* layer;
static AppTimer* timer;

static GPoint max_point;

static GPoint center_point;
static GPoint reference_point;
static GPoint accel_point;

static void direct_callback(void* data) {
  static AccelData last_accel_data;
  AccelData accel_data;
  accel_service_peek(&accel_data);
  if(   ABS(accel_data.x, last_accel_data.x) > VIRTUAL_SCREEN_UPDATE_THRESHOLD
     || ABS(accel_data.y, last_accel_data.y) > VIRTUAL_SCREEN_UPDATE_THRESHOLD) {
    accel_point = GPoint(center_point.x-(reference_point.x - accel_data.x)/VIRTUAL_SCREEN_UPDATE_SENSITIVITY,
                         center_point.x+(reference_point.y - accel_data.y)/VIRTUAL_SCREEN_UPDATE_SENSITIVITY);
    if(accel_point.x > max_point.x) accel_point.x = max_point.x;
    if(accel_point.x < 0) accel_point.x = 0;
    if(accel_point.y > max_point.y) accel_point.y = max_point.y;
    if(accel_point.y < 0) accel_point.y = 0;
    GRect bounds = layer_get_bounds(layer);
    bounds.origin = GPoint(-accel_point.x, -accel_point.y);
    layer_set_bounds(layer, bounds);
    layer_mark_dirty(layer);
  }
  last_accel_data = accel_data;
  timer = app_timer_register(VIRTUAL_SCREEN_UPDATE_TIMEOUT_MS, direct_callback, data);
}

static void auto_callback(void* data) {
  static int pos[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
  pos[1][0] = pos[2][0] = -max_point.x;
  pos[2][1] = pos[3][1] = -max_point.y;
  static uint8_t curr_pos = 0;
  accel_point = GPoint(pos[curr_pos][0], pos[curr_pos][1]); // animation target
  curr_pos = (curr_pos+1)%4;
  
  GRect bounds = layer_get_bounds(layer);
  reference_point = bounds.origin; // animation start
  
  static PropertyAnimation* animation;
  animation = property_animation_create_bounds_origin(layer, &reference_point, &accel_point);
  animation_set_duration((Animation*)animation, VIRTUAL_SCREEN_UPDATE_AUTO_ANIM_SPEED);
  animation_set_curve((Animation*)animation, AnimationCurveLinear);
  animation_schedule((Animation*)animation);
  
  timer = app_timer_register(VIRTUAL_SCREEN_UPDATE_AUTO_TIMEOUT_MS, auto_callback, data);
}

Layer* virtual_screen_init(GRect screen_bounds, GSize virtual_size, VirutalScreenMode virtual_screen_mode) {
  max_point = GPoint(virtual_size.w-screen_bounds.size.w, virtual_size.h-screen_bounds.size.h);
  
  switch(virtual_screen_mode) {
    case VIRTUAL_SCREEN_MODE_DIRECT: {
      center_point = GPoint(max_point.x/2, max_point.y/2);
      accel_point = center_point;
    
      AccelData accel_data;
      accel_service_peek(&accel_data);
      reference_point = GPoint(accel_data.x, accel_data.y);
      
      timer = app_timer_register(VIRTUAL_SCREEN_UPDATE_TIMEOUT_MS, direct_callback, NULL);
      break;
    }
    case VIRTUAL_SCREEN_MODE_AUTO: {
      timer = app_timer_register(VIRTUAL_SCREEN_UPDATE_AUTO_TIMEOUT_MS, auto_callback, NULL);
      break;
    }
  }
  layer = layer_create(GRect(0, 0, virtual_size.w, virtual_size.h));
  return layer;
}

void virtual_screen_deinit() {
  app_timer_cancel(timer);
  layer_destroy(layer);
}

GPoint virtual_screen_get_offset() {
  return layer_get_bounds(layer).origin;
}
