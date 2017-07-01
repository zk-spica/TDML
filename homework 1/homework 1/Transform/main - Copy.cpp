#define _USE_MATH_DEFINES

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <list>
#include <map>

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

#define NONE (GLuint(-1))

using namespace std;

void throwError(char *s)
{
	printf("error: %s\n", s);
	system("pause");
}

int windowWidth = 1024, windowHeight = 768;
int beginX, beginY, dx, dy;
float rotateX, rotateY;

struct Vector3f
{
    float x;
    float y;
    float z;

    Vector3f() {}

    Vector3f(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }
   
	Vector3f Cross(const Vector3f& v) const
	{
		return Vector3f(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
	}

	Vector3f& Normalize()
	{
		float module = sqrt(x*x+y*y+z*z);
		x /= module;
		y /= module;
		z /= module;
	}

	void Rotate(float Angle, const Vector3f& Axis)
	{

	}

    void Print() const
    {
        printf("(%.02f, %.02f, %.02f)", x, y, z);
    }
};

struct Camera
{
	bool enabled=false;
	Vector3f position, lookAt, normal;
	
} camera;

struct Property
{
	enum Name
	{
		x_offset, y_offset, z_offset,
		x_length, y_length, z_length,
		color,
		radius,
		type,
		look_at,
		texture,
		texture_x_positive, texture_x_negative, texture_y_positive, texture_y_negative, texture_z_positive, texture_z_negative
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

struct DOMNode
{
	enum TagName
	{
		tdml,
		head,
		body,
		camera,
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
		texture_x_positive = texture_x_negative = texture_y_positive = texture_y_negative = texture_z_positive = texture_z_negative = NONE;
		texture_x_positive_modified = texture_x_negative_modified = texture_y_positive_modified = texture_y_negative_modified = texture_z_positive_modified = texture_z_negative_modified = false;
	}

	DOMNode *firstChild, *nextSibling;
	float x_length, y_length, z_length, x_base, y_base, z_base;
	TagName tagName;
	list<Property*> property;
	GLuint texture_x_positive, texture_x_negative, texture_y_positive, texture_y_negative, texture_z_positive, texture_z_negative;
	bool texture_x_positive_modified, texture_x_negative_modified, texture_y_positive_modified, texture_y_negative_modified, texture_z_positive_modified, texture_z_negative_modified;
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

map<string,GLuint> textureIdMap;

void loadTexture(char *fileName)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	fif = FreeImage_GetFileType(fileName, 0);
	if (fif == FIF_UNKNOWN) fif = FreeImage_GetFIFFromFilename(fileName);
	if (fif == FIF_UNKNOWN) throwError("unknown image format");
	FIBITMAP *bitmap = FreeImage_Load(fif, fileName, 0);
	int width = FreeImage_GetWidth(bitmap);
	int height = FreeImage_GetHeight(bitmap);
	void *imageRawBit = malloc(width*height*3);
	FreeImage_ConvertToRawBits((BYTE*)imageRawBit, bitmap, ((24*width)+23)/24*3, 24, 0xff0000, 0x00ff00, 0x0000ff, false);
    GLuint textureId;
	glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, (const GLvoid*)imageRawBit);
	textureIdMap[string(fileName)] = textureId;
}

inline bool loadedTexture(char *fileName)
{
	return textureIdMap.find(string(fileName)) != textureIdMap.end();
}

inline void loadProperty_length(Property *property, float parentLength, float &value)
{
	if (property->valueForm == 0) value = *(float*)property->value;
	else value = parentLength * (*(float*)property->value)/100;
}

inline void loadProperty_texture(Property *property, bool modified, GLuint &value)
{
	if (modified)
	{
		if (!loadedTexture(*(char**)property->value)) loadTexture(*(char**)property->value);
		value = textureIdMap[*(char**)property->value];
		modified = false;
	}
}

void drawQuadrilateral(Vector3f &v0, Vector3f &v1, Vector3f &v2, Vector3f &v3, GLuint &textureId, float &r, float &g, float &b)
{

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
		switch ((*property)->name)
		{
			case Property::x_offset:
				loadProperty_length(*property, parentNode->x_length, x_offset);
				break;
			case Property::y_offset:
				loadProperty_length(*property, parentNode->y_length, y_offset);
				break;
			case Property::z_offset:
				loadProperty_length(*property, parentNode->z_length, z_offset);
				break;
			case Property::x_length:
				loadProperty_length(*property, parentNode->x_length, x_length);
				break;
			case Property::y_length:
				loadProperty_length(*property, parentNode->y_length, y_length);
				break;
			case Property::z_length:
				loadProperty_length(*property, parentNode->z_length, z_length);
				break;
			case Property::radius:
				radius = *(float*)(*property)->value;
				break;
			case Property::color:
				r = *(float*)(*property)->value;
				g = *((float*)(*property)->value+1);
				b = *((float*)(*property)->value+2);
				break;
			case Property::type:
				type = *(char*)(*property)->value;
				break;
			case Property::look_at:

				break;
			case Property::texture:
				loadProperty_texture(*property, currentNode->texture_x_positive_modified, currentNode->texture_x_positive);
				loadProperty_texture(*property, currentNode->texture_x_negative_modified, currentNode->texture_x_negative);
				loadProperty_texture(*property, currentNode->texture_y_positive_modified, currentNode->texture_y_positive);
				loadProperty_texture(*property, currentNode->texture_y_negative_modified, currentNode->texture_y_negative);
				loadProperty_texture(*property, currentNode->texture_z_positive_modified, currentNode->texture_z_positive);
				loadProperty_texture(*property, currentNode->texture_z_negative_modified, currentNode->texture_z_negative);
				break;
			case Property::texture_x_positive:
				loadProperty_texture(*property, currentNode->texture_x_positive_modified, currentNode->texture_x_positive);
				break;
			case Property::texture_x_negative:
				loadProperty_texture(*property, currentNode->texture_x_negative_modified, currentNode->texture_x_negative);
				break;
			case Property::texture_y_positive:
				loadProperty_texture(*property, currentNode->texture_y_positive_modified, currentNode->texture_y_positive);
				break;
			case Property::texture_y_negative:
				loadProperty_texture(*property, currentNode->texture_y_negative_modified, currentNode->texture_y_negative);
				break;
			case Property::texture_z_positive:
				loadProperty_texture(*property, currentNode->texture_z_positive_modified, currentNode->texture_z_positive);
				break;
			case Property::texture_z_negative:
				loadProperty_texture(*property, currentNode->texture_z_negative_modified, currentNode->texture_z_negative);
				break;
		}
	}

