#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>

#define INVENTORY_SIZE 10
#define MAX_ITEMS 10
#define MAX_NAME_LEN 64
#define FARMER_NAME_LEN 33

#define MIN_MAP_SIZE 5
#define MAX_MAP_SIZE 30
#define OBSTACLE_MIN 15
#define OBSTACLE_MAX 30
#define MAX_ESCAPE_TRIES 1000
#define VIEW_RADIUS 4

#define WALL       '#'
#define FLOOR      '.'
#define TREE       'T'
#define ROCK       'R'
#define HERO       '@'
#define STAIR_DOWN 'D'
#define STAIR_UP   'U'
#define UNKNOWN    '?'

char item_names[MAX_ITEMS][MAX_NAME_LEN];
char farmer_name[FARMER_NAME_LEN] = "Фермер";

void load_items(void) {
    const char* defaults[MAX_ITEMS] = {
        "Пусто", "Дерево", "Камень", "Семена",
        "Лопата", "Грабли", "Меч", "Щит",
        "Бинты", "Корзина"
    };
    for (int i = 0; i < MAX_ITEMS; i++) {
        strncpy(item_names[i], defaults[i], MAX_NAME_LEN - 1);
        item_names[i][MAX_NAME_LEN - 1] = '\0';
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

        strncpy(item_names[id], p, MAX_NAME_LEN - 1);
        item_names[id][MAX_NAME_LEN - 1] = '\0';
        id++;
    }
    fclose(f);
    printf("[+] Каталог предметов загружен из items.txt (%d записей).\n", id);
}

void ask_farmer_name(void) {
    printf("Введите имя фермера (до %d символов): ", FARMER_NAME_LEN - 1);
    if (fgets(farmer_name, sizeof(farmer_name), stdin)) {
        size_t len = strlen(farmer_name);
        while (len > 0 && (farmer_name[len - 1] == '\n' || farmer_name[len - 1] == '\r'))
            farmer_name[--len] = '\0';
        if (len == 0) strcpy(farmer_name, "Фермер");
    }
}

void find_item_by_name(int* inventory) {
    char query[MAX_NAME_LEN];
    printf("Введите название предмета для поиска: ");
    if (scanf("%63s", query) != 1) { while (getchar() != '\n'); return; }
    
    int found_id = -1;
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(item_names[i], query) == 0) { found_id = i; break; }
    }
    if (found_id < 0) {
        printf("Предмет \"%s\" не найден в каталоге.\n", query);
        return;
    }
    
    printf("ID предмета \"%s\" = %d. Ищем в рюкзаке...\n", query, found_id);
    int found_any = 0;
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        if (inventory[i] == found_id) {
            printf("  Найден в слоте %d\n", i);
            found_any = 1;
        }
    }
    if (!found_any) printf("  В рюкзаке такого предмета нет.\n");
}

void write_diary(int day, int hour, int* inventory) {
    FILE* f = fopen("diary.txt", "a");
    if (!f) { printf("[!] Не удалось открыть diary.txt для записи.\n"); return; }
    fprintf(f, "===== Запись дневника =====\n");
    fprintf(f, "Фермер: %s\n", farmer_name);
    fprintf(f, "День %d, %02d:00\n", day, hour);
    fprintf(f, "Инвентарь:\n");
    for (int i = 0; i < INVENTORY_SIZE; i++)
        fprintf(f, "  Слот %d: [%d] - %s\n", i, inventory[i], item_names[inventory[i]]);
    fprintf(f, "\n");
    fclose(f);
    printf("[+] Состояние записано в diary.txt\n");
}

static const char* strcasestr_my(const char* haystack, const char* needle) {
     if (!*needle) return haystack;
     for (; *haystack; haystack++) {
         const char* h = haystack, * n = needle;
         while (*h && *n &&
             tolower((unsigned char)*h) == tolower((unsigned char)*n)) {
             h++; n++;
         }
         if (!*n) return haystack;
 }
 return NULL;
}

