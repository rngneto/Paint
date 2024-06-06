/*
 * Computacao Grafica - Departamento de Computação - UFPI
 * Código: Editor Grafico em C/C++, estilo Paint
 * Autor: Raimundo Nonato Gomes Neto
 * Baseado no codigo exemplo: Rasterizacao de Segmentos de Reta com GLUT/OpenGL
 * Autor do codigo exemplo: Prof. Laurindo de Sousa Britto Neto
 */

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

#include <algorithm>
#include <cmath>
#include <array>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <forward_list>
#include <iostream>
#include <list>
#include <utility>
#include <stdexcept>
#include <fstream>
#include "glut_text.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Definição das cores contidas na paleta RGB
GLfloat CORES[][3] =
{
	{1.0, 0.0, 0.0},   // Vermelho
	{0.0, 1.0, 0.0},   // Verde
	{0.0, 0.0, 1.0},   // Azul
		
	{1.0, 1.0, 0.0},   // Amarelo
	{1.0, 0.08, 0.58}, // Rosa Profundo
	{0.0, 1.0, 1.0},   // Ciano
		
	{0.0, 0.0, 0.0},   // Preto
	{0.5, 0.5, 0.5},   // Cinza
	{1.0, 1.0, 1.0},   // Branco
		
	{0.9, 0.0, 0.3},   // Rosa Claro
	{0.75, 1.0, 0.0},  // Lima
	{0.1, 0.1, 0.44},  // Azul Meia-Noite
	
	{0.9, 0.9, 0.0},   // Amarelo Claro
	{1.0, 0.0, 0.5},   // Rosa Brilhante
	{0.53, 0.81, 0.98},// Azul Céu Claro
	
	{0.7, 0.13, 0.13}, // Tijolo refratário 
	{0.96, 0.77, 0.19},// Açafrão	
	{1.0, 0.98, 0.8},  // Seda
		
	
};

// Identificadores das janelas
int window1, window2;

// Largura e altura da primeira janela
int width = 700, height = 600;

// Largura e altura da segunda janela
int width2 = 300, height2 = 600;

using namespace std;

// Variaveis Globais
#define ESC 27
#define SPACE 32

// Formas geometricas
const int LIN = 1; // Linha
const int RET = 2; // Retangulo
const int TRI = 3; // Triangulo
const int POL = 4; // Poligono
const int CIR = 5; // Circulo

// Algoritmo de preenchimento FloodFill
const int FF = 6;

// Transformações Geométricas
const int TRANSLACAO = 7;
const int ESCALA = 8;
const int CISALHAMENTO = 9;
const int ROTACAO = 10;
const int REFLEXAO = 11;

// Identificadores para formas preenchidas
const int TRI_FILLED = 12; // Triangulo
const int QUA_FILLED = 13; //Quadrilatero
const int POL_FILLED = 14; // Poligono

// Ferramenta Lapis
const int PEN = 15;

// Permite capturar o estado atual da janela, salvando em um arquivo de imagem .png
const int PRINT = 16;

#define NENHUMA_TRANSFORMACAO 0

// Declaração global da variável transformacaoAtual
int transformacaoAtual = NENHUMA_TRANSFORMACAO;

// Variável global para armazenar o valor do submenu
int currentSubmenuValue = 0;

int x1, y1, x2, y2, x3, y3; // Para armazenar as coordenadas dos vértices do triângulo
vector<int> x_vector, y_vector; // Para armazenar as coordenadas dos vértices do polígono

// Verifica se R foi teclado (Cor vermelha)
bool rTeclado = false;

// Verifica se G foi teclado (Cor verde)
bool gTeclado = false;

// Verifica se B foi teclado (Cor Azul)
bool bTeclado = false;

// Verifica se foi realizado o primeiro clique do mouse
bool click1 = false;

// Verifica se foi realizado o primeiro clique do mouse para a Lapis
bool clickPen = false;

// Verifica se foi realizado o primeiro clique do mouse para a circunferencia
bool clickCir1 = false;

// Verifica se a funcao desenhar um novo poligono é verdadeiro
bool drawPol = false;

// Verifica se foi realizado o primeiro e segundo clique do mouse para desenhar um triangulo
bool drawTri1 = false, drawTri2 = false;

// Verifica se foi realizado o primeiro clique do mouse para desenhar um quadrilatero
bool drawRet = false;

// Verifica se o mouse está pressionado, uso em toolPen
bool isMousePressed = false;

//Para funções de preenchimento
bool fillTriangulo = false;
bool fillQuadrilatero = false;
bool fillPoligono = false;

// Coordenadas da posicao atual do mouse
int m_x, m_y;

// Coordenadas do primeiro clique e do segundo clique do mouse
int x_1, y_1, x_2, y_2, x_3, y_3;

// Define a cor a utilizada
float fillR = 0, fillG = 0, fillB = 0; //Lógica utilizada denttro do FloodFill

// Indica o tipo de forma geometrica ativa para desenhar
int modo = -1;

//Para função de captura de janela
int contadorImagens = 0;

// Definicao de vertice
struct vertice
{
	int x;
	int y;
};

// Definicao das formas geometricas
struct forma
{
	int tipo;
	float cx, cy;
	forward_list<vertice> v; // Lista encadeada de vertices
	int translacaoX;
	int translacaoY;
	forma() : translacaoX(0), translacaoY(0) {}
};

// Para Lapis
struct Point
{
	int x;
	int y;
	Point(int x, int y) : x(x), y(y) {}
};
vector<vector<Point> > pen; //Lapis

// Lista encadeada de formas geometricas
forward_list<forma> formas;
vector<vertice> color_pts;

// Declaração do vetor para armazenar os pontos coloridos
std::vector<vertice> colored_vertices;

// Variáveis para armazenar a cor capturada
GLfloat corCapturada[3] = {0.0, 0.0, 0.0}; // Inicialmente Preto

