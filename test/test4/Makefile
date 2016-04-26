COMPILE = attiny2313
DISASSEMBLE = tn2313

CFLAGS = -O2 -Wall -Werror

.PHONY: all clean

all: testapp.dis

clean:
	rm -f testapp testapp.bin testapp.dis testapp.map

testapp.bin: testapp
	avr-objcopy -j .text -O binary testapp testapp.bin

testapp: testapp.c
	avr-gcc $(CFLAGS) -mmcu=$(COMPILE) -o testapp testapp.c

testapp.dis: testapp.bin testapp
	../../tools/obj2tag testapp > testapp.map
	../../bin/avrdisas -a1 -p1 -l1 -ttestapp.map -m$(DISASSEMBLE) testapp.bin > testapp.dis
	cat testapp.dis

