#include <virtual_screen.h>

#define VIRTUAL_SCREEN_UPDATE_SENSITIVITY 5
#define VIRTUAL_SCREEN_UPDATE_TIMEOUT_MS 50
#define VIRTUAL_SCREEN_UPDATE_THRESHOLD 100

#define ABS(X, Y) (X) > (Y) ? (X)-(Y) : (Y)-(X)

static GPoint offset;
static Layer* layer;
static AppTimer* timer;

static GPoint center_point;
static GPoint reference_point;
static GPoint accel_point;
static GPoint max_point;

static void accel_callback(void* data) {
  static AccelData last_accel_data;
  AccelData accel_data;
  accel_service_peek(&accel_data);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "accel_data: %d/%d/%d", accel_data.x, accel_data.y, accel_data.z);
  if(   ABS(accel_data.x, last_accel_data.x) > VIRTUAL_SCREEN_UPDATE_THRESHOLD
     || ABS(accel_data.y, last_accel_data.y) > VIRTUAL_SCREEN_UPDATE_THRESHOLD) {
    accel_point = GPoint(center_point.x-(reference_point.x - accel_data.x)/VIRTUAL_SCREEN_UPDATE_SENSITIVITY,
                         center_point.x+(reference_point.y - accel_data.y)/VIRTUAL_SCREEN_UPDATE_SENSITIVITY);
    if(accel_point.x > max_point.x) accel_point.x = max_point.x;
    if(accel_point.x < 0) accel_point.x = 0;
    if(accel_point.y > max_point.y) accel_point.y = max_point.y;
    if(accel_point.y < 0) accel_point.y = 0;
    offset = GPoint(-accel_point.x, -accel_point.y);
    GRect bounds = layer_get_bounds(layer);
    bounds.origin = offset;
    layer_set_bounds(layer, bounds);
    layer_mark_dirty(layer);
  }
  last_accel_data = accel_data;
  timer = app_timer_register(VIRTUAL_SCREEN_UPDATE_TIMEOUT_MS, accel_callback, data);
}

Layer* virtual_screen_init(GRect screen_bounds, GSize virtual_size) {
  max_point = GPoint(virtual_size.w-screen_bounds.size.w, virtual_size.h-screen_bounds.size.h);
  center_point = GPoint(max_point.x/2, max_point.y/2);
  accel_point = center_point;
  offset = GPoint(-accel_point.x, -accel_point.y);
  
  AccelData accel_data;
  accel_service_peek(&accel_data);
  reference_point = GPoint(accel_data.x, accel_data.y);
  
  timer = app_timer_register(VIRTUAL_SCREEN_UPDATE_TIMEOUT_MS, accel_callback, NULL);

  layer = layer_create(GRect(0, 0, virtual_size.w, virtual_size.h));
  return layer;
}

void virtual_screen_deinit() {
  app_timer_cancel(timer);
  layer_destroy(layer);
}

GPoint virtual_screen_get_offset() {
  return offset;
}
