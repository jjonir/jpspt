#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

#include "shaders.h"

static int init(int *argc, char *argv[]);
static void specialKey(int key, int x, int y);

int main(int argc, char *argv[])
{
	if (init(&argc, argv) != 0)
		return 1;

	initShaders();
	loadShaders("shaders/wiggle.vshad", "shaders/colors.fshad");
	loadOutlineShaders("shaders/wiggle.vshad", "shaders/black.fshad");

	shaderDisplayMode();

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

void specialKey(int key, int x, int y)
{
	(void)x; (void)y;
	if (key == GLUT_KEY_F5)
		reloadShaders();
	if (key == GLUT_KEY_F11)
		glutFullScreen();
}
