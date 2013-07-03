%{
#include <stdlib.h>
#include <stdio.h>
#include "parser.tab.h"

YYSTYPE yylval;
%}

float ([+-]?[0-9]+(\.[0-9]*)?([eE][+-]?[0-9]+)?)|([+-]?\.[0-9]+([eE][+-]?[0-9]+)?)
int [+-]?[0-9]+

%%

[ \t\n]+ /* not sure if this is right... */
#.*\n    /* comments */
g.*\n /* ignore for now */
s.*\n /* ignore for now */
mtllib.*\n /* wtf ignore for now */
usemtl.*\n /* wtf ignore for now */

"v" {
	return V;
}

"vt" {
	return VT;
}

"vn" {
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
