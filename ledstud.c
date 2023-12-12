#include "ch32v003fun.h"
#include <stdio.h>
#include <string.h>

#include "anim/frames.h"
#include "anim/font.h"

enum {
	MODE_VIDEO,
	MODE_TEXT,
	MODE_BLINKY,
	MODE_SLEEP,
	NUM_MODES
};

uint8_t mode __attribute__ ((section (".no_init")));

static inline void draw_frame( const uint8_t* bitmap, uint32_t on, uint32_t off )
{
	GPIOD->OUTDR = 1<<0;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[0];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );

	GPIOD->OUTDR = 0;
	GPIOA->OUTDR = 1<<2;
	GPIOC->OUTDR = bitmap[1];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );

	GPIOD->OUTDR = 0;
	GPIOA->OUTDR = 1<<1;
	GPIOC->OUTDR = bitmap[2];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );

	GPIOD->OUTDR = 1<<6;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[3];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );

	GPIOD->OUTDR = 1<<5;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[4];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );

	GPIOD->OUTDR = 1<<4;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[5];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );

	GPIOD->OUTDR = 1<<3;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[6];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );

	GPIOD->OUTDR = 1<<2;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[7];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );
}

static inline void draw_frame_fast( const uint8_t* bitmap )
{
//#define fast_delay() asm volatile("\nnop\nnop\nnop\nnop\nnop\nnop\nnop")
#define fast_delay() DelaySysTick(10)

	GPIOD->OUTDR = 1<<0;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[0];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;

	GPIOD->OUTDR = 0;
	GPIOA->OUTDR = 1<<2;
	GPIOC->OUTDR = bitmap[1];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;

	GPIOD->OUTDR = 0;
	GPIOA->OUTDR = 1<<1;
	GPIOC->OUTDR = bitmap[2];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;

	GPIOD->OUTDR = 1<<6;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[3];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;

	GPIOD->OUTDR = 1<<5;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[4];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;

	GPIOD->OUTDR = 1<<4;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[5];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;

	GPIOD->OUTDR = 1<<3;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[6];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;

	GPIOD->OUTDR = 1<<2;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[7];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;
}

void mode_video()
{
	uint8_t * frame = &frames[0];
	uint8_t * last = &frames[sizeof(frames) - 32];
	int8_t step = 32;
	uint8_t timer = 0;

	while (1) {
		draw_frame( &frame[0],  1, 50 );
		draw_frame( &frame[8],  2, 50 );
		draw_frame( &frame[16], 4, 50 );
		draw_frame( &frame[24], 8, 50 );
		if (timer++ > 2) {
			timer = 0;
			frame += step;
			if (frame == last) {
				step = -32;
			} else if (frame == &frames[0]) {
				step = 32;
			}
		}
	}
}

static inline void set_character( uint8_t* dst, char c )
{
	if (c >= '!' && c <= '~') {
		memcpy(dst, &font[ 5*(c-'!') ], 5);
	} else {
		memset(dst, 0xFF, 5);
	}
}

void mode_text()
{
	static const char msg[] = " This badge is powered by a CH32V003, a RISC-V 48MHz microcontroller  ";

	unsigned int timer = 0;
	uint8_t framebuffer[20] = { [0 ... 15] = 0xFF};
	unsigned int cursor = 0;
	unsigned int fraction = 0;

	while (1) {

		draw_frame( &framebuffer[5],  4, 50 );

		if (timer++ > 90) {
			timer = 0;

			memset(&framebuffer[5], 0xFF, 8);
			set_character(&framebuffer[5-fraction], msg[cursor]);
			set_character(&framebuffer[5+6-fraction], msg[cursor+1]);
			set_character(&framebuffer[5+6+6-fraction], msg[cursor+2]);
			if (fraction++ >4) {
				fraction=0;
				cursor++;
			}
			if (cursor == (sizeof msg) -2) {
				cursor=0;
			}
		}
	}
}

static uint32_t seed;
int32_t random(void) {
	return seed = seed * 16807 % 0x7FFFFFFF;
}

