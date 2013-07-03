#ifndef _GEOM_H
#define _GEOM_H

struct vertex {
	float x, y, z, w;
};
struct face_vertex {
	struct vertex v;
	struct vertex vt;
	struct vertex n;
};
struct face {
	int vertex_count;
	struct face_vertex *v;
	struct vertex c;
};
struct face_array {
	int face_count;
	struct face *f;
};

struct face_array *read_obj_file(const char *fname);
void delete_face_array(struct face_array *f);

#endif
