#define _USE_MATH_DEFINES

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <list>

#include <fstream>
#include <iterator>
#include <string>
#include <windows.h>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/glut.h>
#include <FreeImage.h>
#include <FreeImagePlus.h>

using namespace std;

GLuint tex[3];

int windowWidth = 1024, windowHeight = 768;

GLuint program = 0;
GLint uniformCenter = -1;

class Property
{
public:
	enum Name
	{
		x_offset, y_offset, z_offset,
		x_length, y_length, z_length,
		color,
		radius,
		type
	};

	Property(int propertyNumber)
	{
		name = Name(propertyNumber);
		value = NULL;
	}

	Name name;
	char valueForm;
	void *value;
};

class DOMNode
{
public:
	enum TagName
	{
		tdml,
		head,
		body,
		stylesheet,
		cuboid,
		sphere
	};

	DOMNode(int tagNumber)
	{
		tagName = TagName(tagNumber);
		firstChild = nextSibling = NULL;
		x_length = y_length = z_length = 0;
		x_base = y_base = z_base = 0;
	}

	DOMNode *firstChild, *nextSibling;
	float x_length, y_length, z_length, x_base, y_base, z_base;
	TagName tagName;
	list<Property*> property;
};

DOMNode *domRoot; 

struct DOMNodeStack
{
	DOMNode *node;
	DOMNode *lastChild;
}
nodeStack[100000];

int nNodeStack;

void checkGL()
{
	glFinish();
	GLenum error = glGetError();
	assert(error == GL_NO_ERROR);
}


void drawDOMTree(DOMNode *currentNode, DOMNode *parentNode)
{
	float r=1, g=1, b=1,
	      x_offset=0, y_offset=0, z_offset=0,
          x_length=0, y_length=0, z_length=0,
	      radius=0;
	char type='s';

	for (auto property=currentNode->property.begin(); property!=currentNode->property.end(); property++)
	{
		if ((*property)->name == Property::x_offset)
		{
			if ((*property)->valueForm == 0) x_offset = *(float*)(*property)->value;
			else x_offset = parentNode->x_length * (*(float*)(*property)->value)/100;
		}
		else
		if ((*property)->name == Property::y_offset)
		{
			if ((*property)->valueForm == 0) y_offset = *(float*)(*property)->value;
			else y_offset = parentNode->y_length * (*(float*)(*property)->value)/100;
		}
		else
		if ((*property)->name == Property::z_offset)
		{
			if ((*property)->valueForm == 0) z_offset = *(float*)(*property)->value;
			else z_offset = parentNode->z_length * (*(float*)(*property)->value)/100;
		}
		else
		if ((*property)->name == Property::x_length)
		{
			if ((*property)->valueForm == 0) x_length = *(float*)(*property)->value;
			else x_length = parentNode->x_length * (*(float*)(*property)->value)/100;
		}
		else
		if ((*property)->name == Property::y_length)
		{
			if ((*property)->valueForm == 0) y_length = *(float*)(*property)->value;
			else y_length = parentNode->y_length * (*(float*)(*property)->value)/100;
		}
		else
		if ((*property)->name == Property::z_length)
		{
			if ((*property)->valueForm == 0) z_length = *(float*)(*property)->value;
			else z_length = parentNode->z_length * (*(float*)(*property)->value)/100;
		}
		else
		if ((*property)->name == Property::radius)
		{
			radius = *(float*)(*property)->value;
		}
		else
		if ((*property)->name == Property::color)
		{
			r = *(float*)(*property)->value;
			g = *((float*)(*property)->value+1);
			b = *((float*)(*property)->value+2);
		}
		else
		if ((*property)->name == Property::type)
		{
			type = *(char*)(*property)->value;
		}
	}

	if (currentNode->tagName == DOMNode::body)
	{
		currentNode->x_base = parentNode->x_base;
		currentNode->y_base = parentNode->y_base;
		currentNode->z_base = parentNode->z_base;
		currentNode->x_length = parentNode->x_length;
		currentNode->y_length = parentNode->y_length;
		currentNode->z_length = parentNode->z_length;
	}
	else
	if (currentNode->tagName == DOMNode::cuboid)
	{
		currentNode->x_length = x_length;
		currentNode->y_length = y_length;
		currentNode->z_length = z_length;
		currentNode->x_base = parentNode->x_base + x_offset;
		currentNode->y_base = parentNode->y_base + y_offset;
		currentNode->z_base = parentNode->z_base + z_offset;
		
		glLoadIdentity();
	
		gluLookAt(0, -11, 5, 0, 0, 0, 0, 0, 1);
	
		glScalef(x_length, y_length, z_length);
		glTranslatef(currentNode->x_base, currentNode->y_base, currentNode->z_base);

		GLfloat light_position[] = {1.5f, -1.0f, 2.0f, 0.0f};
		GLfloat light_ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
		GLfloat light_diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
		GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);


		GLfloat mat_ambient[]  = {r, g, b, 1};
		GLfloat mat_diffuse[]  = {r, g, b, 1};
		GLfloat mat_shininess  = 1.0f;

		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialf (GL_FRONT, GL_SHININESS, mat_shininess);

		glColor3f(1, 1, 1);

				printf("type=%c\n", type);

		if (type == 's') glutSolidCube(1);
		else
		if (type == 'w') glutWireCube(1);
		else printf("error: unknown type\n");
	}
	else
	if (currentNode->tagName == DOMNode::sphere)
	{

		printf("get sphere\n");

		printf("rgb=%f %f %f\n", r, g, b);
		printf("radius=%f\n", radius);

		currentNode->x_length = currentNode->y_length = currentNode->z_length = radius*2;
		currentNode->x_base = parentNode->x_base + x_offset;
		currentNode->y_base = parentNode->y_base + y_offset;
		currentNode->z_base = parentNode->z_base + z_offset;
		glLoadIdentity();
	
		gluLookAt(0, -11, 5, 0, 0, 0, 0, 0, 1);
	
		glTranslatef(currentNode->x_base, currentNode->y_base, currentNode->z_base);

		GLfloat light_position[] = {1.5f, -1.0f, 2.0f, 0.0f};
		GLfloat light_ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
		GLfloat light_diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
		GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);


		GLfloat mat_ambient[]  = {r, g, b, 1};
		GLfloat mat_diffuse[]  = {r, g, b, 1};
		GLfloat mat_shininess  = 1.0f;

		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialf (GL_FRONT, GL_SHININESS, mat_shininess);

		glColor3f(r, g, b);

		if (type == 's') glutSolidSphere(radius, 20, 20);
		else
		if (type == 'w') glutWireSphere(radius, 20, 20);
		else printf("error: unknown type\n");
	}

	for (DOMNode *child=currentNode->firstChild; child!=NULL; child=child->nextSibling)
	{
		drawDOMTree(child, currentNode);
	}
}


