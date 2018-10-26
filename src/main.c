#include "types.h"
#include "LightBehaviors.h"
#include "LightUtilities.h"
#include "print.h"
#include "LSM9DSO.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_16MHz       0x00
#define CPU_8MHz        0x01
#define CPU_4MHz        0x02
#define CPU_2MHz        0x03
#define CPU_1MHz        0x04
#define CPU_500kHz      0x05
#define CPU_250kHz      0x06
#define CPU_125kHz      0x07
#define CPU_62kHz       0x08
#define I2C_ACCEL_ADDR 0x1E
#define I2C_MAGNO_ADDR 0x1E


/*static uint8_t ColorList[3*10] = {
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    LIGHT_BLUE,
    BLUE,
    PURPLE,
    PINK,
    DEEP_PINK

};*/

static volatile uint8_t ColorIndex[2] = {0, 1};
static volatile uint16_t timer;

int core(void);

#ifdef __AVR__
int main(void)
{
  core();
}
#endif

int core(void)
{
    cli();
    CPU_PRESCALE(CPU_8MHz);


    PRR0 |= _BV(PRTIM0) | _BV(PRTIM2) | _BV(PRSPI) | _BV(PRADC);
    PRR1 |= _BV(PRTIM3) | _BV(PRUSART1);

    TCNT1 = 63974;   // for 1 sec at 16 MHz  (LIE)

    TCCR1B = (1<<CS10) | (1<<CS12);  // Timer mode with 1024 prescaler
    TIMSK1 = (1 << TOIE1);   // Enable timer1 overflow interrupt(TOIE1)
    DDRD |= (1<<6);
    DDRC |= (1<<7);
    PORTC &=~(1<<7);
    PORTD |= (1<<6);
    sei();

    //LSM_Init();
#ifndef PROD
    usb_init();
    _delay_ms(2000); //so we don't miss the first few usb debug prints
#endif
    LB_Init();
#ifndef PROD
    usb_debug_putchar('L');
#endif

    LB_SetMode(LB_MODE_SOFT_FADE);

    while (1)
    {
        //LSM_Tick();
        //if (timer == 0x00FF)
        //{
        //    LB_SetMode(LB_MODE_PTW);
        //}
        //if (timer == 0)
        //{
        //    LB_SetMode(LB_MODE_HEXTRIP);
        //}
        LB_Tick();
        //usb_debug_putchar('\n');
    }
    return 0;
}

ISR (TIMER1_OVF_vect)    // Timer1 ISR
{
    static uint8_t i = 0;
    uint8_t j = 0;
    LSM_AccelerationData adata;
    LSM_GyroData gdata;
    TCNT1 = 64974;   // for 1 sec at 16 MHz (LIE)
    if (i == 15)
    {
        i = 0;
        if (LSM_GetAccelerationData(&adata) == 0)
        {

#ifndef PROD
            print_P("AX: ");
            phex16(adata.ax);
            print_P("\tY: ");
            phex16(adata.ay);
            print_P("\tZ: ");
            phex16(adata.az);
            usb_debug_putchar('\n');
#endif
        }
        if (LSM_GetGyroData(&gdata) == 0)
        {

#ifndef PROD
            print_P("GX: ");
            phex16(gdata.gx);
            print_P("\tY: ");
            phex16(gdata.gy);
            print_P("\tZ: ");
            phex16(gdata.gz);
            usb_debug_putchar('\n');
#endif
        }
#ifndef PROD
            PORTD ^= (1<<6);
#endif

    }
    i++;
    LB_Refresh();
    timer++;
}
