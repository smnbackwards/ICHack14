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
static bool measuring = false;

/*
 *
 *      TICK HANDLER
 *
 */

static void handle_tick(struct tm* tick_time, TimeUnits units_changed)
{
  if(measuring)
    {

    }
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

  Tuplet value = TupletInteger(1, 1);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();

  window_stack_push(snooze_window, true);
}

static void
bookmark_menu_select_recent_callback(int index, void* context)
{
  bookmark_menu_recent_items[index].subtitle = "You will go to recent places.";
  layer_mark_dirty(simple_menu_layer_get_layer(bookmark_menu_layer));
  window_stack_push(recent_menu_window, true);
}

static void
bookmark_menu_window_load(Window* window)
{
  int num_items = 0;

  bookmark_menu_bookmark_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Home", .subtitle = "Text Home", .callback =
              bookmark_menu_select_bookmark_callback };

  bookmark_menu_bookmark_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Work", .subtitle = "Text Work", .callback =
              bookmark_menu_select_bookmark_callback };

  bookmark_menu_bookmark_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Uni", .subtitle = "Text Uni", .callback =
              bookmark_menu_select_bookmark_callback };

  bookmark_menu_recent_items[0] = (SimpleMenuItem
        )
          { .title = "Recent Places", .subtitle = "I was drunk!", .callback =
              bookmark_menu_select_recent_callback, };

  bookmark_menu_sections[0] = (SimpleMenuSection
        )
          { .title = "Bookmarks", .num_items = NUM_BOOKMARK_MENU_BOOKMARK_ELEMS,
              .items = bookmark_menu_bookmark_items, };

  bookmark_menu_sections[1] =
      (SimpleMenuSection
            )
              { .title = "Recent Places!!", .num_items =
              NUM_BOOKMARK_MENU_RECENT_ELEMS, .items =
                  bookmark_menu_recent_items, };

  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  bookmark_menu_layer = simple_menu_layer_create(bounds, window,
      bookmark_menu_sections, NUM_BOOKMARK_MENU_SECTIONS, NULL);

  layer_add_child(window_layer,
      simple_menu_layer_get_layer(bookmark_menu_layer));
}

static void
bookmark_menu_window_unload(Window* window)
{
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
  recent_menu_items[index].subtitle = "Remember this place?";
  layer_mark_dirty(simple_menu_layer_get_layer(recent_menu_layer));
}

static void
recent_menu_window_load(Window* window)
{
  int num_items = 0;
  measuring = true;

  recent_menu_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Recent 1", .subtitle = "You were here before!",
              .callback = recent_menu_select_callback, };

  recent_menu_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Recent 2", .subtitle = "You were here before.",
              .callback = recent_menu_select_callback, };

  recent_menu_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Recent 3", .subtitle = "You were here before.",
              .callback = recent_menu_select_callback, };

  recent_menu_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Recent 4", .subtitle = "You were here before.",
              .callback = recent_menu_select_callback, };

  recent_menu_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Recent 5", .subtitle = "You were here before.",
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
  measuring = false;
  simple_menu_layer_destroy(recent_menu_layer);
}

/*
 *
 *
 *      LOCATION WINDOW FUNCTIONS
 *
 *
 */

static void
action_layer_bookmark_cancel_handler(ClickRecognizerRef recognizer,
    void* context)
{
  //Window* window = (Window *) context;

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Cancel");
}

static void
action_layer_click_config_provider(void *context)
{
  window_single_click_subscribe(BUTTON_ID_SELECT,
      (ClickHandler) action_layer_bookmark_cancel_handler);
}


static void
action_layer_update_callback(Layer *layer, GContext* ctx)
{
  /*graphics_context_set_text_color(ctx, GColorBlack);

  GRect bounds = layer_get_frame(layer);

  graphics_draw_text(ctx, "180 Queens Gate, SW7 2BB, London UK",
      fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
      GRect(5, 5, bounds.size.w - 22, 72), GTextOverflowModeWordWrap,
      GTextAlignmentCenter,
      NULL);

        graphics_draw_text(ctx, "100 km",
      fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK),
      GRect(5, 75, bounds.size.w - 22, 50), GTextOverflowModeWordWrap,
      GTextAlignmentCenter,
      NULL);

  graphics_draw_text(ctx, "1:20",
      fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK),
      GRect(5, 110, bounds.size.w - 22, 100), GTextOverflowModeWordWrap,
      GTextAlignmentCenter,
      NULL);*/
}

static void
snooze_window_load(Window* window)
{
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  address_text_layer = text_layer_create(GRect(0, 0, bounds.size.w - 20, bounds.size.h));
  text_layer_set_text_alignment(address_text_layer, GTextAlignmentCenter);
  text_layer_set_font(address_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text(address_text_layer, "Location");
  layer_add_child(window_layer, text_layer_get_layer(address_text_layer));

  //layer_set_update_proc(text_layer_get_layer(address_text_layer), action_layer_update_callback);

  snooze_action_layer = action_bar_layer_create();
  action_bar_layer_add_to_window(snooze_action_layer, window);

  action_bar_layer_set_click_config_provider(snooze_action_layer,
      action_layer_click_config_provider);

   action_bar_layer_set_icon(snooze_action_layer, BUTTON_ID_SELECT,
       gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_1));

   layer_add_child(window_layer, action_bar_layer_get_layer(snooze_action_layer));
}

static void
snooze_window_unload(Window* window)
{
  action_bar_layer_destroy(snooze_action_layer);
  text_layer_destroy(address_text_layer);
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

static void
in_received_handler(DictionaryIterator *iter, void *context)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received!");
  // Check for fields you expect to receive
  for(int i = 0; i < 16; ++i)
    {
      Tuple *text_tuple = dict_find(iter, i);

      if(text_tuple != NULL)
        {
          APP_LOG(APP_LOG_LEVEL_DEBUG, "Text: %s", text_tuple->value->cstring);

          if(i % 2 == 0)
             strncpy((char *) bookmark_menu_bookmark_items[i].title, text_tuple->value->cstring, strlen(text_tuple->value->cstring));
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

  /* TODO: Do the unload too. */
  snooze_window = window_create();
  window_set_window_handlers(snooze_window, (WindowHandlers
        )
          { .load = snooze_window_load, .unload = snooze_window_unload });

  window_stack_push(bookmark_menu_window, true);

  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);

  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);

  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

void
de_init()
{
  window_destroy(snooze_window);
  window_destroy(recent_menu_window);
  window_destroy(bookmark_menu_window);

  tick_timer_service_unsubscribe();
}

int
main(void)
{
  init();
  app_event_loop();
  de_init();
}
