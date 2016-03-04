#include <pebble.h>

Window* window;
TextLayer *time_layer, *day_of_week_layer, *temperature_layer, *day_of_month_layer, *month_layer;
TextLayer *time_updated_layer;
GBitmap *background_bitmap;
BitmapLayer *background_layer;

char time_buffer[32], day_of_week_buffer[32], temperature_buffer[32], day_of_month_buffer[32], month_buffer[32];
char time_updated_buffer[32];

enum{
  KEY_TEMPERATURE = 0,
};

void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  //Format the buffer string using tick_time as the time source.
  //For the time:
  strftime(time_buffer, sizeof("00:00"), "%l:%M", tick_time);
  if(' ' == time_buffer[0]) {
    memmove(time_buffer, &time_buffer[1], sizeof(time_buffer)-1);
  }
  
  //For the day of the week:
  strftime(day_of_week_buffer, sizeof("xxx"), "%a", tick_time);
  
  //For the day of the month:
  strftime(day_of_month_buffer, sizeof("00"), "%d", tick_time);
  if('0' == day_of_month_buffer[0]){
    memmove(day_of_month_buffer, &day_of_month_buffer[1], sizeof(day_of_month_buffer)-1);
  }
  
  //For the month:
  strftime(month_buffer, sizeof("00"), "%m", tick_time);
  if('0' == month_buffer[0]){
    memmove(month_buffer, &month_buffer[1], sizeof(month_buffer)-1);
  }
  
  //For the text layer that tells when the temperature was last updated
  //(I don't think I have to use this code)
  /*
  strftime(time_updated_layer, sizeof("00:00"), "%l:%m", tick_time);
  if(' ' == time_updated_buffer[0]) {
    memmove(time_updated_buffer, &time_updated_uffer[1], sizeof(time_updated_buffer)-1);
  }
  */
  
  //Change the TextLayer's text to show the new time!
  text_layer_set_text(time_layer, time_buffer);
  text_layer_set_text(day_of_week_layer, day_of_week_buffer);
  text_layer_set_text(day_of_month_layer, day_of_month_buffer);
  text_layer_set_text(month_layer, month_buffer);
  //text_layer_set_text(time_updated_layer, time_updated_buffer);
}

void process_tuple(Tuple *t){
  //Get key
  int key = t->key;
  
  //Get interger value, if present
  int value = t->value->int32;
  
  //Get string value, if present
  char string_value[32];
  strcpy(string_value, t->value->cstring);
  
  //Decide what to do
  switch(key){
    case KEY_TEMPERATURE:
      //Temperature received
      snprintf(temperature_buffer, sizeof("XX\u00B0"), "%d\u00B0", value);
      text_layer_set_text(temperature_layer, (char*) &temperature_buffer);
      break;
  }
  
  //Set time this update came in
	time_t temp = time(NULL);	
	struct tm *tm = localtime(&temp);
  
	strftime(time_updated_buffer, sizeof("XX:XX"), "%l:%M", tm);
  if(' ' == time_updated_buffer[0]) {
    memmove(time_updated_buffer, &time_updated_buffer[1], sizeof(time_updated_buffer)-1);
  }
	text_layer_set_text(time_updated_layer, (char*) &time_updated_buffer);
}

static void in_received_handler(DictionaryIterator *iter, void *context){
  (void) context;
  
  //Get data
  Tuple *t = dict_read_first(iter);
  while(t != NULL){
    process_tuple(t);
    
    //Get next
    t = dict_read_next(iter);
  }
}

//This following function saves me from repeatedly typing out the same code for the text layers.
static TextLayer* init_text_layer(GRect location, GColor colour, GColor background, char *res_id, GTextAlignment alignment){
  TextLayer *layer = text_layer_create(location);
  text_layer_set_text_color(layer, colour);
  text_layer_set_background_color(layer, background);
  text_layer_set_font(layer, fonts_load_custom_font(res_id));  
  text_layer_set_text_alignment(layer, alignment);
  
  return layer;
}

