#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task1.h"

struct Gsymbol *headGsymbol = NULL;
int staticBinding = 4096; // Global memory starts at 4096
int flabelCounter = 0;    // Function label counter (0, 1, 2...)

struct Gsymbol* GLookup(char *name) {
    struct Gsymbol *curr = headGsymbol;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

void GInstall(char *name, int type, int size, struct Paramstruct *paramlist) {
    // Duplicate declaration check
    if (GLookup(name) != NULL) {
        printf("Error: Redeclaration of identifier '%s'\n", name);
        exit(1);
    }

    struct Gsymbol *newNode = (struct Gsymbol*)malloc(sizeof(struct Gsymbol));
    newNode->name = strdup(name);
    newNode->type = type;
    newNode->size = size;
    newNode->paramlist = paramlist;
    newNode->next = NULL;

    if (size == -1) {
        // It's a function declaration
        newNode->binding = -1;
        newNode->flabel = flabelCounter++;
    } else {
        // Variable or Array declaration
        newNode->binding = staticBinding;
        newNode->flabel = -1;
        staticBinding += size;
    }

    if (headGsymbol == NULL) {
        headGsymbol = newNode;
    } else {
        struct Gsymbol *temp = headGsymbol;
        while (temp->next != NULL) temp = temp->next;
        temp->next = newNode;
    }
}

struct Paramstruct* createParamNode(char *name, int type) {
    struct Paramstruct *p = (struct Paramstruct*)malloc(sizeof(struct Paramstruct));
    p->name = strdup(name);
    p->type = type;
    p->next = NULL;
    return p;
}

struct Paramstruct* appendParam(struct Paramstruct *list, struct Paramstruct *param) {
    if (list == NULL) return param;
    struct Paramstruct *temp = list;
    while (temp->next != NULL) {
        // Check duplicate parameters in function declaration signature
        if (strcmp(temp->name, param->name) == 0) {
            printf("Error: Duplicate parameter name '%s' in function declaration\n", param->name);
            exit(1);
        }
        temp = temp->next;
    }
    if (strcmp(temp->name, param->name) == 0) {
        printf("Error: Duplicate parameter name '%s' in function declaration\n", param->name);
        exit(1);
    }
    temp->next = param;
    return list;
}

void printSymbolTable() {
    struct Gsymbol *curr = headGsymbol;
    printf("\n=================== GLOBAL SYMBOL TABLE ===================\n");
    printf("%-15s %-10s %-8s %-10s %-10s %-20s\n", "Name", "Type", "Size", "Binding", "FLabel", "Parameters");
    printf("-----------------------------------------------------------\n");

    while (curr != NULL) {
        char *typeStr = (curr->type == TYPE_INT) ? "INT" : "STR";
        printf("%-15s %-10s ", curr->name, typeStr);

        if (curr->size == -1) {
            // Function Entry
            printf("%-8s %-10s F%-9d ", "N/A", "N/A", curr->flabel);
            struct Paramstruct *p = curr->paramlist;
            if (!p) {
                printf("None");
            } else {
                while (p) {
                    printf("%s(%s)%s", p->name, (p->type == TYPE_INT ? "INT" : "STR"), p->next ? ", " : "");
                    p = p->next;
                }
            }
        } else {
            // Variable Entry
            printf("%-8d %-10d %-10s %-20s", curr->size, curr->binding, "N/A", "N/A");
        }
        printf("\n");
        curr = curr->next;
    }
    printf("===========================================================\n\n");
}