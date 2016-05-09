/*
	avrdisas - A disassembler for AVR microcontroller units
	Copyright (C) 2007 Johannes Bauer
	
	This file is part of avrdisas.

    avrdisas is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    avrdisas is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with avrdisas; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	Johannes Bauer
	Mussinanstr. 140
	92318 Neumarkt/Opf.
	JohannesBauer@gmx.de
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Globals.h"
#include "Opcodes.h"
#include "MNemonics.h"
#include "Tagfile.h"
#include "JumpCall.h"

extern struct Options Options;
static int JumpCall_Count;
static struct JumpCall *JumpCalls;

void Display_JumpCalls() {
	int i;
	printf("%d jumps/calls found:\n", JumpCall_Count);
	for (i = 0; i < JumpCall_Count; i++) {
		printf("%3d: 0x%-4x -> 0x%-4x     %s (%d)\n", i, (unsigned int)JumpCalls[i].From, (unsigned int)JumpCalls[i].To, MNemonic[JumpCalls[i].Type], JumpCalls[i].FunctionCall);
	}
}

int FixTargetAddress(int Address) {
	if (Options.FlashSize) {
		Address %= Options.FlashSize;
		if (Address < 0) {
			Address += Options.FlashSize;
		}
	}
	return Address;
}

void Register_JumpCall(int From, int To, int Type, unsigned char FunctionCall) {
	if ((Options.Process_Labels == 1) && (Options.Pass == 1)) {
		JumpCall_Count++;
		JumpCalls = realloc(JumpCalls, sizeof(struct JumpCall) * (JumpCall_Count));
		JumpCalls[JumpCall_Count - 1].From = From;
		JumpCalls[JumpCall_Count - 1].To = To;
		JumpCalls[JumpCall_Count - 1].Type = Type;
		JumpCalls[JumpCall_Count - 1].LabelNumber = 0;
		JumpCalls[JumpCall_Count - 1].ContainingFunction = -1;
		JumpCalls[JumpCall_Count - 1].FunctionCall = FunctionCall;
	}
}

int JC_Comparison(const void *Element1, const void *Element2) {
	struct JumpCall *JC1, *JC2;
	JC1 = (struct JumpCall*)Element1;
	JC2 = (struct JumpCall*)Element2;
	if ((JC1->To) > (JC2->To)) {
		return 1;
	} else if ((JC1->To) == (JC2->To)) {
		if ((JC1->From) > (JC2->From)) {
			return 1;
		} else if ((JC1->From) == (JC2->From)) {
			return 0;
		}
		return -1;
	}
	return -1;
}

void Sort_JumpCalls() {
	qsort(JumpCalls, JumpCall_Count, sizeof(struct JumpCall), JC_Comparison);
}

void Correct_Label_Types(void) {
	int i, j;
	int LastIdx = 0;
	int LastDest = JumpCalls[0].To;
	char CurType = JumpCalls[0].FunctionCall;
	
	for (i = 1; i < JumpCall_Count; i++) {
		if (JumpCalls[i].To != LastDest) {
			for (j = LastIdx; j < i; j++) JumpCalls[j].FunctionCall = CurType;
			LastIdx = i;
			LastDest = JumpCalls[i].To;
			CurType = 0;
		}
		CurType = (CurType || JumpCalls[i].FunctionCall);
	}
	for (j = LastIdx; j < JumpCall_Count; j++) JumpCalls[j].FunctionCall = CurType;
}

int Recurse_Entrypoint(char * Bitstream, int * previous_stack, int depth){
    int Address = *previous_stack;
    int recursions_stack[100];

    if(depth > 100){
        return -1;
    }
    recursions_stack[depth] = Address;
    int i;
    for(i = 0; i < JumpCall_Count; i++){
        if(JumpCalls[i].To == Address){
        }
    }

    int Opcode_idx_ret = 0;
    for (i = 0; i < Number_Opcodes; i++) {
        if (memcmp(MNemonic[Opcodes[i].MNemonic], "ret", 3) == 0) {
            Opcode_idx_ret = i;
            break;
        }
    }

    int retval = -1;
    /*printf("Depth: %u\n", depth);*/

    while(Address){
        /*int Opcode = Get_Next_Opcode(Bitstream + Address);*/
        int isret = Match_Opcode(Opcodes[Opcode_idx_ret].Opcode_String, Bitstream + Address);
        int TagIndex;
        /*printf("At addr %04x, opcode %u\n", Address, Opcode);*/
        if(isret){
            /*printf("Is return!\n");*/
            /*retval = Address+2;*/
            return -1;
            /*goto out;*/
        }

        TagIndex = Tagfile_FindLabelAddress(Address);
        if(TagIndex != -1 && Tagfile_GetLabel(TagIndex)){
            /*printf("Found function %s\n", Tagfile_GetLabel(TagIndex));*/
            retval = Address;
            goto out;
        }

        for(i = 0; i < JumpCall_Count; i++){
            if(JumpCalls[i].To == Address){
                if(JumpCalls[i].ContainingFunction >= 0){
                    /*printf("Found previously known calling function here from %04x!\n", JumpCalls[i].ContainingFunction);*/
                    return JumpCalls[i].ContainingFunction;
                }
                /*printf("Found jump to here from %04x!\n", JumpCalls[i].From);*/
                recursions_stack[depth+1] = JumpCalls[i].From;
                /*printf("Recursing...\n");*/
                int result = Recurse_Entrypoint(Bitstream, recursions_stack+depth+1, depth+1);
                if(result != -1){
                    retval = result;
                    goto out;
                }
            }
            if(JumpCalls[i].From == Address){
                /*printf("Found jump from here to %04x!\n", JumpCalls[i].To);*/
                if(JumpCalls[i].ContainingFunction >= 0){
                    /*printf("Found previously known contained function here from %04x!\n", JumpCalls[i].ContainingFunction);*/
                    return JumpCalls[i].ContainingFunction;
                }
            }
        }
        Address -= 2;
    }
