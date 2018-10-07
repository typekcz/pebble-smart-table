#include "day_window.h"

static Window *s_main_window;
static MenuLayer *s_menu_layer;
static int selectedDay = 0;
//static TextLayer *s_list_message_layer;

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
	if(config_table == NULL)
		return 0;
	int rows = 0;
	TableItem* i = config_table->days[selectedDay];
	while(i != NULL){
		rows++;
		i = i->next;
	}
	return rows;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
	uint16_t i = 0;
	TableItem* item = config_table->days[selectedDay];
	while(item != NULL && i < cell_index->row){
		i++;
		item = item->next;
	}
	//menu_cell_basic_draw(ctx, cell_layer, item->name, item->detail, NULL);
	#define MENU_CELL_PADDING ((const int16_t) 5)
	#ifdef PBL_RECT
	#define MENU_DEFAULT_TEXT_ALIGNMENT GTextAlignmentLeft
	#else
	#define MENU_DEFAULT_TEXT_ALIGNMENT GTextAlignmentCenter
	#endif

	GRect frame = layer_get_frame(cell_layer);
	int16_t leftPad = 30;
	int16_t x = MENU_CELL_PADDING + frame.origin.x + leftPad;

	GFont title_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
    GRect title_rect = GRect(x, frame.size.h / 2 - 26, frame.size.w - x - MENU_CELL_PADDING, frame.size.h);
	graphics_draw_text(ctx, item->name, title_font, title_rect, GTextOverflowModeTrailingEllipsis, MENU_DEFAULT_TEXT_ALIGNMENT, 0);

	GFont subtitle_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);
	GRect subtitle_rect = GRect(x, frame.size.h / 2 - 2, frame.size.w - x - MENU_CELL_PADDING, frame.size.h);
    graphics_draw_text(ctx, item->detail, subtitle_font, subtitle_rect, GTextOverflowModeTrailingEllipsis, MENU_DEFAULT_TEXT_ALIGNMENT, 0);

	char* time_from = (char*)malloc(sizeof(char)*6);
	char* time_to = (char*)malloc(sizeof(char)*6);
	snprintf(time_from, 6, "%d:%02d", item->start_hour, item->start_minute);
	snprintf(time_to, 6, "%d:%02d", item->end_hour, item->end_minute);
	GFont time_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);

	GRect time_rect = GRect(x - leftPad, frame.size.h / 2 - 15, leftPad, frame.size.h);
	graphics_draw_text(ctx, time_from, time_font, time_rect, GTextOverflowModeTrailingEllipsis, MENU_DEFAULT_TEXT_ALIGNMENT, 0);

	time_rect = GRect(x - leftPad, frame.size.h / 2 - 1, leftPad, frame.size.h);
	graphics_draw_text(ctx, time_to, time_font, time_rect, GTextOverflowModeTrailingEllipsis, MENU_DEFAULT_TEXT_ALIGNMENT, 0);

	free(time_from);
	free(time_to);
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
	return //PBL_IF_ROUND_ELSE(
		//menu_layer_is_index_selected(menu_layer, cell_index) ?
		//	MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
		DAY_WINDOW_CELL_HEIGHT//)
		;
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	s_menu_layer = menu_layer_create(GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.h));
	menu_layer_set_normal_colors(s_menu_layer, NBGCOLOR, NFGCOLOR);
	menu_layer_set_highlight_colors(s_menu_layer, HBGCOLOR, HFGCOLOR);
	menu_layer_set_click_config_onto_window(s_menu_layer, window);
	menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
			.get_num_rows = get_num_rows_callback,
			.draw_row = draw_row_callback,
			.get_cell_height = get_cell_height_callback,
	});
	layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
	menu_layer_destroy(s_menu_layer);
	//text_layer_destroy(s_list_message_layer);

	window_destroy(window);
	s_main_window = NULL;
}

void day_window_push(int day, struct tm* t) {
	selectedDay = day;
	if(!s_main_window) {
		s_main_window = window_create();
		window_set_window_handlers(s_main_window, (WindowHandlers) {
				.load = window_load,
				.unload = window_unload,
		});
	}
	window_stack_push(s_main_window, true);
	if(t != NULL){
		uint16_t i = 0;
		TableItem* item = config_table->days[selectedDay];
		while(item != NULL && item->end_hour < t->tm_hour && (item->end_hour < t->tm_hour || item->end_minute < t->tm_min)){
			i++;
			item = item->next;
		}
		MenuIndex mi = {0, i};
		menu_layer_set_selected_index(s_menu_layer, mi, MenuRowAlignCenter, false);
	}
}