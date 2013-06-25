#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "geom.h"

#define LINE_MAX 256

static int get_vertex_count(FILE *fp);
static int get_face_count(FILE *fp);
static int element_count(FILE *fp, char c);
static char line_type(const char *line);
static void vertex_read(FILE *fp, struct vertex *vertex, int count);
static void vertex_scan(const char *line, struct vertex *v);
static void face_read(FILE *fp, struct face *face, int face_count,
		struct vertex *vertex, int vertex_count);
static void face_scan(const char *line, struct face *f,
		struct vertex *v, int vertex_count);
static void compute_normal(struct face *f);
static void add_vertex(struct vertex *res, const struct vertex *a, const struct vertex *b);
static void sub_vertex(struct vertex *res, const struct vertex *a, const struct vertex *b);
static void div_vertex(struct vertex *res, const struct vertex *num, const float denom);
static void cross(struct vertex *res, const struct vertex *a, const struct vertex *b);
static void copy_vertex(struct vertex *to, const struct vertex *from);
static void compute_centroid(struct face *f);
static void normalize3(struct vertex *v);

struct face *read_obj_file(const char *fname, int *count)
{
	FILE *fp;
	int n_v, n_f;
	struct vertex *vertex;
	struct face *face;

	fp = fopen(fname, "r");

	n_v = get_vertex_count(fp);
	vertex = (struct vertex *)malloc(n_v * sizeof(struct vertex));
	vertex_read(fp, vertex, n_v);
	n_f = get_face_count(fp);
	face = (struct face *)malloc(n_f * sizeof(struct face));
	face_read(fp, face, n_f, vertex, n_v);

	fclose(fp);
	free(vertex);
	*count = n_f;
	return face;
}
int get_vertex_count(FILE *fp)
{
	return element_count(fp, 'v');
}
int get_face_count(FILE *fp)
{
	return element_count(fp, 'f');
}
int element_count(FILE *fp, char c)
{
	char line[LINE_MAX];
	int count = 0;
	while (fgets(line, LINE_MAX, fp) != NULL) {
		if (line_type(line) == c)
			count++;
	}
	rewind(fp);
	return count;
}
/* TODO some line types are more than one character, for example 'vp', 'vn' etc
*/
char line_type(const char *line)
{
	int i = 0;
	while (((line[i] == ' ') || (line[i] == '\t')) && (i < LINE_MAX))
		i++;
	if (i < LINE_MAX)
		return line[i];
	else
		return 0;
}
void vertex_read(FILE *fp, struct vertex *vertex, int count)
{
	char line[LINE_MAX];
	int i = 0;
	while ((i < count) && (fgets(line, LINE_MAX, fp) != NULL)) {
		if (line_type(line) == 'v') {
			vertex_scan(line, &vertex[i]);
			i++;
		}
	}
	rewind(fp);
}
void vertex_scan(const char *line, struct vertex *v)
{
	int n;
	n = sscanf(line, "v %f %f %f %f", &v->x, &v->y, &v->z, &v->w);
	if (n == 3)
		v->w = 0.0;
	else if (n != 4)
		fprintf(stderr, "bad vertex line (needs 3 or 4 corrds):\n%s\n",
			line);
}
void face_read(FILE *fp, struct face *face, int face_count,
		struct vertex *vertex, int vertex_count)
{
	char line[LINE_MAX];
	int i = 0;
	while ((i < face_count) && (fgets(line, LINE_MAX, fp) != NULL)) {
		if (line_type(line) == 'f') {
			face_scan(line, &face[i], vertex, vertex_count);
			i++;
		}
	}
	rewind(fp);
}
void face_scan(const char *line, struct face *f,
		struct vertex *v, int vertex_count)
{
	int n, i;
	int index[3];
/* TODO face format is more complex than this */
	n = sscanf(line, "f %d %d %d", &index[0], &index[1], &index[2]);
	if (n == 3) {
		printf("face\n");
		for (i = 0; i < 3; i++) {
			if ((index[i] > vertex_count) || (index[i] < 1)) {
				fprintf(stderr, "bad face line "
					"(vertex index %d out of bounds):"
					"\n%s\n", index[i], line);
			} else {
				f->v[i].x = v[index[i]-1].x;
				f->v[i].y = v[index[i]-1].y;
				f->v[i].z = v[index[i]-1].z;
				f->v[i].w = 1.0;
				printf("%.2f, %.2f, %.2f\n",
						f->v[i].x, f->v[i].y,
						f->v[i].z);
			}
		}
		compute_normal(f);
		for (i = 0; i < 3; i++)
			printf("normal\n%.2f, %.2f, %.2f\n", f->n[i].x,
					f->n[i].y, f->n[i].z);
		compute_centroid(f);
		printf("centroid\n%.2f, %.2f, %.2f\n", f->c.x, f->c.y, f->c.z);
	} else {
		fprintf(stderr, "bad face line (needs 3 vertices):\n%s\n",
			line);
	}
}
void compute_normal(struct face *f)
{
/* TODO is this the right direction? I'm bad at rhr */
	struct vertex a, b, n;
	sub_vertex(&a, &f->v[0], &f->v[1]);
	sub_vertex(&b, &f->v[0], &f->v[2]);
	cross(&n, &a, &b);
	normalize3(&n);
	copy_vertex(&f->n[0], &n);
	copy_vertex(&f->n[1], &n);
	copy_vertex(&f->n[2], &n);
}
void add_vertex(struct vertex *res, const struct vertex *a, const struct vertex *b)
{
	res->x = a->x + b->x;
	res->y = a->y + b->y;
	res->z = a->z + b->z;
	res->w = a->w + b->w;
}
void sub_vertex(struct vertex *res, const struct vertex *a, const struct vertex *b)
{
	res->x = a->x - b->x;
	res->y = a->y - b->y;
	res->z = a->z - b->z;
	res->w = a->w - b->w;
}
void div_vertex(struct vertex *res, const struct vertex *num, const float denom)
{
	res->x = num->x / denom;
	res->y = num->y / denom;
	res->z = num->z / denom;
	res->w = num->w / denom;
}
void cross(struct vertex *res, const struct vertex *a, const struct vertex *b)
{
	res->x = a->y * b->z - a->z * b->y;
	res->y = -(a->x * b->z - a->z * b->x);
	res->z = a->x * b->y - a->y * b->x;
}
void copy_vertex(struct vertex *to, const struct vertex *from)
{
	to->x = from->x;
	to->y = from->y;
	to->z = from->z;
	to->w = from->w;
}
void compute_centroid(struct face *f)
{
	add_vertex(&f->c, &f->v[0], &f->v[1]);
	add_vertex(&f->c, &f->c, &f->v[2]);
	div_vertex(&f->c, &f->c, 3);
}
void normalize3(struct vertex *v)
{
	float l;
	l = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	div_vertex(v, v, l);
}
