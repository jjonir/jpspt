#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "shaders.h"

GLuint shaderProgram, vertexShader, fragmentShader;
GLuint outlineShaderProgram, outlineVertexShader, outlineFragmentShader;
char vertexFile[1024]; /* TODO*/
char fragmentFile[1024]; /* TODO dangerous (and wasteful) fixed-size buffers*/
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
	if (loadVertexShaderFromFile(vshad) == 0)
		glAttachShader(shaderProgram, vertexShader);
	if (loadFragmentShaderFromFile(fshad) == 0)
		glAttachShader(shaderProgram, fragmentShader);
	if (link(shaderProgram) == 0)
		glUseProgram(shaderProgram);
}

void loadOutlineShaders(const char *vshad, const char *fshad)
{
	if (loadOutlineVertexShaderFromFile(vshad) == 0)
		glAttachShader(outlineShaderProgram, outlineVertexShader);
	if (loadOutlineFragmentShaderFromFile(fshad) == 0)
		glAttachShader(outlineShaderProgram, outlineFragmentShader);
	if (link(outlineShaderProgram) == 0)
		glUseProgram(outlineShaderProgram);
}

void reloadShaders(void)
{
	loadShaders(vertexFile, fragmentFile);
	loadOutlineShaders(outlineVertexFile, outlineFragmentFile);
}

void updateShaderTime(float t)
{
	GLint t_var;

	t_var = glGetUniformLocation(shaderProgram, "time");
	glUniform1f(t_var, t);
}

int loadVertexShaderFromFile(const char *filename)
{
	strncpy(vertexFile, filename, 1024);
	/*return loadShaderFromFile(filename, vertexShader);*/
	return loadShaderFromFiles(vertexShader, 2, filename, "shaders/noise4D.glsl");
}

int loadFragmentShaderFromFile(const char *filename)
{
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

	return compile(shader);
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

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		infoLog = (char *)malloc(logLen * sizeof(char));
		glGetProgramInfoLog(program, logLen, NULL, infoLog);
		fprintf(stderr, "Program failed to link:\n%s\n", infoLog);
		return 1;
	}

	return 0;
}