// Atualiza o centroide da forma geométrica
void updateCentroide(forma& f)
{
	if (f.v.empty()) return;
	float cx = 0, cy = 0; // centro da forma
	for (auto it = f.v.begin(); it != f.v.end(); ++it)
	{
		vertice& v = *it;
		cx += v.x;
		cy += v.y;
	}
	int size = distance(f.v.begin(), f.v.end());
	cx /= size;
	cy /= size;
	f.cx = cx;
	f.cy = cy;
}

// Função para armazenar formas
void pushForma(int tipo)
{
	forma f;
	f.tipo = tipo;
	f.cx = 0;
	f.cy = 0;
	formas.push_front(f);
}

// Função para armazenar vertices
void pushVertice(int x, int y)
{
	vertice v;
	v.x = x;
	v.y = y;
	formas.front().v.push_front(v);
	updateCentroide(formas.front());
}

// Função para armazenar uma Linha
void pushLinha(int x1, int y1, int x2, int y2)
{
	pushForma(LIN);
	pushVertice(x1, y1);
	pushVertice(x2, y2);
}

// Função para armazenar um Quadrilatero
void pushRet(int x1, int y1, int x2, int y2)
{
	pushForma(RET);
	pushVertice(x1, y1);
	pushVertice(x2, y1);
	pushVertice(x2, y2);
	pushVertice(x1, y2);
}

// Função para armazenar um Triangulo
void pushTriangulo(int x1, int y1, int x2, int y2, int x3, int y3)
{
	pushForma(TRI);
	pushVertice(x1, y1);
	pushVertice(x2, y2);
	pushVertice(x3, y3);
}

// Função para armazenar um Poligono
void pushPoligono(std::vector<vertice> &pts)
{
	pushForma(POL);
	for (std::vector<vertice>::iterator v = pts.begin(); v != pts.end(); ++v)
	{
		pushVertice(v->x, v->y);
	}
}

// Função para armazenar um Circulo
void pushCirculo(int x1, int y1, int x2, int y2)
{
	pushForma(CIR);
	pushVertice(x1, y1);
	pushVertice(x2, y2);
}

// Função para armazenar o preenchimento do o algoritmo Flood Fill
void pushFloodFill(int x1, int y1)
{
	vertice v;
	v.x = x1;
	v.y = y1;
	color_pts.push_back(v);
}

// Função para armazenar o algoritmo de preenchimento de polígonos
void pushFillPolygon(const std::vector<Point>& vertices)
{
	// Percorre todos os vértices do polígono e adiciona-os ao vetor colored_vertices
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		const Point& vertex = vertices[i];
		vertice converted_vertex = {vertex.x, vertex.y}; // Converte o Point para vertice
		colored_vertices.push_back(converted_vertex); // Adiciona o vértice convertido para o vetor colored_vertices
	}
}

/*
 Declaracoes antecipadas (forward) das funcoes (assinaturas das funcoes)
 */

void init(void);
void reshape(int w, int h);
void display(void);
void menu_popup(int value);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void mousePassiveMotion(int x, int y);
void drawPixel(int x, int y);
void drawFormas();

// Funções de desenho
void drawBresenham(int x1, int y1, int x2, int y2);
void drawQuadrilatero(int x1, int y1, int x2, int y2);
void drawTriangulo(int x1, int y1, int x2, int y2, int x3, int y3);
void drawPoligono(std::vector<int>& x, std::vector<int>& y);
void drawCirculo(int x1, int y1, int r);
void updateCentroide(forma& f);

// Funções de preenchimento de formas, Algoritmo FloodFill e FillPolygon
void floodFill(int x, int y);
void fillPolygon(std::vector<Point>& vertices);
void pushFillPolygon(const std::vector<Point>& vertices);
void desenhaPreencheTriangulo();
void desenhaPreencheQuadrilatero();
void desenhaPreenchePoligono();

// Funcões que realizam transformações nas formas geométricas
void translacao(int tx, int ty, forma& f);
void escala(float sx, float sy, forma& f);
void cisalhamento(float shx, float shy, forma& f);
void rotacao(float theta, forma& f);
void reflexao(char eixo, forma& f);

// Ferramenta Lapis
void Lapis(int x, int y);
void SetColor(); // Defini a cor. Utiliza a função contaGotas() para determinar a cor

/*
 * Implementação de Paleta de cores RGB
 */
void desenhaPaleta()
{
	int num_cores = sizeof(CORES) / sizeof(CORES[0]);
	int num_linhas = 6;
	int num_colunas = 3;

	// Tamanho fixo para as células da grade
	GLfloat tamanho_celula = 100.0;

	int indice_cor = 0;

	for (int linha = 0; linha < num_linhas; linha++)
	{
		for (int coluna = 0; coluna < num_colunas; coluna++)
		{
			if (indice_cor < num_cores)
			{
				// Calcula as coordenadas da célula da grade
				GLfloat x1 = coluna * tamanho_celula;
				GLfloat y1 = height2 - (linha + 1) * tamanho_celula;
				GLfloat x2 = (coluna + 1) * tamanho_celula;
				GLfloat y2 = height2 - linha * tamanho_celula; 
				
				glBegin(GL_QUADS);
				glColor3fv(CORES[indice_cor]);
				glVertex2f(x1, y1);
				glVertex2f(x2, y1);
				glVertex2f(x2, y2);
				glVertex2f(x1, y2);
				glEnd();

				indice_cor++;
			}
		}
	}
}

/*
 *Função para capturar a cor em determinada área e armazenar na variável global corCapturada
 */
