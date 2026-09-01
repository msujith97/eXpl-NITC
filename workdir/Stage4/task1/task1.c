#include "task1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global Symbol Table Globals
GST_Node *Ghead = NULL;
int binding = 4096;
int SP;

// Global Symbol Table Functions
GST_Node *GSTLookup(char *name)
{
    GST_Node *temp = Ghead;
    while (temp != NULL)
    {
        if (strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

GST_Node *GSTInstall(char *name, Type type, int size)
{
    GST_Node *new_node = (GST_Node *)malloc(sizeof(GST_Node));
    new_node->name = strdup(name);
    new_node->type = type;
    new_node->size = size;
    new_node->binding = binding;
    binding += size;
    new_node->next = NULL;

    if (Ghead == NULL)
    {
        Ghead = new_node;
    }
    else
    {
        GST_Node *temp = Ghead;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = new_node;
    }
    SP = binding;
    return new_node;
}

void GSTChangeType(AST_Node *root, Type type)
{
    if (root != NULL) {
        if (root->nodetype == VARIABLE) {
            GST_Node *temp = GSTLookup(root->varname);
            if (temp != NULL) {
                temp->type = type;
                root->type = type;
            }
        }
        GSTChangeType(root->left, type);
        GSTChangeType(root->right, type);
    }
}

void GSTPrint(void)
{
    char *type_str;
    GST_Node *temp = Ghead;
    printf("Name\tType\tSize\tBinding\n");
    while (temp != NULL)
    {
        if (temp->type == INTEGER)
            type_str = "int";
        else if (temp->type == STRING)
            type_str = "str";
        else
            type_str = "unknown";
        printf("%s\t%s\t%d\t%d\n", temp->name, type_str, temp->size, temp->binding);
        temp = temp->next;
    }
}

int getSP(void)
{
    return SP;
}

// AST Functions
AST_Node *makeVariableLeafNode(char *varname, char *s)
{
    AST_Node *new_node = (AST_Node *)malloc(sizeof(AST_Node));
    new_node->s = strdup(s);
    new_node->nodetype = VARIABLE;
    new_node->varname = strdup(varname);
    new_node->GSTentry = GSTLookup(varname);
    if (new_node->GSTentry)
        new_node->type = new_node->GSTentry->type;
    else
        new_node->type = VOID;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->mid = NULL;
    return new_node;
}

AST_Node *makeConstantLeafNode(Type type, int val, char *s)
{
    AST_Node *new_node = (AST_Node *)malloc(sizeof(AST_Node));
    new_node->s = strdup(s);
    new_node->nodetype = CONSTANT;
    new_node->type = type;
    new_node->val = val;
    new_node->GSTentry = NULL;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->mid = NULL;
    new_node->varname = NULL;
    return new_node;
}

AST_Node *makeNode(Nodetype node_type, Type type, AST_Node *l, AST_Node *m, AST_Node *r, char *s)
{
    AST_Node *new_node = (AST_Node *)malloc(sizeof(AST_Node));
    
    if (node_type == OPERATOR) {
        if (strcmp(s, "=") == 0) {
            if (l->GSTentry == NULL) {
                printf("Error: Left side of assignment must be a declared variable.\n");
                exit(1);
            }
            if (l->GSTentry->type != r->type) {
                printf("Error: Type mismatch in assignment.\n");
                exit(1);
            }
        }
        else if (type == INTEGER) {
            if (l->type != INTEGER || r->type != INTEGER) {
                printf("Error: Type mismatch. Arithmetic operator requires integer operands.\n");
                exit(1);
            }
        }
        else if (type == BOOLEAN) {
             if (l->type != r->type) {
                 printf("Error: Type mismatch. Relational operator requires compatible operands.\n");
                 exit(1);
             }
        }
    }

    if (node_type == WHILE || node_type == IF) {
        if (l->type != BOOLEAN) {
            printf("Error: Type mismatch. Condition for IF/WHILE must be BOOLEAN.\n");
            exit(1);
        }
    }

    new_node->s = strdup(s);
    new_node->nodetype = node_type;
    new_node->type = type;
    new_node->left = l;
    new_node->mid = m;
    new_node->right = r;
    new_node->GSTentry = NULL;
    new_node->varname = NULL;
    return new_node;
}

// Tree Visualization Functions
void printIndent(int depth, int isRight) {
    for (int i = 0; i < depth - 1; i++) {
        printf("    ");
    }
    if (depth > 0) {
        printf(isRight ? "└─ " : "├─ ");
    }
}

void print_tree(AST_Node *root, int lvl, int isRight)
{
    if (root == NULL) return;
    printIndent(lvl, isRight);
    printf("%s\n", root->s);
    if (root->left != NULL)
        print_tree(root->left, lvl + 1, 0);
    if (root->right != NULL)
        print_tree(root->right, lvl + 1, 1);
    if (root->mid != NULL)
        print_tree(root->mid, lvl + 1, 1);
}