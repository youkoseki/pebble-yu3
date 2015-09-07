#include "simple_analog.h"

#include "pebble.h"

static Window *window;
static Layer *s_simple_bg_layer, *s_date_layer, *s_hands_layer;
static TextLayer *s_num_label,*h_num_label,*d_num_label;

static GPath *s_minute_arrow, *s_hour_arrow;
static char s_num_buffer[4], h_num_buffer[4],d_num_buffer[4];
static GPoint s_center;
static GPoint sHandCircle;
static GPoint mHandCircle;

static void bg_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorWhite);
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  int16_t second_hand_length = bounds.size.w / 2;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  // minute/hour hand
/*  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_filled(ctx, s_minute_arrow);
  gpath_draw_outline(ctx, s_minute_arrow);

  graphics_context_set_fill_color(ctx, GColorRed);
  graphics_context_set_stroke_color(ctx, GColorRed);

  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));

  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);*/

  // dot in the middle
  sHandCircle.x = 60*sin_lookup(TRIG_MAX_ANGLE * t->tm_min / 60)/TRIG_MAX_RATIO + s_center.x;
  sHandCircle.y = -60*cos_lookup(TRIG_MAX_ANGLE * t->tm_min / 60)/TRIG_MAX_RATIO + s_center.y;
  mHandCircle.x = 30*sin_lookup(TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10)) / (12 * 6))/TRIG_MAX_RATIO + s_center.x;
  mHandCircle.y = -30*cos_lookup(TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10)) / (12 * 6))/TRIG_MAX_RATIO + s_center.y;

  /*

  sHandCircle.x = 60*sin_lookup(TRIG_MAX_ANGLE/4)/TRIG_MAX_RATIO + s_center.x;
  sHandCircle.y = -60*cos_lookup(TRIG_MAX_ANGLE/4)/TRIG_MAX_RATIO + s_center.y;
  mHandCircle.x = 30*sin_lookup(TRIG_MAX_ANGLE/4)/TRIG_MAX_RATIO + s_center.x;
  mHandCircle.y = -30*cos_lookup(TRIG_MAX_ANGLE/4)/TRIG_MAX_RATIO + s_center.y;
*/
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, s_center, 16);
/*  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, s_center, 14);*/

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, mHandCircle, 16);
/*  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, mHandCircle, 14);*/

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, sHandCircle, 16);
/*  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, sHandCircle, 14);*/
  
  text_layer_destroy(s_num_label);
  text_layer_destroy(h_num_label);
  text_layer_destroy(d_num_label);

  s_num_label = text_layer_create(GRect(sHandCircle.x-14, sHandCircle.y-12, 30, 30));
  text_layer_set_background_color(s_num_label, GColorClear);
  text_layer_set_text_color(s_num_label, GColorBlack);
  text_layer_set_font(s_num_label, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_num_label,GTextAlignmentCenter);
  strftime(s_num_buffer, sizeof(s_num_buffer), "%M", t);
  text_layer_set_text(s_num_label, s_num_buffer);
  layer_add_child(layer, text_layer_get_layer(s_num_label));


  h_num_label = text_layer_create(GRect(mHandCircle.x-14, mHandCircle.y-12, 30, 30));
  text_layer_set_background_color(h_num_label, GColorClear);
  text_layer_set_text_color(h_num_label, GColorBlack);
  text_layer_set_font(h_num_label, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
  text_layer_set_text_alignment(h_num_label,GTextAlignmentCenter);
  strftime(h_num_buffer, sizeof(h_num_buffer), "%H", t);
  text_layer_set_text(h_num_label, h_num_buffer);
  layer_add_child(layer, text_layer_get_layer(h_num_label));

  
  d_num_label = text_layer_create(GRect(s_center.x-14, s_center.y-12, 30, 30));
  text_layer_set_background_color(d_num_label, GColorClear);
  text_layer_set_text_color(d_num_label, GColorBlack);
  text_layer_set_font(d_num_label, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
  text_layer_set_text_alignment(d_num_label,GTextAlignmentCenter);
  strftime(d_num_buffer, sizeof(d_num_buffer), "%d", t);
  text_layer_set_text(d_num_label, d_num_buffer);
  layer_add_child(layer, text_layer_get_layer(d_num_label));

  

  
  /*  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(s_center.x,s_center.y), 20);*/
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

//  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
//  text_layer_set_text(s_day_label, s_day_buffer);

//  text_layer_set_text(s_num_label,"88");
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  GRect bounds = layer_get_bounds(window_layer);
  s_center = grect_center_point(&bounds);
  
  s_simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_simple_bg_layer);

/*  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);*/


  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);


}

static void window_unload(Window *window) {
  layer_destroy(s_simple_bg_layer);
//  layer_destroy(s_date_layer);

  layer_destroy(s_hands_layer);
  text_layer_destroy(s_num_label);
}

static void init() {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);

  s_num_buffer[0] = '\0';
  h_num_buffer[0] = '\0';
  d_num_buffer[0] = '\0';

  // init hand paths
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);


  tick_timer_service_subscribe(MINUTE_UNIT, handle_second_tick);
}

static void deinit() {
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);

  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
