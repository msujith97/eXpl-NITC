#include <stdio.h>
#include <stdlib.h>
#include "task2.h"

int current_reg = -1;

int getReg() {
    if (current_reg < 19) {
        current_reg++;
        return current_reg;
    }
    printf("Error: Out of registers\n");
    exit(1);
}

void freeReg() {
    if (current_reg >= 0) {
        current_reg--;
    }
    else {
        printf("Error: No registers to free\n");
        exit(1);
    }
}

struct tnode* makeLeafNode(int n) {
    struct tnode *temp = (struct tnode*)malloc(sizeof(struct tnode));
    temp->val = n;
    temp->op = NULL;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}

struct tnode* makeOperatorNode(char op, struct tnode *l, struct tnode *r) {
    struct tnode *temp = (struct tnode*)malloc(sizeof(struct tnode));
    temp->op = malloc(2);
    temp->op[0] = op;
    temp->op[1] = '\0';
    temp->left = l;
    temp->right = r;
    return temp;
}

int codeGen(struct tnode *t, FILE *target_file) {
    if (t->op == NULL) {
        int r = getReg();
        fprintf(target_file, "MOV R%d, %d\n", r, t->val);
        return r;
    }

    int left_reg = codeGen(t->left, target_file);
    int right_reg = codeGen(t->right, target_file);

    switch (*(t->op)) {
        case '+': fprintf(target_file, "ADD R%d, R%d\n", left_reg, right_reg); break;
        case '-': fprintf(target_file, "SUB R%d, R%d\n", left_reg, right_reg); break;
        case '*': fprintf(target_file, "MUL R%d, R%d\n", left_reg, right_reg); break;
        case '/': fprintf(target_file, "DIV R%d, R%d\n", left_reg, right_reg); break;
    }

    freeReg();
    return left_reg;
}