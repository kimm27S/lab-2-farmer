#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"

typedef enum {
    MENU_EXIT = 0,
    MENU_CREATE_PLAYER,
    MENU_CARD,
    MENU_TIME_MENU,
    MENU_INVENTORY,
    MENU_ADD_ITEM,
    MENU_DROP_ITEM,
    MENU_UNIQUE_ITEMS,
    MENU_FIND_ITEM,
    MENU_DIARY,
    MENU_ENCYCLOPEDIA,
    MENU_ADVENTURE
} MenuAction;

typedef struct Player {
    char name[FARMER_NAME_LEN];
    int  hp;
    int  max_hp;
    int  stamina;
    int  max_stamina;
    int  gold;
    int  x, y;
    int  inventory[INVENTORY_SIZE];
} Player;

extern char g_item_names[MAX_ITEMS][MAX_NAME_LEN];
void load_items(void);

Player* player_create(void);
void    player_destroy(Player** p);
void    player_print_card(const Player* p, int day, int hour);
void    player_print_inventory(const Player* p);

void player_add_item(Player* p);
void player_drop_item(Player* p);
void player_print_unique(const Player* p);
void player_find_item(const Player* p);

void player_write_diary(const Player* p, int day, int hour);

void player_rest(Player* p);

#endif /* PLAYER_H */
