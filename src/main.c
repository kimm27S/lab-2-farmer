#define _CRT_SECURE_NO_WARNINGS
#include "utils.h"
#include "player.h"
#include "map.h"
#include "weather.h"
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

static void time_menu(Player* p, int* day, int* hour) {
    while (1) {
        printf("\n--- Время ---\n");
        printf("[1] Посмотреть на часы\n");
        printf("[2] Ждать (промотать часы)\n");
        printf("[3] Лечь спать\n");
        printf("[0] Назад\n");
        int ch = read_int_safe("Выбор: ", 0, 3);

        if (ch == 0) break;

        if (ch == 1) {
            printf("Сейчас: ");
            print_time(*day, *hour);
            printf("\n");
        }
        else if (ch == 2) {
            int hours = read_int_safe("Сколько часов ждать (1..23): ", 1, 23);
            int next = *hour + hours;
            int next_day = *day;
            while (next >= 24) { next -= 24; next_day++; }

            int cur = *hour;
            int crossed = 0;
            for (int i = 0; i < hours; i++) {
                cur++;
                if (cur >= 24) cur -= 24;
                if (cur == NIGHT_HOUR) { crossed = 1; break; }
            }
            if (crossed) {
                printf("[!] Нельзя промотать время через 02:00 ночи — сначала поспите.\n");
                continue;
            }
            *hour = next;
            *day = next_day;
            printf("Прошло %d часов. Сейчас: ", hours);
            print_time(*day, *hour);
            printf("\n");
        }
        else if (ch == 3) {
            if (!p) { printf("[!] Сначала создайте персонажа.\n"); continue; }
            *day += 1;
            *hour = MORNING_HOUR;
            player_rest(p);
            printf("[+] Утро! День %d, %02d:00\n", *day, *hour);
        }
    }
}

int main(void) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    srand((unsigned)time(NULL));

    int current_day = 1;
    int current_hour = 8;

    Player* player = NULL;
    Weather weather = { WEATHER_CLEAR, 20, 0 };

    load_items();

    while (1) {
        printf("\n=== Главное меню (%s) ===\n",
            player ? player->name : "персонаж не создан");
        printf("[0]  Выход\n");
        printf("[1]  Создать / пересоздать персонажа\n");
        printf("[2]  Карточка персонажа\n");
        printf("[3]  Время (часы / ждать / спать)\n");
        printf("[4]  Посмотреть инвентарь\n");
        printf("[5]  Положить предмет\n");
        printf("[6]  Выбросить предмет\n");
        printf("[7]  Уникальные находки\n");
        printf("[8]  Поиск предмета по названию\n");
        printf("[9]  Записать состояние в дневник\n");
        printf("[10] Расшифровать старые записи (энциклопедия)\n");
        printf("[11] Отправиться в приключение\n");

        int choice = read_int_safe("Ваш выбор: ", 0, 11);

        switch (choice) {
        case MENU_EXIT:
            if (player) player_destroy(&player);
            printf("Выход.\n");
            return 0;

        case MENU_CREATE_PLAYER:
            if (player) {
                int ok = read_int_safe("Персонаж уже существует. Удалить и создать заново? [1=да, 0=нет]: ", 0, 1);
                if (!ok) break;
                player_destroy(&player);
            }
            player = player_create();
            break;

        case MENU_CARD:
            if (!player) { printf("[!] Персонаж не создан.\n"); break; }
            player_print_card(player, current_day, current_hour);
            break;

        case MENU_TIME_MENU:
            time_menu(player, &current_day, &current_hour);
            break;

        case MENU_INVENTORY:
            if (!player) { printf("[!] Персонаж не создан.\n"); break; }
            player_print_inventory(player);
            break;

        case MENU_ADD_ITEM:
            if (!player) { printf("[!] Персонаж не создан.\n"); break; }
            player_add_item(player);
            break;

        case MENU_DROP_ITEM:
            if (!player) { printf("[!] Персонаж не создан.\n"); break; }
            player_drop_item(player);
            break;

        case MENU_UNIQUE_ITEMS:
            if (!player) { printf("[!] Персонаж не создан.\n"); break; }
            player_print_unique(player);
            break;

        case MENU_FIND_ITEM:
            if (!player) { printf("[!] Персонаж не создан.\n"); break; }
            player_find_item(player);
            break;

        case MENU_DIARY:
            if (!player) { printf("[!] Персонаж не создан.\n"); break; }
            player_write_diary(player, current_day, current_hour);
            break;

        case MENU_ENCYCLOPEDIA:
            encyclopedia_search();
            break;

        case MENU_ADVENTURE:
            if (!player) { printf("[!] Персонаж не создан.\n"); break; }
            adventure_run(player, &current_day, &current_hour, &weather);
            break;

        default:
            printf("Неверный пункт меню!\n");
        }
    }
}
