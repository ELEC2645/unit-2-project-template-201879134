#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "funcs.h"

char *get_input (void) {
    int size = 16;                     // start with the ideal size 
    char *buffer = malloc(size);       // initial allocation
    int length = 0;
    int c;

    if (!buffer) {
        printf("Error: memory allocation failed.\n");   //failsafe statement
        exit(1);
    }

    while ((c = getchar()) != '\n' && c != EOF) {
        buffer[length++] = c;

        if (length >= size) {                           //buffer can grow if needed
            size *= 2;
            char *new_buffer = realloc(buffer, size);
            if (!new_buffer) {                          //failsafe statement
                free(buffer);
                printf("Error: memory reallocation failed.\n");
                exit(1);
            }
            buffer = new_buffer;
        }
    }

    buffer[length] = '\0';
    return buffer;   // caller must free()
}

void disassembler(void) {
    printf("\n>> Disassembler\n\n");
    printf("Please enter a 16-digit binary number: ");
    char *input = get_input(); 

    if (strlen(input) < 16){                        //check the input is not too long or short
        printf("Error: not enough digits\n");
        free(input);
        return; 
    } else if (strlen(input) > 16){
        printf("Error: too many digits\n");
        free(input);
        return;
    }

    for (int i = 0; i < strlen(input); i++) {               //check the input only includes 0s or 1s
        if (input[i] != '0' && input[i] != '1'){
            printf("Error: input includes characters other than 0 and 1\n");
            free(input);
            return; 
        }
    }   

    Instruction disassembler = get_instr_disassembler(input);       //get the instruction type for the upcoming switch

    switch (disassembler) {
        case A_INSTR:               //A-Instruction
            a_disassembler(input); 
            break; 
        case C_INSTR:               //C-Instruction
            c_disassembler(input);
            break; 
        case INVALID_INSTR:         //Invalid
            printf("Error: the first digit was not 0 or 1\n");
            free(input);
            return;
    }

    free(input);
}

Instruction get_instr_disassembler (char input[]) {
    if (input[0] == '0'){           
        return A_INSTR; 
    } else if (input[0] == '1'){
        return C_INSTR;
    } else {
        return INVALID_INSTR;
    }
}

void a_disassembler(char input[]) {

    int value = 0; 
    for (int i = 0; i < 16; i++){           //binary to decimal converter
        if (input[i] != '0'){
            value += pow(2,15-i);
        }
    }
    
    printf("\nYour instruction in machine language is: @%d\n\n", value);

    char result[MAX_LEN];               //save the result as a char instead of int for save_to_file
    sprintf(result, "@%d", value);          
    save_to_file(result);
}

void c_disassembler(char input[]) {

    /* assigning pointers to value each function returns */
    char *comp = comp_disassembler(input);
    char *dest = dest_disassembler(input);
    char *jump = jump_disassembler(input);

    char machine_language[MAX_LEN];                         //save all the values into one string for save_to_file
    sprintf(machine_language, "%s%s%s", dest, comp, jump);  

    printf("\nYour instruction in machine language is: %s\n\n", machine_language);
    save_to_file(machine_language);
}

char *comp_disassembler(char input[]) { 

    int code = ((input[4] - '0') << 5) | ((input[5] - '0') << 4) | ((input[6] - '0') << 3) | ((input[7] - '0') << 2) | ((input[8] - '0') << 1) | (input[9] - '0');
    int mnemonic = input[3] - '0'; 

    if (mnemonic == 0){                     //compare input[4..9] against these switch statements to return the correct machine code
        switch (code) {
            case 0b101010: return "0"; 
            case 0b111111: return "1"; 
            case 0b111010: return "-1";
            case 0b001100: return "D"; 
            case 0b110000: return "A"; 
            case 0b001101: return "!D"; 
            case 0b110001: return "!A"; 
            case 0b001111: return "-D"; 
            case 0b110011: return "-A"; 
            case 0b011111: return "D+1"; 
            case 0b110111: return "A+1"; 
            case 0b001110: return "D-1";
            case 0b110010: return "A-1"; 
            case 0b000010: return "D+A";
            case 0b010011: return "D-A";
            case 0b000111: return "A-D";
            case 0b000000: return "D&A"; 
            case 0b010101: return "D|A"; 
            default: printf("Error: not a computation for the Hack computer\n"); break;
        }
    } else {
        switch (code) {
            case 0b110000: return "M"; 
            case 0b110001: return "!M"; 
            case 0b110011: return "-M"; 
            case 0b110111: return "M+1"; 
            case 0b110010: return "M-1"; 
            case 0b000010: return "D+M";
            case 0b010011: return "D-M";
            case 0b000111: return "M-D";
            case 0b000000: return "D&M"; 
            case 0b010101: return "D|M"; 
            default: printf("Error: not a computation for the Hack computer\n"); break;
        }
    }
}

