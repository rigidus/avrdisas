#include <unistd.h>

int Compare_Opcode(char *Bitstream, char *Bitmask);
void Register_Opcode(void (*Callback)(char*, int, int), const char *New_Opcode_String, int New_MNemonic);
void Supersede_Opcode(void (*Callback)(char*, int, int), int New_MNemonic);
int Get_Bitmask_Length(char *Bitmask);
void Clear_Registers();
char Get_From_Bitmask(char *Bitmask, int Byte, int Bit);
void Display_Binary(char *Bitstream, int Count);
int Match_Opcode(char *Bitmask, char *Bitstream);
int Get_Next_Opcode(char *Bitstream);
int Registers[256];
int Number_Opcodes;
struct Opcode Opcodes[256];
