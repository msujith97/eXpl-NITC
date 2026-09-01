#include "ex1.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Global Symbol Table
GST_Node *Ghead = NULL;
int binding = 4096;
int SP;

// GST Functions
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

GST_Node *GSTInstall(char *name, Type type, int size1, int size2, int dimensions)
{
    GST_Node *new_node = (GST_Node *)malloc(sizeof(GST_Node));
    new_node->name = strdup(name);
    new_node->type = type;
    new_node->size = size1;
    new_node->size2 = size2;
    new_node->dimensions = dimensions;
    new_node->binding = binding;

    int total_size = size1 * size2;
    if (dimensions == 1) total_size = size1;
    if (dimensions == 0) total_size = 1;

    binding += total_size;
    new_node->next = NULL;

    if (Ghead == NULL) {
        Ghead = new_node;
    } else {
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

void GSTPrint()
{
    char type[10];
    char array[10];
    struct GST_Node *temp = Ghead;
    printf("Name\tType\tSize\tArray\tBinding\n");
    while (temp != NULL)
    {
        if (temp->type == INTEGER) {
            strcpy(type, "int");
        } else if (temp->type == STRING) {
            strcpy(type, "str");
        } else {
            strcpy(type, "void");
        }

        if (temp->dimensions == 0) {
            strcpy(array, "no");
        } else {
            strcpy(array, "yes");
        }

        int total_sz = (temp->dimensions == 2) ? (temp->size * temp->size2) : temp->size;
        printf("%s\t%s\t%d\t%s\t%d\n", temp->name, type, total_sz, array, temp->binding);
        temp = temp->next;
    }
}

int getSP()
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

struct AST_Node *makeArrayLeafNode(char *varname, struct AST_Node *l, char *s)
{
    struct AST_Node *new_node = (struct AST_Node *)malloc(sizeof(struct AST_Node));
    new_node->s = strdup(s);
    new_node->nodetype = ARRAY;
    new_node->varname = strdup(varname);
    new_node->GSTentry = GSTLookup(varname);
    if (new_node->GSTentry != NULL) {
        new_node->type = new_node->GSTentry->type;
    } else {
        new_node->type = VOID;
    }
    new_node->left = l;
    new_node->right = NULL;
    new_node->mid = NULL;
    return new_node;
}

struct AST_Node *makeArray2DLeafNode(char *varname, struct AST_Node *row, struct AST_Node *col, char *s)
{
    struct AST_Node *new_node = (struct AST_Node *)malloc(sizeof(struct AST_Node));
    new_node->s = strdup(s);
    new_node->nodetype = ARRAY;
    new_node->varname = strdup(varname);
    new_node->GSTentry = GSTLookup(varname);
    if (new_node->GSTentry != NULL) {
        new_node->type = new_node->GSTentry->type;
    } else {
        new_node->type = VOID;
    }
    new_node->left = row; 
    new_node->mid = col;
    new_node->right = NULL;
    return new_node;
}

AST_Node *makeNode(Nodetype node_type, Type type, AST_Node *l, AST_Node *m, AST_Node *r, char *s)
{
    AST_Node *new_node = (AST_Node *)malloc(sizeof(AST_Node));
    
    if (node_type == OPERATOR) {
        if (strcmp(s, "=") == 0) {
            if (l != NULL && l->GSTentry == NULL && l->nodetype == VARIABLE) {
                l->GSTentry = GSTLookup(l->varname);
            }
            if (l == NULL || (l->nodetype == VARIABLE && l->GSTentry == NULL)) {
                printf("Error: Left side of assignment must be a declared variable.\n");
                exit(1);
            }
            Type left_type = l->type;
            if (l->GSTentry != NULL) left_type = l->GSTentry->type;
            if (left_type != r->type) {
                printf("Error: Type mismatch in assignment.\n");
                exit(1);
            }
        }
        else if (type == INTEGER) {
            if (l == NULL || r == NULL || l->type != INTEGER || r->type != INTEGER) {
                printf("Error: Type mismatch. Arithmetic operator requires integer operands.\n");
                exit(1);
            }
        }
        else if (type == BOOLEAN) {
             if (l == NULL || r == NULL || l->type != r->type) {
                 printf("Error: Type mismatch. Relational operator requires compatible operands.\n");
                 exit(1);
             }
        }
    }

    if (node_type == WHILE || node_type == IF) {
        if (l != NULL && l->type != BOOLEAN) {
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

struct AST_Node *ASTChangeType(struct AST_Node *root, Type type)
{
    if (root == NULL) return NULL;
    
    if (root->nodetype == VARIABLE) {
        GST_Node *temp = GSTLookup(root->varname);
        if (temp != NULL) {
            temp->type = type;
            root->type = type;
        }
    }
    ASTChangeType(root->left, type);
    ASTChangeType(root->right, type);
    return root;
}

void printIndent(int depth, int isRight) {
    for (int i = 0; i < depth - 1; i++) {
        printf("|   ");
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

int free_reg = -1;
int label = 0;
int u, v, inWhile = 0;

int getReg() {
    free_reg++;
    return free_reg;
}

void freeReg() {
    free_reg--;
}

int getLabel() {
    return label++;
}

int getAddr(AST_Node *t) {
    if (t != NULL && t->GSTentry != NULL) {
        return t->GSTentry->binding;
    }
    if (t != NULL && t->varname != NULL) {
        GST_Node *temp = GSTLookup(t->varname);
        if (temp != NULL) return temp->binding;
    }
    return -1;
}

int codeGen(AST_Node *t, FILE *target_file)
{
    int p, q, r, s, addr;

    if (t == NULL) return -1;

    if (t->nodetype == CONSTANT) {
        p = getReg();
        if (t->type == INTEGER) {
            fprintf(target_file, "MOV R%d, %d\n", p, t->val);
        } else if (t->type == STRING) {
            fprintf(target_file, "MOV R%d, %s\n", p, t->s);
        }
        return p;
    }

    if (t->nodetype == VARIABLE) {
        p = getReg();
        addr = getAddr(t);
        fprintf(target_file, "MOV R%d, [%d]\n", p, addr);
        return p;
    }

    if (t->nodetype == READ) {
        int p = getReg();
        int q = getReg();
        int addr;

        if (t->left->nodetype == ARRAY) {
            GST_Node *gst = t->left->GSTentry ? t->left->GSTentry : GSTLookup(t->left->varname);
            if (gst != NULL && gst->dimensions == 2) {
                addr = getAddr(t->left);
                int row_reg = codeGen(t->left->left, target_file);
                int col_reg = codeGen(t->left->mid, target_file);
                
                int dim_reg = getReg();
                fprintf(target_file, "MOV R%d, %d\n", dim_reg, gst->size2);
                fprintf(target_file, "MUL R%d, R%d\n", row_reg, dim_reg);
                fprintf(target_file, "ADD R%d, R%d\n", row_reg, col_reg);

                fprintf(target_file, "MOV R%d, %d\n", p, addr);
                fprintf(target_file, "ADD R%d, R%d\n", p, row_reg);
                
                freeReg(); // dim_reg
                freeReg(); // col_reg
                freeReg(); // row_reg
            } else {
                addr = getAddr(t->left);
                q = codeGen(t->left->left, target_file);
                fprintf(target_file, "MOV R%d, %d\n", p, addr);
                fprintf(target_file, "ADD R%d, R%d\n", p, q);
                freeReg();
                q = getReg();
            }
        } else {
            addr = getAddr(t->left);
            fprintf(target_file, "MOV R%d, %d\n", p, addr);
        }
        
        fprintf(target_file, "MOV R%d, \"Read\"\n", q);
        fprintf(target_file, "PUSH R%d\n", q);
        fprintf(target_file, "MOV R%d, -1\n", q);
        fprintf(target_file, "PUSH R%d\n", q);
        fprintf(target_file, "PUSH R%d\n", p);
        fprintf(target_file, "PUSH R%d\n", q);
        fprintf(target_file, "PUSH R%d\n", q);
        fprintf(target_file, "CALL 0\n");
        fprintf(target_file, "POP R%d\n", q);
        fprintf(target_file, "POP R%d\n", q);
        fprintf(target_file, "POP R%d\n", q);
        fprintf(target_file, "POP R%d\n", q);
        fprintf(target_file, "POP R%d\n", q);
        freeReg();
        freeReg();
        return -1;
    }

    if (t->nodetype == WRITE) {
        int p;
        int q = getReg();

        if (t->left->nodetype == ARRAY) {
            int addr = getAddr(t->left);
            GST_Node *gst = t->left->GSTentry ? t->left->GSTentry : GSTLookup(t->left->varname);
            if (gst != NULL && gst->dimensions == 2) {
                int row_reg = codeGen(t->left->left, target_file);
                int col_reg = codeGen(t->left->mid, target_file);
                int dim_reg = getReg();
                int addr_reg = getReg();

                fprintf(target_file, "MOV R%d, %d\n", dim_reg, gst->size2);
                fprintf(target_file, "MUL R%d, R%d\n", row_reg, dim_reg);
                fprintf(target_file, "ADD R%d, R%d\n", row_reg, col_reg);

                fprintf(target_file, "MOV R%d, %d\n", addr_reg, addr);
                fprintf(target_file, "ADD R%d, R%d\n", addr_reg, row_reg);

                p = getReg();
                fprintf(target_file, "MOV R%d, [R%d]\n", p, addr_reg);

                freeReg(); // addr_reg
                freeReg(); // dim_reg
                freeReg(); // col_reg
                freeReg(); // row_reg
            } else {
                p = getReg();
                int index_reg = codeGen(t->left->left, target_file);
                fprintf(target_file, "MOV R%d, %d\n", p, addr);
                fprintf(target_file, "ADD R%d, R%d\n", p, index_reg);
                fprintf(target_file, "MOV R%d, [R%d]\n", p, p);
                freeReg(); // index_reg
            }
        } else {
            p = codeGen(t->left, target_file);
        }

        fprintf(target_file, "MOV R%d, \"Write\"\n", q);
        fprintf(target_file, "PUSH R%d\n", q);
        fprintf(target_file, "MOV R%d, -2\n", q);
        fprintf(target_file, "PUSH R%d\n", q);
        fprintf(target_file, "PUSH R%d\n", p);
        fprintf(target_file, "PUSH R%d\n", q);
        fprintf(target_file, "PUSH R%d\n", q);
        fprintf(target_file, "CALL 0\n");
        fprintf(target_file, "POP R%d\n", q);
        fprintf(target_file, "POP R%d\n", q);
        fprintf(target_file, "POP R%d\n", q);
        fprintf(target_file, "POP R%d\n", q);
        fprintf(target_file, "POP R%d\n", q);
        freeReg();
        freeReg();
        return -1;
    }

    if (t->nodetype == STATEMENT) {
        codeGen(t->left, target_file);
        codeGen(t->right, target_file);
        return -1;
    }

    if (t->nodetype == OPERATOR) {
        if (t->type == BOOLEAN) {
            p = codeGen(t->left, target_file);
            q = codeGen(t->right, target_file);
            if (strcmp(t->s, ">") == 0) fprintf(target_file, "GT R%d, R%d\n", p, q);
            else if (strcmp(t->s, "<") == 0) fprintf(target_file, "LT R%d, R%d\n", p, q);
            else if (strcmp(t->s, ">=") == 0) fprintf(target_file, "GE R%d, R%d\n", p, q);
            else if (strcmp(t->s, "<=") == 0) fprintf(target_file, "LE R%d, R%d\n", p, q);
            else if (strcmp(t->s, "==") == 0) fprintf(target_file, "EQ R%d, R%d\n", p, q);
            else if (strcmp(t->s, "!=") == 0) fprintf(target_file, "NE R%d, R%d\n", p, q);
            freeReg();
            return p;
        }

        if (strcmp(t->s, "=") == 0) {
            int p = codeGen(t->right, target_file);
            int addr;

            if (t->left->nodetype == ARRAY) {
                GST_Node *gst = t->left->GSTentry ? t->left->GSTentry : GSTLookup(t->left->varname);
                if (gst != NULL && gst->dimensions == 2) {
                    int row_reg = codeGen(t->left->left, target_file);
                    int col_reg = codeGen(t->left->mid, target_file);
                    int dim_reg = getReg();
                    int addr_reg = getReg();
                    
                    fprintf(target_file, "MOV R%d, %d\n", dim_reg, gst->size2);
                    fprintf(target_file, "MUL R%d, R%d\n", row_reg, dim_reg);
                    fprintf(target_file, "ADD R%d, R%d\n", row_reg, col_reg);

                    addr = getAddr(t->left);
                    fprintf(target_file, "MOV R%d, %d\n", addr_reg, addr);
                    fprintf(target_file, "ADD R%d, R%d\n", addr_reg, row_reg);

                    fprintf(target_file, "MOV [R%d], R%d\n", addr_reg, p);
                    
                    freeReg(); // addr_reg
                    freeReg(); // dim_reg
                    freeReg(); // col_reg
                    freeReg(); // row_reg
                } else {
                    int index_reg = codeGen(t->left->left, target_file);
                    int addr_reg = getReg();
                    addr = getAddr(t->left);
                    fprintf(target_file, "MOV R%d, %d\n", addr_reg, addr);
                    fprintf(target_file, "ADD R%d, R%d\n", addr_reg, index_reg);
                    fprintf(target_file, "MOV [R%d], R%d\n", addr_reg, p);
                    freeReg(); // addr_reg
                    freeReg(); // index_reg
                }
                freeReg(); // p
                return -1;
            } else {
                addr = getAddr(t->left);
                fprintf(target_file, "MOV [%d], R%d\n", addr, p);
                freeReg(); // p
                return -1;
            }
        } else {
            p = codeGen(t->left, target_file);
            q = codeGen(t->right, target_file);
            switch (t->s[0]) {
            case '+': fprintf(target_file, "ADD R%d, R%d\n", p, q); break;
            case '-': fprintf(target_file, "SUB R%d, R%d\n", p, q); break;
            case '*': fprintf(target_file, "MUL R%d, R%d\n", p, q); break;
            case '/': fprintf(target_file, "DIV R%d, R%d\n", p, q); break;
            }
            freeReg();
            return p;
        }
    }

    else if (t->nodetype == WHILE) {
        u = getLabel();
        fprintf(target_file, "L%d:\n", u);
        p = codeGen(t->left, target_file);
        v = getLabel();
        fprintf(target_file, "JZ R%d, L%d\n", p, v);
        freeReg();
        int old_u = u, old_v = v;
        inWhile = 1;
        codeGen(t->right, target_file);
        inWhile = 0;
        u = old_u; v = old_v;
        fprintf(target_file, "JMP L%d\n", u);
        fprintf(target_file, "L%d:\n", v);
        return -1;
    }

    else if (t->nodetype == IF) {
        p = codeGen(t->left, target_file);
        s = getLabel();
        fprintf(target_file, "JZ R%d, L%d\n", p, s);
        freeReg();
        codeGen(t->mid, target_file);
        if (t->right != NULL) {
            r = getLabel();
            fprintf(target_file, "JMP L%d\n", r);
            fprintf(target_file, "L%d:\n", s);
            codeGen(t->right, target_file);
            fprintf(target_file, "L%d:\n", r);
        } else {
            fprintf(target_file, "L%d:\n", s);
        }
        return -1;
    }

    else if (t->nodetype == REPEAT) {
        u = getLabel();
        v = getLabel();
        fprintf(target_file, "L%d:\n", u);
        int old_u = u, old_v = v;
        inWhile = 1;
        codeGen(t->left, target_file);
        inWhile = 0;
        u = old_u; v = old_v;
        p = codeGen(t->right, target_file);
        fprintf(target_file, "JZ R%d, L%d\n", p, v);
        freeReg();
        fprintf(target_file, "JMP L%d\n", u);
        fprintf(target_file, "L%d:\n", v);
        return -1;
    }
    
    else if (t->nodetype == DOWHILE) {
        u = getLabel();
        v = getLabel();
        fprintf(target_file, "L%d:\n", u);
        int old_u = u, old_v = v;
        inWhile = 1;
        codeGen(t->left, target_file);
        inWhile = 0;
        u = old_u; v = old_v;
        p = codeGen(t->right, target_file);
        fprintf(target_file, "JNZ R%d, L%d\n", p, u);
        freeReg();
        fprintf(target_file, "L%d:\n", v);
        return -1;
    }

    else if (t->nodetype == BREAK) {
        if (inWhile) fprintf(target_file, "JMP L%d\n", v);
        return -1;
    }

    else if (t->nodetype == CONTINUE) {
        if (inWhile) fprintf(target_file, "JMP L%d\n", u);
        return -1;
    }
    
    else if (t->nodetype == ARRAY) {
        GST_Node *gst = t->GSTentry ? t->GSTentry : GSTLookup(t->varname);
        if (gst != NULL && gst->dimensions == 2) {
            addr = getAddr(t);
            p = codeGen(t->left, target_file);
            q = codeGen(t->mid, target_file);

            int dim_reg = getReg(); 
            int offset_reg = getReg();

            int n = gst->size2;

            fprintf(target_file, "MOV R%d, %d\n", dim_reg, n);
            fprintf(target_file, "MUL R%d, R%d\n", p, dim_reg);
            fprintf(target_file, "ADD R%d, R%d\n", p, q);

            fprintf(target_file, "MOV R%d, %d\n", offset_reg, addr);
            fprintf(target_file, "ADD R%d, R%d\n", offset_reg, p);

            fprintf(target_file, "MOV R%d, [R%d]\n", p, offset_reg);

            freeReg(); // offset_reg
            freeReg(); // dim_reg
            freeReg(); // q

            return p;
        } else {        
            addr = getAddr(t);
            p = codeGen(t->left, target_file);
            q = getReg();
            fprintf(target_file, "MOV R%d, %d\n", q, addr);
            fprintf(target_file, "ADD R%d, R%d\n", q, p);
            fprintf(target_file, "MOV R%d, [R%d]\n", p, q);
            freeReg(); // q
            return p;
        }
    }
    
    return -1;
}