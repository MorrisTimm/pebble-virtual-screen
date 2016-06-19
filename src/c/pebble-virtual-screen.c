#include <pebble.h>
#include "pebble-virtual-screen.h"

#define VIRTUAL_SCREEN_UPDATE_SENSITIVITY 5
#define VIRTUAL_SCREEN_UPDATE_TIMEOUT_MS 50
#define VIRTUAL_SCREEN_UPDATE_THRESHOLD 100

#define VIRTUAL_SCREEN_UPDATE_AUTO_TIMEOUT_MS 1000
#define VIRTUAL_SCREEN_UPDATE_AUTO_ANIM_SPEED 500

#define ABS(X, Y) (X) > (Y) ? (X)-(Y) : (Y)-(X)

static Layer* s_layer;
static AppTimer* s_timer;

static GPoint s_max_point;

static GPoint s_center_point;
static GPoint s_reference_point;
static GPoint s_accel_point;

static void prv_direct_callback(void* data) {
  static AccelData last_accel_data;
  AccelData accel_data;
  accel_service_peek(&accel_data);
  if(   ABS(accel_data.x, last_accel_data.x) > VIRTUAL_SCREEN_UPDATE_THRESHOLD
     || ABS(accel_data.y, last_accel_data.y) > VIRTUAL_SCREEN_UPDATE_THRESHOLD) {
    s_accel_point = GPoint(
      s_center_point.x-(s_reference_point.x - accel_data.x)/VIRTUAL_SCREEN_UPDATE_SENSITIVITY,
      s_center_point.x+(s_reference_point.y - accel_data.y)/VIRTUAL_SCREEN_UPDATE_SENSITIVITY);
    if(s_accel_point.x > s_max_point.x) s_accel_point.x = s_max_point.x;
    if(s_accel_point.x < 0) s_accel_point.x = 0;
    if(s_accel_point.y > s_max_point.y) s_accel_point.y = s_max_point.y;
    if(s_accel_point.y < 0) s_accel_point.y = 0;
    GRect bounds = layer_get_bounds(s_layer);
    bounds.origin = GPoint(-s_accel_point.x, -s_accel_point.y);
    layer_set_bounds(s_layer, bounds);
    layer_mark_dirty(s_layer);
  }
  last_accel_data = accel_data;
  s_timer = app_timer_register(VIRTUAL_SCREEN_UPDATE_TIMEOUT_MS, prv_direct_callback, data);
}

static void prv_auto_callback(void* data) {
  static int pos[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
  pos[1][0] = pos[2][0] = -s_max_point.x;
  pos[2][1] = pos[3][1] = -s_max_point.y;
  static uint8_t curr_pos = 0;
  s_accel_point = GPoint(pos[curr_pos][0], pos[curr_pos][1]); // animation target
  curr_pos = (curr_pos+1)%4;
  
  GRect bounds = layer_get_bounds(s_layer);
  s_reference_point = bounds.origin; // animation start
  
  static PropertyAnimation* animation;
  animation = property_animation_create_bounds_origin(s_layer, &s_reference_point, &s_accel_point);
  animation_set_duration((Animation*)animation, VIRTUAL_SCREEN_UPDATE_AUTO_ANIM_SPEED);
  animation_set_curve((Animation*)animation, AnimationCurveLinear);
  animation_schedule((Animation*)animation);
  
  s_timer = app_timer_register(VIRTUAL_SCREEN_UPDATE_AUTO_TIMEOUT_MS, prv_auto_callback, data);
}

Layer* virtual_screen_init( GRect screen_bounds, GSize virtual_size,
                            VirutalScreenMode virtual_screen_mode) {
  s_max_point = GPoint(virtual_size.w-screen_bounds.size.w, virtual_size.h-screen_bounds.size.h);
  
  switch(virtual_screen_mode) {
    case VIRTUAL_SCREEN_MODE_DIRECT: {
      s_center_point = GPoint(s_max_point.x/2, s_max_point.y/2);
      s_accel_point = s_center_point;
    
      AccelData accel_data;
      accel_service_peek(&accel_data);
      s_reference_point = GPoint(accel_data.x, accel_data.y);
      
      s_timer = app_timer_register(VIRTUAL_SCREEN_UPDATE_TIMEOUT_MS, prv_direct_callback, NULL);
      break;
    }
    case VIRTUAL_SCREEN_MODE_AUTO: {
      s_timer = app_timer_register(VIRTUAL_SCREEN_UPDATE_AUTO_TIMEOUT_MS, prv_auto_callback, NULL);
      break;
    }
  }
  s_layer = layer_create(GRect(0, 0, virtual_size.w, virtual_size.h));
  return s_layer;
}

void virtual_screen_deinit() {
  app_timer_cancel(s_timer);
  layer_destroy(s_layer);
}

GPoint virtual_screen_get_offset() {
  return layer_get_bounds(s_layer).origin;
}
