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

  bookmark_menu_bookmark_items[num_items++] = (SimpleMenuItem) {
    .title = "Home",
    .subtitle = "Take me home!",
    .callback = bookmark_menu_select_bookmark_callback,
  };

  bookmark_menu_bookmark_items[num_items++] = (SimpleMenuItem) {
      .title = "Work",
      .subtitle = "Oh no!",
      .callback = bookmark_menu_select_bookmark_callback,
    };

  bookmark_menu_bookmark_items[num_items++] = (SimpleMenuItem) {
      .title = "University",
      .subtitle = "Yay Pintos!",
      .callback = bookmark_menu_select_bookmark_callback,
    };

  bookmark_menu_recent_items[0] = (SimpleMenuItem) {
      .title = "Recent Places",
      .subtitle = "I was drunk!",
      .callback = bookmark_menu_select_recent_callback,
    };

  bookmark_menu_sections[0] = (SimpleMenuSection) {
    .title = "Bookmarks",
    .num_items = NUM_BOOKMARK_MENU_BOOKMARK_ELEMS,
    .items = bookmark_menu_bookmark_items,
  };

  bookmark_menu_sections[1] = (SimpleMenuSection) {
      .title = "Recent Places!!",
      .num_items = NUM_BOOKMARK_MENU_RECENT_ELEMS,
      .items = bookmark_menu_recent_items,
    };

  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  bookmark_menu_layer = simple_menu_layer_create(bounds, window, bookmark_menu_sections, NUM_BOOKMARK_MENU_SECTIONS, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(bookmark_menu_layer));
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

  recent_menu_items[num_items++] = (SimpleMenuItem) {
    .title = "Recent 1",
    .subtitle = "You were here before!",
    .callback = recent_menu_select_callback,
  };

  recent_menu_items[num_items++] = (SimpleMenuItem) {
    .title = "Recent 2",
    .subtitle = "You were here before.",
    .callback = recent_menu_select_callback,
  };

  recent_menu_items[num_items++] = (SimpleMenuItem) {
    .title = "Recent 3",
    .subtitle = "You were here before.",
    .callback = recent_menu_select_callback,
  };

  recent_menu_items[num_items++] = (SimpleMenuItem) {
    .title = "Recent 4",
    .subtitle = "You were here before.",
    .callback = recent_menu_select_callback,
  };

  recent_menu_items[num_items++] = (SimpleMenuItem) {
    .title = "Recent 5",
    .subtitle = "You were here before.",
    .callback = recent_menu_select_callback,
  };

  recent_menu_sections[0] = (SimpleMenuSection) {
    .title = "Recent Places",
    .num_items = NUM_RECENT_MENU_RECENT_ELEMS,
    .items = recent_menu_items
  };

  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  recent_menu_layer = simple_menu_layer_create(bounds, window, recent_menu_sections, NUM_RECENT_MENU_SECTIONS, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(recent_menu_layer));
}

static void
recent_menu_window_unload(Window* window)
{
  simple_menu_layer_destroy(recent_menu_layer);
}

void
init()
{
  bookmark_menu_window = window_create();
  window_set_window_handlers(bookmark_menu_window, (WindowHandlers) {
    .load = bookmark_menu_window_load,
    .unload = bookmark_menu_window_unload
  });

  recent_menu_window = window_create();
  window_set_window_handlers(recent_menu_window, (WindowHandlers) {
    .load = recent_menu_window_load,
    .unload = recent_menu_window_unload
  });
  window_stack_push(bookmark_menu_window, true);
}

void
de_init()
{
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
