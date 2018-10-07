#include "week_window.h"

static Window *s_main_window;
static MenuLayer *s_menu_layer;
int *dayArray = NULL;
//static TextLayer *s_list_message_layer;

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
	if(config_table == NULL)
		return 0;
	int rows = 0;
	for(int d = 0; d < 7; d++){
		if(config_table->days[d] != NULL)
			rows++;
	}
	return rows;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
	uint16_t i = 0;
	for(int d = 0; d < 7; d++){
		if(config_table->days[d] != NULL){
			if(i == cell_index->row){
				menu_cell_basic_draw(ctx, cell_layer, long_days[d], NULL, NULL);
				return;
			} else 
				i++;
		}
	}
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
	return PBL_IF_ROUND_ELSE(
		menu_layer_is_index_selected(menu_layer, cell_index) ?
			MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
		WEEK_WINDOW_CELL_HEIGHT);
}

static void select_click_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
	uint16_t i = 0;
	for(int d = 0; d < 7; d++){
		if(config_table->days[d] != NULL){
			if(i == cell_index->row){
				day_window_push(d, NULL);
				return;
			} else 
				i++;
		}
	}
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
			.select_click = select_click_callback
	});
	layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
	menu_layer_destroy(s_menu_layer);

	window_destroy(window);
	s_main_window = NULL;
}

void week_window_push() {
	if(!s_main_window) {
		s_main_window = window_create();
		window_set_window_handlers(s_main_window, (WindowHandlers) {
				.load = window_load,
				.unload = window_unload,
		});
	}
	window_stack_push(s_main_window, true);

	time_t tt = time(NULL);
	struct tm* t = localtime(&tt);
	int day = t->tm_wday;
	if(day == 0)
		day = 6;
	else
		day--;

	if(config_table->days[day] != NULL){
		uint16_t i = 0;
		for(int d = 0; d < 7; d++){
			if(config_table->days[d] != NULL){
				if(d == day){
					break;
				} else 
					i++;
			}
		}
		MenuIndex mi = {0, i};
		menu_layer_set_selected_index(s_menu_layer, mi, MenuRowAlignCenter, false);
		day_window_push(day, t);
	}
}