MCU_GCC = attiny2313
MCU_AVRDISAS = 2313
#MCU_GCC = atmega128
#MCU_AVRDISAS = m128

CFLAGS = -O2 -Wall -Werror

.PHONY: all clean

all: testappnew.bin

clean:
	rm -f testapp testapp.bin testapp.s testappnew testappnew.bin

testapp.bin: testapp
	avr-objcopy -j .text -O binary testapp testapp.bin

testapp: testapp.c
	avr-gcc $(CFLAGS) -mmcu=$(MCU_GCC) -o testapp testapp.c

testapp.s: testapp.bin
	../../bin/avrdisas -p0 -l1 -o0 -ttestapp.tag -m$(MCU_AVRDISAS) testapp.bin > testapp.s

testappnew: testapp.s
	avr-gcc -nostdlib -mmcu=$(MCU_GCC) -o testappnew testapp.s
#	avr-objdump -D testappnew

testappnew.bin: testappnew
	avr-objcopy -j .text -O binary testappnew testappnew.bin
	diff testappnew.bin testapp.bin
	
