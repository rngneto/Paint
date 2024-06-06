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

#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Variáveis globais para armazenar as dimensões da janela
int windowWidth = 800;
int windowHeight = 600;
int contadorImagens = 0;

// Função para capturar a cena e salvar como imagem
void capturarCenaComoImagem()
{
	std::vector<unsigned char> pixels(windowWidth * windowHeight * 3);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

	// Inverte os pixels verticalmente
	std::vector<unsigned char> flippedPixels(windowWidth * windowHeight * 3);
	for (int y = 0; y < windowHeight; ++y)
	{
		for (int x = 0; x < windowWidth; ++x)
		{
			flippedPixels[((windowHeight - 1 - y) * windowWidth + x) * 3 + 0] = pixels[(y * windowWidth + x) * 3 + 0];
			flippedPixels[((windowHeight - 1 - y) * windowWidth + x) * 3 + 1] = pixels[(y * windowWidth + x) * 3 + 1];
			flippedPixels[((windowHeight - 1 - y) * windowWidth + x) * 3 + 2] = pixels[(y * windowWidth + x) * 3 + 2];
		}
	}

	// Gera o nome do arquivo
	std::stringstream ss;
	ss << "cena" << contadorImagens << ".png";
	std::string nomeArquivo = ss.str();
	contadorImagens++;

	// Salva os pixels como uma imagem usando a biblioteca STB Image Write
	stbi_write_png(nomeArquivo.c_str(), windowWidth, windowHeight, 3, flippedPixels.data(), windowWidth * 3);

	std::cout << "Tela capturada! Salvando como imagem: " << nomeArquivo << std::endl;
}


void renderScene()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
}

// Função de teclado para capturar a cena como imagem
void teclado(unsigned char key, int x, int y)
{
	if (key == 's' || key == 'S')  // 'S' ou 's'
	{
		capturarCenaComoImagem();
	}
}

void init()
{

	// Cor de fundo padrão: branco
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Cinza claro
	//glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

	// Azul escuro
	//glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

	// Verde claro
	//glClearColor(0.5f, 1.0f, 0.5f, 1.0f);

	// Vermelho escuro
	//glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
}

// Função principal
int main(int argc, char** argv)
{
	// Inicializa o GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("Teste: salvar tela como imagem");
	init();
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(teclado);
	glutMainLoop();
	return 0;
}
