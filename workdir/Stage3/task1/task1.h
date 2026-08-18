typedef enum NodeType {
    VARIABLE,
    CONSTANT,
    READ,
    WRITE,
    STATEMENT,
    OPERATOR,
    WHILE,
    IF,
}Nodetype;

typedef enum Type {
    INTEGER,
    BOOLEAN,
    VOID,
}Type;

typedef struct AST_Node {
    int val;
    Type type;
    Nodetype nodetype;
    char *varname;
    char *s;
    struct AST_Node *left,*mid,*right;
}AST_Node;

struct AST_Node *makeConstantLeafNode(Type type,int val,char *s);
struct AST_Node *makeVariableLeafNode(Type type,char c,char *s);
struct AST_Node *makeNode(Nodetype, Type, struct AST_Node *, struct AST_Node *, struct AST_Node *, char *);
void print_tree(struct AST_Node *root, int level,int isRight);