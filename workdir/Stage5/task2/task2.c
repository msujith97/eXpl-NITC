#include "task2.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

extern void yyerror(const char *s);

/* Function currently being parsed (for return type checking) */
Gsymbol* current_function = NULL;
int current_return_type = INTEGER_TYPE;

void semanticError(const char* fmt, const char* name) {
    char msg[200];
    snprintf(msg, sizeof(msg), fmt, name);
    yyerror(msg);
    exit(1);
}

void setCurrentFunction(Gsymbol* f, int returnType) {
    current_function = f;
    current_return_type = returnType;
}

int getCurrentReturnType() {
    return current_return_type;
}

int pointerTo(int base_type) {
    if(base_type == INTEGER_TYPE) return INTEGER_POINTER_TYPE;
    if(base_type == STRING_TYPE) return STRING_POINTER_TYPE;
    yyerror("invalid pointer base type");
    exit(1);
}

static int isArrayType(int type) {
    return type == INTEGER_ARRAY_TYPE || type == STRING_ARRAY_TYPE;
}

static int isPointerType(int type) {
    return type == INTEGER_POINTER_TYPE || type == STRING_POINTER_TYPE;
}

tnode* createNode(int val, int type, int nodetype, char* varname, tnode *l, tnode *r, tnode* m) {
    tnode* newNode = (tnode*)malloc(sizeof(tnode));
    newNode->val = val;
    newNode->left = l;
    newNode->right = r;
    newNode->mid = m;
    newNode->type = type;
    newNode->varname = varname != NULL ? strdup(varname) : NULL;
    newNode->nodetype = nodetype;
    newNode->Gentry = NULL;
    newNode->Lentry = NULL;

    switch(nodetype) {
        case PLUS_NODE: case MINUS_NODE: case MUL_NODE: case DIV_NODE: case MODULUS_NODE:
            if(l->type != INTEGER_TYPE || r->type != INTEGER_TYPE) {
                yyerror("type mismatch: arithmetic operands must be integers");
                exit(1);
            }
            newNode->type = INTEGER_TYPE;
            break;

        case LT_NODE: case GT_NODE: case LE_NODE: case GE_NODE: case EQ_NODE: case NE_NODE:
            if(l->type != r->type || isArrayType(l->type) || l->type == BOOLEAN_TYPE) {
                yyerror("type mismatch in comparison");
                exit(1);
            }
            newNode->type = BOOLEAN_TYPE;
            break;

        case AND_NODE: case OR_NODE:
            if(l->type != BOOLEAN_TYPE || r->type != BOOLEAN_TYPE) {
                yyerror("type mismatch: logical operands must be boolean");
                exit(1);
            }
            newNode->type = BOOLEAN_TYPE;
            break;

        case NOT_NODE:
            if(l->type != BOOLEAN_TYPE) {
                yyerror("type mismatch: operand of not must be boolean");
                exit(1);
            }
            newNode->type = BOOLEAN_TYPE;
            break;

        case IF_NODE: case WHILE_NODE: case DO_WHILE_NODE: case REPEAT_UNTIL_NODE:
            if(l->type != BOOLEAN_TYPE) {
                yyerror("type mismatch: condition must be boolean");
                exit(1);
            }
            break;

        case DEREF_NODE:
            if(l->type == INTEGER_POINTER_TYPE) newNode->type = INTEGER_TYPE;
            else if(l->type == STRING_POINTER_TYPE) newNode->type = STRING_TYPE;
            else {
                yyerror("type mismatch: dereferencing a non-pointer");
                exit(1);
            }
            break;

        case ADDRESS_NODE:
            if(l->nodetype != VARIABLE || (l->type != INTEGER_TYPE && l->type != STRING_TYPE)) {
                yyerror("& can only be applied to an int/str variable");
                exit(1);
            }
            newNode->type = pointerTo(l->type);
            break;

        case ASSIGNMENT:
            if(isArrayType(l->type)) {
                yyerror("cannot assign value directly to an array name");
                exit(1);
            }
            if(l->type != r->type) {
                yyerror("type mismatch in assignment");
                exit(1);
            }
            break;

        case WRITE_NODE:
            if(l->type != INTEGER_TYPE && l->type != STRING_TYPE && !isPointerType(l->type)) {
                yyerror("write expects an int or str expression");
                exit(1);
            }
            break;

        case READ_NODE:
            if(l->type != INTEGER_TYPE && l->type != STRING_TYPE) {
                yyerror("read expects an int or str variable");
                exit(1);
            }
            break;

        case RETURN_NODE:
            if(l->type != current_return_type) {
                yyerror("type mismatch: return type does not match function type");
                exit(1);
            }
            break;
    }

    return newNode;
}

