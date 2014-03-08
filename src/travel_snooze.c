#include "pebble.h"

#define NUM_MENU_SECTIONS 2
#define NUM_RECENT_MENU_SECT 1
#define NUM_BOOKMARKS 3
#define NUM_RECENT 5

static bool recent_menu_flag = false;

static Window* main_window;
static TextLayer* text_layer;

static SimpleMenuLayer* bookmark_menu_layer;
static SimpleMenuSection menu_sections[NUM_MENU_SECTIONS];

static SimpleMenuItem bookmark_menu_items[NUM_BOOKMARKS];
static SimpleMenuItem recent_menu_items[NUM_RECENT_MENU_SECT];

static void
bookmark_menu_select_callback(int index, void* context)
{
  bookmark_menu_items[index].subtitle = "Hello there.";
  layer_mark_dirty(simple_menu_layer_get_layer(bookmark_menu_layer));
}

static void
recent_menu_select_callback(int index, void* context)
{

}

static void
window_load(Window* window)
{
  int num_items = 0;

  bookmark_menu_items[num_items++] = (SimpleMenuItem) {
    .title = "Home",
    .subtitle = "Take me home!",
    .callback = bookmark_menu_select_callback,
  };

  bookmark_menu_items[num_items++] = (SimpleMenuItem) {
      .title = "Work",
      .subtitle = "Oh no!",
      .callback = bookmark_menu_select_callback,
    };

  bookmark_menu_items[num_items++] = (SimpleMenuItem) {
      .title = "University",
      .subtitle = "Yay Pintos!",
      .callback = bookmark_menu_select_callback,
    };

  recent_menu_items[0] = (SimpleMenuItem) {
      .title = "Recent Places",
      .subtitle = "I was drunk!",
      .callback = recent_menu_select_callback,
    };

  menu_sections[0] = (SimpleMenuSection) {
    .title = "Bookmarks",
    .num_items = NUM_BOOKMARKS,
    .items = bookmark_menu_items,
  };

  menu_sections[1] = (SimpleMenuSection) {
      .title = "Recent Places!!",
      .num_items = NUM_RECENT_MENU_SECT,
      .items = recent_menu_items,
    };

  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  bookmark_menu_layer = simple_menu_layer_create(bounds, window, menu_sections, NUM_MENU_SECTIONS, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(bookmark_menu_layer));
}

static void
window_unload(Window* window)
{
  simple_menu_layer_destroy(bookmark_menu_layer);
}

void
init()
{
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });

  window_stack_push(main_window, true);
}

void
de_init()
{
  window_destroy(main_window);
}

int
main(void)
{
  init();
  app_event_loop();
  de_init();
}
