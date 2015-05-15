#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_hours_layer;
static TextLayer *s_minutes_layer;
static TextLayer *s_seconds_layer;

static void main_window_load(Window *window) {
  s_hours_layer = text_layer_create(GRect(5, 30, 144, 50));
  text_layer_set_background_color(s_hours_layer, GColorClear);
  text_layer_set_text_color(s_hours_layer, GColorWhite);
  text_layer_set_text(s_hours_layer, "00");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hours_layer));
  
  s_minutes_layer = text_layer_create(GRect(5, 70, 144, 50));
  text_layer_set_background_color(s_minutes_layer, GColorClear);
  text_layer_set_text_color(s_minutes_layer, GColorWhite);
  text_layer_set_text(s_minutes_layer, "00");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minutes_layer));
  
  s_seconds_layer = text_layer_create(GRect(5, 110, 144, 50));
  text_layer_set_background_color(s_seconds_layer, GColorClear);
  text_layer_set_text_color(s_seconds_layer, GColorWhite);
  text_layer_set_text(s_seconds_layer, "00");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_seconds_layer));
  
  text_layer_set_font(s_hours_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_font(s_minutes_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_font(s_seconds_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(s_hours_layer, GTextAlignmentLeft);
  text_layer_set_text_alignment(s_minutes_layer, GTextAlignmentLeft);
  text_layer_set_text_alignment(s_seconds_layer, GTextAlignmentLeft);
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_hours_layer);
  text_layer_destroy(s_minutes_layer);
  text_layer_destroy(s_seconds_layer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer_hours[] = "00";
  static char buffer_minutes[] = "00";
  static char buffer_seconds[] = "00";

  strftime(buffer_hours, sizeof("00:00:00"), "%I", tick_time);
  strftime(buffer_minutes, sizeof("00:00:00"), "%M", tick_time);
  strftime(buffer_seconds, sizeof("00:00:00"), "%S", tick_time);
  

  // Display this time on the TextLayer
  text_layer_set_text(s_hours_layer, buffer_hours);
  text_layer_set_text(s_minutes_layer, buffer_minutes);
  text_layer_set_text(s_seconds_layer, buffer_seconds);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  s_main_window = window_create();
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_background_color(s_main_window, GColorVividCerulean);
  window_stack_push(s_main_window, true);
  update_time();
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}