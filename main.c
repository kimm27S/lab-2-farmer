#include <stdio.h>

#define INVENTORY_SIZE 10

int main(void) {
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

        switch (choice) {
        case 0:
            printf("Выход.\n");
            return 0;

        case 1:
            break;

        case 2:
            break;

        case 3:
            break;

        case 4:
            break;

        case 5:
            break;

        case 6:
            break;

        default:
            printf("Неверный пункт меню!\n");
        }
    }

    return 0;
}