void contaGotas(int x, int y)
{
	unsigned int indice_coluna = x / (width2 / 3);
	unsigned int indice_linha = y / (height2 / 6);
	size_t indice_cor = (size_t)(indice_linha * 3 + indice_coluna);

	if (indice_cor < sizeof(CORES) / sizeof(CORES[0]))
	{
		printf("Cor selecionada: (%.1f, %.1f, %.1f)\n", CORES[indice_cor][0], CORES[indice_cor][1], CORES[indice_cor][2]);
		corCapturada[0] = CORES[indice_cor][0];
		corCapturada[1] = CORES[indice_cor][1];
		corCapturada[2] = CORES[indice_cor][2];

		// Atualiza a primeira janela imediatamente após capturar a cor
		glutSetWindow(window1);
		glutPostRedisplay();
		// Volta para a segunda janela
		glutSetWindow(window2);
	}
}

/*
 * Controla os desenhos na segunda janela
 */
void displaySecondWindow()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	desenhaPaleta();
	glutSwapBuffers();
}

/*
 * Ajusta a projeção para o redesenho da segunda janela
 */
void reshapeSecondWindow(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, 1);
	glMatrixMode(GL_MODELVIEW);
}

// Função de callback de mouse para a segunda janela
void mouseSecondWindow(int botao, int estado, int x, int y)
{
	if (botao == GLUT_LEFT_BUTTON && estado == GLUT_DOWN)
	{
		contaGotas(x, y);
	}
}

// Submenu para transformações geométricas
void submenu_transformacoes(int value)
{
	switch (value)
	{
	case TRANSLACAO:
		transformacaoAtual = TRANSLACAO;
		break;
	case ESCALA:
		transformacaoAtual = ESCALA;
		break;
	case CISALHAMENTO:
		transformacaoAtual = CISALHAMENTO;
		break;
	case ROTACAO:
		transformacaoAtual = ROTACAO;
		break;
	case REFLEXAO:
		transformacaoAtual = REFLEXAO;
		break;
	default:
		break;
	}
}

// Submenu para poligonos preenchidos
void submenu_poligonos_preenchidos(int id)
{
	// Reinicia todas as variáveis de controle para desenhar formas
	fillTriangulo = false;
	fillQuadrilatero = false;
	fillPoligono = false;

	switch (id)
	{
	case TRI_FILLED: // Triângulo preenchido
		fillTriangulo = true;
		break;

	case QUA_FILLED: // Quadrilátero preenchido
		fillQuadrilatero = true;
		break;

	case POL_FILLED: // Polígono preenchido
		fillPoligono = true;
		break;

	default:
		break;
	}
}

/*
 * Função Principal
 */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);// Passagens de parametro C para o glut

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);//Selecao do Modo do Display e do Sistema de cor
	glutInitWindowSize(width, height);// Tamanho da primeira janela do OpenGL
	glutInitWindowPosition(50, 100);//Posicao inicial da primeira janela do OpenGL
	window1 = glutCreateWindow("Computacao Grafica: Paint");// Da nome para uma janela OpenGL

	// Registra as funções de callback para a primeira janela
	init();// Chama funcao init();
	glutReshapeFunc(reshape);// Função callback para redesenhar a tela
	glutKeyboardFunc(keyboard);// Função callback do teclado
	glutSpecialFunc(specialKeys);// Função callback do teclado (teclas especiais)
	glutMouseFunc(mouse);// Função callback do mouse
	glutMotionFunc(Lapis);// Função callback da Lapis
	glutPassiveMotionFunc(mousePassiveMotion);// Função callback do movimento passivo do mouse
	glutDisplayFunc(display);// Função callback de desenho

	// Cria submenus para as transformações
	int submenu_transformacoes_id = glutCreateMenu(submenu_transformacoes);
	glutAddMenuEntry("Translação", TRANSLACAO);
	glutAddMenuEntry("Escala", ESCALA);
	glutAddMenuEntry("Cisalhamento", CISALHAMENTO);
	glutAddMenuEntry("Reflexão", REFLEXAO);
	glutAddMenuEntry("Rotação", ROTACAO);

	// Cria submenus para polígonos preenchidos
	int submenu_poligonos_preenchidos_id = glutCreateMenu(submenu_poligonos_preenchidos);
	glutAddMenuEntry("Triangulo", TRI_FILLED);
	glutAddMenuEntry("Quadrilatero", QUA_FILLED);
	glutAddMenuEntry("Poligono", POL_FILLED);

	// Cria menu principal
	glutCreateMenu(menu_popup);
	glutAddMenuEntry("Forma: Linha", LIN);
	glutAddMenuEntry("Forma: Quadrilatero", RET);
	glutAddMenuEntry("Forma: Triangulo", TRI);
	glutAddMenuEntry("Forma: Poligono", POL);
	glutAddMenuEntry("Forma: Circunferencia", CIR);
	glutAddSubMenu("Transformações Geometricas", submenu_transformacoes_id);
	glutAddMenuEntry("Preenchimento: Flood Fill", FF);
	glutAddSubMenu("Poligonos preenchidos", submenu_poligonos_preenchidos_id);	
	glutAddMenuEntry("Lápis", PEN);
	glutAddMenuEntry("Salvar como imagem (S)", PRINT);
	glutAddMenuEntry("Limpar tela (C)", -1);
	glutAddMenuEntry("Fechar janela (ESC)", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Cria a segunda janela
	glutInitWindowSize(width2, height2); // Tamanho da segunda janela do OpenGL
	glutInitWindowPosition(760, 100); // Posicao inicial da segunda janela do OpenGL
	window2 = glutCreateWindow("Paleta de Cores RGB"); //Nome da segunda janela do GLUT
	glClearColor(0, 0, 0, 1); // Cor de fundo da segunda janela (branco)

	// Registra as funções de callback para a segunda janela
	glutDisplayFunc(displaySecondWindow);
	glutReshapeFunc(reshapeSecondWindow);
	glutMouseFunc(mouseSecondWindow);

	glutMainLoop();

	return EXIT_SUCCESS;
}

/*
 * Inicializa alguns parametros do GLUT
 */
void init(void)
{
	// Cor de fundo padrão: branco
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Cinza claro
	//glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

	// Azul escuro
	//glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

	// Verde claro
	glClearColor(0.5f, 1.0f, 0.5f, 1.0f);

	// Vermelho escuro
	//glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
}