	printf("tagname=%d\n", currentNode->tagName);

	glTranslatef(x_offset, y_offset, z_offset);

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
		printf("get cuboid\n");

		printf("length=%f %f %f\n", x_length, y_length, z_length);
		printf("offset=%f %f %f\n", x_offset, y_offset, z_offset);

		currentNode->x_length = x_length;
		currentNode->y_length = y_length;
		currentNode->z_length = z_length;
		currentNode->x_base = parentNode->x_base + x_offset;
		currentNode->y_base = parentNode->y_base + y_offset;
		currentNode->z_base = parentNode->z_base + z_offset;
		
		printf("base=%f %f %f\n", currentNode->x_base, currentNode->y_base, currentNode->z_base);

		if (type == 'w')
		{
			//glLoadIdentity();
			//gluLookAt(camera.position.x, camera.position.y, camera.position.z, camera.lookAt.x, camera.lookAt.y, camera.lookAt.z, 0, 0, 1);
			//glTranslatef(currentNode->x_base, currentNode->y_base, currentNode->z_base);
			glScalef(x_length, y_length, z_length);
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
			glutWireCube(1);
			glScalef(1/x_length, 1/y_length, 1/z_length);
		}
		else
		if (type == 's')
		{
			Vector3f vertex[8];
			int it = 0;
			for (int fx=-1; fx<=1; fx+=2)
			for (int fy=-1; fy<=1; fy+=2)
			for (int fz=-1; fz<=1; fz+=2) vertex[it++] = Vector3f(x_length/2*fx, y_length/2*fy, z_length/2*fz);

			drawQuadrilateral(vertex[7], vertex[6], vertex[4], vertex[5], currentNode->texture_x_positive, r, g, b);
			

		}
		else throwError("unknown type");

		//glLoadIdentity();
	
		//gluLookAt(camera.position.x, camera.position.y, camera.position.z, camera.lookAt.x, camera.lookAt.y, camera.lookAt.z, 0, 0, 1);
	
		//glTranslatef(currentNode->x_base, currentNode->y_base, currentNode->z_base);
		glScalef(x_length, y_length, z_length);



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
		else throwError("unknown type");

		glScalef(1/x_length, 1/y_length, 1/z_length);
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
		//glLoadIdentity();
	
