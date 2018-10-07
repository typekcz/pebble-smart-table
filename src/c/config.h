#pragma once

#include <pebble.h>

#include "week_window.h"

#define NBGCOLOR GColorWhite
#define NFGCOLOR GColorCobaltBlue
#define HBGCOLOR GColorCobaltBlue
#define HFGCOLOR GColorWhite

#define SETTINGS_KEY_TABLE_MON 1
#define SETTINGS_KEY_TABLE_TUE 2
#define SETTINGS_KEY_TABLE_WED 3
#define SETTINGS_KEY_TABLE_THU 4
#define SETTINGS_KEY_TABLE_FRI 5
#define SETTINGS_KEY_TABLE_SAT 6
#define SETTINGS_KEY_TABLE_SUN 7

#define MON 0
#define TUE 1
#define WED 2
#define THU 3
#define FRI 4
#define SAT 5
#define SUN 6

typedef struct TableItem TableItem;
struct TableItem {
	const char* name;
	const char* detail;
	int start_hour;
	int start_minute;
	int end_hour;
	int end_minute;

	TableItem* next;
};

typedef struct Table {
	TableItem* days[7];
} Table;

extern Table* config_table;

extern const char* short_days[];
extern const char* long_days[];

void config_init();
void config_deinit();