char *dest_disassembler(char input[]) {

    int code = ((input[10] - '0') << 2) | ((input[11] - '0') << 1) | (input[12] - '0');

    switch (code) {                     //compare input[10..12] to return the correct destination
        case 0b000: return ""; 
        case 0b001: return "M="; 
        case 0b010: return "D=";
        case 0b011: return "DM="; 
        case 0b100: return "A=";
        case 0b101: return "AM=";
        case 0b110: return "AD=";
        case 0b111: return "ADM=";
        default: printf("Error: does not match any of the destination cases\n"); break;
    }
}

char *jump_disassembler(char input[]) {

    int code = ((input[13] - '0') << 2) | ((input[14] - '0') << 1) | (input[15] - '0');

    switch (code) {                     //compare input[13..15] to return the right jump functions
        case 0b000: return ""; 
        case 0b001: return ";JGT"; 
        case 0b010: return ";JEQ";
        case 0b011: return ";JGE"; 
        case 0b100: return ";JLT";
        case 0b101: return ";JNE";
        case 0b110: return ";JLE";
        case 0b111: return ";JMP";
        default: printf("Error: does not match any of the jump cases\n"); break;
    }
}

void assembler(void) {
    printf("\n>> Assembler\n\n");
    printf("\nPlease enter a machine code instruction: ");
    char *input = get_input();

    Instruction assembler = get_instr_assembler(input);         //get the instruction type 

    switch (assembler) {            //check if it is a or c instruction or invalid
        case A_INSTR:
            a_assembler(input);
            break;
        case C_INSTR:
            c_assembler(input);
            break;
        case INVALID_INSTR:
            free(input);
            printf("Error: does not include the necessary symbols\n");
            return;
    }

    free(input);
}

Instruction get_instr_assembler (char input[]) {

    for (int i = 0; i < strlen(input); i++){
        if (input[i] == '@'){
            return A_INSTR; 
        } else if (input[i] == ';' || input[i] == '='){
            return C_INSTR;  
        }
    }
    return INVALID_INSTR;
}

void a_assembler (char input[]) {

    int value = 0; 
    for (int i = 1; i < strlen(input); i++) {                    //take the value from the string and into an int for easier handling
        if (input[i] != '0' && input[i] != '1' && input[i] != '2' &&        //check the input does not include anyhting non-numerical
            input[i] != '3' && input[i] != '4' && input[i] != '5' && 
            input[i] != '6'  && input[i] != '7' && input[i] != '8' && input[i] != '9'){
                printf("Error: input includes non-numeric characters\n");
                return;
            }
        value = value * 10 + (input[i] - '0');
    }

    int reverse_binary[16] = {0}; 
    int i = 0;
    while (value > 0) {                     //binary to decimal converter
        reverse_binary[i] = value % 2; 
        value = value / 2; 
        i++;
    }

    char final_binary[17];
    for (int j = 15; j >= 0; j--){              //reverse the binary outpur for correct printing
        final_binary[15-j] = reverse_binary[j] ? '1' : '0';         //pass it to a char for the save_to_file function
    }
    final_binary[16] = '\0';

    printf("\nYour instruction in binary is: %s\n\n", final_binary);
    save_to_file(final_binary);
}

void c_assembler (char input[]) {

    /* assign pointers to the values returned in these functions*/
    const char *dest = dest_assembler(input);
    CompBits comp = comp_assembler(input);
    if (comp.comp == "------") return; 
    const char *jump = jump_assembler(input);

    char binary[MAX_LEN];
    sprintf(binary, "111%d%s%s%s", comp.mnemonic, comp.comp, dest, jump);       //store in one char for easier use and for the save_to_file

    printf("\nYour instruction in binary is: %s\n\n", binary);
    save_to_file(binary);
}

const char *dest_assembler (char input[]) {

    char dest[4]; 
    int i = 0; 

    if (strcmp(input, "=") != 0) return "000";              //if there is no '=' there is no destination

    while(input[i] != '=' && input[i] != '\0') {            //assign the section of the input before '=' to dest
        dest[i] = input[i];
        i++;
    }
    dest[i] = '\0';
    
    if (strcmp(dest, "M") == 0) return "001";           //find the destination and return the binary instruction
    if (strcmp(dest, "D") == 0) return "010";
    if (strcmp(dest, "DM") == 0) return "011";
    if (strcmp(dest, "A") == 0) return "100";
    if (strcmp(dest, "AM") == 0) return "101";
    if (strcmp(dest, "AD") == 0) return "110";
    if (strcmp(dest, "ADM") == 0) return "111"; 

}

