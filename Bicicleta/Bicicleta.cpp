#include "glos.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLAux.h>
#include <math.h>


GLUquadricObj* qObj;
boolean umbra = 0;
GLfloat alfa = 0, x = 0, xnear = 0, xfar = 0, beta = 0, beta2 = 0;
GLfloat position[] = { 0, 2.5, -1, 1.0 };
GLuint IDtextura;
GLuint ID_G1, ID_G2, ID_G3, ID_G4;
const char* sir;


void myinit(void);
void CALLBACK display(void);
void CALLBACK myReshape(GLsizei w, GLsizei h);


void CALLBACK incarca_textura(const char* s) {
	AUX_RGBImageRec* pImagineTextura = auxDIBImageLoad(s);
	if (pImagineTextura != NULL) {
		glGenTextures(1, &IDtextura);

		glBindTexture(GL_TEXTURE_2D, IDtextura);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, pImagineTextura->sizeX, pImagineTextura->sizeY,
			0, GL_RGB, GL_UNSIGNED_BYTE, pImagineTextura->data);
		// GL_RGB8
	}
	if (pImagineTextura) {
		if (pImagineTextura->data) {
			free(pImagineTextura->data);
		}
		free(pImagineTextura);
	}
}

void calcCoeficientiPlan(float P[3][3], float coef[4]) {
	float v1[3], v2[3];
	float length;
	static const int x = 0, y = 1, z = 2;

	// calculeaza 2 vectori din 3 pct
	v1[x] = P[0][x] - P[1][x];
	v1[y] = P[0][y] - P[1][y];
	v1[z] = P[0][z] - P[1][z];
	v2[x] = P[1][x] - P[2][x];
	v2[y] = P[1][y] - P[2][y];
	v2[z] = P[1][z] - P[2][z];

	//se calc produsul vectorial al celor 2 vectori => al3lea vector cu componentele A,B,C chiar coef din ec. planului
	coef[x] = v1[y] * v2[z] - v1[z] * v2[y];
	coef[y] = v1[z] * v2[x] - v1[x] * v2[z];
	coef[z] = v1[x] * v2[y] - v1[y] - v2[x];

	//normalizare vector
	length = (float)sqrt(coef[x] * coef[x] + coef[y] * coef[y] + coef[z] * coef[z]);
	coef[x] /= length;
	coef[y] /= length;
	coef[z] /= length;
}
void MatriceUmbra(GLfloat puncte[3][3], GLfloat pozSursa[4], GLfloat mat[4][4]) {
	// creeaza matricea care da umbra cunoscandu-se coef planului + poz sursei
	// IN mat SE SALVEAZA MATRICEA

	GLfloat coefPlan[4], temp;

	//determina coef planului
	calcCoeficientiPlan(puncte, coefPlan);

	// determinam D
	coefPlan[3] = -(coefPlan[0] * puncte[2][0] + coefPlan[1] * puncte[2][1] + coefPlan[2] * puncte[2][2]);

	// temp= AxL + ByL + CzL + Dw
	temp = coefPlan[0] * pozSursa[0] + coefPlan[1] * pozSursa[1] + coefPlan[2] * pozSursa[2] + coefPlan[3] * pozSursa[3];

	//prima coloana
	mat[0][0] = temp - coefPlan[0] * pozSursa[0];
	mat[1][0] = 0.0f - coefPlan[1] * pozSursa[0];
	mat[2][0] = 0.0f - coefPlan[2] * pozSursa[0];
	mat[3][0] = 0.0f - coefPlan[3] * pozSursa[0];
	//a 2a coloana
	mat[0][1] = 0.0f - coefPlan[0] * pozSursa[1];
	mat[1][1] = temp - coefPlan[1] * pozSursa[1];
	mat[2][1] = 0.0f - coefPlan[2] * pozSursa[1];
	mat[3][1] = 0.0f - coefPlan[3] * pozSursa[1];
	//a 3a coloana
	mat[0][2] = 0.0f - coefPlan[0] * pozSursa[2];
	mat[1][2] = 0.0f - coefPlan[1] * pozSursa[2];
	mat[2][2] = temp - coefPlan[2] * pozSursa[2];
	mat[3][2] = 0.0f - coefPlan[3] * pozSursa[2];
	//a4a coloana
	mat[0][3] = 0.0f - coefPlan[0] * pozSursa[3];
	mat[1][3] = 0.0f - coefPlan[1] * pozSursa[3];
	mat[2][3] = 0.0f - coefPlan[2] * pozSursa[3];
	mat[3][3] = temp - coefPlan[3] * pozSursa[3];
}

