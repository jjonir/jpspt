#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

#include "shaders.h"

static int init(int *argc, char *argv[]);
static void keyboard(unsigned char key, int x, int y);
static void specialKey(int key, int x, int y);
static void reshape(int w, int h);
static void display(void);

int main(int argc, char *argv[])
{
	if (init(&argc, argv) != 0)
		return 1;

	initShaders();
	loadShaders("shaders/wiggle.vshad", "shaders/fshad.txt");
	loadOutlineShaders("shaders/wiggle.vshad", "shaders/outline.fshad");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKey);
	glutMainLoop();

	return 0;
}

static int init(int *argc, char *argv[])
{
	GLenum err;

	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(500, 500);
	glutCreateWindow("JP's Shader Prototyping Tool");
	glutSetCursor(GLUT_CURSOR_NONE);
	err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "GLEW init error: %s\n",
				glewGetErrorString(err));
		return 1;
	}

GLfloat params[2];
glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, params);
printf("line width range: %f -- %f\n", params[0], params[1]);

	return 0;
}

void keyboard(unsigned char key, int x, int y)
{
	(void)x; (void)y;
	if (key == 0x1B)
		exit(0);
}

void specialKey(int key, int x, int y)
{
	(void)x; (void)y;
	if (key == GLUT_KEY_F5)
		reloadShaders();
}

void reshape(int w, int h)
{
	float ratio;

	if(h == 0)
		ratio = 0;
	else
		ratio = (float)w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45, ratio, 0.1, 1000);
	/*glOrtho(0, 1, 0, 1, -1, 1);*/
	glMatrixMode(GL_MODELVIEW);
}

extern GLuint shaderProgram;
extern GLuint outlineShaderProgram;
void display(void)
{
	float t;

	t = (float)glutGet(GLUT_ELAPSED_TIME) / 1000;

	updateShaderTime(t);

	glClearColor(0.2, 0.5, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0, 0, -1.0);
	glRotatef(20*t, 0, 1, 0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glUseProgram(shaderProgram);
	updateShaderTime(t);
	/*glutSolidCone(0.1, 0.2, 20, 20);*/
	glutSolidTorus(0.05, 0.1, 40, 40);

	glLineWidth(10);
	glPolygonMode(GL_BACK, GL_LINE);
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);
	glUseProgram(outlineShaderProgram);
	updateShaderTime(t);
	/*glutSolidCone(0.1, 0.2, 20, 20);*/
	glutSolidTorus(0.05, 0.1, 40, 40);

/*
	glBegin(GL_QUADS);
		glVertex3f(-1, -1, -1);
		glVertex3f( 1, -1, -1);
		glVertex3f( 1,  1, -1);
		glVertex3f(-1,  1, -1);
	glEnd();
*/

	glutSwapBuffers();

	glutPostRedisplay();
}
