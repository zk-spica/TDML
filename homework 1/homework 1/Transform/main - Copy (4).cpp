#define _USE_MATH_DEFINES

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <list>
#include <map>
#include <sstream>
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
#define SEARCH_ID 0
#define SEARCH_CLASS 1

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

inline Vector3f operator-(const Vector3f& l, const Vector3f& r)
{
    Vector3f Ret(l.x - r.x,
                 l.y - r.y,
                 l.z - r.z);
    return Ret;
}

struct Camera
{
	bool enabled=false;
	Vector3f position, lookAt, normal;
	
} camera;

#define NPROPERTYNAME 22
string propertyNameString[NPROPERTYNAME] = {
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

struct Property
{
	enum Name
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

struct Select
{
	list<string> a;
	list<string>::iterator now;
};

struct cssList
{
	list<Select *> select;
	list<Property *> property;
};

list<cssList *> csshd;

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
		texture_x_positive_modified = texture_x_negative_modified = texture_y_positive_modified = texture_y_negative_modified = texture_z_positive_modified = texture_z_negative_modified = true;
		
	}

	string tdmlclass, tdmlid;
	DOMNode *firstChild, *nextSibling;
	float x_length, y_length, z_length, x_base, y_base, z_base;
	TagName tagName;
	list<Property*> property;
	list<cssList *> csslist;
	GLuint texture_x_positive, texture_x_negative, texture_y_positive, texture_y_negative, texture_z_positive, texture_z_negative;
	bool texture_x_positive_modified, texture_x_negative_modified, texture_y_positive_modified, texture_y_negative_modified, texture_z_positive_modified, texture_z_negative_modified;
};

DOMNode *domRoot; 

void find_CLASS_ID(DOMNode * root)
{
	//printf("%d\n", root->tagName);
	root->tdmlclass = string("");
	root->tdmlid = string("");
	for (auto i=root->property.begin(); i!=root->property.end(); i++)
	{
		//printf("*******%d %d\n", root->tagName, (*i)->name);
		if ((*i)->name==Property::tdmlclass)
			root->tdmlclass=string(*((char **)((*i)->value)));
		if ((*i)->name==Property::tdmlid)
			root->tdmlid = string(*((char **)((*i)->value)));
	}

	//printf("alive\n");

	for (DOMNode * i = root->firstChild; i != NULL; i = i->nextSibling)
	{
		//printf("i=%x\n", i);
		find_CLASS_ID(i);
	}

}

bool matchS(const string & nowS, const DOMNode::TagName & tagName, const string & tdmlclass, const string & tdmlid)
{
	if (nowS[0]>='0'&&nowS[1]<='9')
		return DOMNode::TagName(atoi(nowS.c_str()))==tagName;
	else
	{
		if (nowS[0]=='.')
			return tdmlclass.compare( (char *)nowS.c_str() + 1 ) == 0; 
		else
			return tdmlid.compare( (char *)nowS.c_str() + 1 ) == 0;
	}
}

void dfsDOMTree(DOMNode * root)
{
	bool flag;
	root->csslist.clear();
	list<list<string>::iterator> c;
	for (auto i=csshd.begin(); i!=csshd.end(); i++)
	{
		flag=false;
		for (auto j=(*i)->select.begin(); j!=(*i)->select.end(); j++)
		{
			c.push_back((*j)->now);
			if ( matchS(*((*j)->now), root->tagName, root->tdmlclass, root->tdmlid) )
			{
				auto jj = (*j)->now;
				jj++;
				if (jj == (*j)->a.end())
				{
					printf("%d %s\n", root->tagName, root->tdmlclass.c_str());
					//while (1);
					flag = true;
				}
				else
					(*j)->now++;
			}
		}
		if (flag)
			root->csslist.push_back(*i);
	}
	for (DOMNode * i=root->firstChild; i!=NULL; i=i->nextSibling)
		dfsDOMTree(i);
	auto it=c.begin();
	for (auto i=csshd.begin(); i!=csshd.end(); i++)
		for (auto j=(*i)->select.begin(); j!=(*i)->select.end(); j++)
			(*j)->now=*(it++);
}

void parseCSS(DOMNode * root) //call this every time
{
	/*for (auto i=csshd.begin(); i!=csshd.end(); i++) //clear css
		for (auto j=(*i)->select.begin(); j!=(*i)->select.end(); j++)
			(*j)->now = (*j)->a.begin();*/
	dfsDOMTree(root);
}

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



