%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task1.h"
#include "y.tab.h"
%}

%%

"decl"          { return DECL; }
"enddecl"       { return ENDDECL; }
"int"           { return INT; }
"str"           { return STR; }
"main"          { return MAIN; }
"return"        { return RETURN; }
"begin"         { return PBEGIN; }
"end"           { return PEND; }

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.str = strdup(yytext);
    return ID;
}

[0-9]+ {
    yylval.val = atoi(yytext);
    return NUM;
}

"["             { return '['; }
"]"             { return ']'; }
"("             { return '('; }
")"             { return ')'; }
"{"             { return '{'; }
"}"             { return '}'; }
";"             { return ';'; }
","             { return ','; }
"="             { return '='; }
"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }

[ \t\n]         { /* ignore whitespace */ }
.               { printf("Unknown character: %s\n", yytext); exit(1); }

%%

int yywrap() {
    return 1;
}
