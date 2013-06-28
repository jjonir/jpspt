#include <stdlib.h>
#include <string.h>

enum nonterminal {
	SPECIAL_OR,
	OBJ,
	LINE,
	VERTEX,
	TEX_VERTEX,
	NORMAL,
	FACE,
	INDEX_LIST,
	V_INDEX_LIST,
	V_INDICES,
	V_INDEX,
	V_VT_INDEX_LIST,
	V_VT_INDICES,
	V_VT_INDEX,
	V_VN_INDEX_LIST,
	V_VN_INDICES,
	V_VN_INDEX,
	V_VT_VN_INDEX_LIST,
	V_VT_VN_INDICES,
	V_VT_VN_INDEX,
	NUM_NONTERMINALS
};
enum token_type {
	NONE = NUM_NONTERMINALS,
	NEWLINE,
	STRING,
	COMMENT,
	INT,
	FLOAT,
	SLASH,
	NUM_TOKEN_TYPES
};
int tab[] = {
/* OBJ */	LINE, NEWLINE, OBJ, 0, LINE, 0, -1,
/* LINE */	FACE, 0, VERTEX, 0, TEX_VERTEX, 0, NORMAL, 0, COMMENT, 0, -1,
/* VERTEX */	STRING, FLOAT, FLOAT, FLOAT, 0, STRING, FLOAT, FLOAT, FLOAT, FLOAT, -1,
/* TEX_VERTEX*/	STRING, FLOAT, FLOAT, 0, STRING, FLOAT, FLOAT, FLOAT, -1,
/* NORMAL */	STRING, FLOAT, FLOAT, FLOAT, -1,
/* FACE */	STRING, INDEX_LIST, -1,
/* INDEX_LIST*/	V_INDEX_LIST, 0, V_VT_INDEX_LIST, 0, V_VN_INDEX_LIST, 0, V_VT_VN_INDEX_LIST, -1,
/* V_INDEX_LIST */	V_INDEX, V_INDEX, V_INDEX, V_INDICES, -1,
/* V_INDICES */	V_INDEX, V_INDICES, 0, -1,
/* V_INDEX */	INT, -1
/* V_VT_INDEX_LIST */	V_VT_INDEX, V_VT_INDEX, V_VT_INDEX, V_VT_INDICES, -1,
/* V_VT_INDICES */	V_VT_INDEX, V_VT_INDICES, 0, -1,
/* V_VT_INDEX */	INT, SLASH, INT, -1
/* V_VN_INDEX_LIST */	V_VN_INDEX, V_VN_INDEX, V_VN_INDEX, V_VN_INDICES, -1,
/* V_VN_INDICES */	V_VN_INDEX, V_VN_INDICES, 0, -1,
/* V_VN_INDEX */	INT, SLASH, SLASH, INT, -1
/* V_VT_VN_INDEX_LIST*/	V_VT_VN_INDEX, V_VT_VN_INDEX, V_VT_VN_INDEX, V_VT_VN_INDICES, -1,
/* V_VT_VN_INDICES */	V_VT_VN_INDEX, V_VT_VN_INDICES, 0, -1,
/* V_VT_VN_INDEX */	INT, SLASH, INT, SLASH, INT, -1,
};

struct token {
	enum token_type type;
	union {
		float f;
		int i;
		char c;
		char *s;
	} data;
};

static struct token next_token(char **s);
static int scan_number(const char *s, struct token *t);

void parse(FILE *fp)
{
	char line[1024];
	char tmp;
	struct token t;

	fgets(line, 1024, fp);
	while (!feof(fp)) {
		tmp = line;
		t = next_token(&tmp);
	}
}

/* tokens:
 * NEWLINE: \n
 * STRING:  [a-zA-Z]{1-2}  # NOPE see below
 * STRING:  [^\s\0\n/]+    # or something
 * COMMENT: #.*
 * INT:     [0-9]+
 * FLOAT:   [0-9]+\.[0-9]* | \.[0-9]+
 * SLASH:   /
 */
struct token next_token(char **s)
{
	struct token t;
	char *tmp;

	while ((**s == ' ') || (**s == '\t'))
		*s += 1;

	if (**s == '\n') {
		*s += 1;
		t.type = NEWLINE;
	} else if ((**s == '.') || ((**s >= '0') && (**s <= '9'))) {
		*s += scan_number(*s, &t);
	} else if (**s == '#') {
		while ((**s != '\n') && (**s != '\0'))
			*s += 1;
		t.type = COMMENT;
	} else if (**s == '/') {
		**s += 1;
		t.type = SLASH;
	} else {
		/* there are probably some rules, but let's just ignore them */
		tmp = *s;
		while ((*tmp != ' ') && (*tmp != '\t') && (*tmp != '/') &&
				(*tmp != '\n') && (*tmp != '\0'))
			tmp++;
		t.data.s = (char *)malloc((tmp - *s) + 1);
		strncpy(t.data.s, *s, tmp - *s);
		*s = tmp;
		t.type = STRING;
	}

	return t;
}

int scan_number(const char *s, struct token *t)
{
	int i = 0;
	char *tmp;

	while ((s[i] >= '0') && (s[i] <= '9'))
		i++;
	if (s[i] == '.') {
		t->data.f = strtof(s, &tmp);
		t->type = FLOAT;
	} else {
		t->data.i = strtoul(s, &tmp, 10);
		t->type = INT;
	}

	return tmp - s;
}

#ifdef SCAN_TEST
#include <stdio.h>
void print_token(struct token *t)
{
	switch (t->type) {
	case NEWLINE:
		printf("\\n\n");
		break;
	case STRING:
		printf("%s\n", t->data.s);
		break;
	case COMMENT:
		printf("comment\n");
		break;
	case INT:
		printf("%i\n", t->data.i);
		break;
	case FLOAT:
		printf("%f\n", t->data.f);
		break;
	case SLASH:
		printf("/\n");
		break;
	case NONE:
	default:
		printf("wtf is this\n");
		break;
	}
}

int main(void)
{
	char s[1024];
	char *tmp;
	struct token t;

	fgets(s, 1024, stdin);
	while (!feof(stdin)) {
		tmp = s;
		while (*tmp != '\0') {
			t = next_token(&tmp);
			print_token(&t);
			if (t.type == STRING)
				free(t.data.s);
		}
		fgets(s, 1024, stdin);
	}

	return 0;
}
#endif
