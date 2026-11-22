#define _CRT_SECURE_NO_WARNINGS
#include "entity.h"
#include <stdlib.h>

// Инициализация массива сущностей на этаже
void entity_init_floor(Entity** entities, int* count, int floor_depth, int map_width, int map_height, char** map) {
    int max_entities = map_width * map_height;
    *entities = (Entity*)malloc(max_entities * sizeof(Entity));
    if (!*entities) {
        *count = 0;
        return;
    }

    *count = 0;

    for (int y = 1; y < map_height - 1; y++) {
        for (int x = 1; x < map_width - 1; x++) {
            if (map[y][x] == FLOOR || map[y][x] == FLOOR_CRACKED) {

                // Шанс появления объекта на клетке (поднимаем до 25-30%)
                // Рандом 0-99, если выпадает меньше 28, ставим объект
                if (rand() % 100 < 35) {
                    Entity* e = &(*entities)[*count];
                    e->is_alive = 1;
                    e->x = x;
                    e->y = y;

                    int r = rand() % 100;

                    // Распределение: 45% деревья, 35% камни, 10% аптечки, 10% монстры
                    if (r < 45) { // Дерево
                        e->type = ENT_TREE;
                        e->symbol = TREE;
                        e->max_hp = 20 + floor_depth * 5;
                        e->hp = e->max_hp;
                        e->attack = 0;
                        e->drop_item_id = 1;
                    }
                    else if (r < 80) { // Камень
                        e->type = ENT_ROCK;
                        e->symbol = ROCK;
                        e->max_hp = 30 + floor_depth * 5;
                        e->hp = e->max_hp;
                        e->attack = 0;
                        e->drop_item_id = 2;
                    }
                    else if (r < 90) { // Аптечка
                        e->type = ENT_HEAL;
                        e->symbol = '+';
                        e->max_hp = 1;
                        e->hp = 1;
                        e->attack = 0;
                        e->drop_item_id = 0;
                    }
                    else { // Монстр
                        e->type = ENT_MONSTER;
                        e->symbol = 'M';
                        e->max_hp = 10 + floor_depth * 5;
                        e->hp = e->max_hp;
                        e->attack = 3 + floor_depth;
                        e->drop_item_id = 8;
                    }

                    // Ставим символ на карту
                    map[y][x] = e->symbol;
                    (*count)++;
                }
            }
        }
    }
}

void entity_free(Entity** entities, int count) {
    if (!entities || !*entities) return;
    printf("Память массива сущностей по адресу %p освобождена\n", (void*)*entities);
    free(*entities);
    *entities = NULL;
}

Entity* entity_get_at(Entity* entities, int count, int x, int y) {
    for (int i = 0; i < count; i++) {
        if (entities[i].is_alive && entities[i].x == x && entities[i].y == y) {
            return &entities[i];
        }
    }
    return NULL;
}