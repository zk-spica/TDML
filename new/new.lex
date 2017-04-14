%{
#include "main.h"
#include "new.tab.h"
#include <stdio.h>
#include <string.h>
extern char * yylval;
%}  

%option noyywrap

%% 
 
[a-zA-Z_][-a-zA-Z0-9_]* {
	yylval = strdup(yytext);
	return NAME;
}

\"[^"]*\" {
	yylval = strdup(yytext);
	return VALUE;
}

[ \t\n]   ;

. {
	//printf("nonchanged %c\n", yytext[0]);
	return yytext[0];  
}

%%  
