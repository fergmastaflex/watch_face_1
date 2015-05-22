#include <pebble.h>
#define BACKGROUND_PKEY 1
#define TEXT_PKEY 2


static Window *s_main_window;
static TextLayer *s_hours_layer;
static TextLayer *s_minutes_layer;
static TextLayer *s_seconds_layer;
static int background_color_count;
static int text_color_count = 0;
static uint8_t colors[64];
static int num_colors = sizeof(colors)/sizeof(uint8_t);

static void create_colors_array(uint8_t *colors) {
  for(uint8_t byte=0; byte < 64; ++byte){
    uint8_t alpha_byte=192;
    colors[byte] = alpha_byte | byte;
  };
};

static void main_window_load(Window *window) {
  text_color_count = persist_exists(TEXT_PKEY) ? persist_read_int(TEXT_PKEY) : 0;
  
  s_hours_layer = text_layer_create(GRect(5, 30, 144, 50));
  text_layer_set_background_color(s_hours_layer, GColorClear);
  text_layer_set_text_color(s_hours_layer, (GColor8)colors[text_color_count]);
  text_layer_set_text(s_hours_layer, "00");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hours_layer));
  
  s_minutes_layer = text_layer_create(GRect(5, 70, 144, 50));
  text_layer_set_background_color(s_minutes_layer, GColorClear);
  text_layer_set_text_color(s_minutes_layer, (GColor8)colors[text_color_count]);
  text_layer_set_text(s_minutes_layer, "00");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minutes_layer));
  
  s_seconds_layer = text_layer_create(GRect(5, 110, 144, 50));
  text_layer_set_background_color(s_seconds_layer, GColorClear);
  text_layer_set_text_color(s_seconds_layer, (GColor8)colors[text_color_count]);
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

static void up_single_click_handler() {
  background_color_count += 1;
  if(background_color_count == num_colors) {
    background_color_count = 0;        
  }
  
  if(background_color_count == text_color_count){
    background_color_count += 1;
  }
  window_set_background_color(s_main_window, (GColor8)colors[background_color_count]);
}

static void up_multi_click_handler() {
  if(background_color_count == 0) {
    background_color_count = 64;        
  }
  background_color_count -= 1;
  
  if(background_color_count == text_color_count){
    background_color_count -= 1;
  }
  window_set_background_color(s_main_window, (GColor8)colors[background_color_count]);
}

static void down_single_click_handler() {
  text_color_count += 1;
  if(text_color_count == num_colors) {
    text_color_count = 0;        
  }
  if(text_color_count == background_color_count){
    text_color_count += 1;
  }
  text_layer_set_text_color(s_hours_layer, (GColor8)colors[text_color_count]);
  text_layer_set_text_color(s_minutes_layer, (GColor8)colors[text_color_count]);
  text_layer_set_text_color(s_seconds_layer, (GColor8)colors[text_color_count]);
}

static void down_multi_click_handler() {
  if(text_color_count == 0) {
    text_color_count = 64;        
  }
  text_color_count -= 1;
  
  if(text_color_count == background_color_count){
    text_color_count -= 1;
  }
  text_layer_set_text_color(s_hours_layer, (GColor8)colors[text_color_count]);
  text_layer_set_text_color(s_minutes_layer, (GColor8)colors[text_color_count]);
  text_layer_set_text_color(s_seconds_layer, (GColor8)colors[text_color_count]);
}


static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
  window_multi_click_subscribe(BUTTON_ID_UP, 2, 2, 0, true, up_multi_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
  window_multi_click_subscribe(BUTTON_ID_DOWN, 2, 2, 0, true, down_multi_click_handler);
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
  background_color_count = persist_exists(BACKGROUND_PKEY) ? persist_read_int(BACKGROUND_PKEY) : 0;
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  window_set_click_config_provider(s_main_window, click_config_provider);
  create_colors_array(colors);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_background_color(s_main_window, (GColor8)colors[background_color_count]);
  window_stack_push(s_main_window, true);
  update_time();
}

static void deinit() {
  persist_write_int(BACKGROUND_PKEY, background_color_count);
  persist_write_int(TEXT_PKEY, text_color_count);
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}