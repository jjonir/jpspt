#include <GL/glew.h>
#include <GL/glut.h>

#include "shaders.h"
#include "entry.h"

void (*enterFuncCallback)(const char *str);
char enteredStr[1024]; /* TODO fixed size buf */
int enteredChars;

static void displayString(int x, int y, const char *str);
static void entryDisplayFunc(void);
static void entryKeyboardFunc(unsigned char key, int x, int y);
static void entryReshapeFunc(int w, int h);

void stringEntryMode(void (*enterFunc)(const char *str))
{
	glutDisplayFunc(entryDisplayFunc);
	glutKeyboardFunc(entryKeyboardFunc);
	glutReshapeFunc(entryReshapeFunc);
	entryReshapeFunc(glutGet(GLUT_WINDOW_WIDTH),
				glutGet(GLUT_WINDOW_HEIGHT));

	enterFuncCallback = enterFunc;
	enteredStr[0] = '\0';
	enteredChars = 0;

	glutPostRedisplay();
}

static void displayString(int x, int y, const char *str)
{
	int i = 0;
	int h = glutGet(GLUT_WINDOW_HEIGHT);

	glRasterPos2f(x, h - y);
	for (i = 0; str[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);
	}
}

void entryDisplayFunc(void)
{
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	displayString(1, 80, enteredStr);
	glutSwapBuffers();
}

void entryKeyboardFunc(unsigned char key, int x, int y)
{
	(void)x; (void)y;
	if (key == 0x1B) {
		shaderDisplayMode();
	} else if ((key == '\n') || (key == '\r')) {
		(*enterFuncCallback)(enteredStr);
		shaderDisplayMode();
	} else if (key == 0x08) {
		if (enteredChars > 0) {
			enteredChars--;
			enteredStr[enteredChars] = '\0';
		}
	} else {
		enteredStr[enteredChars] = key;
		enteredChars++;
		enteredStr[enteredChars] = '\0';
	}
	glutPostRedisplay();
}

void entryReshapeFunc(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	glOrtho(0, w, 0, h, -10, 10); /* TODO z range is arbitrary */
	glMatrixMode(GL_MODELVIEW);
}
