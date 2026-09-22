#define TYPE_INT 1
#define TYPE_STR 2

// Formal Parameter structure
struct Paramstruct {
    char *name;
    int type;
    struct Paramstruct *next;
};

// Global Symbol Table entry
struct Gsymbol {
    char *name;                 // Name of variable/function
    int type;                   // TYPE_INT or TYPE_STR
    int size;                   // Size for arrays (1 for scalar variables, -1 for functions)
    int binding;                // Static memory location (starts at 4096)
    struct Paramstruct *paramlist; // Parameter list for functions
    int flabel;                 // Label number for function code (F0, F1, ...)
    struct Gsymbol *next;
};

extern struct Gsymbol *headGsymbol;

// Declarations
struct Gsymbol* GLookup(char *name);
void GInstall(char *name, int type, int size, struct Paramstruct *paramlist);
struct Paramstruct* createParamNode(char *name, int type);
struct Paramstruct* appendParam(struct Paramstruct *list, struct Paramstruct *param);
void printSymbolTable();