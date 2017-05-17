#include <stdio.h>
#include <string.h>

Label *labelStack[100000];
int nLabelStack;
int labelCount, stackCleared;

extern FILE *ofp;

int yyerror(char*);

#define NTAGNAME 7
char *tagName[NTAGNAME] =
{
	"tdml",
	"head",
	"body",
	"camera",
	"stylesheet",
	"cuboid",
	"sphere"
};

enum TagName
{
	tdml,
	head,
	body,
	camera,
	stylesheet,
	cuboid,
	sphere,
	undefined_tag
};

#define NPROPERTYNAME 10
char *propertyName[NPROPERTYNAME] =
{
	"x-offset", "y-offset", "z-offset",
	"x-length", "y-length", "z-length",
	"color",
	"radius",
	"type",
	"look-at"
};

enum PropertyName
{
	x_offset, y_offset, z_offset,
	x_length, y_length, z_length,
	color,
	radius,
	type,
	look_at,
	undefined_property
};
	
enum TagName getTagName(char *name)
{
	int i;
	for (i=0; i<NTAGNAME; ++i)
	if (!strcmp(tagName[i],name)) return (enum TagName)i;
	return undefined_tag;
}

int getPropertyNumber(char *name)
{
	int i;
	for (i=0; i<NPROPERTYNAME; ++i)
	if (!strcmp(propertyName[i],name)) return i;
	return -1;
}

void createPropertyValueField(enum TagName tagName, enum PropertyName propertyName, char *propertyValue)
{

	printf("createProperty #%s#\n", propertyValue);

	propertyValue++;
	propertyValue[strlen(propertyValue)-1] = '\0';
	
	printf("createProperty #%s#\n", propertyValue);
	
	
	switch (propertyName)
	{
		case x_offset:
		case y_offset:
		case z_offset:
		case x_length:
		case y_length:
		case z_length:
			if (propertyValue[strlen(propertyValue)-1] != '%')
			{
				fprintf(ofp, "property->valueForm = 0;\n");
				fprintf(ofp, "property->value = malloc(sizeof(float));\n");
				fprintf(ofp, "*(float*)property->value = %s;\n", propertyValue);
			}
			else
			{
				fprintf(ofp, "property->valueForm = 1;\n");
				fprintf(ofp, "property->value = malloc(sizeof(float));\n");
				propertyValue[strlen(propertyValue)-1] = '\0';
				fprintf(ofp, "*(float*)property->value = %s;\n", propertyValue);
				propertyValue[strlen(propertyValue)-1] = '%';
			}
			break;
		case radius:
			fprintf(ofp, "property->valueForm = 0;\n");
			fprintf(ofp, "property->value = malloc(sizeof(float));\n");
			fprintf(ofp, "*(float*)property->value = %s;\n", propertyValue);
			break;
		case color:
			fprintf(ofp, "property->valueForm = 0;\n");
			fprintf(ofp, "property->value = malloc(sizeof(float)*3);\n");
			float r, g, b;
			sscanf(propertyValue, "rgb(%f,%f,%f)", &r, &g, &b);
			fprintf(ofp, "*(float*)property->value = %f;\n", r);
			fprintf(ofp, "*((float*)property->value+1) = %f;\n", g);
			fprintf(ofp, "*((float*)property->value+2) = %f;\n", b);
			break;
		case type:
			fprintf(ofp, "property->valueForm = 0;\n");
			fprintf(ofp, "property->value = malloc(sizeof(char));\n");
			char tp = '?';
			if (!strcmp(propertyValue,"solid")) tp = 's';
			else
			if (!strcmp(propertyValue,"wire")) tp = 'w';
			fprintf(ofp, "*(char*)property->value = '%c';\n", tp);
			break;
	}
}

void onGetEndingLabel(char *name)
{
	printf("onGetEndingLabel %s\n", name);
	
	printf("stacktop:%s\n", labelStack[nLabelStack]->name);
	
	if (strcmp(labelStack[nLabelStack]->name,name)) printf("tag <%s> not matched", name);
	fprintf(ofp, "nNodeStack--;\n");
	nLabelStack--;
}

char* tagNameString(int x)
{
	return tagName[x];
}

void onGetStartingLabel(Label *label,int isSingle)
{
	enum TagName tagName;
	Prop *p;
	printf("onGetStartingLabel %s\n", label->name);
	labelStack[++nLabelStack] = label;
	labelStack[nLabelStack]->childCount = 0;
	++labelCount;
	tagName = getTagName(label->name);
	if (tagName == undefined_tag) printf("unknown tag name: %s\n", label->name);
	
	printf("new Node no=%d  name=%s\n", tagName, tagNameString(tagName));
	
	fprintf(ofp, "nodeStack[++nNodeStack].node = new DOMNode(%d);\n", (int)tagName);
	if (labelCount == 1)
	{
		if (strcmp(label->name,"tdml")) yyerror("root tag not <tdml>");
		fprintf(ofp, "domRoot = nodeStack[1].node;\n");
	}
	fprintf(ofp, "nodeStack[nNodeStack].lastChild = NULL;\n");
	for (p=label->hd; p!=NULL; p=p->next)
	{
		int propertyNumber = getPropertyNumber(p->name);
		if (propertyNumber < 0) printf("unknown propery name: %s\n", p->name);
		fprintf(ofp, "property = new Property(%d);\n", propertyNumber);
		createPropertyValueField(tagName, (enum PropertyName)propertyNumber, p->val);
		fprintf(ofp, "nodeStack[nNodeStack].node->property.push_back(property);\n");
	}
	if (labelStack[nLabelStack-1]->childCount == 0)
	{
		fprintf(ofp, "nodeStack[nNodeStack-1].node->firstChild = nodeStack[nNodeStack].node;\n");
		fprintf(ofp, "nodeStack[nNodeStack-1].lastChild = nodeStack[nNodeStack].node;\n");
	}
	else
	{
		fprintf(ofp, "nodeStack[nNodeStack-1].lastChild->nextSibling = nodeStack[nNodeStack].node;\n");
		fprintf(ofp, "nodeStack[nNodeStack-1].lastChild = nodeStack[nNodeStack].node;\n");
	}
	
	printf("parse properties over\n");
	
	labelStack[nLabelStack-1]->childCount++;
	if (isSingle) onGetEndingLabel(label->name);
}

