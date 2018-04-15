#ifndef _LIGHTUTILITIES_H
#define _LIGHTUTILITIES_H

#include "types.h"
#include "CommonDefines.h"
#include <stdlib.h>

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} COLOR;

#define PURPLE 255, 0, 255
#define YELLOW 255, 255, 0
#define WHITE 255, 255, 255
#define GREEN 0, 255, 0
#define BLUE 0, 0, 255
#define RED 255, 0, 0
#define ORANGE 255, 120, 0
#define LIGHT_BLUE 0, 191, 255
#define PINK 255, 105, 180
#define DEEP_PINK 255, 20, 147

void Lights_Init(uint8_t num_leds);
void Lights_SetColor(uint8_t led_id, uint8_t r, uint8_t g, uint8_t b);
void Lights_LightOutput(void);
void Lights_SetAllLedsToColor(uint8_t r, uint8_t g, uint8_t b);

#endif /* _LIGHTUTILITIES_H */
