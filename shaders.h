#ifndef SHADERS_H
#define SHADERS_H

void initShaders(void);
void loadShaders(const char *vshad, const char *fshad);
void reloadShaders(void);
void updateShaderTime(float t);

#endif
