#include <pebble.h>

#include "config.h"
#include "week_window.h"
#include "config_window.h"

static void prv_init(void) {
	config_init();

	if(config_table != NULL){
		for(int i = 0; i < 7; i++){
			if(config_table->days[i] != NULL){
				week_window_push();
				return;
			}
		}
	}
	dialog_config_window_push();
}

static void prv_deinit(void) {
	config_deinit();
}

int main(void) {
	prv_init();
	app_event_loop();
	prv_deinit();
}
