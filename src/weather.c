#define _CRT_SECURE_NO_WARNINGS
#include "weather.h"

const char* weather_name(WeatherType t) {
    switch (t) {
    case WEATHER_CLEAR:     return "Ясно";
    case WEATHER_RAIN:      return "Ливень";
    case WEATHER_FOG:       return "Туман";
    case WEATHER_HURRICANE: return "Ураган";
    default:                return "?";
    }
}

void weather_randomize(Weather* w) {
    if (!w) return;
    w->type = (WeatherType)(rand() % 4);

    switch (w->type) {
    case WEATHER_CLEAR:
        w->temperature = 15 + rand() % 15;
        w->wind_power = rand() % 3;
        break;
    case WEATHER_RAIN:
        w->temperature = 8 + rand() % 10;
        w->wind_power = 2 + rand() % 5;
        break;
    case WEATHER_FOG:
        w->temperature = 5 + rand() % 10;
        w->wind_power = rand() % 3;
        break;
    case WEATHER_HURRICANE:
        w->temperature = 10 + rand() % 10;
        w->wind_power = 8 + rand() % 3;
        break;
    default:
        w->temperature = 20;
        w->wind_power = 0;
        break;
    }
}

int weather_stamina_multiplier(const Weather* w) {
    if (!w) return 1;
    return (w->type == WEATHER_RAIN) ? 2 : 1;
}

int weather_hurricane_chance(const Weather* w) {
    if (!w) return 0;
    return (w->type == WEATHER_HURRICANE) ? 20 : 0;
}

void weather_print(const Weather* w) {
    if (!w) return;
    printf("Погода: %-7s | Температура: %d°C | Ветер: %d/10\n",
        weather_name(w->type), w->temperature, w->wind_power);
}
