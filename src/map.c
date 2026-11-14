#define _CRT_SECURE_NO_WARNINGS
#include "map.h"
#include <ctype.h>
#include <time.h>

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
    if (!m) return;
    for (int i = 0; i < h; i++) free(m[i]);
    free(m);
}

static int is_floor_char(char c) {
    return c == FLOOR || c == FLOOR_CRACKED;
}

Direction char_to_direction(int cmd) {
    switch (tolower(cmd)) {
        case 'w': return DIR_UP;
        case 's': return DIR_DOWN;
        case 'a': return DIR_LEFT;
        case 'd': return DIR_RIGHT;
        case 'q': return DIR_QUIT;
        default:  return DIR_NONE;
    }
}

static int find_free_cell(const Map* d, int* ox, int* oy) {
    for (int t = 0; t < MAX_ESCAPE_TRIES; t++) {
        int x = rand() % d->width;
        int y = rand() % d->height;
        if (is_floor_char(d->map[y][x])) { *ox = x; *oy = y; return 1; }
    }
    for (int y = 1; y < d->height - 1; y++)
        for (int x = 1; x < d->width - 1; x++)
            if (is_floor_char(d->map[y][x])) { *ox = x; *oy = y; return 1; }
    return 0;
}

Map* map_generate(int depth, int with_up_stair) {
    Map* d = (Map*)malloc(sizeof(Map));
    if (!d) return NULL;

    d->width = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    d->height = MIN_MAP_SIZE + rand() % (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    d->depth = depth;

    d->map = alloc_map(d->width, d->height);
    if (!d->map) { free(d); return NULL; }

    for (int y = 0; y < d->height; y++) {
        for (int x = 0; x < d->width; x++) {
            if (x == 0 || y == 0 || x == d->width - 1 || y == d->height - 1)
                d->map[y][x] = WALL;
            else
                d->map[y][x] = (rand() % 2) ? FLOOR : FLOOR_CRACKED;
        }
    }

    int obstacle_pct = OBSTACLE_MIN + rand() % (OBSTACLE_MAX - OBSTACLE_MIN + 1);
    for (int y = 1; y < d->height - 1; y++)
        for (int x = 1; x < d->width - 1; x++)
            if (is_floor_char(d->map[y][x]) && (rand() % 100) < obstacle_pct)
                d->map[y][x] = (rand() % 2) ? TREE : ROCK;

    int hx, hy;
    if (!find_free_cell(d, &hx, &hy)) { map_destroy(&d); return NULL; }

    if (with_up_stair) {
        int placed = 0;
        int dirs[4][2] = { { 0,-1 },{ 0,1 },{ -1,0 },{ 1,0 } };
        for (int k = 0; k < 4 && !placed; k++) {
            int nx = hx + dirs[k][0];
            int ny = hy + dirs[k][1];
            if (ny > 0 && ny < d->height - 1 && nx > 0 && nx < d->width - 1 &&
                is_floor_char(d->map[ny][nx])) {
                d->map[ny][nx] = STAIR_UP;
                placed = 1;
            }
        }
        if (!placed) {
            int ux, uy;
            if (find_free_cell(d, &ux, &uy)) d->map[uy][ux] = STAIR_UP;
        }
    }

    int dx, dy;
    if (find_free_cell(d, &dx, &dy)) d->map[dy][dx] = STAIR_DOWN;

    return d;
}

void map_destroy(Map** pm) {
    if (!pm || !*pm) return;
    free_map((*pm)->map, (*pm)->height);
    printf("Память структуры по адресу %p освобождена\n", (void*)*pm);
    free(*pm);
    *pm = NULL;
}

static int is_visible(int hero_x, int hero_y, int px, int py, int radius) {
    int dx = px - hero_x;
    int dy = py - hero_y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

static void draw_map(const Map* d, const Player* p, const Weather* w, int hour) {
    printf("\n===== ЭТАЖ %d =====\n", d->depth);
    printf("HP: %d/%d | Выносливость: %d/%d | Золото: %d | Этаж: %d | Время: %02d:00\n",
           p->hp, p->max_hp, p->stamina, p->max_stamina,
           p->gold, d->depth, hour);
    weather_print(w);
    
    int current_radius = weather_view_radius(w);
    printf("Радиус обзора: %d\n", current_radius);
    printf("-------------------------------------------------------------\n");

    for (int y = 0; y < d->height; y++) {
        for (int x = 0; x < d->width; x++) {
            if (x == p->x && y == p->y)
                putchar(HERO);
            else if (is_visible(p->x, p->y, x, y, current_radius))
                putchar(d->map[y][x]);
            else
                putchar(UNKNOWN);
        }
        putchar('\n');
    }
    printf("Легенда: '.' пол, ',' трещины, '#' стена, 'T' дерево, 'R' камень,\n");
    printf("         'D' спуск, 'U' выход, '@' вы\n");
    printf("[W/A/S/D] шаг, [T] ждать 1 час, [Q] выход (только стоя на U)\n");
}

static int can_step(const Map* d, int nx, int ny) {
    if (nx < 0 || ny < 0 || nx >= d->width || ny >= d->height) return 0;
    char c = d->map[ny][nx];
    return !(c == WALL || c == TREE || c == ROCK);
}

static int is_night(int hour) {
    return hour == NIGHT_HOUR;
}

void adventure_run(Player* p, int* day, int* hour, Weather* w) {
    if (!p) { printf("[!] Персонаж не создан.\n"); return; }
    if (p->stamina <= 0) { printf("[!] Нет выносливости для похода.\n"); return; }
    if (is_night(*hour)) { printf("[!] Ночью (02:00) фермер должен спать.\n"); return; }

    weather_randomize(w);
    printf("\n[ Погода на локации ]\n");
    weather_print(w);

    Map* d = map_generate(1, 0);
    if (!d) { printf("[!] Не удалось сгенерировать карту.\n"); return; }

    int hx, hy;
    if (!find_free_cell(d, &hx, &hy)) { map_destroy(&d); return; }
    p->x = hx;
    p->y = hy;

    int running = 1;

    while (running) {
        draw_map(d, p, w, *hour);

        printf("Команда: ");
        int cmd = read_char_choice();
        Direction dir = char_to_direction(cmd);

        if (dir == DIR_QUIT) {
            if (d->map[p->y][p->x] == STAIR_UP) {
                printf("[+] Штатный выход. Этаж: %d, золото сохранено: %d\n",
                    d->depth, p->gold);
                running = 0;
                break;
            } else {
                printf("[!] Здесь нет лестницы наверх (U).\n");
                continue;
            }
        }

        if (tolower(cmd) == 't') {
            int next_hour = *hour + 1;
            int next_day = *day;
            if (next_hour >= 24) { next_hour -= 24; next_day++; }
            if (next_hour == NIGHT_HOUR) {
                printf("[!] Нельзя ждать через 02:00 — нужно спать.\n");
                continue;
            }
            *hour = next_hour;
            *day = next_day;
            continue;
        }

        int dx = 0, dy = 0;
        switch (dir) {
            case DIR_UP:    dy = -1; break;
            case DIR_DOWN:  dy = 1;  break;
            case DIR_LEFT:  dx = -1; break;
            case DIR_RIGHT: dx = 1;  break;
            case DIR_NONE:
            default:
                printf("Неизвестная команда.\n");
                continue;
        }

        if (weather_hurricane_chance(w) > 0 &&
            (rand() % 100) < weather_hurricane_chance(w)) {
            int r = rand() % 4;
            const int dirs[4][2] = { { 0,-1 },{ 0,1 },{ -1,0 },{ 1,0 } };
            int ndx = dirs[r][0], ndy = dirs[r][1];
            if (can_step(d, p->x + ndx, p->y + ndy)) {
                dx = ndx;
                dy = ndy;
                printf("[!] Ураган снёс вас на соседнюю клетку!\n");
            }
        }

        int nx = p->x + dx, ny = p->y + dy;
        if (!can_step(d, nx, ny)) { printf("[!] Стена или препятствие.\n"); continue; }

        char dest = d->map[ny][nx];
        p->x = nx;
        p->y = ny;

        int cost = STEP_STAMINA_COST * weather_stamina_multiplier(w);
        p->stamina -= cost;
        if (p->stamina < 0) p->stamina = 0;

        if (dest == STAIR_DOWN) {
            int new_depth = d->depth + 1;
            int reward = BASE_GOLD_MULTIPLIER * new_depth;
            p->gold += reward;
            printf("[+] Спуск! Этаж %d. Награда: +%d золота.\n", new_depth, reward);

            p->stamina -= FLOOR_STAMINA_COST;
            if (p->stamina < 0) p->stamina = 0;

            (*hour)++;
            if (*hour >= 24) { *hour -= 24; (*day)++; }

            weather_randomize(w);

            Map* newd = map_generate(new_depth, 1);
            if (!newd) { printf("[!] Не удалось сгенерировать этаж.\n"); running = 0; break; }
            map_destroy(&d);
            d = newd;

            if (!find_free_cell(d, &hx, &hy)) { running = 0; break; }
            p->x = hx;
            p->y = hy;

            if (p->stamina <= 0) {
                printf("\n[!] ОБМОРОК! Выносливость на нуле.\n");
                int loss = p->gold / 2;
                p->gold -= loss;
                printf("[!] Спасатели забрали %d золота. Возврат домой.\n", loss);
                running = 0;
            }
            else if (is_night(*hour)) {
                printf("\n[!] Наступило 02:00 — фермер падает без сил!\n");
                int loss = p->gold / 2;
                p->gold -= loss;
                printf("[!] Спасатели забрали %d золота. Возврат домой.\n", loss);
                running = 0;
            }
            continue;
        }

        if (dest == STAIR_UP) {
            printf("[!] Вы на лестнице наверх. Нажмите Q.\n");
        }

        if (p->stamina <= 0) {
            printf("\n[!] ОБМОРОК! Выносливость на нуле.\n");
            int loss = p->gold / 2;
            p->gold -= loss;
            printf("[!] Спасатели забрали %d золота. Возврат домой.\n", loss);
            running = 0;
        }
        else if (is_night(*hour)) {
            printf("\n[!] 02:00 — фермер падает без сил!\n");
            int loss = p->gold / 2;
            p->gold -= loss;
            printf("[!] Спасатели забрали %d золота. Возврат домой.\n", loss);
            running = 0;
        }
    }

    map_destroy(&d);
}
