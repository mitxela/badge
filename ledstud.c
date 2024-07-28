#include "ch32v003fun.h"
#include <stdio.h>
#include <string.h>

#include "anim/frames.h"
#include "anim/font.h"

// AHB prescaler is set to 32
#undef FUNCONF_SYSTEM_CORE_CLOCK  
#define FUNCONF_SYSTEM_CORE_CLOCK 1500000  // 48MHz / 32 = 1.5MHz
#undef DELAY_US_TIME
#undef DELAY_MS_TIME
#define DELAY_US_TIME ((FUNCONF_SYSTEM_CORE_CLOCK)/1000000)
#define DELAY_MS_TIME ((FUNCONF_SYSTEM_CORE_CLOCK)/1000)


enum {
	MODE_VIDEO,
	MODE_TEXT,
	MODE_BLINKY,
	MODE_SLEEP,
	NUM_MODES,
	MODE_LEARN = 99 // outside of modulo loop
};

uint8_t mode __attribute__ ((section (".no_init")));

static inline void draw_frame( const uint8_t* bitmap, uint32_t on, uint32_t off )
{
	GPIOD->OUTDR = (1<<0) |(1<<7);
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[0];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );

	GPIOD->OUTDR = 0 |(1<<7);
	GPIOA->OUTDR = 1<<2;
	GPIOC->OUTDR = bitmap[1];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );

	GPIOD->OUTDR = 0 |(1<<7);
	GPIOA->OUTDR = 1<<1;
	GPIOC->OUTDR = bitmap[2];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );

	GPIOD->OUTDR = (1<<6) |(1<<7);
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[3];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );

	GPIOD->OUTDR = (1<<5) |(1<<7);
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[4];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );

	GPIOD->OUTDR = (1<<4) |(1<<7);
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[5];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );

	GPIOD->OUTDR = (1<<3) |(1<<7);
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[6];
	DelaySysTick( on );

	GPIOC->OUTDR = 0x00FF;
	DelaySysTick( off-on );

	GPIOD->OUTDR = (1<<2) |(1<<7);
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

	GPIOD->OUTDR = (1<<0) |(1<<7);
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[0];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;

	GPIOD->OUTDR = 0 |(1<<7);
	GPIOA->OUTDR = 1<<2;
	GPIOC->OUTDR = bitmap[1];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;

	GPIOD->OUTDR = 0 |(1<<7);
	GPIOA->OUTDR = 1<<1;
	GPIOC->OUTDR = bitmap[2];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;

	GPIOD->OUTDR = (1<<6) |(1<<7);
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[3];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;

	GPIOD->OUTDR = (1<<5) |(1<<7);
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[4];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;

	GPIOD->OUTDR = (1<<4) |(1<<7);
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[5];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;

	GPIOD->OUTDR = (1<<3) |(1<<7);
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = bitmap[6];
	fast_delay();

	GPIOC->OUTDR = 0x00FF;

	GPIOD->OUTDR = (1<<2) |(1<<7);
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

void init_adc()
{
	RCC->APB2PRSTR |= RCC_APB2Periph_ADC1;
	RCC->APB2PRSTR &= ~RCC_APB2Periph_ADC1;

	RCC->CFGR0 &= ~(0x1F<<11); //RCC_ADCPRE = 0
	ADC1->RSQR1 = 0;
	ADC1->RSQR2 = 0;
	ADC1->RSQR3 = 7;
	// sampling time
	ADC1->SAMPTR2 &= ~(ADC_SMP0<<(3*7));
	ADC1->SAMPTR2 |= 7<<(3*7);

	// turn on, set sw trig
	ADC1->CTLR2 |= ADC_ADON | ADC_EXTSEL;
	// reset calibration
	ADC1->CTLR2 |= ADC_RSTCAL;
	while(ADC1->CTLR2 & ADC_RSTCAL);
	// calibrate
	ADC1->CTLR2 |= ADC_CAL;
	while(ADC1->CTLR2 & ADC_CAL);
}


uint16_t read_adc(void)
{
	ADC1->CTLR2 |= ADC_SWSTART;
	while(!(ADC1->STATR & ADC_EOC));
	return ADC1->RDATAR;
}

static inline _Bool read_input()
{
	#define threshold 0x100
	ADC1->CTLR2 |= ADC_SWSTART;
	while(!(ADC1->STATR & ADC_EOC));
	return (ADC1->RDATAR > threshold);
}


uint8_t receive()
{
	#define bitperiod 150
	#define numbits 7
	uint8_t x = 0;
	uint32_t until;

	// wait for start condition
	while (read_input());
	until = SysTick->CNT;
	#define wait(n) \
		until += (n)*DELAY_MS_TIME; \
		while( ((int32_t)( SysTick->CNT - until )) < 0 ) {};

	wait(bitperiod/2);

	if (read_input()) {printf("glitch\n");return 255;} // glitch

	for (uint8_t i=0;i<numbits;i++) {
		wait(bitperiod);
		x |= ( read_input()?(1<<i):0 );
	}

	// expect stop bit
	wait(bitperiod);
	if (read_input()==0) printf("frame error\n");
	while (read_input()==0);

	return x;
}

