
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "Globals.h"
#include "Opcodes.h"
#include "MNemonics.h"

int Compare_Opcode(char *Bitstream, char *Bitmask) {
	size_t i;
	char Bit;

	for (i = 0; i < strlen(Bitmask); i++) {
		if ((Bitmask[i] != 'x') && (Bitmask[i] != '1') && (Bitmask[i] != '0')) {
			fprintf(stderr, "Invalid Bitmask!\n");
			return 0;
		}
		
		if (Bitmask[i] == 'x') continue;	/* Ignore character */
		/* Retrieve the i-th Bit of Bitstream */
		Bit = (Bitstream[i / 8] >> (7 - (i % 8))) & 1;
/*		printf("Bit %d is %d [should be %c]\n",i,Bit,Bitmask[i]); */
		if ((Bitmask[i] == '1') && (Bit == 1)) continue;
		if ((Bitmask[i] == '0') && (Bit == 0)) continue;
		return 0;	/* No match */
	}
	return 1;		/* Match */
}

void Register_Opcode(void (*Callback)(char*, int, int), const char *New_Opcode_String, int New_MNemonic) {
	Number_Opcodes++;
	Opcodes[Number_Opcodes-1].Opcode_String = malloc(strlen(New_Opcode_String) + 1);
	strcpy(Opcodes[Number_Opcodes-1].Opcode_String, New_Opcode_String);
	Opcodes[Number_Opcodes-1].MNemonic = New_MNemonic;
	Opcodes[Number_Opcodes-1].Callback = Callback;
}

void Supersede_Opcode(void (*Callback)(char*, int, int), int New_MNemonic) {
	int i;
	for (i = 0; i < Number_Opcodes; i++) {
		if (Opcodes[i].MNemonic == New_MNemonic) {
			/* Supersede callback */
			Opcodes[i].Callback = Callback;
			return;
		}
	}
	fprintf(stderr, "Error: No callback to supersede opcode %d found (%s).\n", New_MNemonic, MNemonic[New_MNemonic]);
}

int Get_Bitmask_Length(char *Bitmask) {
	int Length = 0;
	size_t i;
	for (i = 0; i < strlen(Bitmask); i++) {
		if (Bitmask[i] != ' ') Length++;
	}
	return Length;
}

void Clear_Registers() {
	int i;
	for (i = 0; i < 256; i++) Registers[i] = 0;
}

char Get_From_Bitmask(char *Bitmask, int Byte, int Bit) {
	size_t i;
	int Cnt = 0;
	int GetBit;
	GetBit = (Byte * 8) + Bit;
	for (i = 0; i < strlen(Bitmask); i++) {
		if (Bitmask[i] != ' ') {
			if (Cnt == GetBit) return Bitmask[i];
			Cnt++;
		}
	}
	return '?';
}

void Display_Binary(char *Bitstream, int Count) {
	int i, j;
	for (i = 0; i < Count; i++) {
		for (j = 7; j >= 0; j--) {
			if ((Bitstream[i] & (1 << j)) != 0) printf("1");
				else printf("0");
			if (j == 4) printf(" ");
		}
		printf("  ");
		if ((((i + 1) % 2) == 0) && (i != 0)) printf("  ");
	}
	printf("\n");
}


int Match_Opcode(char *Bitmask, char *Bitstream) {
	int i;
	int Length;
	int Byte_Mask, Bit_Mask;
	int Byte_Stream, Bit_Stream;
	char Mask_Val, Stream_Val;
	
	Clear_Registers();
	Length = Get_Bitmask_Length(Bitmask);
	
	for (i = 0; i < Length; i++) {
		Byte_Mask = i / 8;
		Bit_Mask = i % 8;
		
		Byte_Stream = i / 8;
		Byte_Stream ^= 1;	/* Invert last bit */
		Bit_Stream = 7 - (i % 8);
		
		Mask_Val = Get_From_Bitmask(Bitmask, Byte_Mask, Bit_Mask);
		Stream_Val = (Bitstream[Byte_Stream] >> Bit_Stream) & 0x01;
		
/*		printf("Extracting Bit %2d: Maske = (%d, %d) [%c], Stream = (%d, %d) [%d] ",i,Byte_Mask,Bit_Mask,Mask_Val,Byte_Stream,Bit_Stream,Stream_Val); */
		if ((Mask_Val == '0') || (Mask_Val == '1')) {
			/* This Bit is a identification Bit */
			if (Mask_Val == '0') {
				if (Stream_Val == 1) {
/*					printf("\nMatch failed.\n");*/
					return 0;
				}
			} else {
				if (Stream_Val == 0) {
/*					printf("\nMatch failed.\n");*/
					return 0;
				}
			}
		} else {
			/* This Bit is a register Bit, set in appropriate place */
			Registers[(int)Mask_Val] <<= 1;
			Registers[(int)Mask_Val] |= Stream_Val;
/*			printf("-> %d Stored [%x]",Stream_Val,Registers[(int)Mask_Val]); */
		}
/*		printf("\n"); */
	}
	return 1;
}

int Get_Next_Opcode(char *Bitstream) {
	int i;
	for (i = 0; i < Number_Opcodes; i++) {
		if (Match_Opcode(Opcodes[i].Opcode_String, Bitstream) == 1) {
			return i;
		}
	}
	return -1;
}
