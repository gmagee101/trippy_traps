#include "types.h"
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

//hextrip
static uint8_t ColorList[3*22] = {
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

    LSM_Init();
#ifndef PROD
    usb_init();
    _delay_ms(2000); //so we don't miss the first few usb debug prints
#endif
    Lights_Init(2);
#ifndef PROD
    usb_debug_putchar('L');
#endif


    while (1)
    {
        LSM_Tick();
        /*LIGHTS*/
        //Set neopixel output to current color
        Lights_SetColor(0, ColorList[ColorIndex[0]*3], ColorList[(ColorIndex[0]*3) + 1], ColorList[(ColorIndex[0]*3)+ 2]);
        Lights_SetColor(1, ColorList[ColorIndex[1]*3], ColorList[(ColorIndex[1]*3) + 1], ColorList[(ColorIndex[1]*3)+ 2]);
        Lights_LightOutput();
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
    if (i == 3)
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
    for (j = 0; j < sizeof(ColorIndex); j++)
    {
        ColorIndex[j]++;
        if (ColorIndex[j] > (((sizeof(ColorList))/3) - 3))
        {
            ColorIndex[j] = 0;
        }
    }
}
