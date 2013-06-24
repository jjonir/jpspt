#ifndef _GEOM_H
#define _GEOM_H

struct vertex {
	float x, y, z, w;
};
struct face {
	struct vertex v[3];
	struct vertex n[3];
	struct vertex t[3];
	struct vertex c;
};

struct face *read_obj_file(const char *fname, int *count);

#endif
