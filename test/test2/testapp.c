#include <avr/io.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

const char string1[] PROGMEM = "I'm a string in flash.";
const char string2[] PROGMEM = "So am I!";
const char string3[] PROGMEM = "This is the third string.";
const char string4[] PROGMEM = "dddddddddddddddddddddd";
const char string5[] PROGMEM = "xData: \x12\x34\x56\x78\x90";
const char string6[] PROGMEM = "Intermixed \x11\x12\x13\x14\x15 and more";

const char databyte[] PROGMEM = { 0x13, 0x84, 0x19, 0x00, 0x84, 0x19, 0x04, 0x03, 0x00 };
const int dataword[] PROGMEM = { 0x1234, 0x5678, 0x1122, 0x3344, 0x5566, 0x7788, 0x8899, 0xaabb };

void transmit_char(unsigned char x) {
	UDR = x;
}

void transmit_string(PGM_P string) {
	PGM_P ptr;
	unsigned char zeichen;
	
	ptr = string;
	while ((zeichen = pgm_read_byte(&ptr[0]))) {
		transmit_char(zeichen);
		ptr++;
	}
}

int main() {
	unsigned char x, y;
	x = PORTD;
	while (x != 123) {
		y = PORTB;
		PORTD = y;
		PORTB = x;
		transmit_char('A');
		transmit_string(string1);
		transmit_string(string2);
		transmit_string(string3);
		transmit_string(string4);
		transmit_string(string5);
		transmit_string(string6);
		transmit_char('Z');
	}
	return 0;
}