/*
 * Ajusta a projecao para o redesenho da janela
 */
void reshape(int w, int h)
{
	// Muda para o modo de projecao e reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	glOrtho (0, w, 0, h, -1 , 1);
	// Muda para o modo de desenho
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Função para capturar a cena e salvar como imagem
void capturarCenaComoImagem()
{
	std::vector<unsigned char> pixels(width * height * 3);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

	// Inverte os pixels verticalmente
	std::vector<unsigned char> flippedPixels(width * height * 3);
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			flippedPixels[((height - 1 - y) * width + x) * 3 + 0] = pixels[(y * width + x) * 3 + 0];
			flippedPixels[((height - 1 - y) * width + x) * 3 + 1] = pixels[(y * width + x) * 3 + 1];
			flippedPixels[((height - 1 - y) * width + x) * 3 + 2] = pixels[(y * width + x) * 3 + 2];
		}
	}
	// Gera o nome do arquivo
	std::stringstream ss;
	ss << "Imagem" << contadorImagens << ".png";
	std::string nomeArquivo = ss.str();
	contadorImagens++;// Contador para gerar vários arquivos de imagens diferentes

	// Salva os pixels como uma imagem usando a biblioteca STB Image Write
	stbi_write_png(nomeArquivo.c_str(), width, height, 3, flippedPixels.data(), width * 3);
	std::cout << "Tela capturada! Salvando como imagem: " << nomeArquivo << std::endl;
}

/*
 * Função para limpar toda a janela
 */

void limparJanela()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	formas.clear(); // Remove todas as formas da lista
	color_pts.clear(); // Limpa a lista de pontos coloridos
	pen.clear(); // Limpa as linhas desenhadas pela ferramenta toolPen
	modo = -1; // Atualiza o modo para um valor não utilizado
	drawPol = false; // Reinicia a variável para desenhar polígonos

	// Define as variáveis de controle das formas preenchidas como false
	fillTriangulo = false;
	fillQuadrilatero = false;
	fillPoligono = false;
	glutPostRedisplay();
}

/*
 * Controla os desenhos na tela
 */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT); // Limpa o buffer de cores e reinicia a matriz
	glColor3f(0.0, 0.0, 0.0); // Seleciona a cor default como preto

	// Desenhe todas as linhas acumuladas em lines
	for (size_t i = 0; i < pen.size(); ++i)
	{
		if (pen[i].size() >= 1)
		{
			SetColor();
			glBegin(GL_LINE_STRIP);
			for (size_t j = 0; j < pen[i].size(); ++j)
			{
				glVertex2i(pen[i][j].x, height - pen[i][j].y);
			}
			glEnd();
		}
	}

	// Aplica a translação acumulada antes de desenhar as formas
	glPushMatrix();

	drawFormas();// Desenha as formas geometricas da lista

	//Desenha texto com as coordenadas da posicao do mouse
	draw_text_stroke(0, 0, "(" + to_string(m_x) + "," + to_string(m_y) + ")", 0.2);

	// Desenha e preenche o triângulo, se a variável de controle for verdadeira
	if (fillTriangulo)
	{
		desenhaPreencheTriangulo();
		glFlush(); // Atualiza o estado de renderização
	}

	// Desenha e preenche o quadrilátero, se a variável de controle for verdadeira
	if (fillQuadrilatero)
	{
		desenhaPreencheQuadrilatero();
		glFlush(); // Atualiza o estado de renderização
	}

	// Desenha e preenche o polígono, se a variável de controle for verdadeira
	if (fillPoligono)
	{
		desenhaPreenchePoligono();
		glFlush(); // Atualiza o estado de renderização
	}

	glPopMatrix();

	glutSwapBuffers(); // Manda o OpenGL renderizar as primitivas
}

/*
 * Controla o menu pop-up
 */
void menu_popup(int value)
{
	if (value == 0) exit(EXIT_SUCCESS);
	if (value == -1)
	{
		limparJanela();
	}
	if (value >= TRANSLACAO && value <= REFLEXAO)
	{
		translacao(0, 1, formas.front());
	}
	if (value == PRINT)
	{
		capturarCenaComoImagem();
	}
	else
	{
		modo = value;
	}
}

/*
 * Controle das teclas comuns do teclado
 */
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ESC:
		exit(EXIT_SUCCESS);
		break;

	case '0': // Cor: Preto
		fillR = 0.0f;
		fillG = 0.0f;
		fillB = 0.0f;
		break;

	case '1': // Cor: Vermelho
		fillR = 1.0f;
		fillG = 0.0f;
		fillB = 0.0f;
		break;

	case '2': // Cor: Verde
		fillR = 0.0f;
		fillG = 1.0f;
		fillB = 0.0f;
		break;

	case '3': // Cor: Azul
		fillR = 0.0f;
		fillG = 0.0f;
		fillB = 1.0f;
		break;

	case '4': // Cor: Amarelo
		fillR = 1.0f;
		fillG = 1.0f;
		fillB = 0.0f;
		break;

	case '5': // Cor: Ciano
		fillR = 0.0f;
		fillG = 1.0f;
		fillB = 1.0f;
		break;

	case '6': // Cor: Magenta
		fillR = 1.0f;
		fillG = 0.0f;
		fillB = 1.0f;
		break;

	case '7': // Cor: Branco
		fillR = 1.0f;
		fillG = 1.0f;
		fillB = 1.0f;
		break;

	case '8': // Cor: Laranja escuro
		fillR = 0.8f;
		fillG = 0.4f;
		fillB = 0.0f;
		break;

	case '9': // Cor: Púrpura
		fillR = 0.5f;
		fillG = 0.0f;
		fillB = 0.7f;
		break;

	case 'c': 
	case 'C': 
		limparJanela();
		break;	
	
	case 's': 
	case 'S': 
		capturarCenaComoImagem();
		break;
	
	//Caso a tecla Barra de Espaço for teclada, termina o desenho de poligono	
	case SPACE:
		drawPol = false;
		break;	
	}

	glutPostRedisplay(); // Atualiza a exibição após a mudança de cor
}

