#include "object.h"

extern Shader *current_shader, *default_shader;
extern Mesh *default_mesh;
extern GLuint current_texture, default_texture;
extern Mat4 VP, V;
extern float delta_t, game_time, monkey_life;
extern Object *platform_obj[GRID_SIZE][GRID_SIZE], *monkey_obj;
extern void load_level2(void);
float last_play;

Object *Object_create(void){
	Object *toret;
	toret=(Object *)malloc(sizeof(Object));
	toret->mesh=NULL;
	toret->shader=default_shader;
	toret->model=GLM_identityMat4();
	toret->pos=GLM_createVec3(0, 0, 0);
	toret->vel=GLM_createVec3(0, 0, 0);
	toret->acc=GLM_createVec3(0, 0, 0);
	toret->theta=GLM_createVec3(0, 0, 0);
	toret->f_num=0;
	toret->flags=0;
	toret->emit_color=GLM_createVec3(0, 0, 0);
	toret->spec_color=GLM_createVec3(0.1, 0.1, 0.1);
	toret->texture=default_texture;

	glGenVertexArrays(1, &(toret->vao));
	return toret;
}

void Object_attachMesh(Object *obj, Mesh *mesh){

	if(obj==NULL)
		return ;

	obj->mesh=mesh;
	glBindVertexArray(obj->vao);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[POSITION_VB]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[UV_VB]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[NORMAL_VB]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 0, (void *)0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->buffers[INDEX_VB]);
	

	glBindVertexArray(0);
}

void Object_render(Object *obj){

	Mat4 mvp;
	Mat4 m;

	if(obj==NULL)
		return ;

	if(obj->mesh==NULL){
		printf("Object not assigned any mesh. Using default mesh\n");
		Object_attachMesh(obj, default_mesh);
	}
	
	Shader_bind(obj->shader);

	/* check texture */
	if(current_texture!=obj->texture){
		Texture_bind(obj->texture, 0);
		current_texture=obj->texture;
	}
	mvp=GLM_mulMat4Mat4(GLM_eulerMat4(obj->theta.x, obj->theta.y, obj->theta.z), obj->model);
	mvp=GLM_mulMat4Mat4(GLM_translateMat4(obj->pos), mvp);
	if(obj==monkey_obj){
		mvp=GLM_mulMat4Mat4(GLM_translateMat4(GLM_createVec3(0, 0.5, 0)), mvp);
	}
	m=mvp;
	mvp=GLM_mulMat4Mat4(VP, mvp);

	Shader_update_Mat4(current_shader, MVP_U, mvp);
	Shader_update_Mat4(current_shader, MODEL_U, m);
	Shader_update_Vec3(current_shader, EMIT_U, obj->emit_color);
	Shader_update_Vec3(current_shader, SPEC_U, obj->spec_color);

	glBindVertexArray(obj->vao);
	glDrawElements(GL_TRIANGLES, obj->mesh->drawCount, GL_UNSIGNED_INT, (void *)0);
	glBindVertexArray(0);
}

void Object_render_static(Object *obj){
	Mat4 mvp;

	if(obj==NULL)
		return ;

	if(obj->mesh==NULL){
		printf("Object not assigned any mesh. Using default mesh\n");
		Object_attachMesh(obj, default_mesh);
	}
	
	Shader_bind(obj->shader);

	/* check texture */
	if(current_texture!=obj->texture){
		Texture_bind(obj->texture, 0);
		current_texture=obj->texture;
	}
	mvp=GLM_mulMat4Mat4(GLM_eulerMat4(obj->theta.x, obj->theta.y, obj->theta.z), obj->model);
	mvp=GLM_mulMat4Mat4(GLM_translateMat4(obj->pos), mvp);

	Shader_update_Mat4(current_shader, MVP_U, mvp);
	Shader_update_Mat4(current_shader, MODEL_U, mvp);

	glBindVertexArray(obj->vao);
	glDrawElements(GL_TRIANGLES, obj->mesh->drawCount, GL_UNSIGNED_INT, (void *)0);
	glBindVertexArray(0);
}


void Object_update(Object *obj){
	int i;

	if(obj==NULL)
		return ;

	obj->vel=GLM_addVec3(obj->vel, GLM_mulScalarVec3(delta_t, obj->acc));
	obj->pos=GLM_addVec3(obj->pos, GLM_mulScalarVec3(delta_t, obj->vel));
	if(obj->f_num>MAX_FUNCS){
		fprintf(stderr, "Function limit crossed. Has %d/%d functions\n", obj->f_num, MAX_FUNCS);
		exit(1);
	}
	for(i=0;i<obj->f_num;i++)
		obj->funcs[i](obj);
}

