#ifndef __openglstruct__
#define __openglstruct__
#include "include.h"

enum{
        POSITION_VB,
        UV_VB,
	NORMAL_VB,
        INDEX_VB,
        NUM_BUFFERS
};

enum{
        MVP_U,
	MODEL_U,
	VIEW_U,
	LIGHT_U,
	TORCH_POS_U,
	TORCH_DIR_U,
	TORCH_COLOR_U,
	TORCH_POWER_U,
	EMIT_U,
	SPEC_U,
        NUM_UNIFORMS
};

enum{
        VERTEX_SHADER,
        FRAGMENT_SHADER,
        NUM_SHADERS
};

enum{
	OBJ_FLAG_OSCILLATION=1<<0,
	
};

typedef struct __ogl_Shader{
        GLuint prog,shaders[NUM_SHADERS],uniforms[NUM_UNIFORMS];
}Shader;

typedef struct __ogl_Mesh{
        GLuint buffers[NUM_BUFFERS];
        int drawCount;
}Mesh;

typedef struct __ogl_Vec3{
        float x,y,z;
}Vec3;

typedef struct __ogl_Vec4{
        float x,y,z,w;
}Vec4;

typedef struct __ogl_Mat4{
        float e[4][4];
}Mat4;

typedef struct __ogl_Object{
	GLuint vao;
	Mesh *mesh;
	Shader *shader;
	GLuint texture;
	Mat4 model;
	Vec3 pos, vel, acc, theta;
	Vec3 emit_color, spec_color;
	int f_num, flags, var1, var2, var3;
	void (*funcs[MAX_FUNCS])(struct __ogl_Object *);
}Object;
#endif