void window_load(Window *window){
  ResHandle font_handle_1 = resource_get_handle(RESOURCE_ID_SNIGLET_50);
  ResHandle font_handle_2 = resource_get_handle(RESOURCE_ID_SNIGLET_31);
  ResHandle font_handle_3 = resource_get_handle(RESOURCE_ID_SNIGLET_43);
  
  //Load bitmaps into GBitmap structures.
  //The ID you chose when uploading is prefixed with 'RESOURCE_ID_'
  background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
  
  //Create BitmapLayer to show GBitmap and add to Window.
  background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(background_layer, background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(background_layer));
  
  time_layer = init_text_layer(GRect(0, 0, 144, 70), GColorWhite, GColorClear, font_handle_1, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
  
  temperature_layer = init_text_layer(GRect(0, 53, 144, 49), GColorWhite, GColorClear, font_handle_3, GTextAlignmentCenter);
	text_layer_set_text(temperature_layer, "??\u00B0");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(temperature_layer));
  
  day_of_week_layer = init_text_layer(GRect(0, 100, 144, 49), GColorWhite, GColorClear, font_handle_2, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(day_of_week_layer));

  day_of_month_layer = init_text_layer(GRect(0, 117, 40, 49), GColorBlack, GColorClear, font_handle_3, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(day_of_month_layer));
  
  month_layer = init_text_layer(GRect(104, 117, 40, 49), GColorBlack, GColorClear, font_handle_3, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(month_layer));
  
  time_updated_layer = text_layer_create(GRect(77, 70, 67, 30));
  text_layer_set_text_color(time_updated_layer, GColorWhite);
  text_layer_set_background_color(time_updated_layer, GColorClear);
  text_layer_set_font(time_updated_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));  
  text_layer_set_text_alignment(time_updated_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_updated_layer));
  
  //Get a time structure so that the face doesn't start blank.
  struct tm *t;
  time_t temp;
  temp = time(NULL);
  t = localtime(&temp);
  
  //Manually call the tick handler when the window is loading.
  tick_handler(t, MINUTE_UNIT);
}

void window_unload(Window *window){
  text_layer_destroy(time_layer);
  text_layer_destroy(day_of_week_layer);
  text_layer_destroy(temperature_layer);
  text_layer_destroy(day_of_month_layer);
  text_layer_destroy(month_layer);
  text_layer_destroy(time_updated_layer);
  
  //Destroy GBitmap.
	gbitmap_destroy(background_bitmap);
	
	//Destroy BitmapLayer.
	bitmap_layer_destroy(background_layer);
}

void send_int(uint8_t key, uint8_t cmd){
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  
  Tuplet value = TupletInteger(key, cmd);
  dict_write_tuplet(iter, &value);
  
  app_message_outbox_send();
}

void tick_callback(struct tm *tick_time, TimeUnits units_changed){
  //Every five minutes
  if(tick_time->tm_min % 5 == 0){
    //Send an arbitary message, the response will be handled by in_received_handler()
    send_int(5,5);
  }
  
  //Format the buffer string using tick_time as the time source.
  //For the time:
  strftime(time_buffer, sizeof("00:00"), "%l:%M", tick_time);
  if(' ' == time_buffer[0]) {
    memmove(time_buffer, &time_buffer[1], sizeof(time_buffer)-1);
  }
  
  //For the day of the week:
  strftime(day_of_week_buffer, sizeof("xxx"), "%a", tick_time);
  
  //For the day of the month:
  strftime(day_of_month_buffer, sizeof("00"), "%d", tick_time);
  if('0' == day_of_month_buffer[0]){
    memmove(day_of_month_buffer, &day_of_month_buffer[1], sizeof(day_of_month_buffer)-1);
  }
  
  //For the month:
  strftime(month_buffer, sizeof("00"), "%m", tick_time);
  if('0' == month_buffer[0]){
    memmove(month_buffer, &month_buffer[1], sizeof(month_buffer)-1);
  }
  
  //For the text layer that tells when the temperature was last updated
  /*
  strftime(time_updated_layer, sizeof("00:00"), "%l:%m", tick_time);
  if(' ' == time_updated_buffer[0]) {
    memmove(time_updated_buffer, &time_updated_buffer[1], sizeof(time_updated_buffer)-1);
  }
  */

  //Change the TextLayer's text to show the new time!
  text_layer_set_text(time_layer, time_buffer);
  text_layer_set_text(day_of_week_layer, day_of_week_buffer);
  text_layer_set_text(day_of_month_layer, day_of_month_buffer);
  text_layer_set_text(month_layer, month_buffer);
  //text_layer_set_text(time_updated_layer, time_updated_buffer);
}

void init(){
  window = window_create();
  WindowHandlers handlers = {
    .load = window_load,
    .unload = window_unload
  };
  window_set_window_handlers(window, handlers);
  
  //Register AppMessage events
  app_message_register_inbox_received(in_received_handler);
  //Largest possible input and output buffer sizes
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  //Register to receive minutely updates
  tick_timer_service_subscribe(MINUTE_UNIT, tick_callback);
  //tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
  
  window_stack_push(window, true);
}

void deinit(){
  tick_timer_service_unsubscribe();
  
  window_destroy(window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}