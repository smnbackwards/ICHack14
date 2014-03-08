#include "pebble.h"

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

/* Action Layer code. */
static Window* snooze_window;
static ActionBarLayer* snooze_action_layer;

void
action_layer_bookmark_click_handler(ClickRecognizerRef recognizer, void* context)
{
  Window* window = (Window *) context;
}

void
action_layer_cancel_click_handler(ClickRecognizerRef recognizer, void* context)
{
  Window* window = (Window *) context;
}

void
click_config_provider(void *context)
{
  window_single_click_subscribe(BUTTON_ID_DOWN,
      (ClickHandler) action_layer_bookmark_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP,
      (ClickHandler) action_layer_cancel_click_handler);
}

static void update_layer_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_text_color(ctx, GColorBlack);

  GRect bounds = layer_get_frame(layer);

  graphics_draw_text(ctx,
      "Text here.",
      fonts_get_system_font(FONT_KEY_FONT_FALLBACK),
      GRect(5, 5, bounds.size.w-10, 100),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);

  graphics_draw_text(ctx,
      "And text here as well.",
      fonts_get_system_font(FONT_KEY_FONT_FALLBACK),
      GRect(90, 100, bounds.size.w-95, 60),
      GTextOverflowModeWordWrap,
      GTextAlignmentRight,
      NULL);
}


void
snooze_window_load(Window *window)
{
// Initialize the action bar:
  snooze_action_layer = action_bar_layer_create();
// Associate the action bar with the window:
  action_bar_layer_add_to_window(snooze_action_layer, window);
// Set the click config provider:
  action_bar_layer_set_click_config_provider(snooze_action_layer,
      click_config_provider);

  Layer *window_layer = window_get_root_layer(window);
  Layer* layer = (Layer *) snooze_action_layer;
   GRect bounds = layer_get_frame(window_layer);
   layer = layer_create(bounds);
   layer_set_update_proc(layer, update_layer_callback);
   layer_add_child(window_layer, layer);

// Set the icons:
// The loading the icons is omitted for brevity... See HeapBitmap.
  /*action_bar_layer_set_icon(snooze_action_layer, BUTTON_ID_UP,
      &my_icon_previous);
  action_bar_layer_set_icon(snooze_action_layer, BUTTON_ID_DOWN, &my_icon_next);*/
}

/* Bookmark menu items. */
static Window* bookmark_menu_window;

static SimpleMenuLayer* bookmark_menu_layer;
static SimpleMenuSection bookmark_menu_sections[NUM_BOOKMARK_MENU_SECTIONS];

static SimpleMenuItem bookmark_menu_bookmark_items[NUM_BOOKMARK_MENU_BOOKMARK_ELEMS];
static SimpleMenuItem bookmark_menu_recent_items[NUM_BOOKMARK_MENU_RECENT_ELEMS];

/* Recent places menu items. */
static Window* recent_menu_window;

static SimpleMenuLayer* recent_menu_layer;
static SimpleMenuSection recent_menu_sections[NUM_RECENT_MENU_SECTIONS];

static SimpleMenuItem recent_menu_items[NUM_RECENT_MENU_RECENT_ELEMS];

/* Bookmark menu init functions. */
static void
bookmark_menu_select_bookmark_callback(int index, void* context)
{
  window_stack_push(snooze_window, true);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  Tuplet value = TupletCString(1, "Stephen the Banana King");
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();

  bookmark_menu_bookmark_items[index].subtitle = "Hello there.";
  layer_mark_dirty(simple_menu_layer_get_layer(bookmark_menu_layer));
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
          { .title = "Home", .subtitle = "Take me home!", .callback =
              bookmark_menu_select_bookmark_callback, };

  bookmark_menu_bookmark_items[num_items++] = (SimpleMenuItem
        )
          { .title = "Work", .subtitle = "Oh no!", .callback =
              bookmark_menu_select_bookmark_callback, };

  bookmark_menu_bookmark_items[num_items++] = (SimpleMenuItem
        )
          { .title = "University", .subtitle = "Yay Pintos!", .callback =
              bookmark_menu_select_bookmark_callback, };

  bookmark_menu_recent_items[0] = (SimpleMenuItem
        )
          { .title = "Recent Places", .subtitle = "I was drunk!", .callback =
              bookmark_menu_select_recent_callback, };

  bookmark_menu_sections[0] = (SimpleMenuSection
        )
          { .title = "Bookmarks", .num_items = NUM_BOOKMARK_MENU_BOOKMARK_ELEMS,
              .items = bookmark_menu_bookmark_items, };

  bookmark_menu_sections[1] = (SimpleMenuSection
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

/* Recent places menu init functions. */
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
  simple_menu_layer_destroy(recent_menu_layer);
}

/* AppMessage API */
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

/*void
in_received_handler(DictionaryIterator *received, void *context)
{
  // incoming message received
}*/

void
in_dropped_handler(AppMessageResult reason, void *context)
{
  // incoming message dropped
}

enum
{
  AKEY_NUMBER, AKEY_TEXT,
};

static void
in_received_handler(DictionaryIterator *iter, void *context)
{
  vibes_short_pulse();
  return;

  // Check for fields you expect to receive
  Tuple *text_tuple = dict_find(iter, AKEY_TEXT);

  // Act on the found fields received
  if (text_tuple)
    {
      vibes_short_pulse();
    }
}

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
  window_set_window_handlers(snooze_window, (WindowHandlers) {
   .load = snooze_window_load
  });

  window_stack_push(bookmark_menu_window, true);

  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);

  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
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
  if(snooze_window == NULL)
  init();
  app_event_loop();
  de_init();
}
