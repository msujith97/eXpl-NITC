#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include "task1.h"

struct AST_Node *makeVariableLeafNode(Type type, char varname, char *s)
{
    struct AST_Node *new_node = (struct AST_Node *)malloc(sizeof(struct AST_Node));
    new_node->s = (char *)malloc(sizeof(char) * strlen(s));
    new_node->s = strdup(s);
    new_node->nodetype = VARIABLE;
    new_node->type = type;
    new_node->varname = (char *)malloc(sizeof(char));
    new_node->varname[0] = varname;
    new_node->left = (struct AST_Node *)NULL;
    new_node->right = (struct AST_Node *)NULL;
    new_node->mid = (struct AST_Node *)NULL;
    return new_node;
}

struct AST_Node *makeConstantLeafNode(Type type, int val, char *s)
{
    struct AST_Node *new_node = (struct AST_Node *)malloc(sizeof(struct AST_Node));
    new_node->s = (char *)malloc(sizeof(char) * strlen(s));
    new_node->s = strdup(s);
    new_node->nodetype = CONSTANT;
    new_node->type = type;
    new_node->val = val;
    new_node->left = (struct AST_Node *)NULL;
    new_node->right = (struct AST_Node *)NULL;
    new_node->mid = (struct AST_Node *)NULL;
    new_node->varname = (char *)NULL;
    return new_node;
}

struct AST_Node *makeNode(Nodetype nodetype, Type type, struct AST_Node *left, struct AST_Node *mid, struct AST_Node *right, char *s)
{
    struct AST_Node *new_node = (struct AST_Node *)malloc(sizeof(struct AST_Node));
    if (nodetype == OPERATOR && type == INTEGER)
    {
        if (left->type == BOOLEAN || right->type == BOOLEAN)
        {
            printf("Error: int mismatch 1\n");
            exit(1);
        }
    }
    if (nodetype == WHILE || nodetype == IF)
    {
        if (left->type == INTEGER)
        {
            printf("Error: int mismatch 2\n");
            exit(1);
        }
    }
    new_node->s = (char *)malloc(sizeof(char) * strlen(s));
    new_node->s = strdup(s);
    new_node->nodetype = nodetype;
    new_node->type = type;
    new_node->left = left;
    new_node->mid = mid;
    new_node->right = right;
    new_node->varname = (char *)NULL;
    return new_node;
}


void printIndent(int depth, int isRight) {
    for (int i = 0; i < depth - 1; i++) {
        printf("    ");
    }
    if (depth > 0) {
        printf(isRight ? "└─ " : "├─ ");
    }
}



void print_tree(struct AST_Node *root, int lvl, int isLast) {
    if (root == NULL) return;

    printIndent(lvl, isLast);

    // Print node string label, or handle numeric constants if string label isn't set
    if (root->s != NULL) {
        printf("%s\n", root->s);
    } else {
        printf("%d\n", root->val);
    }

    // Determine how many active children this node has
    int childCount = 0;
    if (root->left != NULL) childCount++;
    if (root->mid != NULL) childCount++;
    if (root->right != NULL) childCount++;

    int currentChild = 0;

    // Traverse Left Child (e.g., Condition in IF, Var in READ)
    if (root->left != NULL) {
        currentChild++;
        print_tree(root->left, lvl + 1, currentChild == childCount);
    }

    // Traverse Middle Child (e.g., THEN block in IF)
    if (root->mid != NULL) {
        currentChild++;
        print_tree(root->mid, lvl + 1, currentChild == childCount);
    }

    // Traverse Right Child (e.g., ELSE block in IF, Right Operand in Expression)
    if (root->right != NULL) {
        currentChild++;
        print_tree(root->right, lvl + 1, currentChild == childCount);
    }
}
