#include "gpio.h"
#include "uart.h"
#include <stdio.h>

typedef struct {
	volatile uint32_t RESERVED0[321];
	volatile uint32_t OUT;
	volatile uint32_t OUTSET;
	volatile uint32_t OUTCLR;
	volatile uint32_t IN;
	volatile uint32_t DIR;
	volatile uint32_t DIRSET;
	volatile uint32_t DIRCLR;
	volatile uint32_t RESERVED1[120];
	volatile uint32_t PIN_CNF[32];
} NRF_GPIO_REGS;

void toggle_leds();
ssize_t _write(int fd, const void *buf, size_t count);

int main(){
	// Configure LED Matrix
	for(int i = 4; i <= 15; i++){
		GPIO->DIRSET = (1 << i);
		GPIO->OUTCLR = (1 << i);
	}

	// Configure buttons
	GPIO->PIN_CNF[17] = 0;
	GPIO->PIN_CNF[18] = 0;

	int sleep = 0;
	while(1){
		char rx = uart_read();

		uart_send(rx);

		if (!(GPIO->IN & (1<< 17))){
			uart_send('A');
		}
		if(!(GPIO->IN & (1<<26))){
			uart_send('B');
		}

		if (rx != '\0') toggle_leds();

		sleep = 10000;
		while(--sleep);
	}
	return 0;
}

void toggle_leds(){
	static int leds_on;
	if(leds_on){
		GPIO->OUTCLR = (1 << 13);
		GPIO->OUTCLR = (1 << 14);
		GPIO->OUTCLR = (1 << 15);
		leds_on = 0;
	}
	else{
		GPIO->OUTSET = (1 << 13);
		GPIO->OUTSET = (1 << 14);
		GPIO->OUTSET = (1 << 15);
		leds_on = 1;
	}
}

ssize_t _write(int fd, const void *buf, size_t count){
	char * letter = (char *)(buf);
	for(int i = 0; i < count; i++){
		uart_send(*letter);
		letter++;
	}
	return count;
}