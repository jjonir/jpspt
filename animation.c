#include <GL/glew.h>
#ifdef USING_FREEGLUT
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

#include "animation.h"
#include "shaders.h"
#include "entry.h"
#include "geom.h"

shader_program_t fill;
shader_program_t outline;

static void loadVertexShader(const char *vshad);
static void loadBothVertexShaders(const char *vshad);
static void loadFragmentShader(const char *fshad);
static void loadOutlineFragmentShader(const char *fshad);
static void loadGeom(const char *geom);
static void shaderDrawGeom(void);
static void shaderDisplayFunc(void);
static void shaderKeyboardFunc(unsigned char key, int x, int y);
static void shaderMouseFunc(int button, int state, int x, int y);
static void shaderMotionFunc(int x, int y);
static void shaderReshapeFunc(int w, int h);

int rotating = 0, panning = 0;
float panx = 0, pany = 0, rotx = 0, roty = 0;
int prevx = 0, prevy = 0;

struct face *customGeom = NULL;
int customGeomCount = 0;

void initShaders(void)
{
	fill = newShader("shaders/spin.vshad", "shaders/phong.fshad");
	outline = newShader("shaders/spin.vshad", "shaders/phong.fshad");
}

void loadVertexShader(const char *vshad)
{
	changeVertexShader(fill, vshad);
}

void loadBothVertexShaders(const char *vshad)
{
	changeVertexShader(fill, vshad);
	changeVertexShader(outline, vshad);
}

void loadFragmentShader(const char *fshad)
{
	changeFragmentShader(fill, fshad);
}

void loadOutlineFragmentShader(const char *fshad)
{
	changeFragmentShader(outline, fshad);
}

void loadGeom(const char *geom)
{
	if (customGeom != NULL)
		free(customGeom);
	customGeom = read_obj_file(geom, &customGeomCount);
}

void reloadAllShaders(void)
{
	reloadShaders(fill);
	reloadShaders(outline);
}

void shaderDisplayMode(void)
{
	glutDisplayFunc(shaderDisplayFunc);
	glutKeyboardFunc(shaderKeyboardFunc);
	glutMouseFunc(shaderMouseFunc);
	glutMotionFunc(shaderMotionFunc);
	glutReshapeFunc(shaderReshapeFunc);
	shaderReshapeFunc(glutGet(GLUT_WINDOW_WIDTH),
				glutGet(GLUT_WINDOW_HEIGHT));

	glutPostRedisplay();
}

