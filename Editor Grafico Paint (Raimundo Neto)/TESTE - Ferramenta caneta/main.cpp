//Autor: Raimundo Nonato Gomes Neto

// Bibliotecas utilizadas pelo OpenGL
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <iostream>
#include <vector>

using namespace std;

struct Point
{
	int x;
	int y;
};

int width = 600;
int height = 600;

vector<vector<Point> > lines;
bool isMousePressed = false;


void InitWindow();
void display();
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void toolPen(int x, int y);
void SetColor();

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	InitWindow();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(toolPen);
	glutMainLoop();
	return 0;
}

void InitWindow()
{
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Teste: Ferramenta Caneta");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

void keyboard(unsigned char key, int x, int y)
{

}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (!isMousePressed)
		{
			vector<Point> newLine;
			lines.push_back(newLine);
		}
		isMousePressed = true;
		Point point = {x, height - y};
		lines.back().push_back(point);
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		isMousePressed = false;
	}
}

void toolPen(int x, int y)
{
	if (isMousePressed)
	{
		Point point = {x, height - y};
		lines.back().push_back(point);

		glClear(GL_COLOR_BUFFER_BIT);
		for (size_t i = 0; i < lines.size(); ++i)
		{
			if (lines[i].size() >= 2)
			{
				SetColor();
				glBegin(GL_LINE_STRIP);
				for (size_t j = 0; j < lines[i].size(); ++j)
				{
					glVertex2i(lines[i][j].x, height - lines[i][j].y);
				}
				glEnd();
			}
		}
		glFlush();
	}
}

// Define as cores da ferramente Caneta
void SetColor()
{

// Preto
	//glColor3ub(0, 0, 0);

// Vermelho puro
	//glColor3ub(255, 0, 0);

// Verde puro
	glColor3ub(0, 255, 0);

// Azul puro
	//glColor3ub(0, 0, 255);

// Amarelo
	//glColor3ub(255, 255, 0);

// Ciano
	//glColor3ub(0, 255, 255);

// Magenta
	//glColor3ub(255, 0, 255);

// Marrom
	//glColor3ub(139, 69, 19);

// Cinza
	//glColor3ub(128, 128, 128);

}
