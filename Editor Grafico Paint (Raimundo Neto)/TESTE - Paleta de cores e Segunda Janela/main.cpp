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

#include <stdio.h>
#include <math.h>

// Definição das cores contidas na paleta RGB
GLfloat CORES[][3] =
{
    {1.0, 0.0, 0.0},  // Vermelho
    {0.0, 1.0, 0.0},  // Verde
    {0.0, 0.0, 1.0},  // Azul
    {1.0, 1.0, 0.0},  // Amarelo
    {1.0, 0.0, 1.0},  // Magenta
    {0.0, 1.0, 1.0},  // Ciano
    {1.0, 0.5, 0.0},  // Laranja
    {0.5, 0.0, 1.0},  // Roxo
    {0.0, 0.5, 0.5}   // Turquesa
};

// Tamanho da janela
int width = 300;
int height = 300;

// Identificadores das janelas
int window1, window2;

// Variáveis para controlar o movimento do círculo
float centerX = 200.0;
float centerY = 200.0;
float radius = 50.0;
float step = 2.0;

// Variáveis para armazenar a cor capturada
GLfloat corCapturada[3] = {1.0, 1.0, 1.0}; // Inicialmente branco

void desenhaPaleta()
{
    int num_cores = sizeof(CORES) / sizeof(CORES[0]);
    int num_linhas = 3;
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
                GLfloat y1 = height - (linha + 1) * tamanho_celula; 
                GLfloat x2 = (coluna + 1) * tamanho_celula;
                GLfloat y2 = height - linha * tamanho_celula; 
                
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
    unsigned int indice_coluna = x / (width / 3);
    unsigned int indice_linha = y / (height / 3);
    size_t indice_cor = (size_t)(indice_linha * 3 + indice_coluna);

    if (indice_cor < sizeof(CORES) / sizeof(CORES[0]))
    {
        printf("Cor selecionada: (%1.f, %1.f, %1.f)\n", CORES[indice_cor][0], CORES[indice_cor][1], CORES[indice_cor][2]);
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
 * Função para desenhar o círculo na segunda janela com base na cor capturada
 */
void drawCircle()
{
    glBegin(GL_TRIANGLE_FAN);
    glColor3fv(corCapturada); // Usando a cor capturada
    glVertex2f(centerX, centerY); // Centro do círculo

    // Desenha o círculo
    for (int i = 0; i <= 360; i++)
    {
        float angle = i * 3.14159 / 180;
        glVertex2f(centerX + radius * cos(angle), centerY + radius * sin(angle));
    }
    glEnd();
}

/*
 * Controla os desenhos na tela
 */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    drawCircle();
    glutSwapBuffers();
}

/*
 * Ajusta a projecao para o redesenho da janela
 */
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    width = w;
    height = h;
}

/*
 * Controle dos botoes do mouse
 */
void mouse(int botao, int estado, int x, int y)
{
    if (botao == GLUT_LEFT_BUTTON && estado == GLUT_DOWN)
    {
        contaGotas(x, y);
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

/*
 * Função para atualizar a posição do círculo na segunda janela
 */
void updateCirclePosition(int value)
{
    // Move o círculo na horizontal
    centerX += step;

    // Verifica se o círculo atingiu a borda direita da janela e inverte a direção
    if (centerX + radius >= width || centerX - radius <= 0)
    {
        step *= -1;
    }

    // Atualiza a primeira janela
    glutPostRedisplay();

    // Chama a função novamente após um intervalo de tempo
    glutTimerFunc(20, updateCirclePosition, 1);
}

/*
 * Função Principal
 */
int main(int argc, char** argv)
{
    // Inicializa o GLUT
    glutInit(&argc, argv);

    // Configura o modo de exibição e a criação da primeira janela
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100); // Posição da primeira janela
    glutInitWindowSize(width, height);
    window1 = glutCreateWindow("Círculo - Primeira Janela");

    // Registra as funções de callback para a primeira janela
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    // Configura a cor de fundo da primeira janela para preto
    glClearColor(0, 0, 0, 1);

    // Cria a segunda janela
    glutInitWindowPosition(500, 100); // Posição da segunda janela
    glutInitWindowSize(300, 300); // Tamanho da segunda janela
    window2 = glutCreateWindow("Paleta de Cores - Segunda Janela");

    // Registra as funções de callback para a segunda janela
    glutDisplayFunc(displaySecondWindow);
    glutReshapeFunc(reshapeSecondWindow);
    glutMouseFunc(mouse); // Registra a função de callback de mouse para a segunda janela (paleta)

    // Configura a cor de fundo da segunda janela para branco
    glClearColor(1, 1, 1, 1);

    // Inicia o movimento do círculo na primeira janela
    glutTimerFunc(0, updateCirclePosition, 0);

    // Inicia o loop de eventos do GLUT
    glutMainLoop();

    return 0;
}
