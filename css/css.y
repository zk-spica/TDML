%{
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct data1
{
	char *name, *val;
	struct data1 * next;
}Prop;
typedef struct data2
{
	char *name;
	struct data2 * next;
}Namelist;
typedef struct data3
{
	Namelist * list;
	struct data3 * next;
}Nameset;

void getProp(Namelist * s1, Prop * s2)
{
	printf("getProp: ");
	while (s1!=NULL)
	{
		printf("%s ", s1->name);
		s1 = s1->next;
	}
	printf("\n");
	while (s2!=NULL)
	{
		printf("     %s %s\n", s2->name, s2->val);
		s2 = s2->next;
	}
	printf("\n");
}

%}
%token NAME CLASS ID VALUE

%%

S:
	S block {}
|	block {}
;

block:
	nameset_ '{' propset '}' {
		Nameset * s1 = (Nameset *)$1;
		Prop * s2 = (Prop *)$3;
		while (s1!=NULL)
		{
			getProp(s1->list, s2);
			s1 = s1->next;
		}
	}
;

nameset_:
	nameset_ ',' namelist_{
		Nameset *s1 = (Nameset *)$1;
		Namelist *s2 = (Namelist *)$3;
		Nameset * tmp = (Nameset *)malloc(sizeof(Nameset));
		tmp->next = s1->next;
		s1->next = tmp;
		tmp->list = s2;
		$$ = $1;
	}
|	namelist_ {
		Nameset * s1 = (Nameset *)malloc(sizeof(Nameset));
		s1->list = (Namelist *)$1;
		s1->next = NULL;
		$$ = (char *)s1;
	}
;

namelist_:
	namelist_ threetype {
		Namelist * s1 = (Namelist *)$1;
		while (s1->next!=NULL) s1 = s1->next;
		Namelist * tmp = (Namelist *)malloc(sizeof(Namelist));
		tmp->next = NULL;
		tmp->name = $2;
		s1->next = tmp;
		$$=$1;
	}
|	threetype {
		Namelist * s1 = (Namelist *)malloc(sizeof(Namelist));
		s1->name = $1;
		s1->next = NULL;
		$$ = (char *)s1;
	}
;

threetype:
	NAME {$$=$1;}
|	CLASS {$$=$1;}
|	ID {$$=$1;}
;

propset:
	propset NAME VALUE {
		int n = strlen($3)-1;
		while ($3[n-1]==' ') n--;
		$3[n] = '\0';
		char * s3 = $3+1;
		while (s3[0]==' ')s3++;
		Prop * tmp = (Prop *)malloc(sizeof(Prop));
		tmp->name = $2;
		tmp->val = s3;
		Prop * s1 = (Prop *)$1;
		while (s1->next!=NULL)
			s1 = s1->next;
		s1->next = tmp;
		tmp->next = NULL;
		$$ = $1;
	}
|	NAME VALUE {
		int n = strlen($2)-1;
		while ($2[n-1]==' ') n--;
		$2[n] = '\0';
		char * s2 = $2+1;
		while (s2[0]==' ')s2++;
		Prop * tmp = (Prop *)malloc(sizeof(Prop));
		tmp->name = $1;
		tmp->val = s2;
		tmp->next = NULL;
		$$ = (char *)tmp;
	}
;

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