void mode_blinky()
{
	uint8_t buffer[8] = { 0 };
	uint32_t period[64] = { [0 ... 63] = 0xffffffff };
	uint32_t phase[64] = {0};

	seed = SysTick->CNT;
	uint32_t spread = random() % 256;
	uint32_t offset = 300 + (random() % 512);

	//printf("%d, %d\n",offset,spread);

	for (int i =0; i<64; i++) {
		period[i] = offset + (random() % spread);
	}

	while (1) {
		draw_frame_fast( &buffer[0] );

		for (int i =0; i<8; i++) {
			for (int j =0; j<8; j++) {
				uint8_t k = i*8 +j;
				phase[k]++;
				if (phase[k]>period[k]) {
					phase[k]=0;
					buffer[ i ] ^= (1<<j); 
				}
			}
		}
	}

}

int main()
{
	SystemInit();

	// check reset-pin flag
	mode = (RCC->RSTSCKR & RCC_PINRSTF)? (mode+1)%NUM_MODES : 0 ;

	RCC->RSTSCKR |= RCC_RMVF; // clear reset flags

	if (mode == MODE_SLEEP) {

		// Power consumption is reduced significantly if GPIOs are enabled and set to pull-up
		RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD;

		GPIOA->CFGLR = (GPIO_CNF_IN_PUPD<<(4*2)) |
					   (GPIO_CNF_IN_PUPD<<(4*1));
		GPIOA->BSHR = GPIO_BSHR_BS2 |
					  GPIO_BSHR_BS1;

		GPIOD->CFGLR = (GPIO_CNF_IN_PUPD<<(4*7)) |
					   (GPIO_CNF_IN_PUPD<<(4*6)) |
					   (GPIO_CNF_IN_PUPD<<(4*5)) |
					   (GPIO_CNF_IN_PUPD<<(4*4)) |
					   (GPIO_CNF_IN_PUPD<<(4*3)) |
					   (GPIO_CNF_IN_PUPD<<(4*2)) |
					   (GPIO_CNF_IN_PUPD<<(4*1)) |
					   (GPIO_CNF_IN_PUPD<<(4*0));
		GPIOD->BSHR = GPIO_BSHR_BS7 |
					  GPIO_BSHR_BS6 |
					  GPIO_BSHR_BS5 |
					  GPIO_BSHR_BS4 |
					  GPIO_BSHR_BS3 |
					  GPIO_BSHR_BS2 |
					  GPIO_BSHR_BS1 |
					  GPIO_BSHR_BS0;

		GPIOC->CFGLR = (GPIO_CNF_IN_PUPD<<(4*7)) |
					   (GPIO_CNF_IN_PUPD<<(4*6)) |
					   (GPIO_CNF_IN_PUPD<<(4*5)) |
					   (GPIO_CNF_IN_PUPD<<(4*4)) |
					   (GPIO_CNF_IN_PUPD<<(4*3)) |
					   (GPIO_CNF_IN_PUPD<<(4*2)) |
					   (GPIO_CNF_IN_PUPD<<(4*1)) |
					   (GPIO_CNF_IN_PUPD<<(4*0));
		GPIOC->BSHR = GPIO_BSHR_BS7 |
					  GPIO_BSHR_BS6 |
					  GPIO_BSHR_BS5 |
					  GPIO_BSHR_BS4 |
					  GPIO_BSHR_BS3 |
					  GPIO_BSHR_BS2 |
					  GPIO_BSHR_BS1 |
					  GPIO_BSHR_BS0;

		NVIC->SCTLR |= (1<<2); //SLEEPDEEP
		PWR->CTLR &= PWR_CTLR_PDDS;
		asm volatile ("wfi");
	}

	// All other modes - enable display

	RCC->CFGR0 |= (12<<4); // set AHB prescaler to 32 -> sysclk 1.5MHz

	RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC;

	// Anodes shuffled
	// D0 : col1
	// A2 : col2
	// A1 : col3
	// D6 : col4
	// D5 : col5
	// D4 : col6
	// D3 : col7
	// D2 : col8
	GPIOA->CFGLR = ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*1))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*2));

	GPIOD->CFGLR = ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*2))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*3))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*4))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*5))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*6))
				 | (GPIO_CNF_IN_FLOATING << (4*1)); // don't disable SWIO

	// GPIO C rows / cathodes, in order
	GPIOC->CFGLR = ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*1))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*2))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*3))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*4))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*5))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*6))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*7));

	switch (mode) {
		case MODE_VIDEO: mode_video();
		case MODE_TEXT: mode_text();
		case MODE_BLINKY: mode_blinky();
	}
}
