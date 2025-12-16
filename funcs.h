#ifndef FUNCS_H
#define FUNCS_H
#define MAX_LEN 100

typedef enum {
    A_INSTR, C_INSTR, INVALID_INSTR
} Instruction; 

typedef struct {
    int mnemonic; 
    const char *comp; 
} CompBits;

void main_menu(void); /*from main.c goes back to start*/

char *get_input(void);               /* get input with correct size buffer */
void disassembler(void);                     /* checks the input and calls the next function */
Instruction get_instr_disassembler(char input[]);
void a_disassembler(char input[]);           /* convert the binary value into decimal */
void c_disassembler(char input[]);           /* call computation, destination and jump functions and print their return */
char *comp_disassembler(char input[]);       /* check the c-bits against switch statement */
char *dest_disassembler(char input[]);       /* check the d-bits against switch statement */
char *jump_disassembler(char input[]);       /* check the j-bits against switch statement */
void assembler(void);                        /* get input, check it and pass it to next functions */
Instruction get_instr_assembler(char input[]);
void a_assembler(char input[]);              /* convert the decimal number into binary */
void c_assembler(char input[]);              /* call destination, computation and jump functions */
const char *dest_assembler(char input[]);    /* check the destination with if statements */
CompBits comp_assembler(char input[]);       /* check the computation with if statements*/
const char *jump_assembler(char input[]);    /* check the jump with if statements */
void save_to_file(char result[]);

#endif