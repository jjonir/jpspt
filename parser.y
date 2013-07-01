%{
#include <stdlib.h>
#include <stdio.h>

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

void add_vertex(float x, float y, float z, float w);
void add_texture_vertex(float u, float v, float w);
void add_normal(float a, float b, float c);
struct face_vertex_list *new_face_vertex(int v, int vt, int n);

struct vertex_list vertex_head = {0, 0, 0, 0, NULL};
struct vertex_list *vertex_tail = &vertex_head;
struct texture_vertex_list texture_vertex_head = {0, 0, 0, NULL};
struct texture_vertex_list *texture_vertex_tail = &texture_vertex_head;
struct normal_list normal_head = {0, 0, 0, NULL};
struct normal_list *normal_tail = &normal_head;
struct face_vertex_list face_vertex_stack = {0, 0, 0, NULL};
struct face_list face_head = {NULL, NULL};
struct face_list *face_tail = &face_head;
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

%error-verbose

%%

obj:	/* Empty. */ {} |
	line obj;

line:	face |
	vertex |
	tex_vertex |
	normal;

vertex:	V FLOAT FLOAT FLOAT { add_vertex($2, $3, $4, 0.0); } |
	V FLOAT FLOAT FLOAT FLOAT { add_vertex($2, $3, $4, $5); } ;

tex_vertex:
	VT FLOAT FLOAT { add_texture_vertex($2, $3, 0.0); } |
	VT FLOAT FLOAT FLOAT { add_texture_vertex($2, $3, $4); } ;

normal:	N FLOAT FLOAT FLOAT { add_normal($2, $3, $4); } ;

face:	F index_list { printf("pop\n"); } ;

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

int main(int argc, char *argv[])
{
	extern FILE *yyin;
(void)argc; (void)argv;
	yyin = stdin;
	yyparse();
	return 0;
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
}
void add_texture_vertex(float u, float v, float w)
{
	texture_vertex_tail->next = (struct texture_vertex_list *)malloc(sizeof(struct texture_vertex_list));
	texture_vertex_tail->next->u = u;
	texture_vertex_tail->next->v = v;
	texture_vertex_tail->next->w = w;
	texture_vertex_tail->next->next = NULL;
	texture_vertex_tail = texture_vertex_tail->next;
}
void add_normal(float a, float b, float c)
{
	normal_tail->next = (struct normal_list *)malloc(sizeof(struct normal_list));
	normal_tail->next->a = a;
	normal_tail->next->b = b;
	normal_tail->next->c = c;
	normal_tail->next->next = NULL;
	normal_tail = normal_tail->next;
}
struct face_vertex_list *new_face_vertex(int v, int vt, int n)
{
	struct face_vertex_list *tmp = (struct face_vertex_list *)malloc(sizeof(struct face_vertex_list));
	tmp->vertex_index = v;
	tmp->texture_vertex_index = vt;
	tmp->normal_index = n;
	tmp->next = NULL;
	return tmp;
}
int yyerror(const char *s)
{
	fprintf(stderr, "%s\n", s);
	return 0;
}
