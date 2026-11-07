#define _CRT_SECURE_NO_WARNINGS
#include "player.h"
#include <string.h>
#include <ctype.h>

char g_item_names[MAX_ITEMS][MAX_NAME_LEN];

void load_items(void) {
    const char* defaults[MAX_ITEMS] = {
        "Пусто", "Дерево", "Камень", "Семена",
        "Лопата", "Грабли", "Меч", "Щит",
        "Бинты", "Корзина"
    };
    for (int i = 0; i < MAX_ITEMS; i++) {
        strncpy(g_item_names[i], defaults[i], MAX_NAME_LEN - 1);
        g_item_names[i][MAX_NAME_LEN - 1] = '\0';
    }

    FILE* f = fopen("items.txt", "r");
    if (!f) {
        printf("[!] Файл items.txt не найден — используются названия по умолчанию.\n");
        return;
    }

    char line[256];
    int id = 0;
    while (id < MAX_ITEMS && fgets(line, sizeof(line), f)) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';
        if (len == 0) continue;

        char* p = line;
        while (*p && isdigit((unsigned char)*p)) p++;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') { id++; continue; }

        strncpy(g_item_names[id], p, MAX_NAME_LEN - 1);
        g_item_names[id][MAX_NAME_LEN - 1] = '\0';
        id++;
    }
    fclose(f);
    printf("[+] Каталог предметов загружен из items.txt (%d записей).\n", id);
}

Player* player_create(void) {
    Player* p = (Player*)malloc(sizeof(Player));
    if (!p) {
        printf("[!] Ошибка выделения памяти под игрока.\n");
        return NULL;
    }

    printf("Введите имя фермера (до %d символов): ", FARMER_NAME_LEN - 1);
    if (fgets(p->name, sizeof(p->name), stdin)) {
        size_t len = strlen(p->name);
        while (len > 0 && (p->name[len - 1] == '\n' || p->name[len - 1] == '\r'))
            p->name[--len] = '\0';
        if (len == 0) strcpy(p->name, "Фермер");
    }
    else {
        strcpy(p->name, "Фермер");
    }

    p->max_hp = 100;
    p->hp = 100;
    p->max_stamina = 50;
    p->stamina = 50;
    p->gold = START_GOLD;
    p->x = p->y = 0;
    for (int i = 0; i < INVENTORY_SIZE; i++) p->inventory[i] = 0;

    printf("[+] Персонаж \"%s\" создан! Адрес: %p\n", p->name, (void*)p);
    return p;
}

void player_destroy(Player** pp) {
    if (!pp || !*pp) return;
    printf("Память структуры по адресу %p освобождена\n", (void*)*pp);
    free(*pp);
    *pp = NULL;
}

void player_print_inventory(const Player* p) {
    if (!p) return;
    for (int i = 0; i < INVENTORY_SIZE; i++)
        printf("  Слот %d: [%d] - %s\n",
            i, p->inventory[i], g_item_names[p->inventory[i]]);
}

void player_print_card(const Player* p, int day, int hour) {
    if (!p) { printf("[!] Персонаж не создан.\n"); return; }
    printf("\n===== КАРТОЧКА ПЕРСОНАЖА =====\n");
    printf("Имя:          %s\n", p->name);
    printf("HP:           %d/%d\n", p->hp, p->max_hp);
    printf("Выносливость: %d/%d\n", p->stamina, p->max_stamina);
    printf("Золото:       %d\n", p->gold);
    printf("Время:        ");
    print_time(day, hour);
    printf("\nИнвентарь:\n");
    player_print_inventory(p);
}

