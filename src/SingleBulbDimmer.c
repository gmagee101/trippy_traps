#include "SingleBulbDimmer.h"

void SBD_Init(SBD_CONTEXT_t * context, uint8_t index)
{
    if (context)
    {
        context->index = index;
        context->dimLvl = DIMMEST;
        context->dimTimer = TIMER_INIT_VAL;
        context->state = SBD_STATE_OFF;
    }
}

void SBD_Tick(SBD_CONTEXT_t * context)
{
    if (context)
    {
        switch(context->state)
        {
            case SBD_STATE_OFF:
                break;
            case SBD_STATE_DIMMING:
                context->dimLvl++;
                if (context->dimLvl == DIMMEST)
                {
                    context->state = SBD_STATE_OFF;
                }
                break;
            case SBD_STATE_BRIGHTENING:
                context->dimLvl--;
                if (context->dimLvl == 1)
                {
                    context->state = SBD_STATE_HOLD;
                }
                break;
            case SBD_STATE_HOLD:
                context->dimTimer--;
                if (context->dimTimer == 0)
                {
                    context->state = SBD_STATE_DIMMING;
                    context->dimTimer = TIMER_INIT_VAL;
                }
                break;
        }
    }
}

SBD_STATUS_t SBD_Kickoff(SBD_CONTEXT_t * context)
{
    if (context && context->state == SBD_STATE_OFF)
    {
        context->state = SBD_STATE_BRIGHTENING;
        return SBD_STATUS_SUCCESS;
    }
    return SBD_STATUS_ERROR;
}
