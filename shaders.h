#ifndef SHADERS_H
#define SHADERS_H

void initShaders(void);
void loadShaders(const char *vshad, const char *fshad);
void loadVertexShader(const char *vshad);
void loadFragmentShader(const char *fshad);
void reloadShaders(void);
void shaderDisplayMode(void);
void updateShaderUniforms(void);

#endif