out: {
        int j;
        for(j = 0; j < depth; j++){
            if(recursions_stack[i] == retval){
                return -1;
            }
        }
        JumpCalls[i].ContainingFunction = retval;
        return retval;
    }
}

void Label_JumpCalls(char* Bitstream){
    int i;
    for(i = 0; i < JumpCall_Count; i++){
        /*int callee = JumpCalls[i].To;*/
        /*int TagIndex;*/
        /*TagIndex = Tagfile_FindLabelAddress(callee);*/
        if(JumpCalls[i].FunctionCall){
            /*printf("Function call to function %s at %04x from %04x\n", Tagfile_GetLabel(TagIndex), JumpCalls[i].To, JumpCalls[i].From);*/
            int caller = Recurse_Entrypoint(Bitstream, &JumpCalls[i].From, 0);
            JumpCalls[i].ContainingFunction = caller;
            /*TagIndex = Tagfile_FindLabelAddress(caller);*/
            /*printf("Label %04x called by function %s at %04x (%04x)\n", JumpCalls[i].To, Tagfile_GetLabel(TagIndex), JumpCalls[i].From, JumpCalls[i].ContainingFunction);*/
        } else {
            /*printf("Jump to label at %04x from %04x\n", JumpCalls[i].To, JumpCalls[i].From);*/
            int caller = Recurse_Entrypoint(Bitstream, &JumpCalls[i].From, 0);
            JumpCalls[i].ContainingFunction = caller;
            /*TagIndex = Tagfile_FindLabelAddress(caller);*/
            /*printf("Label %04x jumped to by function %s at %04x (%04x)\n", JumpCalls[i].To, Tagfile_GetLabel(TagIndex), JumpCalls[i].From, JumpCalls[i].ContainingFunction);*/
        }
    }
}

void Enumerate_Labels(char* Bitstream) {
	int i;
	int CurrentLabelNumber = 0;
	int CurrentFunctionNumber = 0;
	int Destination;

	if (JumpCall_Count < 2) return;
	
	Sort_JumpCalls();
	Correct_Label_Types();	
	
	Destination = JumpCalls[0].To;
	if (JumpCalls[0].FunctionCall) CurrentFunctionNumber++;
		else CurrentLabelNumber++;
	for (i = 0; i < JumpCall_Count; i++) {
		if (Destination != JumpCalls[i].To) {
			if (JumpCalls[i].FunctionCall) CurrentFunctionNumber++;
				else CurrentLabelNumber++;
			Destination = JumpCalls[i].To;
		}
		if (JumpCalls[i].FunctionCall) JumpCalls[i].LabelNumber = CurrentFunctionNumber;
			else JumpCalls[i].LabelNumber = CurrentLabelNumber;
	}
    Label_JumpCalls(Bitstream);
}

char *Get_Label_Name(int Destination, char **LabelComment) {
	int i;
	static char Buffer[256];
	int TagIndex;
	char *TagLabel;

	TagIndex = Tagfile_FindLabelAddress(Destination);
	if (TagIndex != -1) {
		TagLabel = Tagfile_GetLabel(TagIndex);
		if (LabelComment != NULL) *LabelComment = Tagfile_GetLabelComment(TagIndex);
        if(TagLabel){
            snprintf(Buffer, sizeof(Buffer), "%s", TagLabel);
        }
		return Buffer;
	}
	
	for (i = 0; i < JumpCall_Count; i++) {
		if ((JumpCalls[i].To) == Destination) {
			if (JumpCalls[i].FunctionCall) {
 				snprintf(Buffer, sizeof(Buffer), "Function_0x%x", JumpCalls[i].To);
			} else {
                snprintf(Buffer, sizeof(Buffer), "Label_0x%x", JumpCalls[i].To);
			}
			return Buffer;
		}
	}
	
  snprintf(Buffer, sizeof(Buffer), "UNKNOWN(0x%x)",Destination);
	return Buffer;
}

char *Get_Containing_Label(int Address){
    char * comment = NULL;
    return Get_Label_Name(Address, &comment);
}

/* Show all references which refer to "Position" as destination */
void Print_JumpCalls(int Position) {
	int i;
	int Match = 0;
	
	for (i = 0; i < JumpCall_Count; i++) {
		if ((JumpCalls[i].To) == Position) {
			if (Match == 0) {
                printf("\n");
				Match = 1;
			}
            const char* caller;
            if(JumpCalls[i].ContainingFunction >= 0){
                caller = Get_Containing_Label(JumpCalls[i].ContainingFunction);
            } else {
                caller = Get_Containing_Label(JumpCalls[i].From);
            }
            printf("; Referenced from offset 0x%02x <%s> by %s\n", JumpCalls[i].From, caller, MNemonic[JumpCalls[i].Type]);
		}
	}
    int TagIndex = Tagfile_FindLabelAddress(Position);
    if(TagIndex != -1){
        Match = 1;
    }
	if (Match == 1) {
		char *LabelName = NULL;
		char *LabelComment = NULL;
        LabelName = Get_Label_Name(Position, &LabelComment);
        if(Tagfile_GetLabel(TagIndex)){
            printf("%s:", LabelName);
        }
        if(LabelComment) {
             printf("; %s", LabelComment);
        }
        printf("\n");
	}
}