/*
Teclas especiais usadas nas transformações geométricas.
Só é possível usar as Transformações, quando elas são selecionadas no menu do mouse.
*/
void specialKeys(int key, int x, int y)
{
	switch (transformacaoAtual)
	{

	case TRANSLACAO:

		switch (key)
		{
		case GLUT_KEY_UP:
			translacao(0, 10, formas.front());
			glutPostRedisplay();
			break;
		case GLUT_KEY_LEFT:
			translacao(-10, 0, formas.front());
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			translacao(10, 0, formas.front());
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			translacao(0, -10, formas.front());
			glutPostRedisplay();
			break;
		default:
			break;
		}
		break;

	case ESCALA:

		switch (key)
		{
		case GLUT_KEY_RIGHT:
			escala(1.1, 1.1, formas.front());
			glutPostRedisplay();
			break;
		case GLUT_KEY_LEFT:
			escala(0.9, 0.9, formas.front());
			glutPostRedisplay();
			break;
		}
		break;

	case CISALHAMENTO:

		switch (key)
		{
		case GLUT_KEY_LEFT:
			cisalhamento(-0.1, 0.0, formas.front());
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			cisalhamento(0.1, 0.0, formas.front());
			glutPostRedisplay();
			break;
		}
		break;

	case ROTACAO:

		switch (key)
		{
		case GLUT_KEY_LEFT:
			rotacao(1, formas.front());
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			rotacao(-1, formas.front());
			glutPostRedisplay();
		}
		break;

	case REFLEXAO:

		switch (key)
		{
		case GLUT_KEY_UP:
			reflexao('x', formas.front());
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			reflexao('x', formas.front());
			glutPostRedisplay();
			break;
		case GLUT_KEY_LEFT:
			reflexao('y', formas.front());
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			reflexao('y', formas.front());
			glutPostRedisplay();
			break;
		}
		break;

	default:
		break;
	}
}

/*
 * Controle dos botoes do mouse
 */
void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		switch (modo)
		{

			//Linhas
		case LIN:
			if (state == GLUT_DOWN)
			{
				if (click1)
				{
					x_2 = x;
					y_2 = height - y - 1;
					printf("Clique 2(%d, %d)\n", x_2, y_2);
					drawBresenham(x_1, y_1, x_2, y_2);
					pushLinha(x_1, y_1, x_2, y_2);
					click1 = false;
					glutPostRedisplay();
				}
				else
				{
					click1 = true;
					x_1 = x;
					y_1 = height - y - 1;
					printf("Clique 1(%d, %d)\n", x_1, y_1);
				}
			}
			break;
			//Quadriláteros
		case RET:
			if (state == GLUT_DOWN)
			{
				if (drawRet)
				{
					x_2 = x;
					y_2 = height - y - 1;
					printf("Clique 2(%d, %d)\n", x_2, y_2);
					pushRet(x_1, y_1, x_2, y_2);
					drawRet = false;
					glutPostRedisplay();
				}
				else
				{
					drawRet = true;
					x_1 = x;
					y_1 = height - y - 1;
					printf("Clique 1(%d, %d)\n", x_1, y_1);
				}
			}
			break;
			//Triângulos
		case TRI:
			if (state == GLUT_DOWN)
			{
				if (drawTri1)
				{
					x_2 = x;
					y_2 = height - y - 1;
					printf("Clique 2(%d, %d)\n", x_2, y_2);
					drawTri1 = false;
					drawTri2 = true;
				}
				else if (drawTri2)
				{
					x_3 = x;
					y_3 = height - y - 1;
					printf("Clique 3(%d, %d)\n", x_3, y_3);
					pushTriangulo(x_1, y_1, x_2, y_2, x_3, y_3);
					drawTri1 = false;
					drawTri2 = false;
					glutPostRedisplay();
				}
				else
				{
					drawTri1 = true;
					x_1 = x;
					y_1 = height - y - 1;
					printf("Clique 1(%d, %d)\n", x_1, y_1);
				}
			}
			break;
			//Polígonos
		case POL:
			if (state == GLUT_DOWN)
			{
				vertice vi = {x, height - y - 1};
				vector<vertice> pol;
				if (!drawPol)
				{
					pol.push_back(vi);
					pushPoligono(pol);
					printf("Clique (%d, %d)\n", x, height - y - 1);
					drawPol = true;
				}
				else
				{
					formas.front().v.push_front(vi);
					updateCentroide(formas.front());
					pol.clear();
					glutPostRedisplay();
				}
			}
			break;
			//Circunferências
		case CIR:
			if (state == GLUT_DOWN)
			{
				if (clickCir1)
				{
					x_2 = x;
					y_2 = height - y - 1;
					printf("Clique 2(%d, %d)\n", x_2, y_2);
					pushCirculo(x_1, y_1, x_2, y_2);
					clickCir1 = false;
					glutPostRedisplay();
				}
				else
				{
					clickCir1 = true;
					x_1 = x;
					y_1 = height - y - 1;
					printf("Clique 1(%d, %d)\n", x_1, y_1);
				}
			}
			break;

			//Algoritmo Flood Fill
		case FF:
			if (state == GLUT_DOWN)
			{
				x_1 = x;
				y_1 = height - y - 1;
				printf("Clique 1(%d, %d)\n", x_1, y_1);
				pushFloodFill(x_1, y_1);
				floodFill(x_1, y_1);
			}
			break;

			//Ferramenta Lapis
		case PEN:
			if (state == GLUT_DOWN)
			{
				if (!isMousePressed)
				{
					vector<Point> newLine;
					pen.push_back(newLine);
				}
				isMousePressed = true;
				Point point = {x, y};
				pen.back().push_back(point);
			}
			else if (state == GLUT_UP)
			{
				isMousePressed = false;
			}
			break;
		}
		break;
	}
}

