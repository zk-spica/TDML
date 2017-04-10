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

%}
%token LABEL CLASS ID VALUE

%%

S:
	S comlabel {}
|	comlabel {}
;

comlabel:
	'<' label '>' {
		getlabel((data2 *)$2, 0);
	}
|	'<' label '/' '>' {
		getlabel((data2 *)$2, 1);
	}
|	'<' '/' NAME '>' {
		finishlabel($3);
	}
;

label:
	label property {
		data2 * tmp1 = (data2 *)$1;
		data1 * tmp2 = (data1 *)$2;
		tmp2->next = tmp1->hd;
		tmp1->hd = tmp2;
		//printf("label property: %s %s\n",tmp2->name,tmp2->val);
		$$ = (char *)tmp1;
	}
|	NAME {
	data2 * tmp = (data2 *)malloc(sizeof(data2));
	tmp->name = $1; tmp->hd = NULL;
	$$ = (char *)tmp;
}
;


property:
	NAME '=' VALUE {
		data1 * tmp = (data1 *)malloc(sizeof(data1));
		//printf("%d",strlen($3));
		//tmp->name = (char *)malloc(sizeof(char)*(1+strlen($1))); memcpy(tmp->name, $1, strlen($1)+1);
		//tmp->val = (char *)malloc(sizeof(char)*(1+strlen($3))); memcpy(tmp->val, $3, strlen($3)+1);
		//printf("SS:%s\n",tmp->name);
		tmp->name = $1; tmp->val = $3;
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

void getlabel(data2* s,int flag)
{
	printf("getlabel: %s %d\n" ,s->name ,flag);
	data1* tmp = s->hd;
	while (tmp!=NULL)
	{
		printf("%s=%s\n", tmp->name, tmp->val);
		tmp = tmp->next;
	}
	printf("\n");
}

void finishlabel(char * name)
{
	printf("%s finished\n" ,name);
}