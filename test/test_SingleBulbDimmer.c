#include "unity.h"
#include "SingleBulbDimmer.h"
#include "types.h"

void sbd_fields_assert(
        SBD_CONTEXT_t * bulb,
        uint8_t index,
        uint8_t dimLvl,
        uint8_t dimTimer,
        SBD_STATE_t state);

void setUp(void)
{
}

void tearDown(void)
{
}

void test_SingleBulbDimmer_StructType(void)
{
    SBD_CONTEXT_t bulb;
    bulb.index = 0;
    bulb.dimLvl = 1;
    bulb.dimTimer = 5;
    bulb.state = SBD_STATE_DIMMING;
}

void test_SingleBulbDimmer_InitializeStruct(void)
{
    SBD_CONTEXT_t bulb;
    uint8_t index = 0;
    SBD_Init(&bulb, index);
    sbd_fields_assert(&bulb, index, DIMMEST, TIMER_INIT_VAL, SBD_STATE_OFF);
}

void test_SingleBulbDimmer_NullPointerStructDoesntBreak(void)
{
    SBD_Init(NULL, 0);
}

void test_SingleBulbDimmer_Tick_NothingHappens(void)
{
    SBD_CONTEXT_t bulb;
    uint8_t index = 0;
    SBD_Init(&bulb, index);
    SBD_Tick(&bulb);
    sbd_fields_assert(&bulb, index, DIMMEST, TIMER_INIT_VAL, SBD_STATE_OFF);
}

void test_SingleBulbDimmer_TickOnNull(void)
{
    SBD_Tick(NULL);
}

void test_SingleBulbDimmer_Kickoff(void)
{
    SBD_CONTEXT_t bulb;
    uint8_t index = 0;
    SBD_Init(&bulb, index);
    TEST_ASSERT_EQUAL(SBD_STATUS_SUCCESS, SBD_Kickoff(&bulb));
    sbd_fields_assert(&bulb, index, DIMMEST, TIMER_INIT_VAL, SBD_STATE_BRIGHTENING);
}

void test_SingleBulbDimmmer_KickoffWithNull(void)
{
    TEST_ASSERT_EQUAL(SBD_STATUS_ERROR, SBD_Kickoff(NULL));
}

void test_SingleBulbDimmer_KickoffAlreadyKickedOff(void)
{
    SBD_CONTEXT_t bulb;
    uint8_t index = 0;
    SBD_Init(&bulb, index);
    SBD_Kickoff(&bulb);
    TEST_ASSERT_EQUAL(SBD_STATUS_ERROR, SBD_Kickoff(&bulb));
}

void test_SingleBulbDimmer_TickAfterKickoffBehavior(void)
{
    SBD_CONTEXT_t bulb;
    uint8_t index = 0, i;
    SBD_Init(&bulb, index);
    SBD_Kickoff(&bulb);
    // Brightening
    for (i = DIMMEST-1; i > 1; i--)
    {
        SBD_Tick(&bulb);
        sbd_fields_assert(&bulb, index, i, TIMER_INIT_VAL, SBD_STATE_BRIGHTENING);
    }
    // Hold at brightest
    for (i = TIMER_INIT_VAL; i > 0; i--)
    {
        SBD_Tick(&bulb);
        sbd_fields_assert(&bulb, index, 1, i, SBD_STATE_HOLD);
    }
    // Dimming
    for (i = 1; i < DIMMEST; i++)
    {
        SBD_Tick(&bulb);
        sbd_fields_assert(&bulb, index, i, TIMER_INIT_VAL, SBD_STATE_DIMMING);
    }
    SBD_Tick(&bulb);
    sbd_fields_assert(&bulb, index, DIMMEST, TIMER_INIT_VAL, SBD_STATE_OFF);
}

void sbd_fields_assert(
        SBD_CONTEXT_t * bulb,
        uint8_t index,
        uint8_t dimLvl,
        uint8_t dimTimer,
        SBD_STATE_t state)
{
    TEST_ASSERT_NOT_NULL(bulb)
    TEST_ASSERT_EQUAL(index, bulb->index);
    TEST_ASSERT_EQUAL(dimLvl, bulb->dimLvl);
    TEST_ASSERT_EQUAL(dimTimer, bulb->dimTimer);
    TEST_ASSERT_EQUAL(state, bulb->state);
}
