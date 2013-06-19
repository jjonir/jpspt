#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#if !defined(_WIN32) || defined(WITH_DIRENT)
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#endif

#include "animation.h"
#include "shaders.h"
#include "entry.h"

void (*enterFuncCallback)(const char *str);
char enteredStr[1024]; /* TODO fixed size buf */
int enteredChars;

#if !defined(_WIN32) || defined(WITH_DIRENT)
static void displayCompletions(const char *str);
#endif
static void displayString(int x, int y, const char *str);
static void entryDisplayFunc(void);
static void entryKeyboardFunc(unsigned char key, int x, int y);
static void entryReshapeFunc(int w, int h);

void stringEntryMode(void (*enterFunc)(const char *str))
{
	glUseProgram(0);

	glutDisplayFunc(entryDisplayFunc);
	glutKeyboardFunc(entryKeyboardFunc);
	glutMouseFunc(NULL);
	glutMotionFunc(NULL);
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
	int i;

	glRasterPos2i(x, y);
	for (i = 0; str[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);
	}
}

#if !defined(_WIN32) || defined(WITH_DIRENT)
static void displayCompletions(const char *str)
{
	const char dot[4] = ".";
	const char *dirName;
	char name[1024];
	char *s;
	DIR *dirp;
	struct dirent *dp;
	int i;

	strncpy(name, str, 1024);
	s = strrchr(name, '/');
	if (s != NULL) {
		dirName = name;
		s[0] = '\0';
		s++;
	} else {
		dirName = dot;
		s = name;
	}

	if ((dirp = opendir(dirName)) == NULL) {
		fprintf(stderr, "opendir (%s)", dirName);
		perror(" ");
		return; /* TODO display 'no such dir' in gl instead */
	}

	i = 0;
	do {
		if ((dp = readdir(dirp)) != NULL) {
			if (strncmp(dp->d_name, s, strlen(s)) == 0) {
/* TODO display foo/ if foo is a directory */
				displayString(10, (2 + i) * 20, dp->d_name);
				i++;
			}
		}
		/* TODO something on readdir error */
	} while (dp != NULL);

	closedir(dirp); /* TODO closedir error? */
}
#endif

void entryDisplayFunc(void)
{
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	displayString(1, 20, enteredStr);
#if !defined(_WIN32) || defined(WITH_DIRENT)
	displayCompletions(enteredStr);
#endif

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
	glOrtho(0, w, h, 0, -1, 1); /* TODO z range is arbitrary */
	glMatrixMode(GL_MODELVIEW);
}
