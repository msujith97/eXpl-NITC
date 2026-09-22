%{
#include <stdio.h>
#include <stdlib.h>
#include "task1.h"

int yylex();
void yyerror(const char *s);

extern FILE *yyin;
int currentType;
%}

%union {
    int val;
    char *str;
    struct Paramstruct *param;
}

%token DECL ENDDECL INT STR MAIN RETURN PBEGIN PEND
%token <str> ID
%token <val> NUM

%type <val> Type
%type <param> ParamList Param

%%

Program : GDeclBlock FdefBlock MainBlock
        | GDeclBlock MainBlock
        | MainBlock
        ;

GDeclBlock : DECL GDeclList ENDDECL {
                printSymbolTable(); // Display GST after parsing declarations
             }
           | DECL ENDDECL {
                printSymbolTable();
             }
           ;

GDeclList : GDeclList GDecl
          | GDecl
          ;

GDecl : Type { currentType = $1; } GidList ';' ;

GidList : GidList ',' Gid
        | Gid
        ;

Gid : ID {
        GInstall($1, currentType, 1, NULL); // Scalar variable
      }
    | ID '[' NUM ']' {
        GInstall($1, currentType, $3, NULL); // Array variable
      }
    | ID '(' ParamList ')' {
        GInstall($1, currentType, -1, $3); // Function declaration
      }
    ;

ParamList : ParamList ',' Param { $$ = appendParam($1, $3); }
          | Param               { $$ = $1; }
          | /* empty */         { $$ = NULL; }
          ;

Param : Type ID { $$ = createParamNode($2, $1); } ;

Type : INT { $$ = TYPE_INT; }
     | STR { $$ = TYPE_STR; }
     ;

/* Extended rules so FdefBlock and MainBlock parse valid ExpL syntax */
FdefBlock : FdefBlock Fdef 
          | Fdef 
          ;

Fdef : Type ID '(' ParamList ')' '{' LdeclBlock Body '}' ;

LdeclBlock : DECL LDecList ENDDECL
           | DECL ENDDECL
           | /* empty */
           ;

LDecList : LDecList LDecl
         | LDecl
         ;

LDecl : Type IdList ';' ;

IdList : IdList ',' ID
       | ID
       ;

MainBlock : INT MAIN '(' ')' '{' LdeclBlock Body '}' ;

Body : PBEGIN PEND ;

%%

void yyerror(const char *s) {
    printf("Parse Error: %s\n", s);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            yyin = fp;
        } else {
            printf("Error opening file: %s\n", argv[1]);
            return 1;
        }
    }
    yyparse();
    return 0;
}