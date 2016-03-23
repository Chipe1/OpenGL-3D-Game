#include "object.h"

extern Shader *current_shader, *default_shader;
extern Object *objects[];
extern Mesh *default_mesh, *hard_mesh, *weak_hard_mesh, *soft_mesh, *weak_soft_mesh;
extern Mat4 VP;
extern float delta_t;
extern int num_objects, volley, num_anims;
extern Anim *anims[];

Object *Object_create(void){
	Object *toret;
	toret=(Object *)malloc(sizeof(Object));
	toret->mesh=NULL;
	toret->shader=default_shader;
	toret->model=GLM_identityMat4();
	toret->pos=GLM_createVec3(0, 0, 0);
	toret->vel=GLM_createVec3(0, 0, 0);
	toret->acc=GLM_createVec3(0, 0, 0);
	toret->mass=1;
	toret->radius=1;
	toret->theta=0;
	toret->omega=0;
	toret->flags=0;
	toret->life=1;
	toret->points=5;
	toret->death_time=0;
	glGenVertexArrays(1, &(toret->vao));
	return toret;
}

void Object_attachMesh(Object *obj, Mesh *mesh){

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

	if(obj->mesh==NULL){
		printf("Object not assigned any mesh. Using default mesh\n");
		Object_attachMesh(obj, default_mesh);
	}
	
	if(current_shader!=obj->shader){
		current_shader=obj->shader;
		Shader_bind(current_shader);
	}
	mvp=GLM_mulMat4Mat4(GLM_rotateMat4(GLM_createVec3(0, 0, 1),obj->theta), obj->model);
	mvp=GLM_mulMat4Mat4(GLM_translateMat4(obj->pos), mvp);
	mvp=GLM_mulMat4Mat4(VP, mvp);
	Shader_update(current_shader, mvp);

	glBindVertexArray(obj->vao);
	glDrawElements(GL_TRIANGLES, obj->mesh->drawCount, GL_UNSIGNED_INT, (void *)0);
	glBindVertexArray(0);
}

void Object_draw(Object *obj){
	if(obj->mesh==NULL){
		printf("Object not assigned any mesh. Using default mesh\n");
		Object_attachMesh(obj, default_mesh);
	}
	
	if(current_shader!=obj->shader){
		current_shader=obj->shader;
		Shader_bind(current_shader);
	}
	
	Shader_update(current_shader, obj->model);

	glBindVertexArray(obj->vao);
	glDrawElements(GL_TRIANGLES, obj->mesh->drawCount, GL_UNSIGNED_INT, (void *)0);
	glBindVertexArray(0);
}

