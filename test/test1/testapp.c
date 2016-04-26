#include <avr/io.h>

int main() {
	unsigned char var1 = 10;
	unsigned char var2 = 20;
	
	var1 -= var2;

	__asm__ __volatile__("nop");
	__asm__ __volatile__("sts 0x00e9, r24");
	__asm__ __volatile__("lds r24, 0x00e9");
	__asm__ __volatile__("tst r11");
	__asm__ __volatile__("clr r12");
	__asm__ __volatile__("lsl r13");
	__asm__ __volatile__("lsr r14");
	__asm__ __volatile__("rol r15");
	__asm__ __volatile__("ror r16");
	__asm__ __volatile__("nop");
	
	return 0;
}