void encyclopedia_search(void) {
    char word[128];
    printf("Введите поисковое слово: ");
    if (scanf("%127s", word) != 1) { while (getchar() != '\n'); return; }
    
    FILE* in = fopen("input.txt", "r");
    if (!in) { printf("[!] Файл input.txt не найден.\n"); return; }
    FILE* out = fopen("output.txt", "w");
    if (!out) { printf("[!] Не удалось открыть output.txt.\n"); fclose(in); return; }
    
    char line[1024];
    int line_no = 0, found = 0;
    
    printf("\n=== Результаты поиска по слову \"%s\" ===\n", word);
    
    while (fgets(line, sizeof(line), in)) {
        line_no++;
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';
    
        if (strcasestr_my(line, word)) {
            found = 1;
            fprintf(out, "Статья #%d: ", line_no);
            printf("Статья #%d: ", line_no);
    
            const char* pos = line, * rest = line;
            int guard = 0;
            while ((pos = strcasestr_my(rest, word)) != NULL && guard++ < 100) {
                fwrite(rest, 1, pos - rest, out);
                fprintf(out, "[%.*s]", (int)strlen(word), pos);
                rest = pos + strlen(word);
            }
            fprintf(out, "%s\n", rest);
    
            pos = line; rest = line; guard = 0;
            while ((pos = strcasestr_my(rest, word)) != NULL && guard++ < 100) {
                fwrite(rest, 1, pos - rest, stdout);
                printf("[%.*s]", (int)strlen(word), pos);
                rest = pos + strlen(word);
            }
            printf("%s\n", rest);
    
            found = 1;
        }
    }
    
    if (!found) {
        printf("Ничего не найдено по слову \"%s\".\n", word);
        fprintf(out, "Ничего не найдено по слову \"%s\".\n", word);
    }
    
    fclose(in);
    fclose(out);
    printf("[+] Результат сохранён в output.txt\n");
}

typedef struct {
    char** map;
    int width;
    int height;
    int hero_x;
    int hero_y;
    int depth;
    int on_stair_up;
} Dungeon;

static int read_char_choice(void) {
    int c;
    do { c = getchar(); } while (c == '\n' || c == '\r');
    int tmp;
    while ((tmp = getchar()) != '\n' && tmp != EOF) {}
    return c;
}

static char** alloc_map(int w, int h) {
    char** m = (char**)malloc(h * sizeof(char*));
    if (!m) return NULL;
    for (int i = 0; i < h; i++) {
        m[i] = (char*)malloc(w * sizeof(char));
        if (!m[i]) {
            for (int j = 0; j < i; j++) free(m[j]);
            free(m);
            return NULL;
        }
    }
    return m;
}

static void free_map(char** m, int h) {
}

static void destroy_dungeon(Dungeon* d) {
}

static int find_free_cell(Dungeon* d, int* ox, int* oy) {
}

static int generate_floor(Dungeon* d, int depth, int with_up_stair) {
}

static int is_visible(const Dungeon* d, int x, int y) {
}

static void draw_dungeon(const Dungeon* d) {
}

static int can_step(const Dungeon* d, int nx, int ny) {
}

static void do_adventure(void) {
}

