%{
int yyerror(const char *s);
int yylex(void);

int lit_int;
int lit_float;
%}
%token V
%token VT
%token N
%token F
%token INT
%token FLOAT

%error-verbose

%%

obj:	/* Empty. */ |
	line obj;

line:	face |
	vertex |
	tex_vertex |
	normal;

vertex:	V FLOAT FLOAT FLOAT |
	V FLOAT FLOAT FLOAT FLOAT;

tex_vertex:
	VT FLOAT FLOAT |
	VT FLOAT FLOAT FLOAT;

normal:	N FLOAT FLOAT FLOAT;

face:	F index_list;

index_list:
	v_index_list |
	v_vt_index_list |
	v_n_index_list |
	v_vt_n_index_list;

v_index_list:
	v_index v_index v_index v_indices;

v_indices:
	v_index v_indices |
	/* Empty. */;

v_index:
	INT;

v_vt_index_list:
	v_vt_index v_vt_index v_vt_index v_vt_indices;

v_vt_indices:
	v_vt_index v_vt_indices |
	/* Empty. */;

v_vt_index:
	INT '/' INT;

v_n_index_list:
	v_n_index v_n_index v_n_index v_n_indices;

v_n_indices:
	v_n_index v_n_indices |
	/* Empty. */;

v_n_index:
	INT '/' '/' INT;

v_vt_n_index_list:
	v_vt_n_index v_vt_n_index v_vt_n_index v_vt_n_indices;

v_vt_n_indices:
	v_vt_n_index v_vt_n_indices |
	/* Empty. */;

v_vt_n_index:
	INT '/' INT '/' INT;