/* Look up the local symbol table first, then the global symbol table */
tnode* makeVarNode(char* name, tnode* index1, tnode* index2) {
    tnode* node = createNode(0, INTEGER_TYPE, VARIABLE, name, index1, index2, NULL);

    Lsymbol* L = LLookup(name);
    if(L != NULL) {
        if(index1 != NULL) semanticError("%s is not an array", name);
        node->Lentry = L;
        node->type = L->type;
        return node;
    }

    Gsymbol* G = Lookup(name);
    if(G == NULL) semanticError("undeclared variable %s", name);
    if(G->isFunction) semanticError("%s is a function, not a variable", name);
    node->Gentry = G;

    if(isArrayType(G->type)) {
        if(index1 == NULL) {
            node->type = G->type;      // bare array name
        } else {
            if(index1->type != INTEGER_TYPE || (index2 && index2->type != INTEGER_TYPE)) {
                semanticError("array index of %s must be an integer", name);
            }
            node->type = (G->type == INTEGER_ARRAY_TYPE) ? INTEGER_TYPE : STRING_TYPE;
        }
    } else {
        if(index1 != NULL) semanticError("%s is not an array", name);
        node->type = G->type;
    }
    return node;
}

tnode* makeCallNode(char* name, tnode* args) {
    Gsymbol* G = Lookup(name);
    if(G == NULL || !G->isFunction) semanticError("call to undeclared function %s", name);

    /* check number and types of arguments against the declaration */
    Paramstruct* p = G->paramlist;
    tnode* a = args;
    while(p != NULL && a != NULL) {
        if(p->type != a->left->type) semanticError("type mismatch in argument to %s", name);
        p = p->next;
        a = a->right;
    }
    if(p != NULL || a != NULL) semanticError("wrong number of arguments in call to %s", name);

    tnode* node = createNode(0, G->type, FUNC_CALL_NODE, name, args, NULL, NULL);
    node->Gentry = G;
    return node;
}

/* ---------------- AST printing (Task 2: no code generation yet) ---------------- */

static const char* nodeName(int nodetype) {
    switch(nodetype) {
        case STATEMENT: return "STMT_LIST";
        case VARIABLE: return "VAR";
        case ASSIGNMENT: return "ASSIGN";
        case PLUS_NODE: return "+";
        case MINUS_NODE: return "-";
        case MUL_NODE: return "*";
        case DIV_NODE: return "/";
        case MODULUS_NODE: return "%";
        case CONSTANT: return "NUM";
        case READ_NODE: return "READ";
        case WRITE_NODE: return "WRITE";
        case EQ_NODE: return "==";
        case NE_NODE: return "!=";
        case GT_NODE: return ">";
        case LT_NODE: return "<";
        case GE_NODE: return ">=";
        case LE_NODE: return "<=";
        case AND_NODE: return "AND";
        case OR_NODE: return "OR";
        case NOT_NODE: return "NOT";
        case IF_NODE: return "IF";
        case WHILE_NODE: return "WHILE";
        case DO_WHILE_NODE: return "DO_WHILE";
        case REPEAT_UNTIL_NODE: return "REPEAT_UNTIL";
        case BREAK_NODE: return "BREAK";
        case CONTINUE_NODE: return "CONTINUE";
        case STRING_CONSTANT_NODE: return "STRING";
        case ADDRESS_NODE: return "&";
        case DEREF_NODE: return "DEREF";
        case FUNC_CALL_NODE: return "CALL";
        case ARG_NODE: return "ARG";
        case RETURN_NODE: return "RETURN";
    }
    return "?";
}

static void printTree(tnode* t, int depth) {
    int i;
    if(t == NULL) return;
    for(i = 0; i < depth; i++) printf("  ");
    printf("%s", nodeName(t->nodetype));
    if(t->nodetype == CONSTANT) printf(" %d", t->val);
    if(t->nodetype == STRING_CONSTANT_NODE) printf(" %s", t->varname);
    if(t->nodetype == VARIABLE) printf(" %s (%s)", t->varname, t->Lentry ? "local" : "global");
    if(t->nodetype == FUNC_CALL_NODE) printf(" %s -> F%d", t->varname, t->Gentry->flabel);
    printf("  [type %d]\n", t->type);
    printTree(t->left, depth + 1);
    printTree(t->mid, depth + 1);
    printTree(t->right, depth + 1);
}

void printAST(char* fname, tnode* body) {
    printf("---- AST: %s ----\n", fname);
    printTree(body, 1);
}