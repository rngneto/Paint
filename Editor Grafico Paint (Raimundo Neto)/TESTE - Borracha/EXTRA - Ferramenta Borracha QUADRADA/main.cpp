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
#include <algorithm>
#include <cmath>

// Definindo uma estrutura para representar um ponto
struct Point
{
	int x, y;
	Point(int _x, int _y) : x(_x), y(_y) {}
};

bool eraserMode = false; // Indica se a borracha est� ativa ou n�o
float brushSize = 20.0f; // Tamanho da borracha

// Fun��o para desenhar um pixel na coordenada (x, y)
void drawPixel(int x, int y)
{
	if (eraserMode)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 0.0f); // Cor transparente para apagar

	}
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

// Fun��o para preencher um pol�gono com cor
void fillPolygon(std::vector<Point>& vertices)
{
	// Encontra as coordenadas y m�nimas e m�ximas
	int minY = vertices[0].y;
	int maxY = vertices[0].y;
	for (size_t i = 1; i < vertices.size(); ++i)
	{
		if (vertices[i].y < minY) minY = vertices[i].y;
		if (vertices[i].y > maxY) maxY = vertices[i].y;
	}

	// Inicializa uma lista para armazenar as bordas do pol�gono
	std::vector<std::vector<int> > edges(maxY - minY + 1);

	// Encontra as interse��es de cada aresta com as linhas horizontais e armazena as bordas
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		const Point& v1 = vertices[i];
		const Point& v2 = vertices[(i + 1) % vertices.size()];  // Pr�ximo v�rtice
		// Calcula a diferen�a entre as coordenadas y
		int dy = v2.y - v1.y;
		if (dy != 0)
		{
			// Calcula a inclina��o da aresta
			float m = static_cast<float>(v2.x - v1.x) / dy;
			int y = std::min(v1.y, v2.y);
			while (y < std::max(v1.y, v2.y))
			{
				edges[y - minY].push_back(v1.x + m * (y - v1.y));
				++y;
			}
		}
	}

	// Preenche os pixels entre as bordas do pol�gono para cada linha horizontal
	for (size_t i = 0; i < edges.size(); ++i)
	{
		std::sort(edges[i].begin(), edges[i].end());
		for (size_t j = 0; j < edges[i].size(); j += 2)
		{
			for (int x = edges[i][j]; x <= edges[i][j + 1]; ++x)
			{
				drawPixel(x, i + minY);
			}
		}
	}
}

// Fun��o para desenhar e preencher as formas geom�tricas
void drawAndFillShapes()
{
	// Tri�ngulo
	std::vector<Point> triangleVertices;
	triangleVertices.push_back(Point(150, 150));
	triangleVertices.push_back(Point(250, 250));
	triangleVertices.push_back(Point(50, 250));
	glColor3f(1.0f, 0.0f, 0.0f); // Cor vermelha
	fillPolygon(triangleVertices);

	// Quadrado
	std::vector<Point> squareVertices;
	squareVertices.push_back(Point(300, 150));
	squareVertices.push_back(Point(400, 150));
	squareVertices.push_back(Point(400, 250));
	squareVertices.push_back(Point(300, 250));
	glColor3f(0.0f, 1.0f, 0.0f); // Cor verde
	fillPolygon(squareVertices);

	// Hex�gono
	std::vector<Point> hexagonVertices;
	hexagonVertices.push_back(Point(450, 150));
	hexagonVertices.push_back(Point(550, 200));
	hexagonVertices.push_back(Point(650, 150));
	hexagonVertices.push_back(Point(650, 250));
	hexagonVertices.push_back(Point(550, 300));
	hexagonVertices.push_back(Point(450, 250));
	glColor3f(0.0f, 0.0f, 1.0f); // Cor azul
	fillPolygon(hexagonVertices);
}

// Fun��o de desenho
void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawAndFillShapes();
	glFlush();
}

// Fun��o de inicializa��o
void init()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // Cor de fundo branca
	gluOrtho2D(0, 800, 0, 600);  // Configura��o da janela de visualiza��o
}

// Fun��o para lidar com eventos de mouse
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// Troca o modo da borracha quando o bot�o esquerdo � pressionado
		eraserMode = !eraserMode;
	}
}

// Fun��o para lidar com movimentos do mouse
void borracha(int x, int y)
{
	// Desenha ou apaga pixels conforme a posi��o do mouse e o modo da borracha
	int xCenter = x;
	int yCenter = glutGet(GLUT_WINDOW_HEIGHT) - y; // Inverte o eixo y
	for (int i = -brushSize / 2; i <= brushSize / 2; ++i)
	{
		for (int j = -brushSize / 2; j <= brushSize / 2; ++j)
		{
			int xPixel = xCenter + i;
			int yPixel = yCenter + j;
			if (xPixel >= 0 && xPixel < glutGet(GLUT_WINDOW_WIDTH) && yPixel >= 0 && yPixel < glutGet(GLUT_WINDOW_HEIGHT))
			{
				drawPixel(xPixel, yPixel);
			}
		}
	}
	glFlush();
}

// Fun��o principal
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Formas Geom�tricas");
	init();
	glutDisplayFunc(draw);
	glutMouseFunc(mouse);
	glutMotionFunc(borracha);
	glutMainLoop();
	return 0;
}
