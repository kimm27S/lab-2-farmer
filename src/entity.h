#ifndef ENTITY_H
#define ENTITY_H

#include "utils.h"
#include "player.h"

typedef enum {
    ENT_NONE = 0,
    ENT_TREE,    // Дерево (T)
    ENT_ROCK,    // Камень (R)
    ENT_HEAL,    // Аптечка (+)
    ENT_MONSTER  // Монстр (M)
} EntityType;

typedef struct {
    EntityType type;
    char symbol;       // 'T', 'R', '+', 'M'
    int x, y;          // Координаты на карте
    int hp, max_hp;    // Прочность ресурса или здоровье зверя
    int attack;        // Урон (для монстров)
    int drop_item_id;  // Какой ID предмета выпадет
    int is_alive;      // 1 - существует, 0 - уничтожен
} Entity;

// Прототипы функций для работы с сущностями
void entity_init_floor(Entity** entities, int* count, int floor_depth, int map_width, int map_height, char** map);
void entity_free(Entity** entities, int count);
Entity* entity_get_at(Entity* entities, int count, int x, int y);

#endif // ENTITY_H
