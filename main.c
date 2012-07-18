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
	loadShaders("vshad.txt", "fshad.txt");

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

	return 0;
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 0x1B)
		exit(0);
}

void specialKey(int key, int x, int y)
{
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
	//gluPerspective(45, ratio, 0.1, 1000);
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
}

void display(void)
{
	float t;

	t = (float)glutGet(GLUT_ELAPSED_TIME) / 1000;

	updateShaderTime(t);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_QUADS);
		glVertex3f(-1, -1, -1);
		glVertex3f( 1, -1, -1);
		glVertex3f( 1,  1, -1);
		glVertex3f(-1,  1, -1);
	glEnd();

	glutSwapBuffers();

	glutPostRedisplay();
}