void textura() {
	sir = ".\\iarba1.bmp";
	incarca_textura(sir);
	ID_G1 = IDtextura;

	sir = ".\\iarba.bmp";
	incarca_textura(sir);
	ID_G2 = IDtextura;

	sir = ".\\pamant.bmp";
	incarca_textura(sir);
	ID_G3 = IDtextura;

	sir = ".\\Sky.bmp";
	incarca_textura(sir);
	ID_G4 = IDtextura;

}
void myinit(void) {


	GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess[] = { 22.0f };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	//permite urmarirea culorilor
	glEnable(GL_COLOR_MATERIAL);
	//seteaza proprietatile de material pt a urma valorile glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	//seteaza sursa
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	//seteaza materialul
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glEnable(GL_TEXTURE_2D);
	//glShadeModel(GL_FLAT);

	// pt fundal
	glClearColor(0, 0.3, 0.7, 1);
	glEnable(GL_DEPTH_TEST); // ascunderea suprafetelor

	
	textura();
}

void CALLBACK mutaBicicletaFata(AUX_EVENTREC* event)
{
	x += 0.2;
	xnear += 0.195;
	xfar += 0.205;
}
void CALLBACK mutaBicicletaSpate(AUX_EVENTREC* event)
{
	x -= 0.2;
	xnear -= 0.195;
	xfar -= 0.205;

}
void CALLBACK rotateYleft(void) {
	alfa -= 20;
}
void CALLBACK rotateYright(void) {
	alfa += 20;

}
void CALLBACK mutaSursaDreapta(void) {
	if (position[0] < 8) {
		position[0] += 0.4;

	}
}
void CALLBACK mutaSursaStanga(void) {
	if (position[0] > -8) {
		position[0] -= 0.4;

	}
}
void CALLBACK animatie1(void)
{
	if (beta < 30) {
		beta += 15;
		beta2 += 15;
	}
}
void CALLBACK animatie2(void)
{

	if (beta > -30) {
		beta -= 15;
		beta2 -= 15;
	}
}


