%{
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "geom.h"

int yyerror(const char *s);
int yylex(void);

/* structures to hold parsed data */
struct vertex_list {
	float x, y, z, w;
	struct vertex_list *next;
};
struct texture_vertex_list {
	float u, v, w;
	struct texture_vertex_list *next;
};
struct normal_list {
	float a, b, c;
	struct normal_list *next;
};
struct face_vertex_list {
	int vertex_index;
	int texture_vertex_index;
	int normal_index;
	struct face_vertex_list *next;
};
struct face_list {
	struct face_vertex_list *v;
	struct face_list *next;
};

void clean(void);
void add_vertex(float x, float y, float z, float w);
void add_texture_vertex(float u, float v, float w);
void add_normal(float a, float b, float c);
struct face_vertex_list *new_face_vertex(int v, int vt, int n);
void print_face_vertex_list(struct face_vertex_list *l);
void add_face(struct face_vertex_list *v);
struct face_array *build_face_array(void);
void translate_vertex(struct face_vertex *dest, struct face_vertex_list *src);

static void compute_normal(struct face *f);
static void sum_vertex(struct vertex *res, const struct vertex *a, const struct vertex *b);
static void sub_vertex(struct vertex *res, const struct vertex *a, const struct vertex *b);
static void div_vertex(struct vertex *res, const struct vertex *num, const float denom);
static void cross(struct vertex *res, const struct vertex *a, const struct vertex *b);
static void copy_vertex(struct vertex *to, const struct vertex *from);
static void compute_centroid(struct face *f);
static void normalize3(struct vertex *v);

struct vertex_list vertex_head = {0, 0, 0, 0, NULL};
struct vertex_list *vertex_tail = &vertex_head;
int vertex_list_size = 0;
struct texture_vertex_list texture_vertex_head = {0, 0, 0, NULL};
struct texture_vertex_list *texture_vertex_tail = &texture_vertex_head;
int texture_vertex_list_size = 0;
struct normal_list normal_head = {0, 0, 0, NULL};
struct normal_list *normal_tail = &normal_head;
int normal_list_size = 0;
struct face_vertex_list face_vertex_stack = {0, 0, 0, NULL};
struct face_list face_head = {NULL, NULL};
struct face_list *face_tail = &face_head;
int face_list_size = 0;
%}
%union {
	int i;
	float f;
	char c;
	struct face_vertex_list *v_list;
}
%token <c> V
%token <c> VT
%token <c> N
%token <c> F
%token <i> INT
%token <f> FLOAT
%type <i> obj
%type <i> line
%type <i> vertex
%type <i> tex_vertex
%type <i> normal
%type <i> face
%type <v_list> index_list
%type <v_list> v_index_list
%type <v_list> v_indices
%type <v_list> v_index
%type <v_list> v_vt_index_list
%type <v_list> v_vt_indices
%type <v_list> v_vt_index
%type <v_list> v_n_index_list
%type <v_list> v_n_indices
%type <v_list> v_n_index
%type <v_list> v_vt_n_index_list
%type <v_list> v_vt_n_indices
%type <v_list> v_vt_n_index
%type <f> float

%error-verbose

%%

obj:	/* Empty. */ {} |
	obj line;

line:	face |
	vertex |
	tex_vertex |
	normal;

vertex:	V float float float { add_vertex($2, $3, $4, 0.0); } |
	V float float float float { add_vertex($2, $3, $4, $5); } ;

tex_vertex:
	VT float float { add_texture_vertex($2, $3, 0.0); } |
	VT float float float { add_texture_vertex($2, $3, $4); } ;

normal:	N float float float { add_normal($2, $3, $4); } ;

float:	FLOAT { $$ = $1; } |
	INT { $$ = (float)$1; };

face:	F index_list { add_face($2); /* print_face_vertex_list($2); */ } ;

index_list:
	v_index_list |
	v_vt_index_list |
	v_n_index_list |
	v_vt_n_index_list;

v_index_list:
	v_index v_index v_index v_indices
		{ $1->next = $2; $2->next = $3; $3->next = $4; $$ = $1; } ;

v_indices:
	v_index v_indices { $1->next = $2; $$ = $1; } |
	/* Empty. */ { $$ = NULL; } ;

v_index:
	INT { $$ = new_face_vertex($1, 0, 0); } ;

v_vt_index_list:
	v_vt_index v_vt_index v_vt_index v_vt_indices
		{ $1->next = $2; $2->next = $3; $3->next = $4; $$ = $1; } ;

