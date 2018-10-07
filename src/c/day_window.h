#pragma once

#include <pebble.h>

#include "config.h"

#define DAY_WINDOW_CELL_HEIGHT 50

void day_window_push(int day, struct tm* t);