/*
 * Controle da posicao do cursor do mouse
 */
void mousePassiveMotion(int x, int y)
{
	m_x = x;
	m_y = height - y - 1;
	glutPostRedisplay();
}

/*
 * Função para desenhar apenas um pixel na tela
 */
void drawPixel(int x, int y)
{
	glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
	glVertex2i(x, y);
	glEnd();  // indica o fim do ponto
}

/*
 * Função que desenha a lista de formas geometricas
 */
void drawFormas()
{
	for(vector<vertice>::iterator it = color_pts.begin(); it != color_pts.end(); ++it)
	{
		int x = it->x;
		int y = it->y;
		glPointSize(1.0f);
		glColor3f(fillR, fillG, fillB);
		glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
		glVertex2i(x, y);
		glEnd();  // indica o fim do ponto
	}

	// Preto
	glColor3f(0.0, 0.0, 0.0);

	int raioEXT = sqrt(pow(x_1 - m_x, 2) + pow(y_1 - m_y, 2));
	//Apos o primeiro clique, desenha a reta com a posicao atual do mouse
	if(click1)drawBresenham(x_1, y_1, m_x, m_y);
	else if(clickCir1) drawCirculo(x_1, y_1, raioEXT);
	else if(drawRet) drawQuadrilatero(x_1, y_1, m_x, m_y);
	else if(drawTri1) drawBresenham(x_1, y_1, m_x, m_y);
	else if(drawTri2) drawTriangulo(x_1, y_1, x_2, y_2 , m_x, m_y);
	else if(drawPol) drawBresenham(formas.front().v.front().x, formas.front().v.front().y, m_x, m_y);

	//Percorre a lista de formas geometricas para desenhar
	for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++)
	{
		int i = 0;
		std::vector<int> x;
		std::vector<int> y;
		switch (f->tipo)
		{
		case LIN:
			//Percorre a lista de vertices da forma linha para desenhar
			for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
			{
				x.push_back(v->x);
				y.push_back(v->y);
			}
			//Desenha o segmento de reta apos dois cliques
			drawBresenham(x[0], y[0], x[1], y[1]);
			break;
		case RET:
			//Percorre a lista de vertices da forma linha para desenhar
			for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
			{
				x.push_back(v->x);
				y.push_back(v->y);
			}
			//Desenha o quadrilatero apos dois cliques
			drawPoligono(x, y);
			break;
		case TRI:
			for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
			{
				x.push_back(v->x);
				y.push_back(v->y);
			}
			//Desenha o triangulo apos tres cliques na janela
			drawTriangulo(x[0], y[0], x[1], y[1], x[2], y[2]);
			break;
		case POL:
			for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
			{
				x.push_back(v->x);
				y.push_back(v->y);
				// printf("x: %d, y: %d \n",v->x, v->y);
			}
			//Desenha um poligono
			drawPoligono(x, y);
			break;
		case CIR:
			for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
			{
				x.push_back(v->x);
				y.push_back(v->y);
				// printf("x: %d, y: %d \n",v->x, v->y);
			}
			//Desenha um circulo apos dois cliques
			int raio = sqrt(pow(x[0] - x[1], 2) + pow(y[0] - y[1], 2));
			drawCirculo(x[1], y[1], raio);
			break;
		}
	}
}

/*
 * b) Algoritmo de Bresenham
 */
void drawBresenham(int x1, int y1, int x2, int y2)
{

	bool declive = false;
	bool simetrico = false;
	int aux1 = 0;
	int aux2 = 0;

	//Redução do octante
	if((x2 - x1) * (y2 - y1) < 0)
	{

		y1 = y1 * -1;
		y2 = y2 * -1;
		simetrico = true;

	}
	if(abs((x2 - x1)) < abs((y2 - y1)))
	{

		aux1 = x1;
		x1 = y1;
		y1 = aux1;
		aux2 = x2;
		x2 = y2;
		y2 = aux2;
		declive = true;

	}
	if(x1 > x2)
	{

		aux1 = x1,
		aux2 = y1;
		x1 = x2 ;
		y1 = y2;
		x2 = aux1;
		y2 = aux2;
	}

	int d = (2 * (y2 - y1)) - (x2 - x1);
	int incE = 2 * (y2 - y1);
	int incNE = 2 * ((y2 - y1) - (x2 - x1));

	while(x1 < x2)
	{

		if(d <= 0)
		{
			d += incE;
		}
		else
		{
			d += incNE;
			y1 += 1;
		}

		x1 += 1;

		if(declive && simetrico)
		{
			drawPixel(y1, x1 * -1);
		}

		if(declive && !simetrico)
		{
			drawPixel(y1, x1);
		}

		if(!declive && simetrico)
		{
			drawPixel(x1, y1 * -1);
		}

		if(!declive && !simetrico)
		{
			drawPixel(x1, y1);
		}
	}
}

/*
 *c) Função para desenhar quadriláteros utlizando Algoritmo de Bresenham
 */
void drawQuadrilatero(int x1, int y1, int x2, int y2)
{
	drawBresenham(x1, y1, x2, y1);
	drawBresenham(x1, y2, x2, y2);
	drawBresenham(x1, y1, x1, y2);
	drawBresenham(x2, y1, x2, y2);
}

/*
 * d) Função para desenhar triângulos a cada três coordenadas utlizando Algoritmo de Bresenham
 */
void drawTriangulo(int x1, int y1, int x2, int y2, int x3, int y3)
{
	drawBresenham(x1, y1, x2, y2);
	drawBresenham(x2, y2, x3, y3);
	drawBresenham(x3, y3, x1, y1);

}

/*
 *e) Função para desenhar polígonos, dadas quatro ou mais coordenada sutlizando Algoritmo de Bresenham
 */
