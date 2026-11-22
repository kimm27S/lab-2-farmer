#ifndef ENTITY_H
#define ENTITY_H

#include "utils.h"
#include "player.h"

typedef enum {
    ENT_NONE = 0,
    ENT_TREE,
    ENT_ROCK,
    ENT_HEAL,
    ENT_MONSTER
} EntityType;

typedef struct {
    EntityType type;
    char symbol;
    int x, y;
    int hp, max_hp;
    int attack;
    int drop_item_id;
    int is_alive;
} Entity;

void entity_init_floor(Entity** entities, int* count, int floor_depth, int map_width, int map_height, char** map);
void entity_free(Entity** entities, int count);
Entity* entity_get_at(Entity* entities, int count, int x, int y);

#endif
