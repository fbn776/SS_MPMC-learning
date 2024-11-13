#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 25

int main() {
    FILE *input_fp = fopen("input.txt", "r");
    FILE *optab_fp = fopen("optab.txt", "r");
    FILE *symtab_fp_write = fopen("symtab.txt", "w");
    FILE *intermediate_fp = fopen("intermediate.txt", "w");
    FILE *len_fp = fopen("length.txt", "w");
    FILE *symtab_fp_read = fopen("symtab.txt", "r");

    char label[SIZE], opcode[SIZE], operand[SIZE], symbol[SIZE], address[SIZE], opvalue[SIZE], mnemonic[SIZE],
            error_desc[100];

    int start, locctr, op_found, error = 0;

    // Get the first line
    fscanf(input_fp, "%s\t%s\t%s", label, opcode, operand);
    if (strcmp(opcode, "START") == 0) {
        sscanf(operand, "%X", &start);
        locctr = start;
    } else {
        locctr = 0;
    }

    fprintf(intermediate_fp, "*\t%s\t%s\t%s\n", label, opcode, operand);
    fscanf(input_fp, "%s\t%s\t%s", label, opcode, operand);
    while (strcmp(opcode, "END") != 0 && error == 0) {
        fprintf(intermediate_fp, "%4X\t", locctr);
        if (strcmp(label, "-") != 0) {
            if (strcmp(label, "*") != 0) {
                while (fscanf(symtab_fp_read, "%s\t%s\t", symbol, address) != EOF) {
                    if (strcmp(label, symbol) == 0) {
                        error = 1;
                        strcat(error_desc, "ERROR : DUPLICATE SYMBOL ");
                        strcat(error_desc, label);
                        break;
                    }
                }
                rewind(symtab_fp_read);
                if (!error) {
                    fprintf(symtab_fp_write, "%s\t%X\t\n", label, locctr);
                    fflush(symtab_fp_write);
                }
            }
            if (strcmp(opcode, "WORD") == 0) {
                locctr += 3;
            } else if (strcmp(opcode, "RESW") == 0) {
                locctr += 3 * atoi(operand);
            } else if (strcmp(opcode, "RESB") == 0) {
                locctr += atoi(operand);
            } else if (strcmp(opcode, "BYTE") == 0) {
                locctr += strlen(operand) - 3;
            } else {
                while (fscanf(optab_fp, "%s\t%s", opvalue, mnemonic) != EOF) {
                    op_found = 0;
                    if (strcmp(opcode, opvalue) == 0) {
                        op_found = 1;
                        break;
                    }
                }
                rewind(optab_fp);
                if (op_found) {
                    locctr += 3;
                } else {
                    error = 1;
                    strcat(error_desc, "ERROR : OPCODE NOT FOUND ");
                    strcat(error_desc, opcode);
                }
            }
        }
        fprintf(intermediate_fp, "%s\t%s\t%s\n", label, opcode, operand);
        fscanf(input_fp, "%s\t%s\t%s", label, opcode, operand);
    }
    if (error) {
        printf("%s\n", error_desc);
    } else {
        int length = locctr - start;
        int size = locctr - start;
        fprintf(intermediate_fp, "*\t*\t%s\t*\n", opcode);
        printf("ASSEMBLED SUCCESSFULLY\n");
        printf("Length of the program : %d\n", length);
        fprintf(len_fp, "%d\t%X", length, size);
    }

    fclose(input_fp);
    fclose(optab_fp);
    fclose(symtab_fp_write);
    fclose(intermediate_fp);
    fclose(len_fp);
    fclose(symtab_fp_read);
}
