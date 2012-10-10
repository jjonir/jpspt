#include <GL/glew.h>
#include <GL/glut.h>

#include "animation.h"
#include "shaders.h"
#include "entry.h"

shader_program_t fill;
shader_program_t outline;

static void loadVertexShader(const char *vshad);
static void loadBothVertexShaders(const char *vshad);
static void loadFragmentShader(const char *fshad);
static void loadOutlineFragmentShader(const char *fshad);
static void shaderDrawGeom(void);
static void shaderDisplayFunc(void);
static void shaderKeyboardFunc(unsigned char key, int x, int y);
static void shaderReshapeFunc(int w, int h);

void initShaders(void)
{
	fill = newShader("shaders/wiggle.vshad", "shaders/colors.fshad");
	outline = newShader("shaders/wiggle.vshad", "shaders/black.fshad");
}

void loadVertexShader(const char *vshad)
{
	changeVertexShader(fill, vshad);
}

void loadBothVertexShaders(const char *vshad)
{
	changeVertexShader(fill, vshad);
	changeVertexShader(outline, vshad);
}

void loadFragmentShader(const char *fshad)
{
	changeFragmentShader(fill, fshad);
}

void loadOutlineFragmentShader(const char *fshad)
{
	changeFragmentShader(outline, fshad);
}

void reloadAllShaders(void)
{
	reloadShaders(fill);
	reloadShaders(outline);
}

void shaderDisplayMode(void)
{
	glutDisplayFunc(shaderDisplayFunc);
	glutKeyboardFunc(shaderKeyboardFunc);
	glutReshapeFunc(shaderReshapeFunc);
	shaderReshapeFunc(glutGet(GLUT_WINDOW_WIDTH),
				glutGet(GLUT_WINDOW_HEIGHT));

	glutPostRedisplay();
}

void shaderDrawGeom(void)
{
	glutSolidTorus(0.1, 0.5, 90, 90);
	/*glutSolidTorus(0.05, 0.1, 4, 40);*/
	/*glutSolidCone(0.1, 0.2, 20, 20);*/
	/*
	int res = 100;
	int i, j;

	glBegin(GL_QUADS);
		for (i = 0; i < res; i++) {
			for (j = 0; j < res; j++) {
				glVertex3f(-1+2*i/res,     -1+2*j/res, -1);
				glVertex3f(-1+2*(i+1)/res, -1+2*j/res, -1);
				glVertex3f(-1+2*(i+1)/res, -1+2*(j+1)/res, -1);
				glVertex3f(-1+2*i/res,     -1+2*(j+1)/res, -1);
			}
		}
	glEnd();
	*/
	/*
	glBegin(GL_QUADS);
		glVertex3f(-1, -1, -1);
		glVertex3f( 1, -1, -1);
		glVertex3f( 1,  1, -1);
		glVertex3f(-1,  1, -1);
	*/
	/*
		glVertex3f( 1, -1, -1);
		glVertex3f( 1, -1, -3);
		glVertex3f( 1,  1, -3);
		glVertex3f( 1,  1, -1);

		glVertex3f( 1, -1, -3);
		glVertex3f(-1, -1, -3);
		glVertex3f(-1,  1, -3);
		glVertex3f( 1,  1, -3);

		glVertex3f(-1, -1, -3);
		glVertex3f(-1, -1, -1);
		glVertex3f(-1,  1, -1);
		glVertex3f(-1,  1, -3);

		glVertex3f(-1,  1, -1);
		glVertex3f( 1,  1, -1);
		glVertex3f( 1,  1, -3);
		glVertex3f(-1,  1, -3);

		glVertex3f(-1,  1, -1);
		glVertex3f(-1,  1, -3);
		glVertex3f( 1,  1, -3);
		glVertex3f( 1,  1, -1);
	glEnd();
	*/
}

void shaderDisplayFunc(void)
{
	glClearColor(0.2, 0.5, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0, 0, -2.0);
	glRotatef(20 * (float)glutGet(GLUT_ELAPSED_TIME) / 1000, 0, 1, 0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	switchToShader(fill);
	shaderDrawGeom();

	glLineWidth(10);
	glPolygonMode(GL_BACK, GL_LINE);
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);
	switchToShader(outline);
	shaderDrawGeom();

	glutSwapBuffers();

	glutPostRedisplay();
}

void shaderKeyboardFunc(unsigned char key, int x, int y)
{
	(void)x; (void)y;
	if (key == 0x1B)
		exit(0);
	else if (key == 'v')
		stringEntryMode(loadVertexShader);
	else if (key == 'V')
		stringEntryMode(loadBothVertexShaders);
	else if (key == 'f')
		stringEntryMode(loadFragmentShader);
	else if (key == 'o')
		stringEntryMode(loadOutlineFragmentShader);
	else if (key == 'u')
		{}/*addShaderUniform();*/
}

void shaderReshapeFunc(int w, int h)
{
	float ratio;

	if (h == 0)
		ratio = 0;
	else
		ratio = (float)w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45, ratio, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
}
