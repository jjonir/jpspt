#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "shaders.h"
#include "entry.h"

GLuint shaderProgram, vertexShader, fragmentShader;
GLuint outlineShaderProgram, outlineVertexShader, outlineFragmentShader;
char vertexFile[1024]; /* TODO */
char fragmentFile[1024]; /* TODO dangerous (and wasteful) fixed-size buffers */
char outlineVertexFile[1024];
char outlineFragmentFile[1024];

static int loadVertexShaderFromFile(const char *filename);
static int loadFragmentShaderFromFile(const char *filename);
static int loadOutlineVertexShaderFromFile(const char *filename);
static int loadOutlineFragmentShaderFromFile(const char *filename);
static int loadShaderFromFile(const char *filename, GLuint shader);
static int loadShaderFromFiles(GLuint shader, int n, ...);
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

	outlineShaderProgram = glCreateProgram();
	outlineVertexShader = glCreateShader(GL_VERTEX_SHADER);
	outlineFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
}

void loadShaders(const char *vshad, const char *fshad)
{
	if ((loadVertexShaderFromFile(vshad) == 0) &&
		(loadFragmentShaderFromFile(fshad) == 0)) {
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		if (link(shaderProgram) == 0)
			glUseProgram(shaderProgram);
	}
	/*TODO consider not calling glUseProgram, but instead returning a success/failure indicator*/
}

void loadOutlineShaders(const char *vshad, const char *fshad)
{
	if ((loadOutlineVertexShaderFromFile(vshad) == 0) &&
		(loadOutlineFragmentShaderFromFile(fshad) == 0)) {
		glAttachShader(outlineShaderProgram, outlineVertexShader);
		glAttachShader(outlineShaderProgram, outlineFragmentShader);
		if (link(outlineShaderProgram) == 0)
			glUseProgram(outlineShaderProgram);
	}
	/*TODO see above*/
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
	loadOutlineShaders(outlineVertexFile, outlineFragmentFile);
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
	/*return loadShaderFromFile(filename, vertexShader);*/
	return loadShaderFromFiles(vertexShader, 2, filename, "shaders/noise4D.glsl");
}

int loadFragmentShaderFromFile(const char *filename)
{
	fprintf(stderr, "Loading fragment shader:\n");
	strncpy(fragmentFile, filename, 1024);
	return loadShaderFromFile(filename, fragmentShader);
}

int loadOutlineVertexShaderFromFile(const char *filename)
{
	strncpy(outlineVertexFile, filename, 1024);
	/*return loadShaderFromFile(filename, outlineVertexShader);*/
	return loadShaderFromFiles(outlineVertexShader, 2, filename, "shaders/noise4D.glsl");
}

int loadOutlineFragmentShaderFromFile(const char *filename)
{
	strncpy(outlineFragmentFile, filename, 1024);
	return loadShaderFromFile(filename, outlineFragmentShader);
}

int loadShaderFromFile(const char *filename, GLuint shader)
{
	char *shaderText;
	FILE *shaderFile;
	GLint len;

/* TODO read as binary because of \r\n, maybe there's a better way.*/
	if ((shaderFile = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "failed to open shader source file: %s\n", filename);
		return 1;
	}
/* TODO catch errors in file reading*/
	fseek(shaderFile, 0, SEEK_END);
	len = ftell(shaderFile);
	rewind(shaderFile);
/* TODO this leaks memory */
	shaderText = (char *)malloc(len+1);
	fread(shaderText, 1, len, shaderFile);
	shaderText[len]=0;
	fclose(shaderFile);

	glShaderSource(shader, 1, (const char **)&shaderText, &len);

	if (compile(shader) == 0) {
		fprintf(stderr, "loaded and compiled successfully.\n");
		return 0;
	} else {
		return 1;
	}
}

static int loadShaderFromFiles(GLuint shader, int n, ...)
{
	char **shaderText;
	FILE *shaderFile;
	GLint *len;
	va_list filenames;
	int i;
	const char *filename;

	shaderText = (char **)malloc(n * sizeof(char *));
	len = (GLint *)malloc(n * sizeof(GLint));
	va_start(filenames, n);
	for(i = 0; i < n; i++) {
		filename = va_arg(filenames, const char *);

/* TODO read as binary because of \r\n, maybe there's a better way.*/
		if ((shaderFile = fopen(filename, "rb")) == NULL) {
			fprintf(stderr, "failed to open shader source file: %s\n", filename);
			return 1;
		}
/* TODO catch errors in file reading*/
		fseek(shaderFile, 0, SEEK_END);
		len[i] = ftell(shaderFile);
		rewind(shaderFile);
/* TODO this leaks memory */
		shaderText[i] = (char *)malloc(len[i]+1);
		fread(shaderText[i], 1, len[i], shaderFile);
		shaderText[i][len[i]]=0;
		fclose(shaderFile);
	}
	va_end(filenames);
	
	glShaderSource(shader, n, (const char **)shaderText, len);

	return compile(shader);
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

void shaderDrawGeom(void)
{
	glutSolidTorus(0.05, 0.1, 40, 40);
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
	updateShaderUniforms();

	glClearColor(0.2, 0.5, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0, 0, -1.0);
	glRotatef(20 * (float)glutGet(GLUT_ELAPSED_TIME) / 1000, 0, 1, 0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glUseProgram(shaderProgram);
	updateShaderUniforms();
	shaderDrawGeom();

	glLineWidth(10);
	glPolygonMode(GL_BACK, GL_LINE);
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);
	glUseProgram(outlineShaderProgram);
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
	else if (key == 'f')
		stringEntryMode(loadFragmentShader);
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