void renderLighting(float r, float g, float b)
{

	GLfloat light_ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
	GLfloat light_diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	GLfloat mat_ambient[]  = {r, g, b, 0.5};
	GLfloat mat_diffuse[]  = {r, g, b, 0.5};
	GLfloat mat_shininess  = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf (GL_FRONT, GL_SHININESS, mat_shininess);
}

void drawQuadrilateral(Vector3f &v0, Vector3f &v1, Vector3f &v2, Vector3f &v3, GLuint &textureId, float &r, float &g, float &b)
{
	if (textureId != NONE)
	{

		printf("in drawQuads texutureId=%d\n", textureId);

		renderLighting(1, 1, 1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		renderLighting(r, g, b);
		
	}
	Vector3f normal = (v2-v1).Cross(v1-v0);
	glBegin(GL_QUADS);
	if (textureId != NONE) glTexCoord2f(0, 1);
	glNormal3f(normal.x, normal.y, normal.z);
	glVertex3f(v0.x, v0.y, v0.z); 
	if (textureId != NONE) glTexCoord2f(1, 1);
	glNormal3f(normal.x, normal.y, normal.z);
	glVertex3f(v1.x, v1.y, v1.z); 
	if (textureId != NONE) glTexCoord2f(1, 0);
	glNormal3f(normal.x, normal.y, normal.z);
	glVertex3f(v2.x, v2.y, v2.z); 
	if (textureId != NONE) glTexCoord2f(0, 0);
	glNormal3f(normal.x, normal.y, normal.z);
	glVertex3f(v3.x, v3.y, v3.z); 
	glEnd(); 

	if (textureId != NONE) glDisable(GL_TEXTURE_2D);
}

struct TempProperty
{
	float r=1, g=1, b=1,
	      x_offset=0, y_offset=0, z_offset=0,
	      x_length=0, y_length=0, z_length=0,
		  x_rotation=0, y_rotation=0, z_rotation=0,
	      radius=0;
	char type='s';
	map<string,GLuint> textureIdMap;

	void loadTexture(const char *fileName)
	{
		printf("in loadTexture fileName=%s\n", fileName);

		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		fif = FreeImage_GetFileType(fileName, 0);
		if (fif == FIF_UNKNOWN) fif = FreeImage_GetFIFFromFilename(fileName);
		if (fif == FIF_UNKNOWN) throwError("unknown image format");

		printf("fif=%d bmp=%d\n", fif, FIF_BMP);

		FIBITMAP *bitmap = FreeImage_Load(fif, fileName, 0);

		printf("bitmap=%x\n", bitmap);

		int width = FreeImage_GetWidth(bitmap);
		int height = FreeImage_GetHeight(bitmap);

		printf("width=%d height=%d\n", width, height);


		void *imageRawBit = malloc(width*height*3);
		FreeImage_ConvertToRawBits((BYTE*)imageRawBit, bitmap, ((24*width)+23)/24*3, 24, 0xff0000, 0x00ff00, 0x0000ff, false);
		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, (const GLvoid*)imageRawBit);
		textureIdMap[string(fileName)] = textureId;

		printf("textureId=%d\n", textureId);

	}

	inline bool loadedTexture(const char *fileName)
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
			string fileName = "..\\..\\..\\"+string(*(char**)property->value);

			printf("loadProperty_texture fileName=%s\n", fileName.c_str());

			if (!loadedTexture(fileName.c_str())) loadTexture(fileName.c_str());
			value = textureIdMap[fileName.c_str()];
			modified = false;
		}
	}

	inline void loadProperty_rotation(Property *property, float &value)
	{
		if (property->valueForm == 0) value = *(float*)property->value;
		else value = (*(float*)property->value)/M_PI*180;
	}

	void load(Property *property, DOMNode *parentNode, DOMNode *currentNode)
	{
		switch (property->name)
		{
			case Property::x_offset:
				loadProperty_length(property, parentNode->x_length, x_offset);
				break;
			case Property::y_offset:
				loadProperty_length(property, parentNode->y_length, y_offset);
				break;
			case Property::z_offset:
				loadProperty_length(property, parentNode->z_length, z_offset);
				break;
			case Property::x_length:
				loadProperty_length(property, parentNode->x_length, x_length);
				break;
			case Property::y_length:
				loadProperty_length(property, parentNode->y_length, y_length);
				break;
			case Property::z_length:
				loadProperty_length(property, parentNode->z_length, z_length);
				break;
			case Property::x_rotation:
				loadProperty_rotation(property, x_rotation);
			case Property::y_rotation:
				loadProperty_rotation(property, y_rotation);
			case Property::z_rotation:
				loadProperty_rotation(property, z_rotation);
			case Property::radius:
				radius = *(float*)property->value;
				break;
			case Property::color:
				r = *(float*)property->value;
				g = *((float*)property->value+1);
				b = *((float*)property->value+2);
				break;
			case Property::type:
				type = *(char*)property->value;
				break;
			case Property::look_at:

				break;
			case Property::texture:
				loadProperty_texture(property, currentNode->texture_x_positive_modified, currentNode->texture_x_positive);
				loadProperty_texture(property, currentNode->texture_x_negative_modified, currentNode->texture_x_negative);
				loadProperty_texture(property, currentNode->texture_y_positive_modified, currentNode->texture_y_positive);
				loadProperty_texture(property, currentNode->texture_y_negative_modified, currentNode->texture_y_negative);
				loadProperty_texture(property, currentNode->texture_z_positive_modified, currentNode->texture_z_positive);
				loadProperty_texture(property, currentNode->texture_z_negative_modified, currentNode->texture_z_negative);
				break;
			case Property::texture_x_positive:
				loadProperty_texture(property, currentNode->texture_x_positive_modified, currentNode->texture_x_positive);
				break;
			case Property::texture_x_negative:
				loadProperty_texture(property, currentNode->texture_x_negative_modified, currentNode->texture_x_negative);
				break;
			case Property::texture_y_positive:
				loadProperty_texture(property, currentNode->texture_y_positive_modified, currentNode->texture_y_positive);
				break;
			case Property::texture_y_negative:
				loadProperty_texture(property, currentNode->texture_y_negative_modified, currentNode->texture_y_negative);
				break;
			case Property::texture_z_positive:
				loadProperty_texture(property, currentNode->texture_z_positive_modified, currentNode->texture_z_positive);
				break;
			case Property::texture_z_negative:
				loadProperty_texture(property, currentNode->texture_z_negative_modified, currentNode->texture_z_negative);
				break;
		}
	}
};