void mode_learn()
{
	// cancel if button pressed again
	mode = MODE_TEXT-1;

	//start with ack animation
	const uint8_t temp_anim[2][8] = {
		{0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF},
		{0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00}
	};
	for (int j=0;j<15;j++){
		for (int i=0;i<200;i++) {
			draw_frame( &temp_anim[0], 8, 50 );
		}
		for (int i=0;i<200;i++) {
			draw_frame( &temp_anim[1], 8, 50 );
		}
	}

	//RCC->APB2PCENR &= ~RCC_APB2Periph_AFIO;
	NVIC_DisableIRQ( EXTI7_0_IRQn );

	// Enable op-amp on N1 (PD0) and P1 (PD7)
	EXTEN->EXTEN_CTR |= EXTEN_OPA_EN | EXTEN_OPA_PSEL | EXTEN_OPA_NSEL;

	// reset states
	GPIOA->CFGLR = 0x44444444;
	GPIOD->CFGLR = 0x44444444;
	GPIOC->CFGLR = 0x44444444;
	GPIOA->OUTDR = 0;
	GPIOC->OUTDR = 0;
	GPIOD->OUTDR = 0;

	// set row 1 to row 8 as output high
	GPIOC->CFGLR = 0xFFFFFFFF;
	GPIOC->CFGLR |= (
		(GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0) |
		(GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*1) |
		(GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*2) |
		(GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*3) |
		(GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*4) |
		(GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*5) |
		(GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*6) |
		(GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*7));
	GPIOC->OUTDR |= 0xFF;

	// PD7 (NRST) as input with pullup
	GPIOD->CFGLR &= ~(0xf<<(4*7));
	GPIOD->CFGLR |= ((GPIO_CNF_IN_PUPD)<<(4*7));
	GPIOD->OUTDR |= (1<<7);

	RCC->APB2PCENR |= RCC_APB2Periph_ADC1;
	GPIOD->CFGLR &= ~((0xf<<(4*4)) | (0xf<<(4*5)));
	GPIOD->CFGLR |= ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*5));
	GPIOD->OUTDR |= (1<<5);
	GPIOA->CFGLR &= ~(0xf<<(4*1));
	GPIOA->OUTDR &= ~(1<<1);
	init_adc();


#ifdef bargraph
	char bar[65]={0};
	while (1) {
		uint16_t x = read_adc();
		memset(bar,' ',64);
		memset(bar,'*',x>>4);
		printf("ADC: %s%04x\n", bar,x);
		Delay_Ms(50);
	}
#endif

	while(1){
		uint8_t x = receive();
		printf("%c",x);
	}


	NVIC_SystemReset();
}

void EXTI7_0_IRQHandler( void ) __attribute__((interrupt));
void EXTI7_0_IRQHandler( void )
{
	GPIOC->OUTDR = 0x00FF;

	uint32_t start = SysTick->CNT;

	while((GPIOD->INDR & (1<<7)) == 0) {};

	if (SysTick->CNT - start > 500*DELAY_MS_TIME) {
		mode = MODE_LEARN;
	}
	if (SysTick->CNT - start > 16*DELAY_MS_TIME) {
		NVIC_SystemReset();
		while(1) {};
	}

	EXTI->INTFR = 1<<7;
}

int main()
{
	SystemInit();

	if (mode!=MODE_LEARN) {
		// check software reset and not power-on reset
		if ((RCC->RSTSCKR & RCC_SFTRSTF) && !(RCC->RSTSCKR & RCC_PORRSTF)) {
			mode = (mode+1)%NUM_MODES;
		} else mode = 0;
	}

	RCC->RSTSCKR |= RCC_RMVF; // clear reset flags


	if (mode == MODE_SLEEP) {

		// Power consumption is reduced significantly if GPIOs are enabled and set to pull-up
		RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_AFIOEN;

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

		AFIO->EXTICR = 3<<(7*2); // PD7 [NRST]
		EXTI->EVENR = 1<<7;
		EXTI->FTENR = 1<<7; // falling edge trigger

		NVIC->SCTLR |= (1<<2); //SLEEPDEEP
		PWR->CTLR &= PWR_CTLR_PDDS;
		__WFE();
		NVIC_SystemReset();
	}

	// All other modes - enable display

	RCC->CFGR0 |= (12<<4); // set AHB prescaler to 32 -> sysclk 1.5MHz

	RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO;

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
				 | (GPIO_CNF_IN_PUPD << (4*7))
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

	//Configure reset pin as GPIO with pullup triggering ISR
	GPIOD->OUTDR |= (1<<7); // pull up
	// wait for release before enabling interupt
	while((GPIOD->INDR & (1<<7)) == 0) {};

	AFIO->EXTICR = 3<<(7*2); // PD7 [NRST]
	EXTI->INTENR = 1<<7;
	EXTI->FTENR = 1<<7; // falling edge trigger
	NVIC_EnableIRQ( EXTI7_0_IRQn );

	switch (mode) {
		case MODE_VIDEO: mode_video();
		case MODE_TEXT: mode_text();
		case MODE_BLINKY: mode_blinky();
		case MODE_LEARN: mode_learn();
	}
}
