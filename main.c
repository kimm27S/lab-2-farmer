#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define INVENTORY_SIZE 10

int main(void) {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    
    int current_day = 1;
    int current_hour = 8;
    int inventory[INVENTORY_SIZE] = { 4, 6, 0, 2, 0, 3, 8, 0, 1, 0 };

    const char *item_names[10] = {
        "пусто", "дерево", "камень", "семена",
        "лопата", "грабли", "меч", "щит",
        "бинты", "корзина"
    };

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
        printf("Ваш выбор: ");

        scanf("%d", &choice);
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

            int found_any = 0;  // Нашли ли хоть один предмет

            for (int i = 0; i < INVENTORY_SIZE; i++) {
                if (inventory[i] == 0) continue;  // Пропускаем пустые слоты

                // Проверка, стречался ли этот ID раньше
                int already_seen = 0;
                for (int j = 0; j < i; j++) {
                    if (inventory[j] == inventory[i]) {
                        already_seen = 1;
                        break;
                    }
                }
                if (already_seen) continue;  // Уже выводили — пропускаем

                // Счетчик, сколько раз этот ID встречается во всём массиве
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
        default:
            printf("Неверный пункт меню!\n");
    }

    return 0;
}

return 0;
}
