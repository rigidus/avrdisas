#include <avr/io.h>

volatile unsigned char LargeArray[10];
volatile unsigned char StorageByte;
uint16_t mycnt;
volatile uint16_t HugeArray[20];

void bar(unsigned char x) {
	PORTD = x;
	PORTB = LargeArray[7];
	LargeArray[4] = PORTD;
	StorageByte = TCNT0;
}

void foo(unsigned char y) {
	PORTB = y;
	bar(y - 1);
	TCNT1 = HugeArray[13];
	SREG = (mycnt & 0xff00) >> 8;
	OSCCAL = (mycnt & 0x00ff) >> 0;
}


int main() {
	while (1) {
		mycnt = TCNT1;
		foo(UBRRL);
		bar(UBRRH);
	}
	return 0;
}
