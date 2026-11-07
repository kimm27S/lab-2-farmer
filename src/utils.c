#define _CRT_SECURE_NO_WARNINGS
#include "utils.h"
#include <string.h>
#include <ctype.h>

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int read_int_safe(const char* prompt, int min, int max) {
    int value;
    while (1) {
        if (prompt && *prompt) printf("%s", prompt);
        if (scanf("%d", &value) != 1) {
            printf("[!] Ошибка: нужно ввести число!\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();
        if (value < min || value > max) {
            printf("[!] Ошибка: значение должно быть в диапазоне %d..%d\n", min, max);
            continue;
        }
        return value;
    }
}

int read_char_choice(void) {
    int c;
    do { c = getchar(); } while (c == '\n' || c == '\r');
    clear_input_buffer();
    return c;
}

void print_time(int day, int hour) {
    printf("День %d, %02d:00", day, hour);
}

static const char* strcasestr_my(const char* haystack, const char* needle) {
    if (!*needle) return haystack;
    for (; *haystack; haystack++) {
        const char* h = haystack;
        const char* n = needle;
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
    if (scanf("%127s", word) != 1) { clear_input_buffer(); return; }
    clear_input_buffer();

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

            const char* pos;
            const char* rest = line;
            int guard = 0;
            while ((pos = strcasestr_my(rest, word)) != NULL && guard++ < 100) {
                fwrite(rest, 1, pos - rest, out);
                fprintf(out, "[%.*s]", (int)strlen(word), pos);
                rest = pos + strlen(word);
            }
            fprintf(out, "%s\n", rest);

            rest = line; guard = 0;
            while ((pos = strcasestr_my(rest, word)) != NULL && guard++ < 100) {
                fwrite(rest, 1, pos - rest, stdout);
                printf("[%.*s]", (int)strlen(word), pos);
                rest = pos + strlen(word);
            }
            printf("%s\n", rest);
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
