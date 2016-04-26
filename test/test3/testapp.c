#include <avr/io.h>

int main() {

	asm("symbol1:");
	asm("rcall symbol1");
	asm("rjmp symbol1");
	
	asm("symbol2:");
	asm("rjmp symbol2");
	asm("rcall symbol2");

	{
		char x, y;
		x = PORTB;
		y = PORTD;
		if (x == 0) {
			DDRB = 0x00;
		}
		if (y < 0) {
			DDRD = 0x00;
		}
	}

	asm("icall");
	
	return 0;
}