CompBits comp_assembler(char input[]) {

    CompBits result; 
    result.mnemonic = 0; 

    char comp[4]; 
    int i = 0; 
    if (strchr(input, '=') != NULL){                        //if there is a '=' only read from one after this
        while (input[i] != '=' && input[i] != '\0') i++; 
        if (input[i] == '=') i++; 
    }

    int j = 0; 
    while (input[i] != ';' && input[i] != '\0') {           //assign the section of the input after '=' and until ';' to comp
        comp[j] = input[i];
        if (input[i] == 'M') result.mnemonic = 1;           //if the input includes an 'M', the mnemonic is 1
        i++;
        j++; 
    }
    comp[j] = '\0'; 
    
    if (result.mnemonic == 0) {                            //find the computation and return the binary instruction with the mnemonic
        if (strcmp(comp, "0") == 0) {result.comp = "101010"; return result;} 
        if (strcmp(comp, "1") == 0) {result.comp = "111111"; return result;} 
        if (strcmp(comp, "-1") == 0) {result.comp = "111010"; return result;} 
        if (strcmp(comp, "D") == 0) {result.comp = "001100"; return result;}
        if (strcmp(comp, "A") == 0) {result.comp = "110000"; return result;} 
        if (strcmp(comp, "!D") == 0) {result.comp = "001101"; return result;} 
        if (strcmp(comp, "!A") == 0) {result.comp = "110001"; return result;} 
        if (strcmp(comp, "-D") == 0) {result.comp = "001111"; return result;} 
        if (strcmp(comp, "-A") == 0) {result.comp = "110011"; return result;} 
        if (strcmp(comp, "D+1") == 0) {result.comp = "011111"; return result;}
        if (strcmp(comp, "A+1") == 0) {result.comp = "110111"; return result;} 
        if (strcmp(comp, "D-1") == 0) {result.comp = "001110"; return result;} 
        if (strcmp(comp, "A-1") == 0) {result.comp = "110010"; return result;}
        if (strcmp(comp, "D+A") == 0) {result.comp = "000010"; return result;} 
        if (strcmp(comp, "D-A") == 0) {result.comp = "010011"; return result;} 
        if (strcmp(comp, "A-D") == 0) {result.comp = "000111"; return result;} 
        if (strcmp(comp, "D&A") == 0) {result.comp = "000000"; return result;} 
        
    } else {
        if (strcmp(comp, "M") == 0) {result.comp = "110000"; return result;} 
        if (strcmp(comp, "!M") == 0) {result.comp = "110001"; return result;} 
        if (strcmp(comp, "-M") == 0) {result.comp = "110011"; return result;} 
        if (strcmp(comp, "M+1") == 0) {result.comp = "110111"; return result;} 
        if (strcmp(comp, "M-1") == 0) {result.comp = "110010"; return result;}
        if (strcmp(comp, "D+M") == 0) {result.comp = "000010"; return result;} 
        if (strcmp(comp, "D-M") == 0) {result.comp = "010011"; return result;} 
        if (strcmp(comp, "M-D") == 0) {result.comp = "000111"; return result;} 
        if (strcmp(comp, "D&M") == 0) {result.comp = "000000"; return result;} 
    }

    printf("Error: input did not match any computation cases\n");
    result.comp = "------";
    return result;   
}

const char *jump_assembler (char input[]) {

    char jump[4];
    int i = 0;

    while (input[i] != ';' && input[i] != '\0') i++;            //read only after the ';'
    if (input[i] == ';') i++;

    int j = 0; 
    while (input[i] != '\0') {              //assign the section of the input after ';' to jump
        jump[j] = input[i];
        i++;
        j++; 
    }
    jump[j] = '\0'; 

    if (strcmp(jump, "") == 0) return "000";            //find the jump command and return its binary equivalent
    if (strcmp(jump, "JGT") == 0) return "001"; 
    if (strcmp(jump, "JEQ") == 0) return "010"; 
    if (strcmp(jump, "JGE") == 0) return "011"; 
    if (strcmp(jump, "JLT") == 0) return "100"; 
    if (strcmp(jump, "JNE") == 0) return "101"; 
    if (strcmp(jump, "JLE") == 0) return "110"; 
    if (strcmp(jump, "JMP") == 0) return "111";  
}

void save_to_file (char result[]) {
    printf("If you would like to save your result to a file please press 'Y', if not enter any other key: ");

    char choice; 
    scanf("%c", &choice);
    getchar();                      //remove the extra new line 

    if (choice == 'y' || choice == 'Y'){

        printf("Please enter a file name: ");

        char filename[MAX_LEN];
        fgets(filename, MAX_LEN, stdin);
        filename[strcspn(filename, "\n")] = '\0';       //remove the new line for a null terminator

        FILE *f = fopen(filename, "w");                 //open a file the users choice of name
        if (!f){
            printf("Error: unable to open file\n");
            return;
        }

        fprintf(f, "%s", result);               //print the result to this file
        fclose(f);
    } else {
        printf("Result not saved.\n");
    }
}