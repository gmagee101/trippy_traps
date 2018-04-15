#include "LightUtilities.h"
#include <avr/io.h>

static uint8_t LightData[256 * 3] = {0}; //3 bytes per LED, max 256 LEDs (0 thru 255)
static uint8_t NumLeds = 0;

static const uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

static void lightOutputHelper(uint8_t * pixels, uint16_t count);

void Lights_Init(uint8_t num_leds)
{
    //No need to size check, data buffer can hold data for up to 0xFF LEDs
    NumLeds = num_leds;

}

void Lights_SetColor(uint8_t led_id, uint8_t r, uint8_t g, uint8_t b)
{
    if (led_id > NumLeds)
    {
        return;
    }
    uint8_t index = 3 * led_id;
    LightData[index++] = gamma8[g];
    LightData[index++] = gamma8[r];
    LightData[index++] = gamma8[b];
}

void Lights_LightOutput(void)
{
    lightOutputHelper(LightData, NumLeds * 3);
}

void Lights_SetAllLedsToColor(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t i;
    for (i = 0; i < NumLeds * 3; i+=3)
    {
        LightData[i] = gamma8[g];
        LightData[i + 1] = gamma8[r];
        LightData[i + 2] = gamma8[b];
    }
}

//Private Functions
void lightOutputHelper(uint8_t * pixels, uint16_t count)
{
	asm volatile(
	"rgbController:"				//output
	"movw   r26, r24\n\t"		// r26:27 = X = p_buf  /Copy r24 into r26
	"movw   r24, r22\n\t"		// r24:25 = count      /Copy r22 into r24
	"cli\n\t"					// no interrupts from here on, we're cycle-counting
	"ldi    r19, 7\n\t"			// 7 bit counter (8th bit is different)
	"ld     r18,X+\n\t"			// get first data byte
	
	"loop1:"
	"sbi	0x05, 7\n\t"		//	set Bit   //B7
	"lsl    r18\n\t"			// 1   +1 next bit into C, MSB first
	"brcs   L1\n\t"				// 1/2 +2 branch if 1
	"cbi	0x05, 7\n\t"		// clear bit   //B7
	"nop\n\t"					// 1   +4
	"bst    r18, 7\n\t"			// 1   +5 save last bit of data for fast branching
	"subi   r19, 1\n\t"			// 1   +6 how many more bits for this byte?
	"breq   bit8\n\t"			// 1/2 +7 last bit, do differently
	"rjmp   loop1\n\t"			// 2   +8, 10 total for 0 bit
	
	"L1:"
	"nop\n\t"					// 1   +4
	"bst    r18, 7\n\t"			// 1   +5 save last bit of data for fast branching
	"subi   r19, 1\n\t"			// 1   +6 how many more bits for this byte
	"cbi	0x05, 7\n\t"		// clear bit  //B7
	"brne   loop1\n\t"			// 2/1 +8 10 total for 1 bit (fall thru if last bit)
	
	"bit8:"
	"ldi    r19, 7\n\t"			// 1   +9 bit count for next byte
	"sbi	0x05, 7\n\t"		// set Bit   //B7
	"brts   L2\n\t"				// 1/2 +1 branch if last bit is a 1
	"nop\n\t"					// 1   +2
	"cbi	0x05, 7\n\t"		// clear bit   //B7
	"ld     r18, X+\n\t"		// 2   +4 fetch next byte
	"sbiw   r24, 1\n\t"			// 2   +6 dec byte counter
	"brne   loop1\n\t"			// 2   +8 loop back or return
	"sei\n\t"
	"ret\n\t"
	
	"L2:"
	"ld     r18, X+\n\t"		// 2   +3 fetch next byte
	"sbiw   r24, 1\n\t"			// 2   +5 dec byte counter
	"cbi 0x05, 7\n\t"			// clear bit   B7
	"brne   loop1\n\t"			// 2   +8 loop back or return
	"sei\n\t"
	"ret"
	);
}
