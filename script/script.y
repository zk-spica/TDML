%{
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

%}
%token VAR FUNCTION NEW IF FOR WHILE NAME DOUBLE INT

%%

S:
	Claim Body
	{
		FILE * fp=fopen("script.out", "w");
		fprintf(fp, "class Script\n{\npublic:\n");
		fclose(fp);
		fp=fopen("script.out", "a");
		putTo(fp,"Type__");
		fprintf(fp, "%s%s};\n", $1,$2);
	}
Claim:
	Claim oneClaim
	{
		
	}

%%

int main()  
{
    yyparse();  
    return 0;  
}  
  
int yyerror(char *s)  
{  
    printf("error: %s\n",s);  
    return 0;  
}

void putTo(FILE * fp, char * b)
{
	FILE * fp2=fopen(b, "r");
	char c;
	while (fscanf(fp2, "%c", &c)!=EOF)
		fprintf(fp, "%c", c);
	fclose(fp2);
}

char * mergech(char * a, char * b)
{
	int n1=strlen(a), n2=strlen(b);
	char * c=(char *)malloc(n1+n2+1);
	memcpy(c, a, n1);
	memcpy(c+n1, b, n2);
	c[n1+n2]='\0';
	return c;
}