void drawDOMTree(DOMNode *currentNode, DOMNode *parentNode)
{
	TempProperty tempProperty;

	for (auto i=currentNode->csslist.begin(); i!=currentNode->csslist.end(); i++)
	for (auto property=(*i)->property.begin(); property!=(*i)->property.end(); property++)
	{
//		printf("in css property\n");
//		printf("property no=%d\n", (*property)->name);
		tempProperty.load(*property, parentNode, currentNode);
	}	

	for (auto property=currentNode->property.begin(); property!=currentNode->property.end(); property++)
	{
//		printf("in self property\n");
		tempProperty.load(*property, parentNode, currentNode);
	}

	printf("tagname=%d\n", currentNode->tagName);
	printf("rgb=%f %f %f\n", tempProperty.r, tempProperty.g, tempProperty.b);

	glTranslatef(tempProperty.x_offset, tempProperty.y_offset, tempProperty.z_offset);

	GLfloat light_position[] = {1.5f, -1.0f, 2.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	printf("rotation=%f %f %f\n", tempProperty.x_rotation, tempProperty.y_rotation, tempProperty.z_rotation);

	glRotatef(tempProperty.x_rotation, 1, 0, 0);
	glRotatef(tempProperty.y_rotation, 0, 1, 0);
	glRotatef(tempProperty.z_rotation, 0, 0, 1);

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

//		printf("length=%f %f %f\n", tempProperty.x_length, tempProperty.y_length, tempProperty.z_length);
//		printf("offset=%f %f %f\n", tempProperty.x_offset, tempProperty.y_offset, tempProperty.z_offset);

		currentNode->x_length = tempProperty.x_length;
		currentNode->y_length = tempProperty.y_length;
		currentNode->z_length = tempProperty.z_length;
		currentNode->x_base = parentNode->x_base + tempProperty.x_offset;
		currentNode->y_base = parentNode->y_base + tempProperty.y_offset;
		currentNode->z_base = parentNode->z_base + tempProperty.z_offset;
		
//		printf("base=%f %f %f\n", currentNode->x_base, currentNode->y_base, currentNode->z_base);

		if (tempProperty.type == 'w')
		{
			//glLoadIdentity();
			//gluLookAt(camera.position.x, camera.position.y, camera.position.z, camera.lookAt.x, camera.lookAt.y, camera.lookAt.z, 0, 0, 1);
			//glTranslatef(currentNode->x_base, currentNode->y_base, currentNode->z_base);
			glScalef(tempProperty.x_length, tempProperty.y_length, tempProperty.z_length);
			
			renderLighting(tempProperty.r, tempProperty.g, tempProperty.b);

			glColor3f(1, 1, 1);
			glutWireCube(1);
			glScalef(1/tempProperty.x_length, 1/tempProperty.y_length, 1/tempProperty.z_length);
		}
		else
		if (tempProperty.type == 's')
		{
			Vector3f vertex[8];
			int it = 0;
			for (int fx=-1; fx<=1; fx+=2)
			for (int fy=-1; fy<=1; fy+=2)
			for (int fz=-1; fz<=1; fz+=2) vertex[it++] = Vector3f(tempProperty.x_length/2*fx, tempProperty.y_length/2*fy, tempProperty.z_length/2*fz);

//			renderLighting(tempProperty.r, tempProperty.g, tempProperty.b);

			drawQuadrilateral(vertex[5], vertex[7], vertex[6], vertex[4], currentNode->texture_x_positive, tempProperty.r, tempProperty.g, tempProperty.b);
			drawQuadrilateral(vertex[3], vertex[1], vertex[0], vertex[2], currentNode->texture_x_negative, tempProperty.r, tempProperty.g, tempProperty.b);
			drawQuadrilateral(vertex[7], vertex[3], vertex[2], vertex[6], currentNode->texture_y_positive, tempProperty.r, tempProperty.g, tempProperty.b);
			drawQuadrilateral(vertex[1], vertex[5], vertex[4], vertex[0], currentNode->texture_y_negative, tempProperty.r, tempProperty.g, tempProperty.b);
			drawQuadrilateral(vertex[3], vertex[7], vertex[5], vertex[1], currentNode->texture_z_positive, tempProperty.r, tempProperty.g, tempProperty.b);
			drawQuadrilateral(vertex[0], vertex[4], vertex[6], vertex[2], currentNode->texture_z_negative, tempProperty.r, tempProperty.g, tempProperty.b);
		}
		else throwError("unknown type");

		//glLoadIdentity();
	
		//gluLookAt(camera.position.x, camera.position.y, camera.position.z, camera.lookAt.x, camera.lookAt.y, camera.lookAt.z, 0, 0, 1);
	
		//glTranslatef(currentNode->x_base, currentNode->y_base, currentNode->z_base);
		/*glScalef(tempProperty.x_length, tempProperty.y_length, tempProperty.z_length);

		renderLighting(tempProperty.r, tempProperty.g, tempProperty.b);

		glColor3f(1, 1, 1);

		printf("type=%c\n", tempProperty.type);

		if (tempProperty.type == 's') glutSolidCube(1);
		else
		if (tempProperty.type == 'w') glutWireCube(1);
		else throwError("unknown type");

		glScalef(1/tempProperty.x_length, 1/tempProperty.y_length, 1/tempProperty.z_length);*/
	}
	else
	if (currentNode->tagName == DOMNode::sphere)
	{

		printf("get sphere\n");

		printf("rgb=%f %f %f\n", tempProperty.r, tempProperty.g, tempProperty.b);
		printf("radius=%f\n", tempProperty.radius);

		currentNode->x_length = currentNode->y_length = currentNode->z_length = tempProperty.radius*2;
		currentNode->x_base = parentNode->x_base + tempProperty.x_offset;
		currentNode->y_base = parentNode->y_base + tempProperty.y_offset;
		currentNode->z_base = parentNode->z_base + tempProperty.z_offset;
		//glLoadIdentity();
	
		//gluLookAt(camera.position.x+rotateX, camera.position.y+rotateY, camera.position.z, camera.lookAt.x, camera.lookAt.y, camera.lookAt.z, 0, 0, 1);
	
		//glTranslatef(currentNode->x_base, currentNode->y_base, currentNode->z_base);


		printf("sphere translate %f %f %f\n",currentNode->x_base, currentNode->y_base, currentNode->z_base);

		renderLighting(tempProperty.r, tempProperty.g, tempProperty.b);

		glColor3f(tempProperty.r, tempProperty.g, tempProperty.b);

		if (tempProperty.type == 's') glutSolidSphere(tempProperty.radius, 20, 20);
		else
		if (tempProperty.type == 'w') glutWireSphere(tempProperty.radius, 20, 20);
		else throwError("unknown type");
	}

	for (DOMNode *child=currentNode->firstChild; child!=NULL; child=child->nextSibling)
	{
		drawDOMTree(child, currentNode);
	}

	glRotatef(-tempProperty.z_rotation, 0, 0, 1);
	glRotatef(-tempProperty.y_rotation, 0, 1, 0);
	glRotatef(-tempProperty.x_rotation, 1, 0, 0);

	glTranslatef(-tempProperty.x_offset, -tempProperty.y_offset, -tempProperty.z_offset);
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

	parseCSS(domRoot);
	drawDOMTree(domRoot, nodeStack[0].node);


	checkGL();

	glutSwapBuffers();
//	glutPostRedisplay();
}

