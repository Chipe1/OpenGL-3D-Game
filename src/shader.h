#include "struct.h"

Shader *Shader_create(const char *, const char *);
GLuint Shader_load(const char *, GLenum);
void Shader_bind(Shader *);
void Shader_checkError(GLuint, GLuint, char, const char *);
void Shader_update_Mat4(Shader *, int,  Mat4);
void Shader_update_Vec3(Shader *, int,  Vec3);
void Shader_update_float(Shader *, int,  float);
