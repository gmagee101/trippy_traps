#include "types.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include "LightUtilities.h"
#include "print.h"
#include "i2c.h"
#include <alloca.h>
#include <util/delay.h>

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
#define ACC_IDX 0
#define MAG_IDX 1


static uint8_t ColorList[3*10] = {
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

static volatile uint8_t ColorIndex[2] = {0, 1};
static int16_t x, y, z;

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

    i2c_init();
#ifndef PROD
    usb_init();
    _delay_ms(2000); //so we don't miss the first few usb debug prints
    usb_debug_putchar('.');
#endif
    Lights_Init(2);
#ifndef PROD
    usb_debug_putchar('L');
#endif

    //accel_init
    i2c_txn_t * txn;
    txn = alloca(sizeof(*txn) + 2 * sizeof(txn->ops[0]));
    uint8_t xyz[2][6]; //mag & accel in 2d array (2s complement, 16b)
    xyz[ACC_IDX][0] = 0;
    xyz[ACC_IDX][1] = 0;
    xyz[ACC_IDX][2] = 0;
    xyz[ACC_IDX][3] = 0;
    xyz[ACC_IDX][4] = 0;
    xyz[ACC_IDX][5] = 0;
    uint8_t ctrl[7];
    usb_debug_putchar('.');
    i2c_txn_init(txn, 1);
    ctrl[0] = 0x20;
    ctrl[1] = 0x1F;
    i2c_op_init(&txn->ops[0], 0x3A, ctrl, 2);
    i2c_post(txn);
    while (!(txn->flags & I2C_TXN_DONE)) {
    }

    if (txn->flags & I2C_TXN_ERR)
    {
        usb_debug_putchar('E');
    }
    while (1)
    {
        /*ACCELEROMETER*/
        i2c_txn_init(txn, 2);
        uint8_t addr = 0xA8; //0x28, plus MSB=1 for multiple read
        i2c_op_init(&txn->ops[0], 0x3A, &addr, 1);
        i2c_op_init(&txn->ops[1], 0x3B, xyz[ACC_IDX], 6);
        i2c_post(txn);
        while (!(txn->flags & I2C_TXN_DONE)) {
        }

        if (txn->flags & I2C_TXN_ERR)
        {
            usb_debug_putchar('E');
        }
        else
        {
            x = ((uint16_t)(xyz[ACC_IDX][1])<<8) | xyz[ACC_IDX][0];
            y = ((uint16_t)(xyz[ACC_IDX][3])<<8) | xyz[ACC_IDX][2];
            z = ((uint16_t)(xyz[ACC_IDX][5])<<8) | xyz[ACC_IDX][4];
        }
        
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
#ifndef PROD
    PORTD ^= (1<<6);
#endif
    TCNT1 = 63974;   // for 1 sec at 16 MHz (LIE)
    if (i == 3)
    {
        for (j = 0; j < sizeof(ColorIndex); j++)
        {
            ColorIndex[j]++;
            if (ColorIndex[j] > (sizeof(ColorList)/3 - 2))
            {
                ColorIndex[j] = 0;
            }
        }
        i = 0;

#ifndef PROD
        print_P("X: ");
        phex16(x);
        print_P("\tY: ");
        phex16(y);
        print_P("\tZ: ");
        phex16(z);
        usb_debug_putchar('\n');
#endif

    }
    i++;
}
