/*
 * travel_snooze.c
 *
 *  Created on: 8 Mar 2014
 *      Author: oskar (and maybe Nick)
 */

#include "pebble.h"

/*
 *
 *
 *      MACROS
 *
 *
 */

/* Number of sections in bookmark menu. */
#define NUM_BOOKMARK_MENU_SECTIONS 2

/* Number of items in bookmark section of bookmark menu. */
#define NUM_BOOKMARK_MENU_BOOKMARK_ELEMS 3

/* Number of items in recent places section of bookmark menu. */
#define NUM_BOOKMARK_MENU_RECENT_ELEMS 1

/* Number of sections on recent places menu. */
#define NUM_RECENT_MENU_SECTIONS 1

/* Number of items in recent places section of recent places menu. */
#define NUM_RECENT_MENU_RECENT_ELEMS 5

#define MAX_CHARS 10
/*
 *
 *
 *      STATIC VARIABLE DECLARATIONS
 *
 *
 */

/*
 *
 *      BOOKMARK MENU DECLARATIONS
 *
 */
static Window* bookmark_menu_window;

static SimpleMenuLayer* bookmark_menu_layer;
static SimpleMenuSection bookmark_menu_sections[NUM_BOOKMARK_MENU_SECTIONS];

static SimpleMenuItem bookmark_menu_bookmark_items[NUM_BOOKMARK_MENU_BOOKMARK_ELEMS];
static SimpleMenuItem bookmark_menu_recent_items[NUM_BOOKMARK_MENU_RECENT_ELEMS];

/*
 *
 *      RECENT PLACES MENU DECLARATIONS
 *
 */
static Window* recent_menu_window;

static SimpleMenuLayer* recent_menu_layer;
static SimpleMenuSection recent_menu_sections[NUM_RECENT_MENU_SECTIONS];

static SimpleMenuItem recent_menu_items[NUM_RECENT_MENU_RECENT_ELEMS];

/*
 *
 *      LOCATION WINDOW DECLARATIONS
 *
 */
static Window* snooze_window;
static ActionBarLayer* snooze_action_layer;
static TextLayer* address_text_layer;
static TextLayer* distance_text_layer;
static TextLayer* timer_text_layer;

static bool reset = false;
static struct tm start_time;
static char dist_text[8];
char names[NUM_BOOKMARK_MENU_BOOKMARK_ELEMS + NUM_RECENT_MENU_RECENT_ELEMS][MAX_CHARS];

AppSync sync;
uint8_t sync_buffer[32];

/*
 *
 *      TICK HANDLER
 *
 */

static void
bookmark_menu_select_bookmark_callback(int index, void* context);

void sync_tuple_changed_callback(const uint32_t key,
        const Tuple* new_tuple, const Tuple* old_tuple, void* context)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", (int) key);
  if(key == 0)
    {
      const char* distance = new_tuple->value->cstring;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Received distance:%s", distance);
      if(distance_text_layer != NULL)
        {
          text_layer_set_text(distance_text_layer, distance);
          layer_mark_dirty(text_layer_get_layer(distance_text_layer));
        }
    }
  else if(key == 1)
    {
      const char* location = new_tuple->value->cstring;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Location:%s", location);
      if(address_text_layer != NULL)
        {
          text_layer_set_text(address_text_layer, location);
          layer_mark_dirty(text_layer_get_layer(address_text_layer));
        }
    }
  else if(key == 2)
    {
      const char* book1 = new_tuple->value->cstring;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Bookmark 1:%s", book1);

      bookmark_menu_bookmark_items[0] = (SimpleMenuItem
            )
              { .title = book1, .subtitle = bookmark_menu_bookmark_items[0].subtitle, .callback =
                  bookmark_menu_select_bookmark_callback };

      layer_mark_dirty(simple_menu_layer_get_layer(bookmark_menu_layer));
    }
  else if(key == 3)
    {
      const char* book1 = new_tuple->value->cstring;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Bookmark 1 sub:%s", book1);

      bookmark_menu_bookmark_items[0] = (SimpleMenuItem
            )
              { .title = bookmark_menu_bookmark_items[0].title, .subtitle = book1, .callback =
                  bookmark_menu_select_bookmark_callback };

      layer_mark_dirty(simple_menu_layer_get_layer(bookmark_menu_layer));
    }
}

void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context)
{

}

