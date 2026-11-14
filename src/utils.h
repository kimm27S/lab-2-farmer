#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define INVENTORY_SIZE      10
#define MAX_ITEMS           10
#define MAX_NAME_LEN        64
#define FARMER_NAME_LEN     33

#define MIN_MAP_SIZE        5
#define MAX_MAP_SIZE        30
#define OBSTACLE_MIN        15
#define OBSTACLE_MAX        30
#define MAX_ESCAPE_TRIES    1000

#define BASE_GOLD_MULTIPLIER    10
#define STEP_STAMINA_COST       1
#define FLOOR_STAMINA_COST      5
#define FLOOR_TIME_COST         1
#define NIGHT_HOUR              2
#define MORNING_HOUR            8
#define START_GOLD              0

#define WALL          '#'
#define FLOOR         '.'
#define FLOOR_CRACKED ','
#define TREE          'T'
#define ROCK          'R'
#define HERO          '@'
#define STAIR_DOWN    'D'
#define STAIR_UP      'U'
#define UNKNOWN       '?'

int  read_int_safe(const char* prompt, int min, int max);
int  read_char_choice(void);
void clear_input_buffer(void);
void print_time(int day, int hour);
void encyclopedia_search(void);

#endif
