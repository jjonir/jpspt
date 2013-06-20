#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <math.h>

#include "shaders.h"

struct shader_program {
	GLuint program;
	GLuint vshad, fshad;
	char *name;
	char *vshadName, *fshadName;
	char *vshadFileName, *fshadFileName;
};

static void initShader(struct shader_program *p);
static int loadShaders(struct shader_program *p, const char *vshad, const char *fshad);
static int loadShaderFromFile(GLuint shader, const char *filename);
static int loadShaderFromFiles(GLuint shader, int n, ...);
static int compile(GLuint shader);
static int link(GLuint program);

struct shader_program *newShader(const char *vshad, const char *fshad)
{
	struct shader_program *p;

	p = (struct shader_program *)malloc(sizeof(struct shader_program));
	initShader(p);
	loadShaders(p, vshad, fshad);

	return p;
}

void initShader(struct shader_program *p)
{
	memset(p, 0, sizeof(struct shader_program));

	p->program = glCreateProgram();
	p->vshad = glCreateShader(GL_VERTEX_SHADER);
	glAttachShader(p->program, p->vshad);
	p->fshad = glCreateShader(GL_FRAGMENT_SHADER);
	glAttachShader(p->program, p->fshad);
}

int loadShaders(struct shader_program *p, const char *vshad, const char *fshad)
{
	int status;

	fprintf(stderr, "Loading vertex shader %s...\n", vshad);
	if (p->vshadFileName)
		free(p->vshadFileName);
	p->vshadFileName = (char *)malloc(sizeof(char) * (strlen(vshad) + 1));
	strcpy(p->vshadFileName, vshad);
	status = loadShaderFromFiles(p->vshad, 2, vshad, "shaders/noise4D.glsl");

	fprintf(stderr, "Loading fragment shader %s...\n", fshad);
	if (p->fshadFileName)
		free(p->fshadFileName);
	p->fshadFileName = (char *)malloc(sizeof(char) * (strlen(fshad) + 1));
	strcpy(p->fshadFileName, fshad);
	status |= loadShaderFromFile(p->fshad, fshad);

	if (status == 0)
		status = link(p->program);

	return status;
}

void reloadShaders(struct shader_program *p)
{
	loadShaders(p, p->vshadFileName, p->fshadFileName);
}

int changeVertexShader(struct shader_program *p, const char *vshad)
{
	int status;

	fprintf(stderr, "Loading vertex shader %s...\n", vshad);
	if (p->vshadFileName)
		free(p->vshadFileName);
	p->vshadFileName = (char *)malloc(sizeof(char) * (strlen(vshad) + 1));
	strcpy(p->vshadFileName, vshad);

	status = loadShaderFromFiles(p->vshad, 2, vshad, "shaders/noise4D.glsl");
	if (status == 0)
		status = link(p->program);

	return status;
}

int changeFragmentShader(struct shader_program *p, const char *fshad)
{
	int status;

	fprintf(stderr, "Loading fragment shader %s...\n", fshad);
	if (p->fshadFileName)
		free(p->fshadFileName);
	p->fshadFileName = (char *)malloc(sizeof(char) * (strlen(fshad) + 1));
	strcpy(p->fshadFileName, fshad);

	status = loadShaderFromFile(p->fshad, fshad);
	if (status == 0)
		status = link(p->program);

	return status;
}

void switchToShader(struct shader_program *p)
{
	glUseProgram(p->program);
}

void updateShaderUniforms(struct shader_program *p)
{
	GLint var;
	float t;
	int width, height;

	glUseProgram(p->program);

	t = (float)glutGet(GLUT_ELAPSED_TIME) / 1000;
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);

	var = glGetUniformLocation(p->program, "time");
	if (var != -1)
		glUniform1f(var, t);

	var = glGetUniformLocation(p->program, "resolution");
	if (var != -1)
		glUniform2i(var, width, height);

	var = glGetUniformLocation(p->program, "pos");
	if (var != -1)
		glUniform3f(var, 15.0 * cos(t / 10.0),
				15.0 * sin(t / 10.0),
				3.0 * sin(t / 4.0));

	var = glGetUniformLocation(p->program, "fade");
	if (var != -1)
		glUniform1f(var, 1.0);

	var = glGetUniformLocation(p->program, "light");
	if (var != -1)
		glUniform3f(var, 10.0, 10.0, 10.0);
}

int loadShaderFromFile(GLuint shader, const char *filename)
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
		fprintf(stderr, "Loaded and compiled successfully.\n");
		return 0;
	} else {
		return 1;
	}
}

int loadShaderFromFiles(GLuint shader, int n, ...)
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

int compile(GLuint shader)
{
	int status;
	char *infoLog;
	int logLen;

	fprintf(stderr, "\tCompiling shader...\n");

	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		infoLog = (char *)malloc(logLen * sizeof(char));
		glGetShaderInfoLog(shader, logLen, NULL, infoLog);
		fprintf(stderr, "Shader failed to compile:\n%s\n", infoLog);
		return 1;
	}

	fprintf(stderr, "\tCompiled successfully.\n");

	return 0;
}

int link(GLuint program)
{
	int status;
	char *infoLog;
	int logLen;

	fprintf(stderr, "Linking shader program...\n");

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		infoLog = (char *)malloc(logLen * sizeof(char));
		glGetProgramInfoLog(program, logLen, NULL, infoLog);
		fprintf(stderr, "Program failed to link:\n%s\n", infoLog);
		return 1;
	}

	fprintf(stderr, "Linked successfully.\n");
	return 0;
}