void Object_update(Object *obj){
	int i;
	float frict, rot;
	Vec3 prev_pos, prev_vel;

	/* change mesh when object is damaged */
	if(obj->life==3 && obj->mesh==hard_mesh)
		Object_attachMesh(obj, weak_hard_mesh);
	else if(obj->life==1 && obj->mesh==soft_mesh)
		Object_attachMesh(obj, weak_soft_mesh);

	if(obj->flags&TYPE_FIXED)
		return ;
	
	prev_pos=obj->pos;
	prev_vel=obj->vel;
	obj->pos=GLM_addVec3(obj->pos,GLM_mulScalarVec3(delta_t, obj->vel));
	obj->pos=GLM_addVec3(obj->pos,GLM_mulScalarVec3(delta_t*delta_t/2, obj->acc));
	obj->vel=GLM_addVec3(obj->vel,GLM_mulScalarVec3(delta_t, obj->acc));
	obj->vel=GLM_mulScalarVec3((1-AIR_FRICTION*delta_t), obj->vel);
	obj->theta+=obj->omega*delta_t;
	obj->acc=GLM_createVec3(0, 0, 0);
	
	if(!(obj->flags&TYPE_LIGHT)){
		obj->acc=GLM_createVec3(0, -GRAVITY, 0);
	}
	
	i=Object_getCollision(obj);
	if(i>=0){
		Vec3 normal, tangent, oth_tangent, unit_normal, new_vel1, new_vel2;
		float tanAbs, vel1, vel2;

		/* decrease life */
		if(obj->flags&TYPE_WEAPON){
			if(objects[i]->hit_volley!=volley){
				objects[i]->life--;
				objects[i]->hit_volley=volley;
				anims[num_anims++]=Anim_create(objects[i]->pos, objects[i]->radius*4);
			}
		}
		else if(objects[i]->flags&TYPE_WEAPON){
			if(obj->hit_volley!=volley){
				obj->life--;
				obj->hit_volley=volley;
				anims[num_anims++]=Anim_create(obj->pos, obj->radius*4);
			}
		}

		normal=GLM_subVec3(obj->pos, objects[i]->pos);
		normal=GLM_projectVec3(obj->vel,normal);
		unit_normal=GLM_unitVec3(normal);
		tangent=GLM_subVec3(obj->vel, normal);
		if(objects[i]->flags&TYPE_FIXED){
			/* rebound */
			frict=FRICTION*(1+ELASTICITY)*sqrtf(GLM_dotVec3(normal, normal));
			normal=GLM_mulScalarVec3(-ELASTICITY, normal);
			/* friction */
			tanAbs=sqrtf(GLM_dotVec3(tangent, tangent));
			if(tanAbs>0){
				tangent=GLM_unitVec3(tangent);
				tangent=GLM_mulScalarVec3(MIN(tanAbs+frict, MAX(0, tanAbs-frict)), tangent);
			}
			frict/=obj->radius;
			rot=sqrtf(GLM_dotVec3(tangent, tangent))/obj->radius;
			if(GLM_crossVec3(GLM_subVec3(objects[i]->pos, obj->pos), tangent).z>0)
				rot*=-1;
			obj->vel=GLM_addVec3(normal, tangent);
			obj->omega=rot+MIN(obj->omega-rot+frict,MAX(0,obj->omega-rot-frict));
		}
		else{
			/* rebound */
			vel1=GLM_magVec3(GLM_projectVec3(obj->vel, unit_normal));
			vel2=GLM_magVec3(GLM_projectVec3(objects[i]->vel, unit_normal));
			oth_tangent=GLM_projectVec3(objects[i]->vel, unit_normal);
			oth_tangent=GLM_subVec3(objects[i]->vel, oth_tangent);
			if(vel1>vel2){
				obj->pos=prev_pos;
				new_vel1=GLM_mulScalarVec3((obj->mass*vel1+objects[i]->mass*(vel2-ELASTICITY*(vel1-vel2)))/(obj->mass+objects[i]->mass), unit_normal);
				new_vel2=GLM_mulScalarVec3((objects[i]->mass*vel2+obj->mass*(vel1+ELASTICITY*(vel1-vel2)))/(obj->mass+objects[i]->mass), unit_normal);
				frict=fabsf(vel1-GLM_magVec3(new_vel1));
				vel1=GLM_magVec3(new_vel1);
				vel2=GLM_magVec3(new_vel2);
				obj->vel=GLM_addVec3(new_vel1, tangent);
				objects[i]->vel=GLM_addVec3(new_vel2, oth_tangent);
			}
		}
	}
	
	/* collision with ground */
	if(obj->pos.y-obj->radius<GROUND_LEVEL){
		prev_vel=obj->vel;
		if(prev_vel.y>0)
			prev_vel.y=0;
		obj->pos.y=GROUND_LEVEL+obj->radius;
		obj->vel.y=-prev_vel.y*ELASTICITY;

		/* translational friction */
		frict=(obj->vel.y-prev_vel.y)*FRICTION;
		obj->vel.x=MIN(obj->vel.x+frict, MAX(0, obj->vel.x-frict));

		/* roataional friction */
		frict=fabsf(frict/obj->radius);rot=-obj->vel.x/obj->radius;
		obj->omega=rot+MIN(obj->omega-rot+frict,MAX(0,obj->omega-rot-frict));

		/* kill stationary weapons */
		if((obj->flags&TYPE_WEAPON)&&fabsf(GLM_magVec3(obj->vel))<=EPS)
			obj->life=0;
	}

	
}

int Object_getCollision(Object *obj){
	int i;
	float x1, y1, x2, y2, r1, r2;
	x1=obj->pos.x;y1=obj->pos.y;
	r1=obj->radius;
	for(i=0;i<num_objects;i++){
		x2=objects[i]->pos.x;y2=objects[i]->pos.y;
		r2=objects[i]->radius;
		if(objects[i]!=obj && (((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)) < (r1+r2)*(r1+r2)))
			return i;
	}
	return -1;
}

void Object_free(Object *obj){
	free(obj);
}