void Object_add_f(Object *obj, void (*func)(Object *)){

	if(obj==NULL)
		return ;

	if(obj->f_num>=MAX_FUNCS){
		fprintf(stderr, "Function limit exceeded\n");
		exit(1);
	}
	obj->funcs[obj->f_num++]=func;
}

void Object_free(Object *obj){

	if(obj==NULL)
		return ;

	glDeleteVertexArrays(1,&(obj->vao));
	free(obj);
}

void Object_f_oscillate(Object *obj){
	/* start oscillation */
	if((obj->flags&OBJ_FLAG_OSCILLATION) == 0){
		obj->flags|=OBJ_FLAG_OSCILLATION;
		obj->vel=GLM_createVec3(0, -1, 0);
	}
	obj->acc.y=(GRID_SIZE-1)/2.0-obj->pos.y;//GLM_createVec3(0, 3*cosf(obj->pos.y), 0);
}

void Object_f_gravity(Object *obj){
	obj->vel=GLM_addVec3(obj->vel, GLM_mulScalarVec3(delta_t, GLM_createVec3(0, -GRAVITY, 0)));
}

void Object_f_collision(Object *obj){
	int i, j, x, z;
	i=(int)(obj->pos.x+0.5);
	j=-(int)(obj->pos.z-0.5);

	/* bottom collision */
	if(i>=0&&i<GRID_SIZE&&j>=0&&j<GRID_SIZE&&platform_obj[i][j]){
		if(obj->pos.y<platform_obj[i][j]->pos.y && obj->pos.y>=platform_obj[i][j]->pos.y-0.2){
			obj->pos.y=platform_obj[i][j]->pos.y;
			if(obj->vel.y<platform_obj[i][j]->vel.y)
				obj->vel.y=platform_obj[i][j]->vel.y;
		}
		if(obj->pos.y<platform_obj[i][j]->pos.y){
			obj->pos.x--;
		}
	}
	
	/* side collision */
	for(x=i-1;x<=i+1;x++){
		for(z=j-1;z<=j+1;z++){
			if(x<0||x>=GRID_SIZE||z<0||z>=GRID_SIZE||platform_obj[x][z]==NULL||platform_obj[x][z]->pos.y<=obj->pos.y+0.1){
				continue;
			}
			if(fabsf(obj->pos.x-platform_obj[x][z]->pos.x)<0.75 && fabsf(obj->pos.z-platform_obj[x][z]->pos.z)<0.75){
				if(fabsf(obj->pos.x-platform_obj[x][z]->pos.x) < fabsf(obj->pos.z-platform_obj[x][z]->pos.z)){
					if(obj->pos.z<platform_obj[x][z]->pos.z){
						obj->vel.z=MIN(obj->vel.z, 0);
						obj->pos.z=platform_obj[x][z]->pos.z-0.75;
					}
					else{
						obj->vel.z=MAX(obj->vel.z, 0);
						obj->pos.z=platform_obj[x][z]->pos.z+0.75;
					}
				}
				else{
					if(obj->pos.x<platform_obj[x][z]->pos.x){
						obj->vel.x=MIN(obj->vel.x, 0);
						obj->pos.x=platform_obj[x][z]->pos.x-0.75;
					}
					else{
						obj->vel.x=MAX(obj->vel.x, 0);
						obj->pos.x=platform_obj[x][z]->pos.x+0.75;
					}
				}
			}
		}
	}
}

void Object_f_dip(Object *obj){
	float dist;
	dist=sqrtf((monkey_obj->pos.x-obj->pos.x)*(monkey_obj->pos.x-obj->pos.x)+(monkey_obj->pos.z-obj->pos.z)*(monkey_obj->pos.z-obj->pos.z));
	dist=MAX(dist-1.2, 0);
	dist=MIN(dist,5);
	obj->pos.y=-dist+(GRID_SIZE-1)*0.5;
}

void Object_f_burn(Object *obj){
	int i, j, x, z;
	i=(int)(monkey_obj->pos.x+0.5);
	j=(int)(monkey_obj->pos.z-0.5);
	
	if(obj->pos.x==i && obj->pos.z==j && monkey_obj->pos.y>=obj->pos.y && monkey_obj->pos.y<obj->pos.y+0.2){
		if(game_time>last_play+0.6){
			last_play=game_time;
			i=fork();
			if(i==0){
				system("aplay --rate=80000 -q ./res/burn.mp3");
				exit(0);
			}
		}
		monkey_life-=10*delta_t;
	}
}

void Object_f_levelup(Object *obj){
	int i, j, x, z;
	i=(int)(monkey_obj->pos.x+0.5);
	j=(int)(monkey_obj->pos.z-0.5);
	
	if(obj->pos.x==i && obj->pos.z==j && monkey_obj->pos.y>=obj->pos.y && monkey_obj->pos.y<obj->pos.y+0.2){
		load_level2();
	}
}
