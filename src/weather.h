#ifndef WEATHER_H
#define WEATHER_H

#include "utils.h"

typedef enum {
    WEATHER_CLEAR = 0,
    WEATHER_RAIN,
    WEATHER_FOG,
    WEATHER_HURRICANE
} WeatherType;

typedef struct {
    WeatherType type;
    int         temperature;
    int         wind_power;
} Weather;

void        weather_randomize(Weather* w);
const char* weather_name(WeatherType t);
int         weather_stamina_multiplier(const Weather* w);
int         weather_hurricane_chance(const Weather* w);
void        weather_print(const Weather* w);

#endif