		//gluLookAt(camera.position.x+rotateX, camera.position.y+rotateY, camera.position.z, camera.lookAt.x, camera.lookAt.y, camera.lookAt.z, 0, 0, 1);
	
		//glTranslatef(currentNode->x_base, currentNode->y_base, currentNode->z_base);


		printf("sphere translate %f %f %f\n",currentNode->x_base, currentNode->y_base, currentNode->z_base);

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
		else throwError("unknown type");
	}

	for (DOMNode *child=currentNode->firstChild; child!=NULL; child=child->nextSibling)
	{
		drawDOMTree(child, currentNode);
	}

	glTranslatef(-x_offset, -y_offset, -z_offset);
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
	glLoadIdentity();
	gluLookAt(camera.position.x, camera.position.y, camera.position.z, camera.lookAt.x, camera.lookAt.y, camera.lookAt.z, 0, 0, 1);

	drawDOMTree(domRoot, nodeStack[0].node);


	checkGL();

	glutSwapBuffers();
//	glutPostRedisplay();
}

void buildTree();

void reshape(int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, width, height);
}

void mouseClick(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		beginX = x;
		beginY = y;
	}
	else
	if (state == GLUT_UP)
	{
		rotateX += dx;
		rotateY += dy;
		dx = dy = 0;
	}
	glutPostRedisplay();
}

void mouseDrag(int x, int y)
{
	dx = x-beginX;
	dy = y-beginY;
	glutPostRedisplay();
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

//	prescanTree(domRoot, nodeStack[0].node);
	camera.normal = {0, 1, 0};

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	if (camera.enabled)
	{
		glutMotionFunc(mouseDrag);
	}
	else
	{
		camera.position = {0, 11, 5};
		camera.lookAt = {0, 0, 0};
	}

	camera.position.y *= -1;
	
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
	nodeStack[++nNodeStack].node = new DOMNode(1);
	nodeStack[nNodeStack].lastChild = NULL;
	nodeStack[nNodeStack-1].node->firstChild = nodeStack[nNodeStack].node;
	nodeStack[nNodeStack-1].lastChild = nodeStack[nNodeStack].node;
	nodeStack[++nNodeStack].node = new DOMNode(3);
	nodeStack[nNodeStack].lastChild = NULL;
	property = new Property(-1);
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(-1);
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(-1);
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(2);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 0;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(1);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 11;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(0);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 10;
	nodeStack[nNodeStack].node->property.push_back(property);
	nodeStack[nNodeStack-1].node->firstChild = nodeStack[nNodeStack].node;
	nodeStack[nNodeStack-1].lastChild = nodeStack[nNodeStack].node;
	nNodeStack--;
	nNodeStack--;
	nodeStack[++nNodeStack].node = new DOMNode(2);
	nodeStack[nNodeStack].lastChild = NULL;
	nodeStack[nNodeStack-1].lastChild->nextSibling = nodeStack[nNodeStack].node;
	nodeStack[nNodeStack-1].lastChild = nodeStack[nNodeStack].node;
	nodeStack[++nNodeStack].node = new DOMNode(5);
	nodeStack[nNodeStack].lastChild = NULL;
	property = new Property(0);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 7;
	nodeStack[nNodeStack].node->property.push_back(property);
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
	*(float*)property->value = 0.000000;
	*((float*)property->value+1) = 1.000000;
	*((float*)property->value+2) = 0.000000;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(8);
	property->valueForm = 0;
	property->value = malloc(sizeof(char));
	*(char*)property->value = 'w';
	nodeStack[nNodeStack].node->property.push_back(property);
	nodeStack[nNodeStack-1].node->firstChild = nodeStack[nNodeStack].node;
	nodeStack[nNodeStack-1].lastChild = nodeStack[nNodeStack].node;
	nodeStack[++nNodeStack].node = new DOMNode(5);
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
	nodeStack[++nNodeStack].node = new DOMNode(6);
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
	nodeStack[++nNodeStack].node = new DOMNode(5);
	nodeStack[nNodeStack].lastChild = NULL;
	property = new Property(0);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = -2;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(10);
	property->valueForm = 0;
	property->value = malloc(sizeof(char*));
	*((char**)(property->value)) = "sun.bmp";
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
	nodeStack[nNodeStack-1].lastChild->nextSibling = nodeStack[nNodeStack].node;
	nodeStack[nNodeStack-1].lastChild = nodeStack[nNodeStack].node;
	nNodeStack--;
	nNodeStack--;
	nNodeStack--;
	
}
