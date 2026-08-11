%{
    #include <stdio.h>
    #include "task2.h"
    #include "y.tab.h"
%}

%%

[0-9]+  {
            yylval.no = makeLeafNode(atoi(yytext));
            return NUM;
        }
"+"     { return '+'; }
"-"     { return '-'; }
"*"     { return '*'; }
"/"     { return '/'; }
"("     { return '('; }
")"     { return ')'; }
[\n]    { return END; }
[ \t]   { /* ignore whitespace */ }
.       { yyerror("Unknown character"); exit(1); }

%%

int yywrap() {
    return 1;
}