void drawPoligono(std::vector<int>& x, std::vector<int>& y)
{
	int n = x.size();

	for (int i = 0; i < n - 1; i++)
	{
		drawBresenham(x[i], y[i], x[i + 1], y[i + 1]);
	}
	drawBresenham(x.back(), y.back(), x.front(), y.front());
}

/*
 *g) Algoritmo de Bresenham para rasterização de circunferências
 */
void drawCirculo(int xc, int yc, int r)
{
	int d = 1 - r;
	int deltE = 3, deltSE = -2 * r + 5;
	int x = 0, y = r;

	drawPixel(xc - x, yc + y);
	drawPixel(xc - y, yc - x);
	drawPixel(xc + x, yc - y);
	drawPixel(xc + y, yc + x);

	while (y > x)
	{
		if (d < 0)
		{
			d += deltE;
			deltE += 2;
			deltSE += 2;
		}
		else
		{
			d += deltSE;
			deltE += 2;
			deltSE += 4;
			y--;
		}
		x++;
		drawPixel(xc + x, yc + y);
		drawPixel(xc + y, yc + x);
		drawPixel(xc + y, yc - x);
		drawPixel(xc + x, yc - y);
		drawPixel(xc - x, yc - y);
		drawPixel(xc - y, yc - x);
		drawPixel(xc - y, yc + x);
		drawPixel(xc - x, yc + y);
	}
	drawPixel(xc + r, yc);
}

/*
 * Função de preenchimento de área usando o algoritmo Flood Fill com vizinhança 4
 */

void floodFill(int x, int y)
{

	// verifica se o pixel esta nas dimensoes da tela
	if(x < 0 || y < 0 || x > width || y > height)
	{
		return;
	}

	float pixelColor[3];

	glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, pixelColor);

	// verifica se a cor atual é diferente da cor de preenchimento
	if (pixelColor[0] != fillR || pixelColor[1] != fillG || pixelColor[2] != fillB)
	{
		// verifica se a cor atual é igual à cor da borda
		if (pixelColor[0] == 0.0f && pixelColor[1] == 0.0f && pixelColor[2] == 0.0f)
		{
			return;
		}
		// preenche o pixel com a cor de preenchimento
		glPointSize(1.0f);
		glColor3f(fillR, fillG, fillB);
		glBegin(GL_POINTS);
		glVertex2i(x, y);
		glEnd();
		pushFloodFill(x, y);
		glutSwapBuffers();

		// chama a função para os pixels adjacentes
		floodFill(x + 1, y);
		floodFill(x, y + 1);
		floodFill(x - 1, y);		
		floodFill(x, y - 1);
	}
}

// Função para preencher um polígono
void fillPolygon(std::vector<Point>& vertices)
{
	// Encontra as coordenadas y mínimas e máximas
	int minY = vertices[0].y;
	int maxY = vertices[0].y;
	for (size_t i = 1; i < vertices.size(); ++i)
	{
		if (vertices[i].y < minY) minY = vertices[i].y;
		if (vertices[i].y > maxY) maxY = vertices[i].y;
	}

	// Inicializa uma lista para armazenar as bordas do polígono
	std::vector<std::vector<int> > edges(maxY - minY + 1);

	// Encontra as interseções de cada aresta com as linhas horizontais e armazene as bordas
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		const Point& v1 = vertices[i];
		const Point& v2 = vertices[(i + 1) % vertices.size()]; // Próximo vértice

		// Calcula a diferença entre as coordenadas y
		int dy = v2.y - v1.y;
		if (dy != 0)
		{
			// Calcula a inclinação da aresta
			float m = static_cast<float>(v2.x - v1.x) / dy;
			int y = std::min(v1.y, v2.y);
			while (y < std::max(v1.y, v2.y))
			{
				edges[y - minY].push_back(v1.x + m * (y - v1.y));
				++y;
			}
		}
	}

	// Preenche os pixels entre as bordas do polígono para cada linha horizontal
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

/*
 * OBSERVAÇÃO: Não consegui passar as funções de desenho para as funções de preenchimento!
 */

//Desenha e preenche um Triangulo
void desenhaPreencheTriangulo()
{
	glColor3fv(corCapturada); // Usando a cor capturada
	//glColor3f(1.0f, 0.5f, 0.0f); // Cor laranja
	//glColor3f(1.0f, 0.0f, 0.0f); // Cor vermelha

	// Triângulo
	std::vector<Point> triangleVertices;
	triangleVertices.push_back(Point(100, 350));
	triangleVertices.push_back(Point(500, 300));
	triangleVertices.push_back(Point(300, 500));

	fillPolygon(triangleVertices);

	// Armazena os pontos preenchidos do triângulo na lista color_pts
	pushFillPolygon(triangleVertices);

	glutPostRedisplay();
}

//Desenha e preenche um Quadrilatero
void desenhaPreencheQuadrilatero()
{
	glColor3fv(corCapturada); // Usando a cor capturada
	//glColor3f(1.0f, 1.0f, 0.0f); // Cor amarela
	//glColor3f(0.0f, 1.0f, 0.0f); // Cor verde

	// Quadrado
	std::vector<Point> squareVertices;
	squareVertices.push_back(Point(300, 150));
	squareVertices.push_back(Point(400, 150));
	squareVertices.push_back(Point(400, 250));
	squareVertices.push_back(Point(300, 250));

	fillPolygon(squareVertices);

	// Armazena os pontos preenchidos do quadrilatero na lista color_pts
	pushFillPolygon(squareVertices);

	glutPostRedisplay();
}

//Desenha e preenche um poligono
void desenhaPreenchePoligono()
{
	glColor3fv(corCapturada); // Usando a cor capturada
	//glColor3f(1.0f, 0.75f, 0.8f); //Tom rosa
	//glColor3f(0.0f, 0.0f, 1.0f); // Cor azul

	// Hexágono
	std::vector<Point> hexagonVertices;
	hexagonVertices.push_back(Point(450, 150));
	hexagonVertices.push_back(Point(550, 200));
	hexagonVertices.push_back(Point(350, 150));
	hexagonVertices.push_back(Point(650, 250));
	hexagonVertices.push_back(Point(550, 300));
	hexagonVertices.push_back(Point(450, 250));

	fillPolygon(hexagonVertices);

	// Armazena os pontos preenchidos do hexagono na lista color_pts
	pushFillPolygon(hexagonVertices);
	glutPostRedisplay();
}

