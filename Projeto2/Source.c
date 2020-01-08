/*
 *  ESTG - IPPortalegre
 *  Computa��o Gr�fica 2019/2020 - Trabalho 2 - Pac-Man
 *
 *   ---->  C�DIGO DE APOIO  <----
 *
 *   Este c�digo cria um tabuleiro com cubos; coloca em cima do tabuleiro
 *   outros 5 cubos que se podem movimentar com as teclas direcionais
 *   (teclas das setas); o cubo a movimentar � escolhido com as teclas 1-5.
 *   O C�digo j� tem vista de perspetiva. O mundo � desenhado no plano XY
 *   e a c�mara est� alinhada com o eixo YY colocada na zona negativa do
 *   eixo e um pouco para cima (em ZZ); aponta para a posi��o (0,0,0).
 *
*/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Tamanho da box (centrada na origem) onde as figuras s�o desenhadas.
#define CELL_SIZE 2.0

// Tamanho (n�mero de casas) do tabuleiro quadrado.
// Cada casa do tabuleiro tem tamanho CELL_SIZE.
// TAB_SIZE tem que ser par (facilita o alinhamento).
#define TAB_SIZE 10


GLfloat vertices[][3] = { {-1.0,-1.0,-1.0}, {1.0,-1.0,-1.0},
						 {1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
						 {1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0} };

GLfloat normals[][3] = { {-1.0,-1.0,-1.0}, {1.0,-1.0,-1.0},
						{1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
						{1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0} };

GLfloat colors[][3] = { {0.0,0.0,0.0}, {1.0,0.0,0.0},
					   {1.0,1.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0},
					   {1.0,0.0,1.0}, {1.0,1.0,1.0}, {0.0,1.0,1.0} };

typedef struct cubo* Cubo;
struct cubo {
	int xTab, yTab;     // Posi��o do cubo nas casas do tabuleiro.
	float scale;
	float xRot, yRot;   // Rota��o necess�ria em x e em y em graus multiplos de 90.
	float xRoti, yRoti; // Rota��o actual em x e em y at� atingir xRot e yRot, respectivamente.

	float xMov, yMov;
	float xMovi, yMovi;

	// Matriz de rota��o que define a posi��o actual do cubo.
	GLfloat m[16];

	float r, g, b;
};

Cubo cubo;     // cubo actual
Cubo* aCubos;  // array de cubos
int nCubos = 5;  // 1..9


int timeUpdate = 20;      // intervalo de tempo (ms) para o update
float velRot = 200.0;     // velocidade de rota��o dos cubos em graus/s
float angRotMax = 90;

float xAnterior, yAnterior;
float xCamera = 0.0, yCamera = -5, zCamera = 0.0;

void polygon(int a, int b, int c, int d)
{
	glBegin(GL_POLYGON);
	//glColor3fv(colors[a]);
	glNormal3fv(normals[a]);
	glVertex3fv(vertices[a]);
	//glColor3fv(colors[b]);
	glNormal3fv(normals[b]);
	glVertex3fv(vertices[b]);
	//glColor3fv(colors[c]);
	glNormal3fv(normals[c]);
	glVertex3fv(vertices[c]);
	//glColor3fv(colors[d]);
	glNormal3fv(normals[d]);
	glVertex3fv(vertices[d]);
	glEnd();
}

void lineloop(int a, int b, int c, int d)
{
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
	glVertex3fv(vertices[a]);
	glVertex3fv(vertices[b]);
	glVertex3fv(vertices[c]);
	glVertex3fv(vertices[d]);
	glEnd();
}

void colorcube(void)
{
	polygon(0, 3, 2, 1);
	polygon(2, 3, 7, 6);
	polygon(0, 4, 7, 3);
	polygon(1, 2, 6, 5);
	polygon(4, 5, 6, 7);
	polygon(0, 1, 5, 4);

	lineloop(0, 3, 2, 1);
	lineloop(2, 3, 7, 6);
	lineloop(0, 4, 7, 3);
	lineloop(1, 2, 6, 5);
	lineloop(4, 5, 6, 7);
	lineloop(0, 1, 5, 4);
}

// Altera uma cor incrementando as suas componentes (RBG) com uma constante;
// se alguma componente ficar maior que 1 come�a no in�cio.
void incColor(float cor[3])
{
	int i;

	for (i = 0; i < 3; i++) {
		cor[i] += 0.2;
		cor[i] = cor[i] >= 1.0 ? cor[i] - 1.0 : cor[i];
	}
}

// Desenha um cubo com diferentes cores nos lados.
void colorcubeWithColor(float r, float g, float b)
{
	// Um dos lados come�a com uma cor e os outros lados s�o varia��es dessa cor.

	float cor[3] = { r, g, b };

	glColor3fv(cor);
	polygon(0, 3, 2, 1);

	incColor(cor);
	glColor3fv(cor);
	polygon(2, 3, 7, 6);

	incColor(cor);
	glColor3fv(cor);
	polygon(0, 4, 7, 3);

	incColor(cor);
	glColor3fv(cor);
	polygon(1, 2, 6, 5);

	incColor(cor);
	glColor3fv(cor);
	polygon(4, 5, 6, 7);

	incColor(cor);
	glColor3fv(cor);
	polygon(0, 1, 5, 4);

	lineloop(0, 3, 2, 1);
	lineloop(2, 3, 7, 6);
	lineloop(0, 4, 7, 3);
	lineloop(1, 2, 6, 5);
	lineloop(4, 5, 6, 7);
	lineloop(0, 1, 5, 4);

}

// Desenha um tabuleiro no plano XY e centrado na origem dos eixos;
// � quadrado, de tamanho TAB_SIZE x TAB_SIZE posi��es (casas do tabuleiro);
// cada posi��o que tem lado de tamanho CELL_SIZE.
void desenhaTabuleiro()
{
	int i, j;

	glPushMatrix();

	// Centrar o tabuleiro no eixo XY e colocar o topo do tabuleiro no eixo dos Z (Z = 0).
	glTranslatef(-(TAB_SIZE * CELL_SIZE) / 2, -(TAB_SIZE * CELL_SIZE) / 2, -CELL_SIZE / 2);

	for (i = 0; i < TAB_SIZE; i++)
		for (j = 0; j < TAB_SIZE; j++) {
			glPushMatrix();
			glTranslatef(j * CELL_SIZE + CELL_SIZE / 2, i * CELL_SIZE + CELL_SIZE / 2, 0.60);
			glScalef(0.90, 0.90, 0.30);
			glColor3f(1.0, 0.0, 0.0);
			colorcube();  // Este cubo tem que ser desenhado dentro do CELL_SIZE.
						  // � o que acontece porque o cubo tem o tamanho do CELL_SIZE.
			glPopMatrix();
		}

	glPopMatrix();
}

// Faz os translates necess�rios para colocar uma forma gr�fica (desenhada
// numa box de tamanho CELL_SIZE) em cima de uma posi��o do tabuleiro.
void translateFormaTabuleiro(int x, int y) {
	glTranslatef(-(TAB_SIZE * CELL_SIZE) / 2, -(TAB_SIZE * CELL_SIZE) / 2, CELL_SIZE / 2);
	glTranslatef(x * CELL_SIZE + CELL_SIZE / 2, y * CELL_SIZE + CELL_SIZE / 2, 0);
}

// aloca a mem�ria para a struct cubo
Cubo criaCubo(int xTab, int yTab, float scale)
{
	Cubo c = (Cubo)malloc(sizeof(*c));
	c->xTab = xTab;
	c->yTab = yTab;
	c->scale = scale;
	c->xRot = c->yRot = c->xRoti = c->yRoti = 0.0;

	c->r = 0;
	c->g = 1;
	c->b = 0;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, c->m);
	glPopMatrix();

	return c;
}

// Liberta a mem�ria ocupada pelo Cubo c
void apagaCubo(Cubo c)
{
	free(c);
}

void actualizaRotCubo(Cubo c, GLfloat ang, GLfloat x, GLfloat y, GLfloat z)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(ang, x, y, z);
	glMultMatrixf(c->m);
	glGetFloatv(GL_MODELVIEW_MATRIX, c->m);
	glPopMatrix();
}

