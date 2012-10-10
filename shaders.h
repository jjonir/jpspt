#ifndef SHADERS_H
#define SHADERS_H

struct shader_program;
typedef struct shader_program * shader_program_t;

shader_program_t newShader(const char *vshad, const char *fshad);
int changeVertexShader(shader_program_t p, const char *vshad);
int changeFragmentShader(shader_program_t p, const char *fshad);
void reloadShaders(shader_program_t p);
void switchToShader(shader_program_t p);

#endif
