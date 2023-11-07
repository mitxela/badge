#include "ch32v003fun.h"
#include <stdio.h>

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

#define ON  1
#define OFF 100
#define PAT1 0x55
#define PAT2 0xAA

	while(1)
	{
		GPIOC->OUTDR = 0x00FF; // display off
		Delay_Ms( OFF );

		GPIOD->OUTDR = 1<<0;
		GPIOA->OUTDR = 0;
		GPIOC->OUTDR = PAT1;
		Delay_Ms(ON);

		GPIOC->OUTDR = 0x00FF;
		Delay_Ms( OFF );

		GPIOD->OUTDR = 0;
		GPIOA->OUTDR = 1<<2;
		GPIOC->OUTDR = PAT2;
		Delay_Ms(ON);

		GPIOC->OUTDR = 0x00FF;
		Delay_Ms( OFF );

		GPIOD->OUTDR = 0;
		GPIOA->OUTDR = 1<<1;
		GPIOC->OUTDR = PAT1;
		Delay_Ms(ON);

		GPIOC->OUTDR = 0x00FF;
		Delay_Ms( OFF );

		GPIOD->OUTDR = 1<<6;
		GPIOA->OUTDR = 0;
		GPIOC->OUTDR = PAT2;
		Delay_Ms(ON);

		GPIOC->OUTDR = 0x00FF;
		Delay_Ms( OFF );

		GPIOD->OUTDR = 1<<5;
		GPIOA->OUTDR = 0;
		GPIOC->OUTDR = PAT1;
		Delay_Ms(ON);

		GPIOC->OUTDR = 0x00FF;
		Delay_Ms( OFF );

		GPIOD->OUTDR = 1<<4;
		GPIOA->OUTDR = 0;
		GPIOC->OUTDR = PAT2;
		Delay_Ms(ON);

		GPIOC->OUTDR = 0x00FF;
		Delay_Ms( OFF );

		GPIOD->OUTDR = 1<<3;
		GPIOA->OUTDR = 0;
		GPIOC->OUTDR = PAT1;
		Delay_Ms(ON);

		GPIOC->OUTDR = 0x00FF;
		Delay_Ms( OFF );

		GPIOD->OUTDR = 1<<2;
		GPIOA->OUTDR = 0;
		GPIOC->OUTDR = PAT2;
		Delay_Ms(ON);

	}
}