void rodaCuboCima(Cubo c)
{
	if (c->yRot == c->yRoti)     // n�o est� a rodar em Y
		c->xRot += -angRotMax;
}

void rodaCuboBaixo(Cubo c)
{
	if (c->yRot == c->yRoti)     // n�o est� a rodar em Y
		c->xRot += angRotMax;
}

void rodaCuboEsq(Cubo c)
{
	if (c->xRot == c->xRoti)     // n�o est� a rodar em X
		c->yRot += -angRotMax;
}

void rodaCuboDir(Cubo c)
{
	if (c->xRot == c->xRoti)     // n�o est� a rodar em X
		c->yRot += angRotMax;
}

void select(Cubo c)
{
	c->scale *= 1.1;
}

void unselect(Cubo c)
{
	c->scale /= 1.1;
}

void desenhaCubo(Cubo c)
{
	if (c == NULL) return;

	glPushMatrix();

	translateFormaTabuleiro(c->xTab, c->yTab);
	glScalef(c->scale, c->scale, 1);

	glRotatef(c->xRoti, 1, 0, 0);
	glRotatef(c->yRoti, 0, 1, 0);

	glMultMatrixf(c->m);

	colorcubeWithColor(c->r, c->g, c->b);

	glPopMatrix();
}


void updateFantasma() {

}

