#ifndef _LIGHTBEHAVIORS_H
#define _LIGHTBEHAVIORS_H

#include "types.h"

#define NUM_LEDS 10

static uint8_t HexTripColors[] = {
    0xE5, 0x00, 0x04,
    0xE1, 0x3C, 0x00,
    0xDD, 0x7B, 0x00,
    0xD9, 0xb8, 0x00,
    0xB8, 0xD5, 0x00,
    0x77, 0xD2, 0x00,
    0x39, 0xC3, 0x00,
    0x00, 0xCa, 0x01,
    0x00, 0xC6, 0x3B,
    0x00, 0xC2, 0x72,
    0x00, 0xBF, 0xa8,
    0x00, 0x9D, 0xC2,
    0x00, 0x63, 0xc6,
    0x00, 0x26, 0xca,
    0x18, 0x00, 0xce,
    0x5a, 0x00, 0xD2,
    0x9d, 0x00, 0xd5,
    0xd9, 0x00, 0xcf,
    0xdd, 0x00, 0x8e,
    0xe1, 0x00, 0x4a
};

static uint8_t TechWeekColors[]={
    20, 178, 204,
    20, 178, 204,
    20, 178, 204,
    20, 178, 204,
    127, 193, 66,
    127, 193, 66,
    127, 193, 66,
    127, 193, 66,
};

typedef enum
{
    LB_MODE_OFF,
    LB_MODE_HEXTRIP,
    LB_MODE_STARRY_SKY,
    LB_MODE_SOFT_FADE,
    LB_MODE_PTW
} LB_MODE;

void LB_Init(void);
void LB_Tick(void);
uint8_t LB_SetMode(LB_MODE);
void LB_Refresh(void);


#endif // _LIGHTBEHAVIORS_H
