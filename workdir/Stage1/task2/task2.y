%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "task2.h"

    int yylex(void);
    void yyerror(char const *s);
%}

%union {
    struct tnode *no;
}

%type <no> expr
%token <no> NUM
%token END

%left '+' '-'
%left '*' '/'

%%

program : expr END {
            FILE *fp = fopen("target.xsm", "w");

            // Header
            fprintf(fp, "0\n2056\n0\n0\n0\n0\n0\n0\n");

            // 1. MUST initialize stack pointer before any PUSH
            fprintf(fp, "MOV SP, 4095\n");

            // 2. Generate tree evaluation code (returns register index containing result)
            int result_reg = codeGen($1, fp);

            // Store result at memory 4096
            fprintf(fp, "MOV [4096], R%d\n", result_reg);

            // 3. Use a different register (R1) for library args to avoid destroying result_reg
            int temp_reg = (result_reg == 0) ? 1 : 0;

            fprintf(fp, "MOV R%d, \"Write\"\n", temp_reg);
            fprintf(fp, "PUSH R%d\n", temp_reg);           // Function code
            fprintf(fp, "MOV R%d, -2\n", temp_reg);
            fprintf(fp, "PUSH R%d\n", temp_reg);           // Argument 1 (stdout)
            fprintf(fp, "PUSH R%d\n", result_reg);         // Argument 2 (Calculated Value)
            fprintf(fp, "PUSH R%d\n", temp_reg);           // Argument 3 (Blank)
            fprintf(fp, "PUSH R%d\n", temp_reg);           // Space for Return Value

            // Call Library
            fprintf(fp, "CALL 0\n");
            fprintf(fp, "SUB SP, 5\n");
            fprintf(fp, "INT 10\n");

            fclose(fp);
            exit(0);
        }
        ;

expr : expr '+' expr  { $$ = makeOperatorNode('+', $1, $3); }
     | expr '-' expr  { $$ = makeOperatorNode('-', $1, $3); }
     | expr '*' expr  { $$ = makeOperatorNode('*', $1, $3); }
     | expr '/' expr  { $$ = makeOperatorNode('/', $1, $3); }
     | '(' expr ')'   { $$ = $2; }
     | NUM            { $$ = $1; }
     ;

%%

void yyerror(char const *s) {
    printf("yyerror: %s\n", s);
}

int main() {
    printf("Enter expression: ");
    yyparse();
    return 0;
}
