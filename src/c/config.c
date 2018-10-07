#include "config.h"

Table* config_table = NULL;

const char* short_days[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
const char* long_days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

void init_table();
void config_init();
void config_parse_table(const char* input, int day);
char* parse_str_until(int* i, const char* input, char c);
int parse_number(const char* input, int* i);
TableItem* parse_subject(int* i, const char* input);
static void message_inbox_handler(DictionaryIterator *iter, void *context);
void load_persistent(uint32_t key, int day);
void build_index();


void init_table(){
	config_table = (Table*)malloc(sizeof(Table));
	for(int d = 0; d < 7; d++)
		config_table->days[d] = NULL;
}

void destroy_table_item(TableItem* item){
	if(item == NULL)
		return;
	if(item->next != NULL)
		destroy_table_item(item->next);

	if(item->name != NULL){
		free((char*)item->name);
		item->name = NULL;
	}
	if(item->detail != NULL){
		free((char*)item->detail);
		item->detail = NULL;
	}

	free(item);
}

void config_init(){
	init_table();
	load_persistent(SETTINGS_KEY_TABLE_MON, 0);
	load_persistent(SETTINGS_KEY_TABLE_TUE, 1);
	load_persistent(SETTINGS_KEY_TABLE_WED, 2);
	load_persistent(SETTINGS_KEY_TABLE_THU, 3);
	load_persistent(SETTINGS_KEY_TABLE_FRI, 4);
	load_persistent(SETTINGS_KEY_TABLE_SAT, 5);
	load_persistent(SETTINGS_KEY_TABLE_SUN, 6);
	app_message_register_inbox_received(message_inbox_handler);
	app_message_open(128, 128);
}

void config_deinit(){
	for(int d = 0; d < 7; d++)
		if(config_table->days[d] != NULL){
			destroy_table_item(config_table->days[d]);
			config_table->days[d] = NULL;
		}
	free(config_table);
}

void load_persistent(uint32_t key, int day){
	int size = persist_get_size(key);
	if(size == E_DOES_NOT_EXIST)
		return;
	char* str = (char*)malloc(sizeof(char) * size);
	persist_read_string(key, str, size);
	config_parse_table(str, day);
}

static void message_inbox_handler(DictionaryIterator *iter, void *context){
	//app_log(APP_LOG_LEVEL_DEBUG, "", 1, "message");
	for(Tuple *t = dict_read_first(iter); t != NULL; t = dict_read_next(iter)){
		//app_log(APP_LOG_LEVEL_DEBUG, "", 1, "key %u", (unsigned int)t->key);
		if(t->type != TUPLE_CSTRING)
			continue;
	
		int day = 0;
		int settings_key = 0;
		if(t->key == MESSAGE_KEY_mon){
			day = 0;
			settings_key = SETTINGS_KEY_TABLE_MON;
		} else if(t->key == MESSAGE_KEY_tue){
			day = 1;
			settings_key = SETTINGS_KEY_TABLE_TUE;
		} else if(t->key == MESSAGE_KEY_wed){
			day = 2;
			settings_key = SETTINGS_KEY_TABLE_WED;
		} else if(t->key == MESSAGE_KEY_thu){
			day = 3;
			settings_key = SETTINGS_KEY_TABLE_THU;
		} else if(t->key == MESSAGE_KEY_fri){
			day = 4;
			settings_key = SETTINGS_KEY_TABLE_FRI;
		} else if(t->key == MESSAGE_KEY_sat){
			day = 5;
			settings_key = SETTINGS_KEY_TABLE_SAT;
		} else if(t->key == MESSAGE_KEY_sun){
			day = 6;
			settings_key = SETTINGS_KEY_TABLE_SUN;
		}
			
		persist_write_string(settings_key, t->value->cstring);
		config_parse_table(t->value->cstring, day);
		window_stack_pop_all(false);
		week_window_push();
	}
}

char* parse_str_until(int* i, const char* input, char c){
	int str_start = *i;
	int str_length = 0;
	while(1){
		if(input[*i] == c){
			if(str_length == 0)
				return "";

			char* str = (char*)malloc(sizeof(char) * (str_length + 1));
			strncpy(str, input + str_start, str_length);
			str[str_length] = '\0';
			(*i)++;
			return str;
		}
		switch(input[*i]){
			case '\r':
				(*i)++;
			case ';':
			case '\n':
			case '$':
				(*i)++;
			case '\0':
				return NULL;
		}
		str_length++;
		(*i)++;
	}
}

int parse_number(const char* input, int* i){
	int num = 0;
	while(input[*i] >= '0' && input[*i] <= '9'){
		num *= 10;
		num += (int)(input[*i] - '0');
		(*i)++;
	}
	return num;
}

TableItem* parse_subject(int* i, const char* input){
	TableItem* sub = (TableItem*)malloc(sizeof(TableItem));
	sub->next = NULL;

	sub->name = parse_str_until(i, input, '(');
	if(sub->name == NULL)
		return NULL;

	char* timeStr = parse_str_until(i, input, ')');
	if(timeStr == NULL)
		return NULL;

	//sscanf(timeStr, "%d:%d-%d:%d", &(sub->start_hour), &(sub->start_minute), &(sub->end_hour), &(sub->end_minute));
	sub->start_hour = sub->start_minute = sub->end_hour = sub->end_minute = 0;
	int j = 0;
	sub->start_hour = parse_number(timeStr, &j);
	j++;
	sub->start_minute = parse_number(timeStr, &j);
	j++;
	sub->end_hour = parse_number(timeStr, &j);
	j++;
	sub->end_minute = parse_number(timeStr, &j);
	free(timeStr);

	if(input[*i] == '['){
			(*i)++;
		sub->detail = parse_str_until(i, input, ']');
		if(sub->name == NULL)
			return NULL;
	}

	return sub;
}

void config_parse_table(const char* input, int day){
	int i = 0;

	destroy_table_item(config_table->days[day]);
	config_table->days[day] = NULL;

	TableItem* last_item = NULL;

	do {
		if(input[i] == ';'){
			i++;
		}
		TableItem* sub = parse_subject(&i, input);
		if(sub != NULL){
			if(last_item == NULL){
				config_table->days[day] = last_item = sub;
			} else {
				last_item->next = sub;
				last_item = sub;
			}
		}
	} while(input[i] != '\0');
}
