#include <stdio.h>
#include <stdlib.h>
#include "geom.h"

int main(int argc, char *argv[])
{
	int n, i, j;
	struct face *f;

	if (argc <= 1)
		return -1;

	f = read_obj_file(argv[1], &n);
	printf("%d\n", n);
	for (i = 0; i < n; i++) {
		for (j = 0; j < 3; j++) {
			printf("%.3f, %.3f, %.3f, %.3f\n", f[i].v[j].x,
						f[i].v[j].y, f[i].v[j].z,
						f[i].v[j].w);
		}
		printf("\n");
	}
	free(f);

	return 0;
}
