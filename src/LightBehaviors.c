#include "LightBehaviors.h"
#include "LightUtilities.h"

static LB_MODE CurrentMode;

static uint8_t htIdx, ptIdx, dimLvl=1, dimDir=1, dimTimer=5;

void loopArrayThroughLights(uint8_t * colors, uint8_t numColors,
        uint8_t idx);

void LB_Init(void)
{
    Lights_Init(NUM_LEDS);
    CurrentMode = LB_MODE_OFF;
    return;
}

uint8_t LB_SetMode(LB_MODE m)
{
    switch(m)
    {
        case LB_MODE_HEXTRIP:
            CurrentMode = LB_MODE_HEXTRIP;
            htIdx = 0;
            break;
        case LB_MODE_PTW:
            CurrentMode = LB_MODE_PTW;
            ptIdx = 0;
            break;
        case LB_MODE_SOFT_FADE:
            CurrentMode = LB_MODE_SOFT_FADE;
            dimLvl = 1;
            dimDir = 1;
            dimTimer = 50;

    }
    return CurrentMode;
}

void LB_Refresh(void)
{
    switch(CurrentMode)
    {
        case LB_MODE_OFF:
            Lights_SetAllLedsToColor(0, 0, 0);
            break;
        case LB_MODE_HEXTRIP:
            loopArrayThroughLights(HexTripColors, sizeof(HexTripColors)/3, htIdx);
            htIdx++;
            if (htIdx >= sizeof(HexTripColors)/3)
            {
                htIdx = 0;
            }
            break;
        case LB_MODE_PTW:
            loopArrayThroughLights(TechWeekColors, sizeof(TechWeekColors)/3, ptIdx);
            ptIdx++;
            if (ptIdx >= sizeof(TechWeekColors)/3)
            {
                ptIdx = 0;
            }
            break;
        case LB_MODE_SOFT_FADE:
            if (dimLvl == 9)
            {
                Lights_SetAllLedsToColor(0, 0, 0);
                dimDir = 1;
            }
            else
            {
                Lights_SetAllLedsToColor(255 / dimLvl, 255 / dimLvl, 255 / dimLvl);
                if (dimLvl == 1 && dimDir == 1)
                {
                    dimDir = 2;
                    dimTimer = 5;
                }
            }
            if (dimDir == 1)
            {
                dimLvl--;
            }
            else if (dimDir == 0)
            {
                dimLvl++;
            }
            else if (dimDir == 2 && dimTimer > 0)
            {
                dimTimer--;
            }
            else
            {
                dimDir = 0;
            }
            break;
    }
}

void LB_Tick(void)
{
    Lights_LightOutput();
}

void loopArrayThroughLights(uint8_t * colors, uint8_t numColors,
        uint8_t idx)
{
    uint8_t i;
    uint8_t coloridx;
    if (idx != 0)
    {
        coloridx = numColors - 1 - (idx - 1); //the color belonging with idx light
    }
    else
    {
        coloridx = 0;
    }
    for (i = 0; i < NUM_LEDS; i++)
    {
        if (coloridx >= numColors)
        {
            coloridx = 0;
        }
        Lights_SetColor(i, colors[coloridx * 3], colors[(coloridx*3) + 1], colors[(coloridx*3) + 2]);
        coloridx++;
    }
}
