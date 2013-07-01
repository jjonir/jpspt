%{
#include <stdlib.h>
#include <stdio.h>
#include "parser.tab.h"

YYSTYPE yylval;
%}

float [+-]?([0-9]+\.?[0-9]*)|(\.[0-9]+)
int [+-]?[0-9]+

%%

[ \t\n]+ /* not sure if this is right... */
#.*\n    /* comments */

"v" {
	return V;
}

"vt" {
	return VT;
}

"n" {
	return N;
}

"f" {
	return F;
}

{int} {
	yylval.i = strtoul(yytext, NULL, 10);
	return INT;
}

{float} {
	yylval.f = strtof(yytext, NULL);
	return FLOAT;
}

"/" {
	return yytext[0];
}

%%

int yywrap(void)
{
	return 1;
}