v_vt_indices:
	v_vt_index v_vt_indices { $1->next = $2; $$ = $1; } |
	/* Empty. */ { $$ = NULL; } ;

v_vt_index:
	INT '/' INT { $$ = new_face_vertex($1, $3, 0); } ;

v_n_index_list:
	v_n_index v_n_index v_n_index v_n_indices
		{ $1->next = $2; $2->next = $3; $3->next = $4; $$ = $1; } ;

v_n_indices:
	v_n_index v_n_indices { $1->next = $2; $$ = $1; } |
	/* Empty. */ { $$ = NULL; } ;

v_n_index:
	INT '/' '/' INT { $$ = new_face_vertex($1, 0, $4); } ;

v_vt_n_index_list:
	v_vt_n_index v_vt_n_index v_vt_n_index v_vt_n_indices
		{ $1->next = $2; $2->next = $3; $3->next = $4; $$ = $1; } ;

v_vt_n_indices:
	v_vt_n_index v_vt_n_indices { $1->next = $2; $$ = $1; } |
	/* Empty. */ { $$ = NULL; } ;

v_vt_n_index:
	INT '/' INT '/' INT { $$ = new_face_vertex($1, $3, $5); } ;

%%

#ifdef STANDALONE_PARSER
int main(int argc, char *argv[])
{
	extern FILE *yyin;
(void)argc; (void)argv;
	yyin = stdin;
	yyparse();

	clean();

	return 0;
}
#endif

struct face_array *read_obj_file(const char *fname)
{
	extern FILE *yyin;
	struct face_array *f;
	yyin = fopen(fname, "r");
	yyparse();
	f = build_face_array();

	clean();

	return f;
}

void clean()
{
	struct vertex_list *v, *vp;
	struct texture_vertex_list *vt, *vtp;
	struct normal_list *n, *np;
	struct face_list *f, *fp;
	struct face_vertex_list *fv, *fvp;

	v = vertex_head.next;
	while (v != NULL) {
		vp = v;
		v = v->next;
		free(vp);
	}
	vertex_tail = &vertex_head;

	vt = texture_vertex_head.next;
	while (vt != NULL) {
		vtp = vt;
		vt = vt->next;
		free(vtp);
	}
	texture_vertex_tail = &texture_vertex_head;

	n = normal_head.next;
	while (n != NULL) {
		np = n;
		n = n->next;
		free(np);
	}
	normal_tail = &normal_head;

	f = face_head.next;
	while (f != NULL) {
		fv = f->v;
		while (fv != NULL) {
			fvp = fv;
			fv = fv->next;
			free(fvp);
		}

		fp = f;
		f = f->next;
		free(fp);
	}
	face_tail = &face_head;
}

void add_vertex(float x, float y, float z, float w)
{
	vertex_tail->next = (struct vertex_list *)malloc(sizeof(struct vertex_list));
	vertex_tail->next->x = x;
	vertex_tail->next->y = y;
	vertex_tail->next->z = z;
	vertex_tail->next->w = w;
	vertex_tail->next->next = NULL;
	vertex_tail = vertex_tail->next;
	vertex_list_size++;
}
void add_texture_vertex(float u, float v, float w)
{
	texture_vertex_tail->next = (struct texture_vertex_list *)malloc(sizeof(struct texture_vertex_list));
	texture_vertex_tail->next->u = u;
	texture_vertex_tail->next->v = v;
	texture_vertex_tail->next->w = w;
	texture_vertex_tail->next->next = NULL;
	texture_vertex_tail = texture_vertex_tail->next;
	texture_vertex_list_size++;
}
void add_normal(float a, float b, float c)
{
	normal_tail->next = (struct normal_list *)malloc(sizeof(struct normal_list));
	normal_tail->next->a = a;
	normal_tail->next->b = b;
	normal_tail->next->c = c;
	normal_tail->next->next = NULL;
	normal_tail = normal_tail->next;
	normal_list_size++;
}
struct face_vertex_list *new_face_vertex(int v, int vt, int n)
{
	struct face_vertex_list *tmp = (struct face_vertex_list *)malloc(sizeof(struct face_vertex_list));
	tmp->vertex_index = (v < 0) ? (vertex_list_size - v + 1) : v;
	tmp->texture_vertex_index = (vt < 0) ? (texture_vertex_list_size - vt + 1) : vt;
	tmp->normal_index = (n < 0) ? (normal_list_size - n + 1) : n;
	tmp->next = NULL;
	return tmp;
}
void print_face_vertex_list(struct face_vertex_list *l)
{
	while (l) {
		printf("v:%i, vt:%d, n:%d\n", l->vertex_index,
				l->texture_vertex_index, l->normal_index);
		l = l->next;
	}
	printf("\n");
}
void add_face(struct face_vertex_list *v)
{
	face_tail->next = (struct face_list *)malloc(sizeof(struct face_list));
	face_tail->next->v = v;
	face_tail->next->next = NULL;
	face_tail = face_tail->next;
#ifdef STANDALONE_PARSER
	print_face_vertex_list(v);
#endif
}
struct face_array *build_face_array()
{
	struct face_list *f;
	struct face_vertex_list *v;
	int face_count;
	int vertex_count;
	struct face_array *fa;
	int i, j;

