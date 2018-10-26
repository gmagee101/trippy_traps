#ifndef _SINGLEBULBDIMMER_H
#define _SINGLEBULBDIMMER_H

#include "types.h"

#define DIMMEST 9
#define TIMER_INIT_VAL 5

typedef enum
{
    SBD_STATE_OFF,
    SBD_STATE_DIMMING,
    SBD_STATE_BRIGHTENING,
    SBD_STATE_HOLD
} SBD_STATE_t;

typedef enum
{
    SBD_STATUS_SUCCESS,
    SBD_STATUS_ERROR
} SBD_STATUS_t;

typedef struct
{
    uint8_t index;
    uint8_t dimLvl;
    uint8_t dimTimer;
    SBD_STATE_t state;
} SBD_CONTEXT_t;

void SBD_Init(SBD_CONTEXT_t * context, uint8_t index);
void SBD_Tick(SBD_CONTEXT_t * context);
SBD_STATUS_t SBD_Kickoff(SBD_CONTEXT_t * context);

#endif // _SINGLEBULBDIMMER_H
