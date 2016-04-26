.PHONY: all clean recompile

BINARY = ../bin/avrdisas
#PROGRAM = AsmExample
PROGRAM = myprogram
AVRDISAS_MCU = 2313
MCU = attiny$(AVRDISAS_MCU)

all: $(PROGRAM) $(PROGRAM).c

clean:
	rm -f $(PROGRAM).s $(PROGRAM) $(PROGRAM)_recompiled.bin $(PROGRAM).c

recompile: $(PROGRAM)
	avr-objcopy -j .text -O binary $(PROGRAM) $(PROGRAM)_recompiled.bin

sym: $(PROGRAM)
	./rename_label $(PROGRAM)
	rm -f $(PROGRAM)
	make all

$(PROGRAM).s: $(PROGRAM).bin $(PROGRAM).tag $(BINARY)
	$(BINARY) -p0 -l1 -o0 -t$(PROGRAM).tag -m$(AVRDISAS_MCU) $(PROGRAM).bin > $(PROGRAM).s

$(PROGRAM).c: $(PROGRAM).bin $(PROGRAM).tag $(BINARY)
	$(BINARY) -p1 -l1 -o0 -t$(PROGRAM).tag -m$(AVRDISAS_MCU) $(PROGRAM).bin > $(PROGRAM).c

$(PROGRAM): $(PROGRAM).s
	avr-gcc -nostdlib -mmcu=$(MCU) -o $(PROGRAM) $(PROGRAM).s

