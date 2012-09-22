#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shaders.h"

GLuint shaderProgram, vertexShader, fragmentShader;
char vertexFile[1024]; // TODO
char fragmentFile[1024]; // TODO dangerous fixed-size buffers

static int loadVertexShaderFromFile(const char *filename);
static int loadFragmentShaderFromFile(const char *filename);
static int loadShaderFromFile(const char *filename, GLenum type);
static int compile(GLuint shader);
static int link(GLuint program);
static void shaderDisplayFunc(void);
static void shaderKeyboardFunc(unsigned char key, int x, int y);
static void shaderReshapeFunc(int w, int h);

void initShaders(void)
{
	shaderProgram = glCreateProgram();
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
}

void loadShaders(const char *vshad, const char *fshad)
{
	int status = 0;
	if ((status = loadVertexShaderFromFile(vshad)) == 0)
		glAttachShader(shaderProgram, vertexShader);
	if ((status = loadFragmentShaderFromFile(fshad)) == 0)
		glAttachShader(shaderProgram, fragmentShader);
	if ((status == 0) && (link(shaderProgram) == 0))
		glUseProgram(shaderProgram);
}

void loadVertexShader(const char *vshad)
{
	int status = 0;
	if ((status = loadVertexShaderFromFile(vshad)) == 0)
		glAttachShader(shaderProgram, vertexShader);
	if ((status == 0) && (link(shaderProgram) == 0))
		glUseProgram(shaderProgram);
}

void loadFragmentShader(const char *fshad)
{
	int status = 0;
	if ((status = loadFragmentShaderFromFile(fshad)) == 0)
		glAttachShader(shaderProgram, fragmentShader);
	if ((status == 0) && (link(shaderProgram) == 0))
		glUseProgram(shaderProgram);
}

void reloadShaders(void)
{
	loadShaders(vertexFile, fragmentFile);
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

void updateShaderUniforms(void)
{
	GLint var;
	float t;
	int width, height;

	t = (float)glutGet(GLUT_ELAPSED_TIME) / 1000;
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);

	var = glGetUniformLocation(shaderProgram, "time");
	glUniform1f(var, t);

	var = glGetUniformLocation(shaderProgram, "resolution");
	glUniform2i(var, width, height);
}

int loadVertexShaderFromFile(const char *filename)
{
	fprintf(stderr, "Loading vertex shader:\n");
	strncpy(vertexFile, filename, 1024);
	return loadShaderFromFile(filename, vertexShader);
}

int loadFragmentShaderFromFile(const char *filename)
{
	fprintf(stderr, "Loading fragment shader:\n");
	strncpy(fragmentFile, filename, 1024);
	return loadShaderFromFile(filename, fragmentShader);
}

int loadShaderFromFile(const char *filename, GLuint shader)
{
	char *shaderText;
	FILE *shaderFile;
	GLint len;

	// TODO read as binary because of \r\n, maybe there's a better way.
	if ((shaderFile = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "failed to open shader source file: %s\n", filename);
		return 1;
	}
// TODO catch errors in file reading
	fseek(shaderFile, 0, SEEK_END);
	len = ftell(shaderFile);
	rewind(shaderFile);
	shaderText = (char *)malloc(len+1);
	fread(shaderText, 1, len, shaderFile);
	shaderText[len]=0;
	fclose(shaderFile);

	glShaderSource(shader, 1, &shaderText, &len);

	if (compile(shader) == 0) {
		fprintf(stderr, "loaded and compiled successfully.\n");
		return 0;
	} else {
		return 1;
	}
}

static int compile(GLuint shader)
{
	int status;
	char *infoLog;
	int logLen;

	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		infoLog = (char *)malloc(logLen * sizeof(char));
		glGetShaderInfoLog(shader, logLen, NULL, infoLog);
		fprintf(stderr, "Shader failed to compile:\n%s\n", infoLog);
		return 1;
	}

	return 0;
}

static int link(GLuint program)
{
	int status;
	char *infoLog;
	int logLen;

	fprintf(stderr, "Linking shaders:\n");

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		infoLog = (char *)malloc(logLen * sizeof(char));
		glGetProgramInfoLog(program, logLen, NULL, infoLog);
		fprintf(stderr, "Program failed to link:\n%s\n", infoLog);
		return 1;
	}

	fprintf(stderr, "linked successfully.\n");
	return 0;
}

void shaderDisplayFunc(void)
{
	int res = 100;
	int i, j;

	updateShaderUniforms();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_QUADS);
		for (i = 0; i < res; i++) {
			for (j = 0; j < res; j++) {
				glVertex3f(-1+2*i/res,     -1+2*j/res, -1);
				glVertex3f(-1+2*(i+1)/res, -1+2*j/res, -1);
				glVertex3f(-1+2*(i+1)/res, -1+2*(j+1)/res, -1);
				glVertex3f(-1+2*i/res,     -1+2*(j+1)/res, -1);
			}
		}
/*
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
*/
	glEnd();
	glRotatef(0, 0, 1, glutGet(GLUT_ELAPSED_TIME));

	glutSwapBuffers();

	glutPostRedisplay();
}

void shaderKeyboardFunc(unsigned char key, int x, int y)
{
	if (key == 0x1B)
		exit(0);
	else if (key == 'v')
		stringEntryMode(loadVertexShader);
	else if (key == 'f')
		stringEntryMode(loadFragmentShader);
	else if (key == 'u')
		;//addShaderUniform();
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
