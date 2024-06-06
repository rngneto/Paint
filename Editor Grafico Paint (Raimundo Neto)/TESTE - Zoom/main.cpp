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

float zoomFactor = 1.0f; // Fator de zoom inicial

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Ajusta a escala de acordo com o fator de zoom
	gluOrtho2D(-50.0 * zoomFactor, 50.0 * zoomFactor, -50.0 * zoomFactor, 50.0 * zoomFactor);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Desenha um triângulo
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f); // Cor vermelha
	glVertex2f(0.0f, 20.0f); // Vértice superior
	glVertex2f(-20.0f, -20.0f); // Vértice inferior esquerdo
	glVertex2f(20.0f, -20.0f); // Vértice inferior direito
	glEnd();

	glutSwapBuffers();
}


void specialKeys(int key, int x, int y)
{
	switch (key)// Usa seta pra cima e seta para baixo para controlar o zoom na janela
	{
	case GLUT_KEY_UP:
		zoomFactor += 0.1f; // Aumenta o zoom
		break;
	case GLUT_KEY_DOWN:
		if (zoomFactor > 0.1f)
			zoomFactor -= 0.1f; // Diminui o zoom
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Zoom em OpenGL");
	glutDisplayFunc(display);
	glutSpecialFunc(specialKeys);
	glutMainLoop();
	return 0;
}
