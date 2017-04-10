/*
	0: LABEL
	1: CLASS
	2: ID
*/
%{
#include "main.h"
#include "css.tab.h"
#include <stdio.h>
extern char * yylval;
%}  


%% 

[a-zA-Z_][-a-zA-Z0-9_]* {
	yylval = strdup(yytext);
	return LABEL;
}

[.][a-zA-Z_][-a-zA-Z0-9_]* {
	yylval = strdup(yytext);
	return CLASS;
}

[#][a-zA-Z_][-a-zA-Z0-9_]* {
	yylval = strdup(yytext);
	return ID;
}

:[^;]+[;] {
	yylval = strdup(yytext);
	return VALUE;
}

[ \t\n]   ;

. {
	//printf("nonchanged %c\n", yytext[0]);
	return yytext[0];  
}

%%  

int yywrap()
{
	return 0;
}
