#ifndef MAP_H
#define MAP_H

#include "utils.h"
#include "player.h"
#include "weather.h"

typedef struct Map {
    char** map;
    int    width;
    int    height;
    int    depth;
} Map;

typedef enum {
    DIR_NONE = 0,
    DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT,
    DIR_QUIT
} Direction;

Direction char_to_direction(int cmd);

Map* map_generate(int depth, int with_up_stair);
void map_destroy(Map** m);

void adventure_run(Player* p, int* day, int* hour, Weather* w);

#endif