void updateCubo(Cubo c, int t) //UPDATE PACMAN
{	// ToDo: colocar aqui uma condi��o para s� fazer isto se for preciso...
	float angMov = velRot * (t / 1000.0);

	

	if (c->xRot != 0 || c->xRoti != 0) {
		if (c->xRot > c->xRoti) c->xRoti += angMov;
		if (c->xRot < c->xRoti) c->xRoti -= angMov;
		if (fabs(c->xRot - c->xRoti) < angMov) {
			actualizaRotCubo(c, c->xRot, 1, 0, 0);
			c->xRot = c->xRoti = 0.0;
		}
	}

	if (c->yRot != 0 || c->yRoti != 0) {
		if (c->yRot > c->yRoti) c->yRoti += angMov;
		if (c->yRot < c->yRoti) c->yRoti -= angMov;
		if (fabs(c->yRot - c->yRoti) < angMov) {
			actualizaRotCubo(c, c->yRot, 0, 1, 0);
			c->yRot = c->yRoti = 0.0;
		}
	}

	//if (c->xMov != 0 || c->xMovi != 0) {
		//if (c->xMov > c-> xMovi) c->xMovi += 
	//}
}

void display(void)
{
	int i;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glShadeModel(GL_FLAT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Camara
	gluLookAt(xCamera, yCamera, 1, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

	// Redimensiona a mundo para caber na janela.
	glScalef(0.2, 0.2, 0.2);

	// Desenha o tabuleiro e os cubos.
	desenhaTabuleiro();
	for (i = 0; i < nCubos; i++)
		desenhaCubo(aCubos[i]);

	glFlush();

	glutSwapBuffers();
}


void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glFrustum(-2.0, 2.0, -2.0 * (GLfloat)h / (GLfloat)w,
			2.0 * (GLfloat)h / (GLfloat)w, 3.0, 20.0);
	else
		glFrustum(-2.0 * (GLfloat)w / (GLfloat)h,
			2.0 * (GLfloat)w / (GLfloat)h, -2.0, 2.0, 3.0, 20.0);
	glMatrixMode(GL_MODELVIEW);

}

// timer callback
void update(int v)
{
	int i;

	for (i = 0; i < nCubos; i++)
		updateCubo(aCubos[i], v);

	//if para update pacman ou fantasma

	//ter na estrutura de dados o tipo de objeto

	glutPostRedisplay();
	glutTimerFunc(v, update, v);
}

void selectCubo(int i)
{
	printf("Cubo %i seleccionado!!!\n", i);
	if (i <= 0 || i > nCubos) return;

	unselect(cubo);
	cubo = aCubos[i - 1];
	select(cubo);
}

void teclas(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
	case 'Q':
		exit(0);
		break;
	}

	selectCubo(key - '0');

	glutPostRedisplay();
}

void teclasEspeciais(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		rodaCuboCima(cubo);
		cubo->yTab += 1;
		break;
	case GLUT_KEY_DOWN:
		rodaCuboBaixo(cubo);
		cubo->yTab -= 1;
		break;
	case GLUT_KEY_LEFT:
		rodaCuboEsq(cubo);
		cubo->xTab -= 1;
		break;
	case GLUT_KEY_RIGHT:
		rodaCuboDir(cubo);
		cubo->xTab += 1;
		break;
	}

	glutPostRedisplay();
}

void myInit()
{
	int i;

	// Para que as linhas sejam bem desenhadas � volta dos cubos
	glPolygonOffset(1.0, 1.0);
	glEnable(GL_POLYGON_OFFSET_FILL);

	srand(time());

	// Cria cubos em posi��es aleat�rias.
	aCubos = (Cubo*)malloc(sizeof(Cubo) * nCubos);
	for (i = 0; i < nCubos; i++) {
		aCubos[i] = criaCubo(rand() % TAB_SIZE, rand() % TAB_SIZE, 0.8);
		aCubos[i]->r = ((float)rand()) / RAND_MAX;
		aCubos[i]->g = ((float)rand()) / RAND_MAX;
		aCubos[i]->b = ((float)rand()) / RAND_MAX;
	}




	cubo = aCubos[0];   // o primeiro cubo � o selecionado
	select(cubo);

}

void rato(int x, int y) {

	printf("%i, %i\n", x, y);

	if (x < xAnterior) {
		xCamera = xCamera + 0.01;
	}
		

	if (x > xAnterior) {
		xCamera = xCamera - 0.01;
	}
		

	if (y < yAnterior) {
		zCamera = zCamera + 0.01;
	}
		

	if (y > yAnterior) {
		zCamera = zCamera - 0.01;
	}
		

	xAnterior = x; yAnterior = y;

}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	/* need both double buffering and z buffer */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("colorcube");
	myInit();
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(teclas);
	glutSpecialFunc(teclasEspeciais);
	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
	glutTimerFunc(timeUpdate, update, timeUpdate);
	glutMotionFunc(rato);
	glutMainLoop();
}

