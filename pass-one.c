#include <stdio.h>
#include <string.h>
#define SIZE 50

char label[SIZE],
        operator[SIZE],
        operand[SIZE],
        pgm_name[SIZE];

int start = 0,
        locctr = 0;

FILE *input_fp,
     *intrmt_fp;


void read_instr() {
    fscanf(input_fp, " %s %s %s", label, operator, operand);
}

void print_instr() {
    printf("Label: %10s | Operator: %10s | Operand: %10s\n", label, operator, operand);
}

int main() {
    input_fp = fopen("input.txt", "r");
    intrmt_fp = fopen("intermediate.txt", "w");

    read_instr();

    if (strcmp(operator, "START") == 0) {
        strcpy(pgm_name, label);
        sscanf(operand, "%x", &start);
        locctr = start;
    }

    printf("PGM NAME: %s | Operator: %s | Operand: %X\n", pgm_name, operator, locctr);


    while(strcmp(operator, "END") != 0) {
        read_instr();
        print_instr();
    }


    fclose(input_fp);
    fclose(intrmt_fp);
}
