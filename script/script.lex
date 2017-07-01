%{
#include "main.h"
#include "script.tab.h"
#include <stdio.h>
extern char * yylval;
%}  


%% 

var {
	return VAR;
}

function {
	return FUNCTION;
}

new {
	return NEW;
}

if {
	return IF;
}

for {
	return FOR;
}

while {
	return WHILE;
}

[a-zA-Z_][-a-zA-Z0-9_]* {
	yylval = strdup(yytext);
	return NAME;
}

[0-9]+.[0-9]* {
	yylval = strdup(yytext);
	return DOUBLE;
}

[0-9]+ {
	yylval = strdup(yytext);
	return INT;
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
