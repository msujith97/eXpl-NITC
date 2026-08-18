%{
    #include<stdio.h>
    #include <stdlib.h>
    #include "task1.h"

    extern FILE* yyin;
    extern char *yytext;
    void yyerror(const char *s);
    int yylex(void);
%}

%union {
    struct AST_Node *node;
}

%type <node> program stmt_list stmt expr InputStmt OutputStmt AsgStmt IfStmt WhileStmt
%token BEGIN_ END_ READ_ WRITE_ IF_ THEN_ ELSE_ ENDIF_ WHILE_ DO_ ENDWHILE_
%token <node> ID_ NUM_
%token PLUS_ MINUS_ MUL_ DIV_ LT_ LE_ GT_ GE_ EQ_ NE_
%left LT_ LE_ GT_ GE_ EQ_ NE_
%left PLUS_ MINUS_
%left MUL_ DIV_

%%
    program: BEGIN_ stmt_list END_ {
                                    $$ = $2;
                                    printf("Parsing completed successfully\n");
                                    print_tree($2,0,0);
                                    exit(1);
                                  }
            | BEGIN_ END_ {
                printf("Empty program\n"); printf("Parsing completed successfully\n"); exit(1);
            }

    stmt_list : stmt_list stmt ';' {$$=makeNode(STATEMENT,VOID,$1,NULL,$2,"STATEMENT");}
            | stmt';' {$$=$1;}
    
    stmt: InputStmt  { $$ = $1; }
    | OutputStmt  { $$ = $1; }
    | AsgStmt  { $$ = $1; }
	| IfStmt  { $$ = $1; }
	| WhileStmt  { $$ = $1; }
	
    InputStmt: READ_ '(' ID_ ')' { $$ = makeNode(READ, VOID, $3, NULL, NULL, "READ");}

    OutputStmt: WRITE_ '(' expr ')' { $$ = makeNode(WRITE, VOID, $3, NULL, NULL, "WRITE");}

    AsgStmt: ID_ '=' expr { $$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "="); }

    IfStmt: IF_ '(' expr ')' THEN_ stmt_list ELSE_ stmt_list ENDIF_ { $$ = makeNode(IF, VOID, $3, $6, $8, "IF");}
        | IF_ '(' expr ')' THEN_ stmt_list ENDIF_ { $$ = makeNode(IF, VOID, $3, $6, NULL, "IF");}

    WhileStmt: WHILE_ '(' expr ')' DO_ stmt_list ENDWHILE_ { $$ = makeNode(WHILE, VOID, $3, NULL, $6, "WHILE");}

    expr : expr PLUS_ expr		{$$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "+");}
        | expr MINUS_ expr		{$$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "-");}
        | expr MUL_ expr		{$$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "*");}
        | expr DIV_ expr		{$$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "/");}
        | expr LT_ expr			{$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "<");}
        | expr GT_ expr			{$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, ">");}
        | expr LE_ expr			{$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "<=");}
        | expr GE_ expr			{$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, ">=");}
        | expr NE_ expr			{$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "!=");}
        | expr EQ_ expr			{$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "==");}
        | '(' expr ')' 			{$$ = $2;}
        | NUM_					{$$ = $1;}
        | ID_					{$$ = $1;}
    
%%
void yyerror(const char *s) {
    printf("yyerror %s:%s\n", s,yytext);
}
int main() {
    yyin = fopen("input.expl", "r");
    yyparse();
    return 0;
}
