#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "shaders.h"
#include "entry.h"

struct shader_program {
	GLuint program;
	GLuint vshad, fshad;
	char *name;
	char *vshadName, *fshadName;
	char *vshadFileName, *fshadFileName;
};

struct shader_program fill;
struct shader_program outline;

static void initShader(struct shader_program *p);
static void loadVertexShader(const char *vshad);
static void loadFragmentShader(const char *fshad);
static void updateShaderUniforms(GLuint program);
static int loadVertexShaderFromFile(struct shader_program *p, const char *filename);
static int loadFragmentShaderFromFile(struct shader_program *p, const char *filename);
static int loadShaderFromFile(GLuint shader, const char *filename);
static int loadShaderFromFiles(GLuint shader, int n, ...);
static int compile(GLuint shader);
static int link(GLuint program);
static void shaderDisplayFunc(void);
static void shaderKeyboardFunc(unsigned char key, int x, int y);
static void shaderReshapeFunc(int w, int h);

void initShader(struct shader_program *p)
{
	memset(p, 0, sizeof(struct shader_program));

	p->program = glCreateProgram();
	p->vshad = glCreateShader(GL_VERTEX_SHADER);
	glAttachShader(p->program, p->vshad);
	p->fshad = glCreateShader(GL_FRAGMENT_SHADER);
	glAttachShader(p->program, p->fshad);
}
void initShaders(void)
{
	initShader(&fill);
	initShader(&outline);
}

int loadVertexShaderFromFile(struct shader_program *p, const char *filename)
{
	fprintf(stderr, "Loading vertex shader...\n");
	if (p->vshadFileName)
		free(p->vshadFileName);
	p->vshadFileName = (char *)malloc(sizeof(char) * (strlen(filename) + 1));
	strcpy(p->vshadFileName, filename);

	return loadShaderFromFiles(p->vshad, 2, filename, "shaders/noise4D.glsl");
}
int loadFragmentShaderFromFile(struct shader_program *p, const char *filename)
{
	fprintf(stderr, "Loading fragment shader...\n");
	if (p->fshadFileName)
		free(p->fshadFileName);
	p->fshadFileName = (char *)malloc(sizeof(char) * (strlen(filename) + 1));
	strcpy(p->fshadFileName, filename);

	return loadShaderFromFile(p->fshad, filename);
}
void loadShaders(const char *vshad, const char *fshad)
{
	if ((loadVertexShaderFromFile(&fill, vshad) == 0) &&
			(loadFragmentShaderFromFile(&fill, fshad) == 0))
		link(fill.program);
	/*TODO consider returning a success/failure indicator*/
}

void loadOutlineShaders(const char *vshad, const char *fshad)
{
	if ((loadVertexShaderFromFile(&outline, vshad) == 0) &&
			(loadFragmentShaderFromFile(&outline, fshad) == 0))
		link(outline.program);
	/*TODO see above*/
}

void loadVertexShader(const char *vshad)
{
	if (loadVertexShaderFromFile(&fill, vshad) == 0)
		link(fill.program);
}

void loadBothVertexShaders(const char *vshad)
{
	if (loadVertexShaderFromFile(&fill, vshad) == 0)
		link(fill.program);
	if (loadVertexShaderFromFile(&outline, vshad) == 0)
		link(outline.program);
}

void loadFragmentShader(const char *fshad)
{
	if (loadFragmentShaderFromFile(&fill, fshad) == 0)
		link(fill.program);
}

void loadOutlineFragmentShader(const char *fshad)
{
	if (loadFragmentShaderFromFile(&outline, fshad) == 0)
		link(outline.program);
}

void reloadShaders(void)
{
	loadShaders(fill.vshadFileName, fill.fshadFileName);
	loadOutlineShaders(outline.vshadFileName, outline.fshadFileName);
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

void updateShaderUniforms(GLuint program)
{
	GLint var;
	float t;
	int width, height;

	t = (float)glutGet(GLUT_ELAPSED_TIME) / 1000;
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);

	var = glGetUniformLocation(program, "time");
	glUniform1f(var, t);

	var = glGetUniformLocation(program, "resolution");
	glUniform2i(var, width, height);
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

static int link(GLuint program)
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

void shaderDrawGeom(void)
{
	glutSolidTorus(0.05, 0.1, 10, 10);
	/*glutSolidTorus(0.05, 0.1, 4, 40);*/
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
	glUseProgram(fill.program);
	updateShaderUniforms(fill.program);
	shaderDrawGeom();

	glLineWidth(10);
	glPolygonMode(GL_BACK, GL_LINE);
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);
	glUseProgram(outline.program);
	updateShaderUniforms(outline.program);
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
	else if (key == 'V')
		stringEntryMode(loadBothVertexShaders);
	else if (key == 'f')
		stringEntryMode(loadFragmentShader);
	else if (key == 'o')
		stringEntryMode(loadOutlineFragmentShader);
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
