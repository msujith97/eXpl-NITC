#include<stdio.h>
typedef struct tnode {
    int val;
    char *op;
    struct tnode *left, *right;
} tnode;

struct tnode* makeLeafNode(int n);
struct tnode* makeOperatorNode(char op, struct tnode *l, struct tnode *r);

int getReg();
void freeReg();
int codeGen(struct tnode *t, FILE *target_file);