void buildTree();
void buildcssList();

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

DOMNode *search_id(DOMNode *currentNode, string &id)
{
	if (currentNode->tdmlid == id) return currentNode;
	DOMNode *re;
	for (DOMNode *child=currentNode->firstChild; child!=NULL; child=child->nextSibling)
	{
		re = search_id(child, id);
		if (re != NULL) return re;
	}
	return NULL;
}

	class Script__
	{
	public:
	
	class Type__;
	
	class Object
	{
	public:
		int num;
		map<string, Type__> t;
		Object() {num=1;}
	};
	
	class Type__
	{
	public:
		int val1;
		double val2;
		string val3;
		Object * val4;

		DOMNode *domNode;  //s

		int now;
		Type__()
		{
			now=1; val1=0; domNode=NULL; //s
		}
		Type__(const int & x)
		{
			now=1; val1=x; domNode=NULL; //s
		}
		Type__(const double & x)
		{
			now=2; val2=x; domNode=NULL; //s
		}
		Type__(const char * x)
		{
			now=3; val3=string(x); domNode=NULL; //s
		}
		Type__(const string & x)
		{
			now=3; val3=x; domNode=NULL; //s
		}
		Type__(Object * x)
		{
			now=4; val4=x; domNode=NULL; //s
		}
		Type__(const Type__ & x)
		{
			now=x.now;
			if (now==1) val1=x.val1;
			if (now==2) val2=x.val2;
			if (now==3) val3=x.val3;
			if (now==4) {val4=x.val4; val4->num++;}
			domNode = x.domNode; //s
		}
		Type__ operator= (const int & x)
		{
			if (now==4){val4->num--; if (val4->num==0)delete val4;}
			now=1; val1=x;
			return *this;
		}
		Type__ operator= (const double & x)
		{
			if (now==4){val4->num--; if (val4->num==0)delete val4;}
			now=2; val2=x;
			return *this;
		}
		Type__ operator= (const char * x)
		{
			if (now==4){val4->num--; if (val4->num==0)delete val4;}
			now=3; val3=string(x);
			return *this;
		}
		Type__ operator= (const string & x)
		{
			if (now==4){val4->num--; if (val4->num==0)delete val4;}
			now=3; val3=x;
			return *this;
		}
		Type__ operator= (Object * x)
		{
			if (now==4){val4->num--; if (val4->num==0&&val4!=x)delete val4;}
			now=4; val4=x;
			val4->num++;
			return *this;
		}
		
		Type__ operator= (const Type__ & x)
		{
			if (now==4){val4->num--; if (val4->num==0&&val4!=x.val4)delete val4;}
			now=x.now;
			if (now==1) val1=x.val1;
			if (now==2) val2=x.val2;
			if (now==3) val3=x.val3;
			if (now==4) {val4=x.val4; val4->num++;}
			domNode = x.domNode;  // s
			return *this;
		}
		Type__ operator+ (const Type__ & x) const
		{
			Type__ tmp;
			if (now==1 && x.now==1) { tmp.now=1; tmp.val1=val1+x.val1; return tmp;}
			if (now==1 && x.now==2) { tmp.now=2; tmp.val2=val1+x.val2; return tmp;}
			if (now==2 && x.now==1) { tmp.now=2; tmp.val2=val2+x.val1; return tmp;}
			if (now==2 && x.now==2) { tmp.now=2; tmp.val2=val2+x.val2; return tmp;}
			if (now==3 && x.now==3) { tmp.now=3; tmp.val3=val3+x.val3; return tmp;}
			tmp.now=1; tmp.val1=0; return tmp;
		}
		Type__ operator- (const Type__ & x) const
		{
			Type__ tmp;
			if (now==1 && x.now==1) { tmp.now=1; tmp.val1=val1-x.val1; return tmp;}
			if (now==1 && x.now==2) { tmp.now=2; tmp.val2=val1-x.val2; return tmp;}
			if (now==2 && x.now==1) { tmp.now=2; tmp.val2=val2-x.val1; return tmp;}
			if (now==2 && x.now==2) { tmp.now=2; tmp.val2=val2-x.val2; return tmp;}
			tmp.now=1; tmp.val1=0; return tmp;
		}
		Type__ operator* (const Type__ & x) const
		{
			Type__ tmp;
			if (now==1 && x.now==1) { tmp.now=1; tmp.val1=val1*x.val1; return tmp;}
			if (now==1 && x.now==2) { tmp.now=2; tmp.val2=val1*x.val2; return tmp;}
			if (now==2 && x.now==1) { tmp.now=2; tmp.val2=val2*x.val1; return tmp;}
			if (now==2 && x.now==2) { tmp.now=2; tmp.val2=val2*x.val2; return tmp;}
			tmp.now=1; tmp.val1=0; return tmp;
		}
		Type__ operator/ (const Type__ & x) const
		{
			Type__ tmp;
			if (now==1 && x.now==1) 
			{ 
				tmp.now=2; tmp.val2=(double)val1/x.val1; 
				if (tmp.val2==int(tmp.val2)) {tmp.now=1;tmp.val1=int(tmp.val2);}
				return tmp;
			}
			if (now==1 && x.now==2) { tmp.now=2; tmp.val2=val1/x.val2; return tmp;}
			if (now==2 && x.now==1) { tmp.now=2; tmp.val2=val2/x.val1; return tmp;}
			if (now==2 && x.now==2) { tmp.now=2; tmp.val2=val2/x.val2; return tmp;}
			tmp.now=1; tmp.val1=0; return tmp;
		}
		Type__ operator& (const Type__ & x) const
		{
			Type__ tmp;
			if (now==1 && x.now==1) { tmp.now=1; tmp.val1=val1&x.val1; return tmp;}
			tmp.now=1; tmp.val1=0; return tmp;
		}
		Type__ operator| (const Type__ & x) const
		{
			Type__ tmp;
			if (now==1 && x.now==1) { tmp.now=1; tmp.val1=val1|x.val1; return tmp;}
			tmp.now=1; tmp.val1=0; return tmp;
		}
		Type__ operator^ (const Type__ & x) const
		{
			Type__ tmp;
			if (now==1 && x.now==1) { tmp.now=1; tmp.val1=val1^x.val1; return tmp;}
			tmp.now=1; tmp.val1=0; return tmp;
		}
		Type__ operator>(const Type__ & x) const
		{
			if (now==1 && x.now==1) return Type__(val1>x.val1?1:0);
			if (now==1 && x.now==2) return Type__(val1>x.val2?1:0);
			if (now==2 && x.now==1) return Type__(val2>x.val1?1:0);
			if (now==2 && x.now==2) return Type__(val2>x.val2?1:0);
			return Type__(0);
		}
		Type__ operator<(const Type__ & x) const
		{
			if (now==1 && x.now==1) return Type__(val1<x.val1?1:0);
			if (now==1 && x.now==2) return Type__(val1<x.val2?1:0);
			if (now==2 && x.now==1) return Type__(val2<x.val1?1:0);
			if (now==2 && x.now==2) return Type__(val2<x.val2?1:0);
			return Type__(0);
		}
		Type__ operator>=(const Type__ & x) const
		{
			if (now==1 && x.now==1) return Type__(val1>=x.val1?1:0);
			if (now==1 && x.now==2) return Type__(val1>=x.val2?1:0);
			if (now==2 && x.now==1) return Type__(val2>=x.val1?1:0);
			if (now==2 && x.now==2) return Type__(val2>=x.val2?1:0);
			return Type__(0);
		}
		Type__ operator<=(const Type__ & x) const
		{
			if (now==1 && x.now==1) return Type__(val1<=x.val1?1:0);
			if (now==1 && x.now==2) return Type__(val1<=x.val2?1:0);
			if (now==2 && x.now==1) return Type__(val2<=x.val1?1:0);
			if (now==2 && x.now==2) return Type__(val2<=x.val2?1:0);
			return Type__(0);
		}
		Type__ operator==(const Type__ & x) const
		{
			if (now==1 && x.now==1) return Type__(val1==x.val1?1:0);
			if (now==1 && x.now==2) return Type__(val1==x.val2?1:0);
			if (now==2 && x.now==1) return Type__(val2==x.val1?1:0);
			if (now==2 && x.now==2) return Type__(val2==x.val2?1:0);
			if (now==3 && x.now==3) return Type__(val3==x.val3?1:0);
			return Type__(0);
		}
		Type__ operator++(int)
		{
			if (now==1) {Type__ tmp(val1); val1++; return tmp;}
			return *this;
		}
		const Type__ & operator++()
		{
			if (now==1) val1++;
			return *this;
		}
		Type__ operator--(int)
		{
			if (now==1) {Type__ tmp(val1); val1--; return tmp;}
			return *this;
		}
		const Type__ & operator--()
		{
			if (now==1) val1--;
			return *this;
		}
		bool getbool() const
		{
			if (now==1&&val1==0 || now==2&&val2==0.0 || now==3&&val3==string(""))
				return false;
			return true;
		}
		Type__ operator&&(const Type__ & x) const
		{
			return Type__( (this->getbool()&&x.getbool())?1:0);
		}
		Type__ operator||(const Type__ & x) const
		{
			return Type__( (this->getbool()||x.getbool())?1:0);
		}
		string Tostring(int x)
		{
			ostringstream ss;
			ss<<x;
			return ss.str();
		}
		string Tostring(double x)
		{
			ostringstream ss;
			ss<<x;
			return ss.str();
		}
		Type__ & operator[](const Type__ & x)
		{
			if (now!=4) return *(new Type__(0));
			if (x.now==1) return val4->t[Tostring(x.val1)];
			if (x.now==2) return val4->t[Tostring(x.val2)];
			if (x.now==3) return val4->t[x.val3];
			return *(new Type__(0));
		}
		Type__ & operator[](int x)
		{
			if (now!=4) return *(new Type__(0));
			return val4->t[Tostring(x)];
		}
		


	};

	// zzk xie de
	
	Type__ getElementById(Type__ id)
	{
		DOMNode *element = search_id(domRoot, id.val3);
		if (element == NULL) return 0;
		Type__ re(new Object());
		re.domNode = element;
		return re;
	}

	void modify(Type__ element, Type__ propertyName, Type__ propertyValue)
	{
		if (!element.domNode) throwError("not a DOM object");
		int i;
		for (i=0; i<NPROPERTYNAME; ++i)
		if (propertyNameString[i] == propertyName.val3) break;
		if (i == NPROPERTYNAME) throwError("unknown property name");
		Property *property = new Property(i);
//		char *propertyValueChar = (char*)malloc((propwertValue.length()+1)*sizeof(char));
//		memcpy(propertyValueChar, propertyValue.val3.c_str(), (propwertValue.length()+1)*sizeof(char));
		switch ((Property::Name)i)
		{
			case Property::x_offset:
			case Property::y_offset:
			case Property::z_offset:
			case Property::x_length:
			case Property::y_length:
			case Property::z_length:
				if (propertyValue.val3[propertyValue.val3.length()] != '%')
				{
					property->valueForm = 0;
					property->value = malloc(sizeof(float));
					sscanf(propertyValue.val3.c_str(), "%f", (float*)property->value);
				}
				else
				{
					property->valueForm = 1;
					property->value = malloc(sizeof(float));
					sscanf(propertyValue.val3.c_str(), "%f", (float*)property->value);
				}
				break;
			case Property::radius:
				property->valueForm = 0;
				property->value = malloc(sizeof(float));
				sscanf(propertyValue.val3.c_str(), "%f", (float*)property->value);
				break;
			case Property::color:
				property->valueForm = 0;
				property->value = malloc(sizeof(float)*3);
				sscanf(propertyValue.val3.c_str(), "rgb(%f,%f,%f)", (float*)property->value, (float*)property->value+1, (float*)property->value+2);
				break;
			case Property::type:
				property->valueForm = 0;
				property->value = malloc(sizeof(char));
				char tp = '?';
				if (propertyValue.val3 == "solid") tp = 's';
				else
				if (propertyValue.val3 == "wire") tp = 'w';
				*(char*)property->value = tp;
				break;
			case Property::tdmlclass:
			case Property::tdmlid:
			case Property::texture:
			case Property::texture_x_positive:
			case Property::texture_x_negative:
			case Property::texture_y_positive:
			case Property::texture_y_negative:
			case Property::texture_z_positive:
			case Property::texture_z_negative:
				property->valueForm = 0;
				property->value = malloc(sizeof(char*));
				*((char**)(property->value)) = (char*)malloc((propertyValue.val3.length()+1)*sizeof(char));
				strcpy(*((char**)(property->value)), propertyValue.val3.c_str());
				break;
			case Property::x_rotation:
			case Property::y_rotation:
			case Property::z_rotation:
				1+1==2;
				int len = propertyValue.val3.length();
				if (!strcmp(propertyValue.val3.c_str()+len-3,"deg"))
				{
					property->valueForm = 0;
					property->value = malloc(sizeof(float));
					sscanf(propertyValue.val3.c_str(), "%f", (float*)property->value);
				}
				else
				{
					property->valueForm = 1;
					property->value = malloc(sizeof(float));
					sscanf(propertyValue.val3.c_str(), "%f", (float*)property->value);
				}
				break;
		}
		
		element.domNode->property.push_back(property);
	}

	// zzk xie de end
#define TDML_SCRIPT

	Type__ cal(Type__ a123,Type__ b123b,Type__ c) {
		Type__ a = new Object();
		a[1] = 10;
		a[a[1]] = a[1];
		Type__ b=new Object();
		b[1]=new Object();
		b[1]["c"]=new Object();
		b[1]["c"]["d"]=a123++;
		
		Type__ x = getElementById("id");
		


		//modify(x, "x-length", y);

		

		return a[10]+b[1]["c"]["d"];
	}
	
	Type__ mainScript() {
		return cal(1, 2, 5);
	}
	
#define TDML_SCRIPT_END	
	
	};


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
	buildcssList(); 

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
		camera.position = {0, -11, 5};
		camera.lookAt = {0, 0, 0};
	}

