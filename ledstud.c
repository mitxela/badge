#include "ch32v003fun.h"
#include <stdio.h>


static inline void draw_frame( uint8_t* bitmap, uint32_t on, uint32_t off )
{

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off );

	GPIOD->OUTDR = 1<<0;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[0];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off );

	GPIOD->OUTDR = 0;
	GPIOA->OUTDR = 1<<2;
	GPIOC->OUTDR = bitmap[1];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off );

	GPIOD->OUTDR = 0;
	GPIOA->OUTDR = 1<<1;
	GPIOC->OUTDR = bitmap[2];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off );

	GPIOD->OUTDR = 1<<6;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[3];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off );

	GPIOD->OUTDR = 1<<5;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[4];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off );

	GPIOD->OUTDR = 1<<4;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[5];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off );

	GPIOD->OUTDR = 1<<3;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[6];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off );

	GPIOD->OUTDR = 1<<2;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[7];
	DelaySysTick( on );

}


int main()
{
	SystemInit();

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

	while(1)
	{
		const uint8_t frame[8]= { 0x00, 0x00, 0x55, 0x55, 0xAA, 0xAA, 0x00, 0x00 };
		draw_frame( frame, 0.5*DELAY_US_TIME, 24*DELAY_US_TIME );
	}
}
