#include <stdio.h>
#include <stdlib.h>

// Enumerations for Nodetype and Type
typedef enum Nodetype
{
    VARIABLE,
    CONSTANT,
    READ,
    WRITE,
    STATEMENT,
    OPERATOR,
    WHILE,
    IF,
    BREAK,
    CONTINUE,
    REPEAT,
    DOWHILE
} Nodetype;

typedef enum Type
{
    INTEGER,
    BOOLEAN,
    VOID,
    STRING
} Type;

// Forward declaration
struct GST_Node;

// AST Node Structure
typedef struct AST_Node
{
    int val;
    Type type;
    char *varname;
    Nodetype nodetype;
    struct GST_Node *GSTentry;
    char *s;
    struct AST_Node *left, *mid, *right;
} AST_Node;

// GST Node Structure
typedef struct GST_Node
{
    char *name;
    Type type;
    int size;
    int binding;
    struct GST_Node *next;
} GST_Node;

// AST Node Creation Functions
AST_Node *makeConstantLeafNode(Type type, int val, char *s);
AST_Node *makeVariableLeafNode(char *varname, char *s);
AST_Node *makeNode(Nodetype node_type, Type type, AST_Node *l, AST_Node *m, AST_Node *r, char *s);

// GST Related Functions
GST_Node *GSTLookup(char *name);
GST_Node *GSTInstall(char *name, Type type, int size);
void GSTChangeType(AST_Node *root, Type type);
void GSTPrint(void);
int getSP(void);

// Tree Visualization Functions
void printIndent(int depth, int isRight);
void print_tree(AST_Node *root, int lvl, int isRight);