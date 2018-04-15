#include "types.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "LightUtilities.h"

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

static uint8_t ColorList[3*10] = {
    WHITE,
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    LIGHT_BLUE,
    BLUE,
    PURPLE,
    PINK,
    DEEP_PINK
};

uint8_t i = 0;
static volatile uint8_t ColorIndex[2] = {0, 1};

int core(void);

#ifdef __AVR__
int main(void)
{
  core();
}
#endif

int core(void)
{
    CPU_PRESCALE(0x01);
    TCNT1 = 63974;   // for 1 sec at 16 MHz  (LIE)

    //TCCR1A = 0x00;
    TCCR1B = (1<<CS10) | (1<<CS12);  // Timer mode with 1024 prescaler
    TIMSK1 = (1 << TOIE1);   // Enable timer1 overflow interrupt(TOIE1)

    sei();

    DDRD |= 0xFF;
    //DDRC |= (1<<6);
    DDRB |= (1<<7);

    Lights_Init(2);
    while (1)
    {
        Lights_SetColor(0, ColorList[ColorIndex[0]*3], ColorList[(ColorIndex[0]*3) + 1], ColorList[(ColorIndex[0]*3)+ 2]);
        Lights_SetColor(1, ColorList[ColorIndex[1]*3], ColorList[(ColorIndex[1]*3) + 1], ColorList[(ColorIndex[1]*3)+ 2]);
        //Lights_SetColor(0, ORANGE);
        Lights_LightOutput();
    }
    return 0;
}

ISR (TIMER1_OVF_vect)    // Timer1 ISR
{
    uint8_t j = 0;
    PORTD ^= (0xFF);
    TCNT1 = 63974;   // for 1 sec at 16 MHz (LIE)

    if (i == 3)
    {
        for (j = 0; j < sizeof(ColorIndex); j++)
        {


            ColorIndex[j]++;
            if (ColorIndex[j] > (sizeof(ColorList)-1)/3)
            {
                ColorIndex[j] = 0;
            }
        }
        i = 0;
    }
    i++;
}
