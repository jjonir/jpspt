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

void initShaders(void)
{
	shaderProgram = glCreateProgram();
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
}

void loadShaders(const char *vshad, const char *fshad)
{
	int status = 0;
	if (loadVertexShaderFromFile(vshad) == 0)
		glAttachShader(shaderProgram, vertexShader);
	if (loadFragmentShaderFromFile(fshad) == 0)
		glAttachShader(shaderProgram, fragmentShader);
	if (link(shaderProgram) == 0)
		glUseProgram(shaderProgram);
}

void reloadShaders(void)
{
	loadShaders(vertexFile, fragmentFile);
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
	return loadShaderFromFile(filename, vertexShader);
}

int loadFragmentShaderFromFile(const char *filename)
{
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

