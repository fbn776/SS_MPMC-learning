#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define tsize 10
#define SIZE 50

FILE *intr_fp, *optab_fp, *symtab_fp_write, *len_fp, *output_fp, *obj_fp;

void main() {
    char label[SIZE], opcode[SIZE], operand[SIZE], locctr[SIZE], error_desc[SIZE];
    int start, error = 0, operand_value, t_length = 0;

    intr_fp = fopen("intermediate.txt", "r");
    optab_fp = fopen("optab.txt", "r");
    symtab_fp_write = fopen("symtab.txt", "r");
    len_fp = fopen("length.txt", "r");
    output_fp = fopen("output.txt", "w");
    obj_fp = fopen("object_program.txt", "w");

    fscanf(intr_fp, "%s\t%s\t%s\t%s", locctr, label, opcode, operand);
    if (strcmp(opcode, "START") == 0) {
        char size[SIZE];
        fprintf(output_fp, "\t%s\t%s\t%s\n", label, opcode, operand);
        sscanf(operand, "%d", &start);
        fprintf(obj_fp, "H^%-6s^00%d^0000%s\n", label, start, size);
        fprintf(obj_fp, "T^00%d^00", start);
    }

    fscanf(intr_fp, "%s\t%s\t%s\t%s", locctr, label, opcode, operand);
    while (strcmp(opcode, "END") != 0 && error == 0) {
        char obj_code[SIZE];
        int obj_generated = 0;

        if (strcmp(label, "-") != 0) {
            char mnemonic[SIZE],
                    opvalue[SIZE];
            int op_found = 0;
            while (fscanf(optab_fp, "%s\t%s", opvalue, mnemonic) != EOF) {
                if (strcmp(opcode, opvalue) == 0) {
                    op_found = 1;
                    strcat(obj_code, mnemonic);
                    if (isalpha(operand[0])) {
                        char address[SIZE],
                            symbol[SIZE];

                        int sym_found = 0;
                        while (fscanf(symtab_fp_write, "%s\t%s\t", symbol, address) != EOF) {
                            if (strcmp(symbol, operand) == 0) {
                                sym_found = 1;
                                strcat(obj_code, address);
                                obj_generated = 1;
                                break;
                            }
                        }
                        if (sym_found == 0) {
                            error = 1;
                            strcat(error_desc, "ERROR : UNDEFINED SYMBOL; ");
                            strcat(error_desc, operand);
                        }
                        rewind(symtab_fp_write);
                    }
                    break;
                }
            }
            rewind(optab_fp);
            if (op_found == 0) {
                if (strcmp(opcode, "WORD") == 0) {
                    // Convert operand to integer and append it to obj_code as a hexadecimal string
                    sscanf(operand, "%d", &operand_value); // Assuming WORD is in decimal format
                    char operand_str[10];
                    sprintf(operand_str, "%06X", operand_value); // Convert to a 6-digit hexadecimal string
                    strcat(obj_code, operand_str); // Concatenate operand as a string
                    obj_generated = 1;
                } else if (strcmp(opcode, "BYTE") == 0) {
                    if (operand[0] == 'X') {
                        // Handle BYTE with hexadecimal value
                        strncpy(obj_code, &operand[2], strlen(operand) - 3); // Skip X'...'
                        obj_generated = 1;
                    } else if (operand[0] == 'C') {
                        // Handle BYTE with character value
                        for (int i = 2; i < strlen(operand) - 1; i++) {
                            char byte_val[3];
                            sprintf(byte_val, "%02X", operand[i]); // Convert each char to hex
                            strcat(obj_code, byte_val);
                            obj_generated = 1;
                        }
                    }
                }
            }
        }
        if (obj_generated) {
            fprintf(obj_fp, "^%s", obj_code);
            t_length += 3;
        }
        fprintf(output_fp, "%s\t%s\t%s\t%s\t%s\n", locctr, label, opcode, operand, obj_code);
        strcpy(obj_code, "");
        fscanf(intr_fp, "%s\t%s\t%s\t%s", locctr, label, opcode, operand);
    }

    if (error == 1) {
        printf("%s\n", error_desc);
    } else {
        fseek(obj_fp, 30,SEEK_SET);
        fprintf(obj_fp, "%-2X", t_length);
        fseek(obj_fp, 0,SEEK_END);
        fprintf(obj_fp, "\nE^00%d\n", start);
        fprintf(output_fp, "\t\t%s\n", opcode);
        printf("ASSEMBLED SUCCESSFULLY\n");
    }

    fclose(intr_fp);
    fclose(optab_fp);
    fclose(symtab_fp_write);
    fclose(len_fp);
    fclose(output_fp);
    fclose(obj_fp);
}
