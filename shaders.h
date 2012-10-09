#ifndef SHADERS_H
#define SHADERS_H

struct shader_program;
typedef struct shader_program shader_program_t;

void initShaders(void);
void loadShaders(const char *vshad, const char *fshad);
void loadOutlineShaders(const char *vshad, const char *fshad);
void reloadShaders(void);
void shaderDisplayMode(void);

#endif