void player_add_item(Player* p) {
    if (!p) return;
    printf("0 Пусто\n");
    printf("1 Дерево\n");
    printf("2 Камень\n");
    printf("3 Семена\n");
    printf("4 Лопата\n");
    printf("5 Грабли\n");
    printf("6 Меч\n");
    printf("7 Щит\n");
    printf("8 Бинты\n");
    printf("9 Корзина\n");
    int idx = read_int_safe("Индекс слота (0-9): ", 0, INVENTORY_SIZE - 1);
    int id = read_int_safe("ID предмета (0-9): ", 0, MAX_ITEMS - 1);
    p->inventory[idx] = id;
    printf("Предмет %d (%s) положен в слот %d.\n", id, g_item_names[id], idx);
}

void player_drop_item(Player* p) {
    if (!p) return;
    int idx = read_int_safe("Индекс слота для очистки (0-9): ", 0, INVENTORY_SIZE - 1);
    printf("Выброшен предмет: %d (%s)\n",
        p->inventory[idx], g_item_names[p->inventory[idx]]);
    p->inventory[idx] = 0;
}

void player_print_unique(const Player* p) {
    if (!p) return;
    printf("Массив: ");
    for (int i = 0; i < INVENTORY_SIZE; i++) printf("%d ", p->inventory[i]);
    printf("\n\nУникальные находки:\n");

    int found_any = 0;
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (p->inventory[i] == 0) continue;
        int seen = 0;
        for (int j = 0; j < i; j++)
            if (p->inventory[j] == p->inventory[i]) { seen = 1; break; }
        if (seen) continue;

        int count = 0;
        for (int j = 0; j < INVENTORY_SIZE; j++)
            if (p->inventory[j] == p->inventory[i]) count++;

        printf("ID %d (%s): %d шт.\n",
            p->inventory[i], g_item_names[p->inventory[i]], count);
        found_any = 1;
    }
    if (!found_any) printf("Инвентарь пуст — находок нет.\n");
}

void player_find_item(const Player* p) {
    if (!p) return;
    char query[MAX_NAME_LEN];
    printf("Введите название предмета для поиска: ");
    if (scanf("%63s", query) != 1) { clear_input_buffer(); return; }
    clear_input_buffer();

    int found_id = -1;
    for (int i = 0; i < MAX_ITEMS; i++)
        if (strcmp(g_item_names[i], query) == 0) { found_id = i; break; }

    if (found_id < 0) {
        printf("Предмет \"%s\" не найден в каталоге.\n", query);
        return;
    }
    printf("ID предмета \"%s\" = %d. Ищем в рюкзаке...\n", query, found_id);
    int found_any = 0;
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (p->inventory[i] == found_id) {
            printf("  Найден в слоте %d\n", i);
            found_any = 1;
        }
    }
    if (!found_any) printf("  В рюкзаке такого предмета нет.\n");
}

void player_write_diary(const Player* p, int day, int hour) {
    if (!p) { printf("[!] Персонаж не создан.\n"); return; }
    FILE* f = fopen("diary.txt", "a");
    if (!f) { printf("[!] Не удалось открыть diary.txt.\n"); return; }
    fprintf(f, "===== Запись дневника =====\n");
    fprintf(f, "Фермер: %s\n", p->name);
    fprintf(f, "День %d, %02d:00\n", day, hour);
    fprintf(f, "HP: %d/%d | Стамина: %d/%d | Золото: %d\n",
        p->hp, p->max_hp, p->stamina, p->max_stamina, p->gold);
    fprintf(f, "Инвентарь:\n");
    for (int i = 0; i < INVENTORY_SIZE; i++)
        fprintf(f, "  Слот %d: [%d] - %s\n",
            i, p->inventory[i], g_item_names[p->inventory[i]]);
    fprintf(f, "\n");
    fclose(f);
    printf("[+] Состояние записано в diary.txt\n");
}

void player_rest(Player* p) {
    if (!p) return;
    p->hp = p->max_hp;
    p->stamina = p->max_stamina;
    printf("[+] Вы отдохнули. HP: %d/%d, Стамина: %d/%d\n",
        p->hp, p->max_hp, p->stamina, p->max_stamina);
}