enum geoms {
	TORUS1, TORUS2, TORUS3, CONE, GRID, BOX, QUAD, CUBE, SPHERE,
	TETRAHEDRON, OCTAHEDRON, DODECAHEDRON, ICOSAHEDRON,
#ifdef USING_FREEGLUT
	RHOMBIC_DODECAHEDRON,
	CYLINDER, CYLINDER2, CYLINDER3,
/*
	TEACUP, TEASPOON,
*/
#endif
	TEAPOT,
	CUSTOM,
	NUM_GEOMS
};
int geom = TORUS1;
void shaderDrawGeom(void)
{
	int res = 100;
	int i, j;

	switch (geom) {
	case TORUS1:
		glutSolidTorus(0.3, 0.8, 50, 50);
		break;
	case TORUS2:
		glutSolidTorus(0.1, 0.5, 90, 90);
		break;
	case TORUS3:
		glutSolidTorus(0.05, 0.1, 4, 40);
		break;
	case CONE:
		glutSolidCone(0.1, 0.2, 20, 20);
		break;
	case GRID:
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
		break;
	case BOX:
		glBegin(GL_QUADS);
			glVertex3f(-1, -1, -1);
			glVertex3f( 1, -1, -1);
			glVertex3f( 1,  1, -1);
			glVertex3f(-1,  1, -1);

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
		break;
	case QUAD:
		glBegin(GL_QUADS);
			glVertex3f(-1, 1, -1);
			glVertex3f(-1, -1, -1);
			glVertex3f(1, -1, -1);
			glVertex3f(1, 1, -1);
		glEnd();
		break;
	case CUBE:
		glutSolidCube(0.5);
		break;
	case SPHERE:
		glutSolidSphere(0.5, 5, 5);
		break;
	case TETRAHEDRON:
		glutSolidTetrahedron();
		break;
	case OCTAHEDRON:
		glutSolidOctahedron();
		break;
	case DODECAHEDRON:
		glutSolidDodecahedron();
		break;
	case ICOSAHEDRON:
		glutSolidIcosahedron();
		break;
#ifdef USING_FREEGLUT
	case RHOMBIC_DODECAHEDRON:
		glutSolidRhombicDodecahedron();
		break;
	case CYLINDER:
		glutSolidCylinder(0.2, 0.5, 10, 10);
		break;
	case CYLINDER2:
		glPushMatrix();
		glRotatef(90.0, 1, 0, 0);
		glutSolidCylinder(0.2, 0.5, 10, 10);
		glPopMatrix();
		break;
	case CYLINDER3:
		glPushMatrix();
		glRotatef(45.0, 1, 0, 0);
		glutSolidCylinder(0.2, 0.5, 10, 10);
		glPopMatrix();
		break;
/*
	case TEACUP:
		glutSolidTeacup(0.5);
		break;
	case TEASPOON:
		glutSolidTeaspoon(0.5);
		break;
*/
#endif
	case TEAPOT:
		/* GLUT's teapot is inside-out, this looks terrible */
		glutSolidTeapot(0.5);
		break;
	case CUSTOM:
		if ((customGeom == NULL) || (customGeomCount == 0))
			break;
		glBegin(GL_TRIANGLES);
			for (i = 0; i < customGeomCount; i++) {
				for (j = 0; j < 3; j++) {
					glNormal3f(customGeom[i].n[j].x,
							customGeom[i].n[j].y,
							customGeom[i].n[j].z);
					glVertex3f(customGeom[i].v[j].x,
							customGeom[i].v[j].y,
							customGeom[i].v[j].z);
				}
			}
		glEnd();
		break;
	default:
		break;
	}
}

float dz = 0;
void shaderDisplayFunc(void)
{
	glClearColor(0.2, 0.5, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
/*
	glTranslatef(0, 0, -4.0);
	glTranslatef(0, 0, dz);
*/
	glTranslatef(panx, pany, -4);
	glRotatef(roty, 1, 0, 0);
	glRotatef(rotx, 0, 1, 0);
/*
	glRotatef(200 * (float)glutGet(GLUT_ELAPSED_TIME) / 1000, 0, 1, 0);
	glRotatef(-20 * (float)glutGet(GLUT_ELAPSED_TIME) / 1000, 1, 0, 0);
*/

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	updateShaderUniforms(fill);
	updateShaderUniforms(outline);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	switchToShader(fill);
	shaderDrawGeom();

	glLineWidth(10);
	glPolygonMode(GL_BACK, GL_LINE);
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);
	switchToShader(outline);
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
	else if (key == 'w')
		dz += 0.1;
	else if (key == 's')
		dz -= 0.1;
	else if (key == '\t')
		geom = (geom + 1) % NUM_GEOMS;
	else if (key == 'g')
		stringEntryMode(loadGeom);
}

void shaderMouseFunc(int button, int state, int x, int y)
{
	prevx = x;
	prevy = y;
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN)
			rotating = 1;
		else
			rotating = 0;
	} else if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN)
			panning = 1;
		else
			panning = 0;
	}
}

void shaderMotionFunc(int x, int y)
{
	int dx = x - prevx;
	int dy = y - prevy;
	prevx = x;
	prevy = y;

	if (rotating) {
		rotx += dx;
		roty += dy;
	}
	if (panning) {
		panx -= 0.1 * dx;
		pany -= 0.1 * dy;
/*
		panx -= ((float)dx * (4 + w_io->size.x) / glutGet(GLUT_WINDOW_WIDTH));
		pany -= ((float)dy * (4 + w_io->size.y) / glutGet(GLUT_WINDOW_HEIGHT));
*/
	}
	shaderReshapeFunc(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glutPostRedisplay();
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