static void handle_tick(struct tm* tick_time, TimeUnits units_changed)
{
  if(reset)
    {
      start_time.tm_hour = tick_time->tm_hour;
      start_time.tm_min = tick_time->tm_min;
      start_time.tm_sec = tick_time->tm_sec;
      reset = !reset;
    }

  static char* time;
  if (time != NULL)
    free(time);
  time = malloc(10);
  tick_time->tm_hour -= start_time.tm_hour;
  tick_time->tm_min -= start_time.tm_min;
  tick_time->tm_sec -= start_time.tm_sec;
  strftime(time, 10, "%H:%M:%S", tick_time);
  text_layer_set_text(timer_text_layer, time);
  layer_mark_dirty(text_layer_get_layer(timer_text_layer));
}

/*
 *
 *
 *      BOOKMARK MENU FUNCTIONS
 *
 *
 */
static void
bookmark_menu_select_bookmark_callback(int index, void* context)
{
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  Tuplet value = TupletInteger(1, index);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();

  window_stack_push(snooze_window, true);
  reset = true;
}

static void
bookmark_menu_select_recent_callback(int index, void* context)
{
  window_stack_push(recent_menu_window, true);
}

static void
bookmark_menu_window_load(Window* window)
{
  int num_items = 0;

  bookmark_menu_bookmark_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Home", .subtitle = "SW6 4YF", .callback =
              bookmark_menu_select_bookmark_callback };

  bookmark_menu_bookmark_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Work", .subtitle = "W6 9NJ", .callback =
              bookmark_menu_select_bookmark_callback };

  bookmark_menu_bookmark_items[num_items++] = (SimpleMenuItem
        )
          { .title = "University", .subtitle = "SW7 2AZ", .callback =
              bookmark_menu_select_bookmark_callback };

  bookmark_menu_recent_items[0] = (SimpleMenuItem
        )
          { .title = "Recent Places", .subtitle = "Recently used locations", .callback =
              bookmark_menu_select_recent_callback, };

  bookmark_menu_sections[0] = (SimpleMenuSection
        )
          { .title = "Bookmarks", .num_items = NUM_BOOKMARK_MENU_BOOKMARK_ELEMS,
              .items = bookmark_menu_bookmark_items, };

  bookmark_menu_sections[1] =
      (SimpleMenuSection
            )
              { .title = "Recent Places", .num_items =
              NUM_BOOKMARK_MENU_RECENT_ELEMS, .items =
                  bookmark_menu_recent_items, };

  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  bookmark_menu_layer = simple_menu_layer_create(bounds, window,
      bookmark_menu_sections, NUM_BOOKMARK_MENU_SECTIONS, NULL);

  Tuplet new[] = {};
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), new, 0,
        sync_tuple_changed_callback, sync_error_callback, NULL);

  layer_add_child(window_layer,
      simple_menu_layer_get_layer(bookmark_menu_layer));
}

static void
bookmark_menu_window_unload(Window* window)
{
  app_sync_deinit(&sync);
  simple_menu_layer_destroy(bookmark_menu_layer);
}

/*
 *
 *
 *      RECENT PLACES MENU FUNCTIONS
 *
 *
 */

static void
recent_menu_select_callback(int index, void* context)
{
}

static void
recent_menu_window_load(Window* window)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "recent_menu_load");
  int num_items = 0;

  recent_menu_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Recent 1", .subtitle = "SW14 6TY",
              .callback = recent_menu_select_callback, };

  recent_menu_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Recent 2", .subtitle = "W4 6NU",
              .callback = recent_menu_select_callback, };

  recent_menu_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Recent 3", .subtitle = "W6 5HP",
              .callback = recent_menu_select_callback, };

  recent_menu_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Recent 4", .subtitle = "BT34 1QN",
              .callback = recent_menu_select_callback, };

  recent_menu_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Recent 5", .subtitle = "BT35 6TJ",
              .callback = recent_menu_select_callback, };

  recent_menu_sections[0] = (SimpleMenuSection
        )
          { .title = "Recent Places", .num_items = NUM_RECENT_MENU_RECENT_ELEMS,
              .items = recent_menu_items };

  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  recent_menu_layer = simple_menu_layer_create(bounds, window,
      recent_menu_sections, NUM_RECENT_MENU_SECTIONS, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(recent_menu_layer));
}

static void
recent_menu_window_unload(Window* window)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "recent_menu_unload");
  simple_menu_layer_destroy(recent_menu_layer);
}

/*
 *
 *
 *      LOCATION WINDOW FUNCTIONS
 *
 *
 */

static void snooze_window_unload(Window* window);

static void
action_layer_bookmark_cancel_handler(ClickRecognizerRef recognizer,
    void* context)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Cancel");

  window_stack_pop(true);
}

static void
action_layer_click_config_provider(void *context)
{
  window_single_click_subscribe(BUTTON_ID_SELECT,
      (ClickHandler) action_layer_bookmark_cancel_handler);
}

