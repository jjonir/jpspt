%{
#include <stdlib.h>
#include "parser.tab.h"

extern int lit_int;
extern float lit_float;
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
	lit_int = strtoul(yytext, NULL, 10);
	return INT;
}

{float} {
	lit_float = strtof(yytext, NULL);
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
