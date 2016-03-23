#include "struct.h"
#include "mesh.h"
#include "shader.h"
#include "glm.h"

Object *Object_create(void);
void Object_attachMesh(Object *, Mesh *);
void Object_attachTexture(Object *, GLuint);
void Object_render(Object *);
void Object_render_static(Object *);
void Object_update(Object *);
void Object_add_f(Object *, void (*)(Object *));
void Object_free(Object *);
void Object_f_oscillate(Object *);
void Object_f_gravity(Object *);
void Object_f_collision(Object *);
void Object_f_dip(Object *);
void Object_f_burn(Object *);
void Object_f_levelup(Object *);