void piesaOrizontalSus() {
	glPushMatrix();
		qObj = gluNewQuadric();
		gluQuadricNormals(qObj, GLU_FLAT);
		gluQuadricDrawStyle(qObj, GLU_FILL);
		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(1.0, 0.0, 0.0);
		glTranslatef(0, 0, 0);
		glRotatef(90, 0, 1, 0);
		gluCylinder(qObj, 0.055, 0.055, 1.2, 30, 30);
	glPopMatrix();
}
void piesaVerticalStanga() {
	glPushMatrix();
		qObj = gluNewQuadric();
		gluQuadricDrawStyle(qObj, GLU_FILL);
		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(1.0, 0.0, 0.0);
		glTranslatef(-0.1, 0.22, 0);
		glRotatef(15, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		gluCylinder(qObj, 0.05, 0.05, 1.2, 30, 30);
	glPopMatrix();
}
void piesaOblicDreapta() {
	glPushMatrix();
		qObj = gluNewQuadric();
		gluQuadricDrawStyle(qObj, GLU_FILL);
		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(1.0, 0.0, 0.0);
		glTranslatef(1.2, -0.08, 0);
		glRotatef(-50, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		gluCylinder(qObj, 0.05, 0.05, 1.3, 30, 30);
	glPopMatrix();
}
void piesaVerticalDreapta() {
	glPushMatrix();
		qObj = gluNewQuadric();
		gluQuadricDrawStyle(qObj, GLU_FILL);
		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(1.0, 0.1, 0.2);
		glTranslatef(1.17, 0.22, 0);
		glRotatef(15, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		gluCylinder(qObj, 0.055, 0.055, 0.6, 30, 30);
	glPopMatrix();
}
void piesaOrizontalJos() {
	glPushMatrix();
		qObj = gluNewQuadric();
		gluQuadricDrawStyle(qObj, GLU_FILL);
		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(1.0, 0.0, 0.0);
		glTranslatef(-0.75, -0.9, 0.1);
		glRotatef(90, 0, 1, 0);
		gluCylinder(qObj, 0.05, 0.05, 1, 30, 30);
	glPopMatrix();
}
void piesaOblicStanga() {

	glPushMatrix();
		qObj = gluNewQuadric();
		gluQuadricDrawStyle(qObj, GLU_FILL);
		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(1.0, 0.0, 0.0);
		glTranslatef(-0.05, -0.04, 0);
		glRotatef(-35, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		gluCylinder(qObj, 0.05, 0.05, 0.4, 30, 30);
	glPopMatrix();
}
void piesaRoataDreaptaNear() {
	glPushMatrix();
		qObj = gluNewQuadric();//Piesa Nr.4-Roata Vizibila
		gluQuadricDrawStyle(qObj, GLU_FILL);
		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(1.0, 0.0, 0.0);

		glPushMatrix();
			//glTranslatef(1.27, -0.18, 0.1);
			glTranslatef(1.28, -0.23, 0.2);
			glRotatef(beta2, 0, 1, 0);
			glRotatef(20, 0, 0, 1);
			glRotatef(90, 1, 0, 0);
			gluCylinder(qObj, 0.04, 0.04, 0.75, 30, 30);
			//gluCylinder(qObj, 0.04, 0.04, 0.8, 30, 30);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(1.3, -0.26, 0.2);
			glRotatef(180, 1, 0, 0);
			gluCylinder(qObj, 0.04, 0.04, 0.2, 30, 30);
		glPopMatrix();
	glPopMatrix();

}
void piesaRoataDreaptaFar() {
	glPushMatrix();
		qObj = gluNewQuadric();//Piesa Nr.4-Roata 
		gluQuadricDrawStyle(qObj, GLU_FILL);
		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(1.0, 0.0, 0.0);

		glPushMatrix();
			// glTranslatef(1.28, -0.18, -0.1);
			glTranslatef(1.29, -0.21, -0.2);
			glRotatef(beta2, 0, 1, 0);
			glRotatef(20, 0, 0, 1);
			glRotatef(90, 1, 0, 0);
			gluCylinder(qObj, 0.04, 0.04, 0.8, 30, 30);
			// gluCylinder(qObj, 0.04, 0.04, 0.8, 30, 30);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(1.31, -0.25, -0.2);
			gluCylinder(qObj, 0.05, 0.05, 0.2, 30, 30);
		glPopMatrix();
	glPopMatrix();
}
void piesaRoataStangaNear() {
	glPushMatrix();
		qObj = gluNewQuadric();//Piesa Nr.6-Roata Vizibila
		gluQuadricDrawStyle(qObj, GLU_FILL);
		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(0.0, 0.0, 1.0);
		glTranslatef(-0.25, -0.32, 0.1);
		glRotatef(-40, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		gluCylinder(qObj, 0.04, 0.04, 0.8, 30, 30);
	glPopMatrix();

}
void piesaRoataStangaFar() {
	glPushMatrix();
		qObj = gluNewQuadric();//Piesa Nr.6-Roata 
		gluQuadricDrawStyle(qObj, GLU_FILL);
		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(1.0, 0.0, 0.0);
		glTranslatef(-0.2, -0.3, -0.1);
		glRotatef(-40, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		gluCylinder(qObj, 0.04, 0.04, 0.8, 30, 30);
	glPopMatrix();

}
void Coarne() {
	glPushMatrix();
		qObj = gluNewQuadric();//Piesa Nr.7--Coarne central
		gluQuadricDrawStyle(qObj, GLU_FILL);
		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(1, 0.0, 0.0);
		glPushMatrix();
			glTranslatef(1.17, 0.23, 0);
			glRotatef(beta, 0, 1, 0);
			gluCylinder(qObj, 0.05, 0.05, 0.3, 30, 30);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(1.17, 0.23, 0);
			glRotatef(180, 0, 1, 0);
			glRotatef(beta, 0, 1, 0);
			gluCylinder(qObj, 0.05, 0.05, 0.3, 30, 30);
		glPopMatrix();
	glPopMatrix();

}
void Sa() {
	glPushMatrix();
		glRotatef(-5, 1, 0, 0);
		glTranslatef(0, 0.23, 0);
		glColor3f(0.05, 0.05, 0.05);
		glBegin(GL_QUAD_STRIP);
			glNormal3f(-0.333, 0.333, 0.333); glVertex3f(-0.2, 0, 0.1);
			glNormal3f(-0.333, 0.333, 0.333); glVertex3f(-0.2, 0.05, 0.1);

			glNormal3f(0.333, 0.333, 0.333); glVertex3f(0, 0, 0.1);
			glNormal3f(0.333, 0.333, 0.333); glVertex3f(0, 0.05, 0.1);

			glNormal3f(0.333, 0.333, 0.333); glVertex3f(0.25, 0, 0);
			glNormal3f(0.333, 0.333, 0.333); glVertex3f(0.25, 0.05, 0);

			glNormal3f(0.333, 0.333, -0.333); glVertex3f(0, 0, -0.1);
			glNormal3f(-0.333, 0.333, -0.333); glVertex3f(0, 0.05, -0.1);

			glNormal3f(-0.333, 0.333, -0.333); glVertex3f(-0.2, 0, -0.1);
			glNormal3f(-0.333, 0.333, -0.333); glVertex3f(-0.2, 0.05, -0.1);

			glNormal3f(-0.333, 0.333, 0.333); glVertex3f(-0.2, 0, 0.1);
			glNormal3f(-0.333, 0.333, 0.333); glVertex3f(-0.2, 0.05, 0.1);
		glEnd();


		glBegin(GL_POLYGON);
			glNormal3f(-0.333, 0.333, 0.333); glVertex3f(-0.2, 0, 0.1);
			glNormal3f(0.333, 0.333, 0.333); glVertex3f(0, 0, 0.1);
			glNormal3f(0.333, 0.333, 0.333); glVertex3f(0.25, 0, 0);
			glNormal3f(-0.333, 0.333, -0.333); glVertex3f(0, 0, -0.1);
			glNormal3f(-0.333, 0.333, -0.333); glVertex3f(-0.2, 0, -0.1);
		glEnd();

		glBegin(GL_POLYGON);
			glNormal3f(-0.333, 0.333, 0.333); glVertex3f(-0.2, 0.05, 0.1);
			glNormal3f(0.333, 0.333, 0.333); glVertex3f(0, 0.05, 0.1);
			glNormal3f(0.333, 0.333, 0.333); glVertex3f(0.25, 0.05, 0);
			glNormal3f(0.333, 0.333, -0.333); glVertex3f(0, 0.05, -0.1);
			glNormal3f(-0.333, 0.333, -0.333); glVertex3f(-0.2, 0.05, -0.1);
		glEnd();
	glPopMatrix();
}
void Scena() {

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0, 1.0, 1.0);

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, ID_G1);
		glBegin(GL_QUADS);//iARBA SUS		
			//glColor3f(0, 0.4, 0.1);
			glTexCoord2f(0.0f, 0.0f); glNormal3f(-0.333, -0.333, -0.333);  glVertex3f(-8, -0.9, -2);
			glTexCoord2f(0.0f, 1.0f); glNormal3f(-0.333, 0.333, -0.333);  glVertex3f(-8, 1, -3);
			glTexCoord2f(1.0f, 1.0f); glNormal3f(0.333, 0.333, -0.333); glVertex3f(8, 1, -3);
			glTexCoord2f(1.0f, 0.0f); glNormal3f(0.333, -0.333, -0.333);   glVertex3f(8, -0.9, -2);
		glEnd();
	glPopMatrix();

	glPushMatrix();//IARBA JOS	
		glBindTexture(GL_TEXTURE_2D, ID_G2);
		glBegin(GL_QUADS);		
			//glColor3f(0, 0.4, 0.1);
			glTexCoord2f(0.0f, 0.0f);  glNormal3f(-0.333, -0.333, -0.333); glVertex3f(-8, -3, -1);
			glTexCoord2f(0.0f, 1.0f);  glNormal3f(-0.333, -0.333, -0.333); glVertex3f(-8, -0.9, -2);
			glTexCoord2f(1.0f, 1.0f);  glNormal3f(0.333, -0.333, -0.333);  glVertex3f(8, -0.9, -2);
			glTexCoord2f(1.0f, 0.0f);  glNormal3f(0.333, -0.333, -0.333);  glVertex3f(8, -3, -1);
		glEnd();	
	glPopMatrix();

	glPushMatrix();//pamant
		glBindTexture(GL_TEXTURE_2D, ID_G3);
		glBegin(GL_QUADS);			
			//glColor3f(0.6, 0.5, 0.4);
			glTexCoord2f(0.0f, 0.0f); glNormal3f(-0.333, -0.333, -0.333); glVertex3f(-15, -15, -1);
			glTexCoord2f(0.0f, 1.0f); glNormal3f(-0.333, -0.333, -0.333);  glVertex3f(-15, -3, -2);
			glTexCoord2f(1.0f, 1.0f); glNormal3f(0.333, -0.333, -0.333);  glVertex3f(15, -3, -2);
			glTexCoord2f(1.0f, 0.0f); glNormal3f(0.333, -0.333, -0.333);  glVertex3f(15, -15, -1);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, ID_G4);
			glBegin(GL_QUADS);//CER		
			glTexCoord2f(0.0f, 0.0f); glNormal3f(-0.333, -0.333, -0.333);  glVertex3f(-8, 6, -2);
			glTexCoord2f(0.0f, 1.0f); glNormal3f(-0.333, 0.333, -0.333);  glVertex3f(-8, 1, -3);
			glTexCoord2f(1.0f, 1.0f); glNormal3f(0.333, 0.333, -0.333);  glVertex3f(8, 1, -3);
			glTexCoord2f(1.0f, 0.0f); glNormal3f(0.333, -0.333, -0.333);   glVertex3f(8, 6, -2);
		glEnd();
	glPopMatrix();

	
	glDisable(GL_TEXTURE_2D);
}
void Drum() {
	glPushMatrix();//drum
		glBegin(GL_QUADS);
			glColor3f(0.05, 0.05, 0.05);
			glNormal3f(-0.333, -0.333, -0.333); glVertex3f(-8, -0.9, -0.5);
			glNormal3f(-0.333, 0.333, -0.333);  glVertex3f(-8, 0, -1);
			glNormal3f(0.333, 0.333, -0.333);  glVertex3f(8, 0, -1);
			glNormal3f(0.333, -0.333, -0.333);  glVertex3f(8, -0.9, -0.5);
		glEnd();
	glPopMatrix();

	for (int i = 1, k = 0; i <= 20; i++, k++) {
		glPushMatrix();
			glBegin(GL_QUADS);
				glColor3f(1, 1, 1);
				glNormal3f(-0.333, -0.333, -0.333); glVertex3f(k - 8.3, -0.38, -0.4);
				glNormal3f(-0.333, -0.333, -0.333);  glVertex3f(k - 8.3, -0.26, -0.6);
				glNormal3f(-0.333, -0.333, -0.333);  glVertex3f(k - 7.7, -0.26, -0.6);
				glNormal3f(-0.333, -0.333, -0.333);  glVertex3f(k - 7.7, -0.38, -0.4);
			glEnd();
		glPopMatrix();
	}
}
void RoataStanga() {
	glPushMatrix();//CERC Stanga
		qObj = gluNewQuadric();
		gluQuadricDrawStyle(qObj, GLU_LINE);

		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(0.8, 0.8, 0.8);
		glTranslatef(-0.75, -0.9, 0);
		gluDisk(qObj, 0.05, 0.5, 20, 1);

		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(0, 0, 0);
		auxSolidTorus(0.05, 0.5);

		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(0.8, 0.8, 0.8);
		auxSolidTorus(0.02, 0.45);
	glPopMatrix();

}
void RoataDreapta() {
	glPushMatrix();//CERC DREAPTA
		qObj = gluNewQuadric();
		gluQuadricDrawStyle(qObj, GLU_LINE);

		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(0.8, 0.8, 0.8);
		glTranslatef(1.55, -0.9, 0);
		glRotatef(beta, 0, 1, 0);
		gluDisk(qObj, 0, 0.5, 20, 1);

		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(0, 0, 0);
		auxSolidTorus(0.05, 0.5);

		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(0.8, 0.8, 0.8);
		auxSolidTorus(0.02, 0.45);
	glPopMatrix();



}
void Lant() {
	glPushMatrix();
		qObj = gluNewQuadric();
		gluQuadricDrawStyle(qObj, GLU_FILL);

		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(0.8, 0.8, 0.8);
		glTranslatef(-0.70, -0.9, 0.25);
		gluDisk(qObj, 0.0, 0.15, 50, 50);
	glPopMatrix();

	glPushMatrix();
		qObj = gluNewQuadric();
		gluQuadricDrawStyle(qObj, GLU_FILL);

		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(0.8, 0.8, 0.8);

		glTranslatef(0.2, -0.9, 0.25);
		gluDisk(qObj, 0.0, 0.1, 50, 50);
	glPopMatrix();

	glBegin(GL_LINES);
		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(0.0, 0.0, 0.0);

		glVertex3f(-0.70, -0.75, 0.25);
		glVertex3f(0.2, -0.8, 0.25);
	glEnd();

	glBegin(GL_LINES);
		if (umbra == 1)
			glColor3f(0.05, 0.05, 0.05);
		else
			glColor3f(0.0, 0.0, 0.0);
		glVertex3f(-0.70, -1.05, 0.25);
		glVertex3f(0.2, -1, 0.25);
	glEnd();
}
void Pedala() {
	glPushMatrix();
		glRotatef(-5, 1, 0, 0);
		glTranslatef(0.4, -1, 0.1);
		glColor3f(0.05, 0.05, 0.05);

		glBegin(GL_QUAD_STRIP);
			glNormal3f(-0.333, 0.333, 0.333); glVertex3f(-0.2, 0, 0.1);
			glNormal3f(-0.333, 0.333, 0.333); glVertex3f(-0.2, 0.05, 0.1);

			glNormal3f(0.333, 0.333, 0.333); glVertex3f(0, 0, 0.1);
			glNormal3f(0.333, 0.333, 0.333); glVertex3f(0, 0.05, 0.1);

			glNormal3f(0.333, 0.333, -0.333); glVertex3f(0, 0, -0.1);
			glNormal3f(-0.333, 0.333, -0.333); glVertex3f(0, 0.05, -0.1);

			glNormal3f(-0.333, 0.333, -0.333); glVertex3f(-0.2, 0, -0.1);
			glNormal3f(-0.333, 0.333, -0.333); glVertex3f(-0.2, 0.05, -0.1);

			glNormal3f(-0.333, 0.333, 0.333); glVertex3f(-0.2, 0, 0.1);
			glNormal3f(-0.333, 0.333, 0.333); glVertex3f(-0.2, 0.05, 0.1);
		glEnd();

		glBegin(GL_POLYGON);
			glNormal3f(-0.333, 0.333, 0.333); glVertex3f(-0.2, 0, 0.1);
			glNormal3f(0.333, 0.333, 0.333); glVertex3f(0, 0, 0.1);
			glNormal3f(-0.333, 0.333, -0.333); glVertex3f(0, 0, -0.1);
			glNormal3f(-0.333, 0.333, -0.333); glVertex3f(-0.2, 0, -0.1);
		glEnd();

		glBegin(GL_POLYGON);
			glNormal3f(-0.333, 0.333, 0.333); glVertex3f(-0.2, 0.05, 0.1);
			glNormal3f(0.333, 0.333, 0.333); glVertex3f(0, 0.05, 0.1);
			glNormal3f(0.333, 0.333, -0.333); glVertex3f(0, 0.05, -0.1);
			glNormal3f(-0.333, 0.333, -0.333); glVertex3f(-0.2, 0.05, -0.1);
		glEnd();
	glPopMatrix();
}
void Bicicleta() {
	glPushMatrix();
		glRotatef(alfa, 0, 1, 0);
		glTranslatef(x, 0.9, 0);
		piesaOrizontalSus();
		piesaVerticalStanga();
		piesaOblicDreapta();
		piesaVerticalDreapta();
		piesaOrizontalJos();
		piesaOblicStanga();
		RoataStanga();
		Lant();
		Pedala();
	glPopMatrix();

	glPushMatrix();
		glRotatef(alfa, 0, 1, 0);
		glTranslatef(x, 0.9, 0);
		RoataDreapta();
		Coarne();
	glPopMatrix();


	glPushMatrix();
		glRotatef(alfa, 0, 1, 0);
		glTranslatef(xnear, 0.9, 0);
		piesaRoataDreaptaNear();//ROATA DREAPTA
		piesaRoataStangaNear();//ROATA STANGA
	glPopMatrix();

	glPushMatrix();
		glRotatef(alfa, 0, 1, 0);
		glTranslatef(xfar, 0.9, 0);
		piesaRoataDreaptaFar();//ROATA DREAPTA
		piesaRoataStangaFar();//ROATA STANGA
	glPopMatrix();

	glDisable(GL_AUTO_NORMAL);

	glPushMatrix();
		glRotatef(alfa, 0, 1, 0);
		glTranslatef(x, 0.9, 0);
		Sa();
	glPopMatrix();

}


void CALLBACK display(void) {

	GLfloat matUmbra[4][4];

	GLfloat puncte[3][3] = {
		{ -2.0f, -2.0f, -2.0f },
		{ -2.0f, -2.0f, 2.0f },
		{ 2.0f, -2.0f, 2.0f } };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	MatriceUmbra(puncte, position, matUmbra);

	glPushMatrix();
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		glDisable(GL_AUTO_NORMAL);
		Scena();
		Drum();
		umbra = 0;
		glEnable(GL_AUTO_NORMAL);
		Bicicleta();
	glPopMatrix();

	//deseneaza umbra
	//mai intai se dezactiveaza iluminarea si se salveaza starea matricei de proiectie
	glPushMatrix();
		glPushAttrib(GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);
			glPushMatrix();
				glMultMatrixf((GLfloat*)matUmbra);// se inmulteste matricea curenta cu matricea de umbrire
				glTranslatef(x, -1, 0);
				umbra = 1;
				Bicicleta();
			glPopMatrix();
			//deseneaza sursa de lumina ca o sfera mica galbena in pozitia reala
			glPushMatrix();
				glTranslatef(position[0], position[1], position[2]);
				glRotatef(beta, 0, 1, 0);
				glColor3f(1.0, 0.9, 0);
				auxSolidSphere(0.3);
			glPopMatrix();
			//reseteaza starea variabilelor de iluminare
		glPopAttrib();
	glPopMatrix();

	auxSwapBuffers();
}

void CALLBACK myReshape(GLsizei w, GLsizei h)
{
	if (!h) return;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
	//glRotatef(45, 1, 0, 0);
}

int main(int argc, char** argv)
{
	auxInitDisplayMode(AUX_DOUBLE | AUX_DEPTH16 | AUX_RGB);
	auxInitPosition(0, 0, 1200, 800);
	auxInitWindow("Bicicleta");
	myinit();

	auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSEDOWN, mutaBicicletaFata);
	auxMouseFunc(AUX_RIGHTBUTTON, AUX_MOUSEDOWN, mutaBicicletaSpate);

	auxKeyFunc(AUX_a, rotateYleft);
	auxKeyFunc(AUX_d, rotateYright);

	auxKeyFunc(AUX_q, mutaSursaStanga);
	auxKeyFunc(AUX_e, mutaSursaDreapta);

	auxKeyFunc(AUX_w, animatie1);
	auxKeyFunc(AUX_s, animatie2);

	auxReshapeFunc(myReshape);
	auxMainLoop(display);
	return(0);
}