void init()
{
	glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_ZERO, GL_SRC_COLOR);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void display()
{
	glClearColor(0, 0, 0, 0);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, double(windowWidth) / double(windowHeight), 0.1, 1000.0);

	glMatrixMode(GL_MODELVIEW);

	
	drawDOMTree(domRoot, nodeStack[0].node);



	checkGL();

	glutSwapBuffers();
	glutPostRedisplay();
}


void buildTree();
void reshape(int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, width, height);
}

int main(int argc, char *argv[])
{
	FreeImage_Initialise();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("OpenGL Window");

	glewInit();
	init();

	buildTree();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutMainLoop();

	return 0;
}


void buildTree()
{
	Property *property;
	nodeStack[0].node = new DOMNode(0);

#define TDML_BUILDTREE
	nodeStack[++nNodeStack].node = new DOMNode(0);
	domRoot = nodeStack[1].node;
	nodeStack[nNodeStack].lastChild = NULL;
	nodeStack[nNodeStack-1].node->firstChild = nodeStack[nNodeStack].node;
	nodeStack[nNodeStack-1].lastChild = nodeStack[nNodeStack].node;
	nodeStack[++nNodeStack].node = new DOMNode(4);
	nodeStack[nNodeStack].lastChild = NULL;
	property = new Property(5);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 5;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(4);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 5;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(3);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 7;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(6);
	property->valueForm = 0;
	property->value = malloc(sizeof(float)*3);
	*(float*)property->value = 1.000000;
	*((float*)property->value+1) = 0.000000;
	*((float*)property->value+2) = 1.000000;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(8);
	property->valueForm = 0;
	property->value = malloc(sizeof(char));
	*(char*)property->value = 'w';
	nodeStack[nNodeStack].node->property.push_back(property);
	nodeStack[nNodeStack-1].node->firstChild = nodeStack[nNodeStack].node;
	nodeStack[nNodeStack-1].lastChild = nodeStack[nNodeStack].node;
	nodeStack[++nNodeStack].node = new DOMNode(4);
	nodeStack[nNodeStack].lastChild = NULL;
	property = new Property(6);
	property->valueForm = 0;
	property->value = malloc(sizeof(float)*3);
	*(float*)property->value = 0.000000;
	*((float*)property->value+1) = 1.000000;
	*((float*)property->value+2) = 0.000000;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(5);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 2;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(4);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 2;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(3);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 2;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(0);
	property->valueForm = 1;
	property->value = malloc(sizeof(float));
	*(float*)property->value = -20;
	nodeStack[nNodeStack].node->property.push_back(property);
	nodeStack[nNodeStack-1].node->firstChild = nodeStack[nNodeStack].node;
	nodeStack[nNodeStack-1].lastChild = nodeStack[nNodeStack].node;
	nNodeStack--;
	nodeStack[++nNodeStack].node = new DOMNode(5);
	nodeStack[nNodeStack].lastChild = NULL;
	property = new Property(8);
	property->valueForm = 0;
	property->value = malloc(sizeof(char));
	*(char*)property->value = 'w';
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(7);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 2;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(0);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 1.5;
	nodeStack[nNodeStack].node->property.push_back(property);
	nodeStack[nNodeStack-1].lastChild->nextSibling = nodeStack[nNodeStack].node;
	nodeStack[nNodeStack-1].lastChild = nodeStack[nNodeStack].node;
	nNodeStack--;
	nNodeStack--;
	nNodeStack--;
	
}