//f) Transformação geométrica - Translação
void translacao(int tx, int ty, forma& f)
{
	// Verifica se a forma não está vazia
	if (f.v.empty()) return;

	// Itera sobre os vértices da forma
	for (auto it = f.v.begin(); it != f.v.end(); ++it)
	{
		// Obtém a referência para o vértice atual
		vertice& v = *it;

		// Aplica a translação aos pontos da forma
		v.x += tx;
		v.y += ty;
	}

	// Atualiza as coordenadas do centróide após a translação
	updateCentroide(f);

	// Redesenha a forma na tela
	glutPostRedisplay();
}

//f) Transformação geométrica - Escala
void escala(float sx, float sy, forma& f)
{
	// Verifica se a forma não está vazia
	if (f.v.empty()) return;
	printf("centroide (%f, %f)\n", f.cx, f.cy);
	// Itera sobre os vértices da forma
	for (auto it = f.v.begin(); it != f.v.end(); ++it)
	{
		vertice& v = *it;
		// Calcula a distância do ponto em relação ao centróide
		float dx = v.x - f.cx;
		float dy = v.y - f.cy;
		float dist = sqrt(dx * dx + dy * dy);
		// Verifica se a escala ultrapassa o limiar
		if((sx > 1 && dist >= 420) || (sx < 1 && dist <= 60))
		{
			sx = 1;
			sy = 1;
		}
		// Aplica a transformação de escala na forma selecionada
		v.x = round((v.x - f.cx) * sx + f.cx);
		v.y = round((v.y - f.cy) * sy + f.cy);
	}
	// Atualiza o centróide após a transformação ter sido realizada
	updateCentroide(f);
	// Redesenha a forma na tela
	glutPostRedisplay();
}

//f) Transformação geométrica - Cisalhamento
void cisalhamento(float sx, float sy, forma& f)
{
	// Verifica se a forma não está vazia
	if (f.v.empty()) return;

	// Itera sobre os vértices da forma
	for (auto it = f.v.begin(); it != f.v.end(); ++it)
	{
		// Obtém a referência para o vértice atual
		vertice& v = *it;

		// Translada o vértice para a origem
		v.x -= f.cx;
		v.y -= f.cy;

		// Realiza a transformação de cisalhamento
		float new_x = v.x + sy * v.y;
		float new_y = v.y + sx * v.x;

		// Arredonda as novas coordenadas para valores inteiros
		v.x = round(new_x);
		v.y = round(new_y);

		// Translada o vértice de volta para a posição original
		v.x += f.cx;
		v.y += f.cy;
	}

	// Atualiza a exibição para refletir a transformação
	glutPostRedisplay();
}

//f) Transformação geométrica - Reflexão
void reflexao(char eixo, forma& f)
{
	// Verifica se a forma não está vazia
	if (f.v.empty()) return;

	// Determina o sinal de reflexão com base no eixo escolhido
	float sinal = (eixo == 'x' ? -1.0 : -1.0);

	// Itera sobre os vértices da forma
	for (auto it = f.v.begin(); it != f.v.end(); ++it)
	{
		// Obtém a referência para o vértice atual
		vertice& v = *it;

		// Translada o vértice para a origem
		v.x -= f.cx;
		v.y -= f.cy;

		// Aplica a reflexão no eixo escolhido
		if (eixo == 'x')
		{
			printf("Invertendo eixo x\n");
			v.y *= sinal;
		}
		else
		{
			printf("Invertendo eixo y\n");
			v.x *= sinal;
		}

		// Translada o vértice de volta para a posição original
		v.x += f.cx;
		v.y += f.cy;
	}

	// Atualiza a exibição para refletir a transformação
	glutPostRedisplay();
}

//f) Transformação geométrica - Rotação
void rotacao(float theta, forma& f)
{
	// Verifica se a forma não está vazia
	if (f.v.empty()) return;

	// Converte o ângulo de graus para radianos
	theta = theta * 3.14 / 180.0f;

	// Calcula o cosseno e o seno do ângulo de rotação
	float cosT = cos(theta);
	float sinT = sin(theta);

	// Itera sobre os vértices da forma
	for (auto it = f.v.begin(); it != f.v.end(); ++it)
	{
		// Obtém a referência para o vértice atual
		vertice& v = *it;

		// Calcula as coordenadas relativas ao centro de massa
		float dx = v.x - f.cx;
		float dy = v.y - f.cy;

		// Aplica a transformação de rotação
		v.x = round(f.cx + dx * cosT - dy * sinT);
		v.y = round(f.cy + dy * cosT + dx * sinT);
	}
	// Atualiza a exibição para refletir a transformação
	glutPostRedisplay();
}

/*
 * Ferramenta Lapis. Permite desenhar livremente na janela
 */

void Lapis(int x, int y)
{
	if (isMousePressed)
	{
		Point point = {x, y};
		pen.back().push_back(point);

		mousePassiveMotion(x, y);

		glClear(GL_COLOR_BUFFER_BIT);
		for (size_t i = 0; i < pen.size(); ++i)
		{
			if (pen[i].size() >= 2)
			{
				SetColor();
				glBegin(GL_LINE_STRIP);
				for (size_t j = 0; j < pen[i].size(); ++j)
				{
					glVertex2i(pen[i][j].x, pen[i][j].y);
				}
				glEnd();
			}
		}
		glFlush();
	}
}

// Define as cores da ferramente Lapis
void SetColor()
{
	glutPostRedisplay();
	glColor3fv(corCapturada); // Usando a cor capturada. Cor inicial: preto
}