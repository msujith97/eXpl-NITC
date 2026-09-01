%{
#include <stdlib.h>
#include <stdio.h>
#include "task1.h"

extern FILE* yyin;
extern char* yytext;
extern int yylineno;

void yyerror(char const *s);
int yylex(void);

int current_type;
struct AST_Node *rootNode = NULL;
%}

%union{
    struct AST_Node *node;
}

/* Declare non-terminals that return AST nodes */
%type <node> program Declarations decl_list decl var_list
%type <node> stmt_list stmt InputStmt OutputStmt AsgStmt IfStmt WhileStmt RepeatUntilStmt DoWhileStmt
%type <node> expr id

/* Plain tokens without values */
%token PLUS_ MINUS_ MUL_ DIV_ LT_ GT_ LE_ GE_ NE_ EQ_
%token BEGIN_ END_ READ_ WRITE_ IF_ THEN_ ELSE_ ENDIF_ WHILE_ DO_ ENDWHILE_ REPEAT_ UNTIL_
%token INT_ STR_ DECL_ ENDDECL_

/* Tokens that carry an AST_Node pointer (yylval.node) from the Lexer */
%token <node> ID_ NUM_ TEXT_ BREAK_ CONTINUE_

%left LT_ GT_ LE_ GE_ NE_ EQ_
%left PLUS_ MINUS_
%left MUL_ DIV_

%%

program : Declarations BEGIN_ stmt_list END_ {
                rootNode = $3;
                GSTPrint();
            }
        | Declarations BEGIN_ END_ {
                printf("Empty Program\n");
                rootNode = NULL;
            };

Declarations: DECL_ decl_list ENDDECL_ {}
            | DECL_ ENDDECL_ {};

decl_list: decl_list decl
         | decl;

decl: Type var_list ';' { $$ = NULL; };

Type: INT_ { current_type = INTEGER; }
    | STR_ { current_type = STRING; }
    ;

var_list: var_list ',' ID_ {
                GSTInstall($3->varname, current_type, 1);
            }
        | ID_ {
                GSTInstall($1->varname, current_type, 1);
            };

stmt_list: stmt_list stmt ';' { $$ = makeNode(STATEMENT, VOID, $1, NULL, $2, "STATEMENT"); }
         | stmt ';'          { $$ = $1; };

stmt: InputStmt
    | OutputStmt
    | AsgStmt
    | IfStmt
    | WhileStmt
    | RepeatUntilStmt
    | DoWhileStmt
    | BREAK_    { $$ = $1; }
    | CONTINUE_ { $$ = $1; };

InputStmt: READ_ '(' ID_ ')' { $$ = makeNode(READ, VOID, $3, NULL, NULL, "READ"); };

OutputStmt: WRITE_ '(' expr ')' { $$ = makeNode(WRITE, VOID, $3, NULL, NULL, "WRITE"); };

AsgStmt: ID_ '=' expr { $$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "="); };

IfStmt: IF_ '(' expr ')' THEN_ stmt_list ELSE_ stmt_list ENDIF_ { $$ = makeNode(IF, VOID, $3, $6, $8, "IF"); }
      | IF_ '(' expr ')' THEN_ stmt_list ENDIF_                 { $$ = makeNode(IF, VOID, $3, $6, NULL, "IF"); };

WhileStmt: WHILE_ '(' expr ')' DO_ stmt_list ENDWHILE_ { $$ = makeNode(WHILE, VOID, $3, NULL, $6, "WHILE"); };

RepeatUntilStmt: REPEAT_ stmt_list UNTIL_ '(' expr ')' { $$ = makeNode(REPEAT, VOID, $2, NULL, $5, "REPEAT"); };

DoWhileStmt: DO_ stmt_list WHILE_ '(' expr ')' { $$ = makeNode(DOWHILE, VOID, $2, NULL, $5, "DOWHILE"); };

expr : expr PLUS_ expr   { $$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "+"); }
     | expr MINUS_ expr  { $$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "-"); }
     | expr MUL_ expr    { $$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "*"); }
     | expr DIV_ expr    { $$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "/"); }
     | expr LT_ expr     { $$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "<"); }
     | expr GT_ expr     { $$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, ">"); }
     | expr LE_ expr     { $$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "<="); }
     | expr GE_ expr     { $$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, ">="); }
     | expr NE_ expr     { $$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "!="); }
     | expr EQ_ expr     { $$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "=="); }
     | '(' expr ')'      { $$ = $2; }
     | NUM_              { $$ = $1; }
     | id                { $$ = $1; }
     | TEXT_             { $$ = $1; };

id: ID_ {
        $$ = $1;
        struct GST_Node *curr = GSTLookup($1->varname);
        if (curr == NULL) {
            printf("Error: Variable \"%s\" not declared at line %d\n", $1->varname, yylineno);
            exit(1);
        }
        $$->type = curr->type;
    };

%%

void yyerror(char const *s) {
    printf("yyerror %s at line %d near '%s'\n", s, yylineno, yytext);
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) yyin = fp;
    } else {
        yyin = fopen("input.txt", "r");
    }

    if (!yyin) {
        printf("Error: Could not open input file.\n");
        return 1;
    }

    yyparse();
    return 0;
}