//	camera.position.y *= -1;
	
	glutMainLoop();

	return 0;
}

void buildcssList()
{
	find_CLASS_ID(domRoot);
	Property * property;
	cssList * csslist;
	Select * select;
#define TDML_BUILDCSSLIST
	csslist = new cssList();
	csshd.push_back(csslist);
	select = new Select();
	select->a.push_back(string(".heihei"));
	select->now = select->a.begin();
	csslist->select.push_back(select);
	property = new Property(11);
	property->valueForm = 0;
	property->value = malloc(sizeof(float)*3);
	*(float*)property->value = 1.000000;
	*((float*)property->value+1) = 0.000000;
	*((float*)property->value+2) = 0.000000;
	csslist->property.push_back(property);
	
	csslist = new cssList();
	csshd.push_back(csslist);
	select = new Select();
	select->a.push_back(string(".heihei"));
	select->a.push_back(string("6"));
	select->now = select->a.begin();
	csslist->select.push_back(select);
	property = new Property(11);
	property->valueForm = 0;
	property->value = malloc(sizeof(float)*3);
	*(float*)property->value = 0.000000;
	*((float*)property->value+1) = 1.000000;
	*((float*)property->value+2) = 1.000000;
	csslist->property.push_back(property);
	
	
#define TDML_BUILDCSSLIST_END
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
	nodeStack[++nNodeStack].node = new DOMNode(5);
	nodeStack[nNodeStack].lastChild = NULL;
	property = new Property(7);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 5;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(6);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 5;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(5);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 7;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(10);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 45;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(13);
	property->valueForm = 0;
	property->value = malloc(sizeof(char));
	*(char*)property->value = 'w';
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(0);
	property->valueForm = 0;
	property->value = malloc(sizeof(char*));
	*((char**)(property->value)) = "heihei";
	nodeStack[nNodeStack].node->property.push_back(property);
	nodeStack[nNodeStack-1].node->firstChild = nodeStack[nNodeStack].node;
	nodeStack[nNodeStack-1].lastChild = nodeStack[nNodeStack].node;
	nodeStack[++nNodeStack].node = new DOMNode(5);
	nodeStack[nNodeStack].lastChild = NULL;
	property = new Property(11);
	property->valueForm = 0;
	property->value = malloc(sizeof(float)*3);
	*(float*)property->value = 0.000000;
	*((float*)property->value+1) = 1.000000;
	*((float*)property->value+2) = 0.000000;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(7);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 2;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(6);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 2;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(5);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 2;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(2);
	property->valueForm = 1;
	property->value = malloc(sizeof(float));
	*(float*)property->value = -20;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(3);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 5;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(4);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 1;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(10);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = -45;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(15);
	property->valueForm = 0;
	property->value = malloc(sizeof(char*));
	*((char**)(property->value)) = "sun.bmp";
	nodeStack[nNodeStack].node->property.push_back(property);
	nodeStack[nNodeStack-1].node->firstChild = nodeStack[nNodeStack].node;
	nodeStack[nNodeStack-1].lastChild = nodeStack[nNodeStack].node;
	nNodeStack--;
	nodeStack[++nNodeStack].node = new DOMNode(6);
	nodeStack[nNodeStack].lastChild = NULL;
	property = new Property(13);
	property->valueForm = 0;
	property->value = malloc(sizeof(char));
	*(char*)property->value = 'w';
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(12);
	property->valueForm = 0;
	property->value = malloc(sizeof(float));
	*(float*)property->value = 2;
	nodeStack[nNodeStack].node->property.push_back(property);
	property = new Property(2);
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


