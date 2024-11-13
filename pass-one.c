#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 50

char label[SIZE],
        operator[SIZE],
        operand[SIZE],
        pgm_name[SIZE];

int start = 0,
        locctr = 0;

FILE *input_fp,
        *intrmt_fp,
        *len_fp,
        *symtab_fp_write,
        *symtab_fp_read;

int read_instr() {
    return fscanf(input_fp, " %s %s %s", label, operator, operand);
}

void print_instr() {
    printf("Label: %10s | Operator: %10s | Operand: %10s\n", label, operator, operand);
}

int main() {
    input_fp = fopen("input.txt", "r");
    intrmt_fp = fopen("intermediate.txt", "w");
    len_fp = fopen("length.txt", "w");
    symtab_fp_write = fopen("symtab.txt", "w");
    symtab_fp_read = fopen("symtab.txt", "r");

    // Read first line and check for start
    read_instr();
    if (strcmp(operator, "START") == 0) {
        strcpy(pgm_name, label);
        sscanf(operand, "%x", &start);
        locctr = start;
    }

    fprintf(intrmt_fp, "*\t%s\t%s\t%X\n", label, operator, locctr);
    read_instr();
    while (strcmp(operator, "END") != 0) {
        print_instr();

        if (strcmp(label, "*") != 0) {
            char sym_label[SIZE];
            int sym_addr;
            while (fscanf(symtab_fp_read, " %s %X", sym_label, &sym_addr) != EOF) {
                if (strcmp(sym_label, label) == 0) {
                    printf("ERROR: Duplicate label found\n");
                    exit(1);
                }
            }

            rewind(symtab_fp_read);
            printf("LABEL: %s | ADDR: %X\n", label, locctr);
            fprintf(symtab_fp_write, "%s\t%X\n", label, locctr);
            fflush(symtab_fp_write);
        }

        fprintf(intrmt_fp, "%X", locctr);

        if (strcmp(operator, "WORD") == 0) {
            locctr += 3;
        } else if (strcmp(operator, "RESW") == 0) {
            locctr += 3 * atoi(operand);
        } else if (strcmp(operator, "RESB") == 0) {
            locctr += atoi(operand);
        } else if (strcmp(operator, "BYTE") == 0) {
            locctr += strlen(operand);
        } else {
            locctr += 3;
        }

        fprintf(intrmt_fp, "\t%s\t%s\t%s\n", label, operator, operand);
        read_instr();
    }

    // Add END line to intermediate
    fprintf(intrmt_fp, "*\t*\tEND\t*");


    fprintf(len_fp, "%d", locctr - start);

    fclose(input_fp);
    fclose(intrmt_fp);
    fclose(len_fp);
    fclose(symtab_fp_write);
}
