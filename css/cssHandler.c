#include <stdio.h>
#include <string.h>

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


enum TagName getTagName(char *name)
{
	int i;
	for (i=0; i<NTAGNAME; ++i)
	if (!strcmp(tagName[i],name)) return (enum TagName)i;
	return undefined_tag;
}

#define NPROPERTYNAME 22
char *propertyName[NPROPERTYNAME] =
{
	"tdmlclass",
	"tdmlid",
	"x-offset", "y-offset", "z-offset",
	"x-length", "y-length", "z-length",
	"x-rotation", "y-rotation", "z-rotation",
	"color",
	"radius",
	"type",
	"look-at",
	"texture",
	"texture-x-positive", "texture-x-negative", "texture-y-positive", "texture-y-negative", "texture-z-positive", "texture-z-negative"
};

enum PropertyName
{
	tdmlclass,
	tdmlid,
	x_offset, y_offset, z_offset,
	x_length, y_length, z_length,
	x_rotation, y_rotation, z_rotation,
	color,
	radius,
	type,
	look_at,
	texture,
	texture_x_positive, texture_x_negative, texture_y_positive, texture_y_negative, texture_z_positive, texture_z_negative,
	undefined_property
};

int getPropertyNumber(char *name)
{
	int i;
	for (i=0; i<NPROPERTYNAME; ++i)
	if (!strcmp(propertyName[i],name)) return i;
	return -1;
}
void createPropertyValueField(enum PropertyName propertyName, char *propertyValue)
{

	printf("createProperty #%s#\n", propertyValue);
	
	//printf("createProperty #%s#\n", propertyValue);
	
	
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

void onGetCSS(Nameset * s1, Prop * s2)
{
	fprintf(ofp, "csslist = new cssList();\n");
	fprintf(ofp, "csshd.push_back(csslist);\n");
	Namelist * namelist;
	while (s1!=NULL)
	{
		namelist=s1->list;
		fprintf(ofp, "select = new Select();\n");
		while (namelist!=NULL)
		{
			if (namelist->name[0]!='.'&&namelist->name[0]!='#')
				fprintf(ofp, "select->a.push_back(string(\"%d\"));\n", getTagName(namelist->name));
			else fprintf(ofp, "select->a.push_back(string(\"%s\"));\n", namelist->name);
			namelist=namelist->next;
		}
		fprintf(ofp, "select->now = select->a.begin();\n");
		fprintf(ofp, "csslist->select.push_back(select);\n");
		s1=s1->next;
	}
	while (s2!=NULL)
	{
		int propertyNumber = getPropertyNumber(s2->name);
		if (propertyNumber < 0) printf("unknown propery name: %s\n", s2->name);
		fprintf(ofp, "property = new Property(%d);\n", propertyNumber);
		createPropertyValueField((enum PropertyName)propertyNumber, s2->val);
		fprintf(ofp, "csslist->property.push_back(property);\n");
		s2 = s2->next;
	}
	fprintf(ofp, "\n");
}