	fa = (struct face_array *)malloc(sizeof(struct face_array));

	f = face_head.next;
	face_count = 0;
	while (f != NULL) {
		face_count++;
		f = f->next;
	}

	fa->face_count = face_count;
	fa->f = (struct face *)malloc(face_count * sizeof(struct face));
	f = face_head.next;
	i = 0;
	while (f != NULL) {
		v = f->v;
		vertex_count = 0;
		while (v != NULL) {
			vertex_count++;
			v = v->next;
		}

		fa->f[i].vertex_count = vertex_count;
		fa->f[i].v = malloc(vertex_count * sizeof(struct face_vertex));
		/* fa->f[i].c = foo; */
		v = f->v;
		j = 0;
		while (v != NULL) {
			translate_vertex(&fa->f[i].v[j], v);
			v = v->next;
			j++;
		}

		compute_centroid(&fa->f[i]);
		compute_normal(&fa->f[i]);

		f = f->next;
		i++;
	}

	return fa;
}
void compute_normal(struct face *f)
{
/* TODO is this the right direction? I'm bad at rhr */
	struct vertex a, b, n;
	int i;
	sub_vertex(&a, &f->v[0].v, &f->v[1].v);
	sub_vertex(&b, &f->v[0].v, &f->v[2].v);
	cross(&n, &a, &b);
	normalize3(&n);
	for (i = 0; i < f->vertex_count; i++) {
		copy_vertex(&f->v[i].n, &n);
	}
}
void sum_vertex(struct vertex *res, const struct vertex *a, const struct vertex *b)
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
	int i;
	sum_vertex(&f->c, &f->v[0].v, &f->v[1].v);
	for (i = 2; i < f->vertex_count; i++) {
		sum_vertex(&f->c, &f->c, &f->v[i].v);
	}
	div_vertex(&f->c, &f->c, f->vertex_count);
}
void normalize3(struct vertex *v)
{
	float l;
	l = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	div_vertex(v, v, l);
}
/* TODO fill in error etc */
void translate_vertex(struct face_vertex *dest, struct face_vertex_list *src)
{
	struct vertex_list *v;
	struct texture_vertex_list *vt;
	struct normal_list *n;
	int i;

	if (src->vertex_index <= 0) {
		/* error */
	} else {
		v = vertex_head.next;
		i = 1;
		while ((v != NULL) && (i < src->vertex_index)) {
			v = v->next;
			i++;
		}
		if (v != NULL) {
			dest->v.x = v->x;
			dest->v.y = v->y;
			dest->v.z = v->z;
			dest->v.w = v->w;
		} else {
			/* error */
		}
	}

	if (src->texture_vertex_index == 0) {
		/* default? */
	} else if (src->texture_vertex_index < 0) {
		/* error */
	} else {
		vt = texture_vertex_head.next;
		i = 1;
		while ((vt != NULL) && (i < src->texture_vertex_index)) {
			vt = vt->next;
			i++;
		}
		if (vt != NULL) {
			dest->vt.x = vt->u;
			dest->vt.y = vt->v;
			dest->vt.z = vt->w;
			dest->vt.w = 0.0;
		} else {
			/* error */
		}
	}

	if (src->normal_index == 0) {
		/* compute */
	} else if (src->normal_index < 0) {
		/* error */
	} else {
		n = normal_head.next;
		i = 1;
		while ((n != NULL) && (i < src->normal_index)) {
			n = n->next;
			i++;
		}
		if (n != NULL) {
			dest->n.x = n->a;
			dest->n.y = n->b;
			dest->n.z = n->c;
			dest->n.w = 0.0;
		} else {
			/* error */
		}
	}
}
void delete_face_array(struct face_array *f)
{
	int i;
	for (i = 0; i < f->face_count; i++) {
		free(f->f[i].v);
	}
	free(f->f);
	free(f);
}
int yyerror(const char *s)
{
	fprintf(stderr, "%s\n", s);
	return 0;
}