int main(void) {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    
    int current_day = 1;
    int current_hour = 8;
    int inventory[INVENTORY_SIZE] = { 4, 6, 0, 2, 0, 3, 8, 0, 1, 0 };

    load_items();
    ask_farmer_name();

    int choice;

    while (1) {
        printf("\n=== Меню ===\n");
        printf("[0] Выход\n");
        printf("[1] Посмотреть на часы\n");
        printf("[2] Промотать время\n");
        printf("[3] Посмотреть инвентарь\n");
        printf("[4] Положить предмет\n");
        printf("[5] Выбросить предмет\n");
        printf("[6] Уникальные находки\n");
        printf("[7] Поиск предмета в рюкзаке по названию\n");
        printf("[8] Записать состояние в дневник фермера\n");
        printf("[9] Расшифровать старые записи (энциклопедия)\n");
        printf("[10] Отправиться в приключение\n");
        printf("Ваш выбор: ");

        if (scanf("%d", &choice) != 1) {
            printf("Ошибка: нужно ввести число!\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
        case 0:
            printf("Выход.\n");
            return 0;

        case 1:
            printf("Текущее время: День %d, %02d:00\n", current_day, current_hour);
            break;

        case 2: {
                int hours;
                printf("Сколько часов потратить на работу? ");
                if (scanf("%d", &hours) != 1) {
                    printf("Ошибка: нужно ввести число!\n");
                    while (getchar() != '\n');
                    break;
                }
                if (hours < 0) {
                    printf("Часы не могут быть отрицательными!\n");
                    break;
                }
                current_hour += hours;
                while (current_hour >= 24) {
                    current_hour -= 24;
                    current_day++;
                }
                printf("Прошло %d часов.\n", hours);
            break;
        }

        case 3:
            for (int i = 0; i < INVENTORY_SIZE; i++) {
                    printf("Слот %d: [%d] (%s)\n", i, inventory[i], item_names[inventory[i]]);
                }
            break;

        case 4: {
                int index, id;
                printf("Введите индекс слота (0-%d): ", INVENTORY_SIZE - 1);
                if (scanf("%d", &index) != 1) {
                    printf("Ошибка: нужно ввести число!\n");
                    while (getchar() != '\n');
                    break;
                }
                if (index < 0 || index >= INVENTORY_SIZE) {
                    printf("Ошибка: индекс вне границ массива (0-%d)!\n", INVENTORY_SIZE - 1);
                    break;
                }
                printf("Введите ID предмета (0-9): ");
                if (scanf("%d", &id) != 1) {
                    printf("Ошибка: нужно ввести число!\n");
                    while (getchar() != '\n');
                    break;
                }
                if (id < 0 || id > 9) {
                    printf("Ошибка: такого ID не существует!\n");
                    break;
                }
                inventory[index] = id;
                printf("Предмет %d положен в слот %d.\n", id, index);
            break;
        }

        case 5: {
                int index;
                printf("Введите индекс слота для очистки (0-%d): ", INVENTORY_SIZE - 1);
                if (scanf("%d", &index) != 1) {
                    printf("Ошибка: нужно ввести число!\n");
                    while (getchar() != '\n');
                    break;
                }
                if (index < 0 || index >= INVENTORY_SIZE) {
                    printf("Ошибка: индекс вне границ массива (0-%d)!\n", INVENTORY_SIZE - 1);
                    break;
                }
                printf("Выброшен предмет: %d (%s)\n", inventory[index], item_names[inventory[index]]);
                inventory[index] = 0;
            break;
        }

        case 6: {
            printf("Массив: ");
            for (int i = 0; i < INVENTORY_SIZE; i++) {
                printf("%d ", inventory[i]);
            }
            printf("\n\nУникальные находки:\n");

            int found_any = 0;

            for (int i = 0; i < INVENTORY_SIZE; i++) {
                if (inventory[i] == 0) continue;

                int already_seen = 0;
                for (int j = 0; j < i; j++) {
                    if (inventory[j] == inventory[i]) {
                        already_seen = 1;
                        break;
                    }
                }
                if (already_seen) continue;

                int count = 0;
                for (int j = 0; j < INVENTORY_SIZE; j++) {
                    if (inventory[j] == inventory[i]) count++;
                }

                printf("ID %d (%s): %d шт.\n",
                       inventory[i], item_names[inventory[i]], count);
                found_any = 1;
            }

            if (!found_any) {
                printf("Инвентарь пуст — находок нет.\n");
            }
            break;
        }

        case 7:
            find_item_by_name(inventory);
            break;
        
        case 8:
            write_diary(current_day, current_hour, inventory);
            break;
        
        case 9:
            encyclopedia_search();
            break;

        case 10:
            do_adventure();
            break;
            
        default:
            printf("Неверный пункт меню!\n");
    }
}

return 0;
}