static void
snooze_window_load(Window* window)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "snooze_load");
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  address_text_layer = text_layer_create(GRect(5, 5, bounds.size.w - 20, 72));
  text_layer_set_text_alignment(address_text_layer, GTextAlignmentCenter);
  text_layer_set_font(address_text_layer,
      fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(address_text_layer));

  distance_text_layer = text_layer_create(GRect(5, 50, bounds.size.w - 20, 50));
  text_layer_set_text_alignment(distance_text_layer, GTextAlignmentCenter);
  text_layer_set_font(distance_text_layer,
      fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text(distance_text_layer, "0 km");
  layer_add_child(window_layer, text_layer_get_layer(distance_text_layer));

  timer_text_layer = text_layer_create(GRect(5, 110, bounds.size.w - 20, 100));
  text_layer_set_text_alignment(timer_text_layer, GTextAlignmentCenter);
  text_layer_set_font(timer_text_layer,
      fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(timer_text_layer));

  snooze_action_layer = action_bar_layer_create();
  action_bar_layer_add_to_window(snooze_action_layer, window);

  action_bar_layer_set_click_config_provider(snooze_action_layer,
      action_layer_click_config_provider);

   action_bar_layer_set_icon(snooze_action_layer, BUTTON_ID_SELECT,
       gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_1));

   layer_add_child(window_layer, action_bar_layer_get_layer(snooze_action_layer));

   tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

static void
snooze_window_unload(Window* window)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "snooze_unload");
  action_bar_layer_destroy(snooze_action_layer);
  text_layer_destroy(address_text_layer);
  text_layer_destroy(timer_text_layer);
  text_layer_destroy(distance_text_layer);

  tick_timer_service_unsubscribe();
}

/*
 *
 *
 *      API MESSAGE HANDLERS
 *
 *
 */

void
out_sent_handler(DictionaryIterator *sent, void *context)
{
  // outgoing message was delivered
}

void
out_failed_handler(DictionaryIterator *failed, AppMessageResult reason,
    void *context)
{
  // outgoing message failed
}

void
in_dropped_handler(AppMessageResult reason, void *context)
{
  // incoming message dropped
}

enum
{
  AKEY_NUMBER, AKEY_NAME, AKEY_ADDRESS,
};


static void
in_received_handler(DictionaryIterator *iter, void *context)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received");

  Tuple *name_tuple = dict_find(iter, AKEY_NAME);
  Tuple *add_tuple = dict_find(iter, AKEY_ADDRESS);
  Tuple *dist_tuple = dict_find(iter, AKEY_NUMBER);

  if (name_tuple != NULL)
    {
      const char* name = name_tuple->value->cstring;
      bookmark_menu_bookmark_items[0] = (SimpleMenuItem
            )
              { .title = "SW6 4YF", .subtitle = "Home", .callback =
                  bookmark_menu_select_bookmark_callback };
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Name:%s", name);
    }

  if (add_tuple != NULL)
    {
      const char* add = add_tuple->value->cstring;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Add:%s", add);
    }

  if (dist_tuple != NULL)
    {
      int dist = (int) dist_tuple->value->int32;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Dist:%d", dist);

      snprintf(dist_text, 8, (dist < 1) ? "<1 km" : "%d km", dist);
      APP_LOG(APP_LOG_LEVEL_DEBUG, dist_text);

      text_layer_set_text(distance_text_layer, dist_text);
      layer_mark_dirty(text_layer_get_layer(distance_text_layer));
      if (dist == -2)
        {
          vibes_short_pulse();
        }
    }
}

/*
 *
 *
 *      INITIALISERS AND DEINITIALISERS
 *
 *
 */

void
init()
{
  bookmark_menu_window = window_create();
  window_set_window_handlers(bookmark_menu_window, (WindowHandlers
        )
          { .load = bookmark_menu_window_load, .unload =
              bookmark_menu_window_unload });

  recent_menu_window = window_create();
  window_set_window_handlers(recent_menu_window,
      (WindowHandlers
            )
              { .load = recent_menu_window_load, .unload =
                  recent_menu_window_unload });

  snooze_window = window_create();
  window_set_window_handlers(snooze_window, (WindowHandlers
        )
          { .load = snooze_window_load, .unload = snooze_window_unload });

  window_stack_push(bookmark_menu_window, true);

  app_message_open(64,64);
  /*
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);*/
}

void
de_init()
{
  window_destroy(snooze_window);
  window_destroy(recent_menu_window);
  window_destroy(bookmark_menu_window);
}

int
main(void)
{
  init();
  app_event_loop();
  de_init();
}
