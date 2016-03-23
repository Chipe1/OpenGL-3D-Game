#include "main.h"

Mat4 M, V, P, VP;
float mouseX, mouseY, delta_t, cameraX, cameraY, zoomRatio, last_time, time_scale, game_time, monkey_life, monkey_speed;
Shader *current_shader, *default_shader, *ui_shader;
Mesh *default_mesh, *platform_mesh, *monkey_mesh, *heart_mesh, *gameover_mesh;
Object *monkey_obj, *objects[MAX_OBJECTS], *platform_obj[GRID_SIZE][GRID_SIZE], *tower_obj, *heart_obj;
int num_objects, camera_type, num_frames, torch_mode, torch_power;
Vec3 heli_pos, heli_look, monkey_look, monkey_movement;
GLuint current_texture, default_texture, heart_texture, platform_texture, monkey_texture;
extern float last_play;

int main(int argc, char **argv){
	int i;
	GLFWwindow *window;

	window=initGLFW();
	initGL();
	initGLEW();

	mouseX=0;mouseY=0;
	cameraX=0;cameraY=0;
	zoomRatio=1;
	time_scale=1;
	game_time=0;
	num_frames=0;
	current_shader=NULL;
	current_texture=0;
	camera_type=2;
	monkey_life=100;
	monkey_speed=1.5;
	torch_mode=0;
	torch_power=0;
	heli_pos=GLM_createVec3(0, 4, 0);
	heli_look=GLM_unitVec3(GLM_createVec3(0, 2, 3));
	monkey_look=GLM_createVec3(0, 0, -1);
	monkey_movement=GLM_createVec3(0, 0, 0);

	/* play background sound */	
	load_loading(window);
	load_resources();
	load_level1();
		
	P=GLM_perspMat4(80, 16.0/9.0, 0.1, 500);
	
	/* game loop */
	last_time=glfwGetTime();
	last_play=0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	while(!glfwWindowShouldClose(window)){
		/* check life */
		if(monkey_life<=0){
			quit(window);
		}
		
		/* time difference since last frame */
		delta_t=-last_time;
		last_time=glfwGetTime();
		delta_t+=last_time;
		delta_t*=time_scale;
		game_time+=delta_t;
		num_frames++;
		glfwPollEvents();
		manage_input();

		if(monkey_obj->pos.y<-20){
			monkey_life-=delta_t*100;
		}
		for(i=0;i<num_objects;i++){
			Object_update(objects[i]);
		}

		/* adjust camera */
		switch(camera_type){
		case 1:
			/* adventurer view */
			V=GLM_viewMat4(GLM_addVec3(monkey_obj->pos, GLM_createVec3(0.1*monkey_look.x, 0.5, 0.1*monkey_look.z)), GLM_createVec3(-monkey_look.x, 0.5, -monkey_look.z), GLM_createVec3(0, 1, 0));
			break;
		case 2:
			/* follow cam */
			V=GLM_viewMat4(GLM_addVec3(monkey_obj->pos, GLM_createVec3(-2*monkey_look.x, 3, -2*monkey_look.z)), GLM_mulScalarVec3(-1, GLM_addVec3(monkey_look, GLM_createVec3(0, -0.7314, 0))), GLM_createVec3(0, 1, 0));
			break;
		case 3:
			/* tower view */
			V=GLM_viewMat4(GLM_addVec3(tower_obj->pos, GLM_createVec3(0.5, 0.5, -0.5)), GLM_subVec3(tower_obj->pos, monkey_obj->pos), GLM_createVec3(0, 1, 0));
			break;
 		case 4:
			/* top view */
			V=GLM_viewMat4(GLM_addVec3(monkey_obj->pos, GLM_createVec3(0 , 10, 0)), GLM_createVec3(0, 1, 0), GLM_createVec3(0, 0, -1));
			break;
		case 5:
			/* Heli cam */
			V=GLM_viewMat4(heli_pos,  heli_look, GLM_createVec3(0, 1, 0));
			break;
		}
		Shader_update_Mat4(default_shader, VIEW_U, V);
		VP=GLM_mulMat4Mat4(P, V);
		draw();
		glfwSwapBuffers(window);
	}

	printf("\n\n\nTHIS MUST NOT PRINT!!!\n");
	return 0;
}

void manage_input(void){
	Vec3 forward, right;
	float vely, angle;

	/* movement */
	forward=monkey_look;
	right=GLM_createVec3(-monkey_look.z, 0, monkey_look.x);
	/* preserve vel.y for monkey */
	vely=monkey_obj->vel.y;
	forward=GLM_mulScalarVec3(-monkey_movement.z, forward);
	right=GLM_mulScalarVec3(monkey_movement.x, right);
	monkey_obj->vel=GLM_mulScalarVec3(monkey_speed, GLM_unitVec3(GLM_addVec3(forward, right)));
	monkey_obj->vel.y=vely;
	monkey_look=GLM_convertVec4Vec3(GLM_mulMat4Vec4(GLM_rotateMat4(GLM_createVec3(0, 1, 0), monkey_movement.y*delta_t), GLM_convertVec3Vec4(monkey_look, 0)));
	monkey_obj->theta.x+=monkey_movement.y*delta_t;
	angle=(monkey_speed*monkey_movement.z)/4;
	if(monkey_movement.x){
		angle/=sqrtf(2);
	}
	monkey_obj->theta.y+=(angle-monkey_obj->theta.y)*delta_t*4;
	angle=(monkey_speed*-monkey_movement.x)/4;
	if(monkey_movement.z){
		angle/=sqrtf(2);
	}
	monkey_obj->theta.z+=(angle-monkey_obj->theta.z)*delta_t*4;

	Shader_update_Vec3(default_shader, TORCH_POS_U, GLM_addVec3(monkey_obj->pos, GLM_createVec3(0, 1, 0)));
	Shader_update_Vec3(default_shader, TORCH_DIR_U, monkey_look);
}

void load_loading(GLFWwindow *window){
	Object *lobj;
	ui_shader=Shader_create("./res/ui.vs", "./res/ui.fs");
	default_texture=Texture_load("./res/rainbow.png");
	lobj=Object_create();
	lobj->shader=ui_shader;
	Object_attachMesh(lobj, Mesh_create("./res/loading.mesh"));
	lobj->model=GLM_scaleMat4(0.5,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Object_render_static(lobj);
	glfwSwapBuffers(window);
}

void load_resources(void){
	/* shaders */
	default_shader=Shader_create("./res/default.vs", "./res/default.fs");
	ui_shader=Shader_create("./res/ui.vs", "./res/ui.fs");
	
	/* meshes */
	default_mesh=Mesh_create("./res/platform.mesh");
	platform_mesh=Mesh_create("./res/platform.mesh");
	monkey_mesh=Mesh_create("./res/monkey.mesh");
	heart_mesh=Mesh_create("./res/heart.mesh");
	gameover_mesh=Mesh_create("./res/gameover.mesh");

	/* textures */
	default_texture=Texture_load("./res/rainbow.png");
	heart_texture=Texture_load("./res/heart.png");
	platform_texture=Texture_load("./res/platform.png");
	monkey_texture=Texture_load("./res/monkey.png");
}

void load_level1(void){
	int i, j;

	/* light */
	Shader_update_Vec3(default_shader, LIGHT_U, GLM_createVec3(4, 5, -4));
	Shader_update_Vec3(default_shader, TORCH_COLOR_U, GLM_createVec3(0,0,0));
	Shader_update_float(default_shader, TORCH_POWER_U, 5);
	
	/* heart */
	heart_obj=Object_create();
	heart_obj->shader=ui_shader;
	Object_attachMesh(heart_obj, heart_mesh);
	heart_obj->texture=heart_texture;
	heart_obj->pos.y=0.9;
	heart_obj->model=GLM_scaleMat4(0.05, 0.08, 1);
	

	/* monkey */
	monkey_obj=Object_create();
	Object_attachMesh(monkey_obj, monkey_mesh);
	monkey_obj->texture=monkey_texture;
	monkey_obj->model=GLM_scaleMat4(0.5, 0.7, -0.5);
	monkey_obj->spec_color=GLM_createVec3(0.01, 0.01, 0.01);
	Object_add_f(monkey_obj, Object_f_gravity);
	Object_add_f(monkey_obj, Object_f_collision);
	objects[num_objects++]=monkey_obj;

	/* platform */
	for(i=0;i<7;i++){
		for(j=0;j<2;j++){
			platform_obj[i][j]=Object_create();
			platform_obj[i][j]->model=GLM_rotateMat4(GLM_createVec3(0,1,0), (random()%4)*PI/2);
			platform_obj[i][j]->texture=platform_texture;
			platform_obj[i][j]->pos=GLM_createVec3(i, 0, -j);
			Object_attachMesh(platform_obj[i][j], platform_mesh);
			objects[num_objects++]=platform_obj[i][j];
		}
	}
	platform_obj[5][0]->pos=GLM_createVec3(5, 2, 0);
	platform_obj[6][0]->pos=GLM_createVec3(6, 2, 0);
	for(i=0;i<3;i++){
		platform_obj[i][1]->pos=GLM_createVec3(i, 1+0.2*i, -1);
	}
	for(i=5;i<7;i++){
		for(j=2;j<4;j++){
			platform_obj[i][j]=Object_create();
			platform_obj[i][j]->model=GLM_rotateMat4(GLM_createVec3(0,1,0), (random()%4)*PI/2);
			platform_obj[i][j]->texture=platform_texture;
			platform_obj[i][j]->pos=GLM_createVec3(i, 0, -j);
			Object_attachMesh(platform_obj[i][j], platform_mesh);
			objects[num_objects++]=platform_obj[i][j];
		}
	}
	for(i=1;i<7;i++){
		for(j=4;j<7;j++){
			if(j==4||j==6){
				platform_obj[i][j]=Object_create();
				platform_obj[i][j]->model=GLM_rotateMat4(GLM_createVec3(0,1,0), (random()%4)*PI/2);
				platform_obj[i][j]->texture=platform_texture;
				if(j==6){
					platform_obj[i][j]->pos=GLM_createVec3(i, i*0.6, -j);
				}
				else{
					platform_obj[i][j]->pos=GLM_createVec3(i, 0, -j);
				}
				Object_attachMesh(platform_obj[i][j], platform_mesh);
				objects[num_objects++]=platform_obj[i][j];
				if(j==6 && i>4){
					platform_obj[i][j]->emit_color=GLM_createVec3(0.3, 0, 0);
					Object_add_f(platform_obj[i][j], Object_f_burn);
				}
			}
		}
	}
	for(i=7;i<GRID_SIZE;i++){
		for(j=7;j<GRID_SIZE;j++){
			platform_obj[i][j]=Object_create();
			platform_obj[i][j]->model=GLM_rotateMat4(GLM_createVec3(0,1,0), (random()%4)*PI/2);
			platform_obj[i][j]->texture=platform_texture;
			platform_obj[i][j]->pos=GLM_createVec3(i, 0, -j);
			Object_attachMesh(platform_obj[i][j], platform_mesh);
			objects[num_objects++]=platform_obj[i][j];
		}
	}
	
	platform_obj[GRID_SIZE-1][GRID_SIZE-1]->emit_color=GLM_createVec3(0.2, 0.3, 0.2);
	Object_add_f(platform_obj[GRID_SIZE-1][GRID_SIZE-1], Object_f_levelup);

	/* tower */
	tower_obj=Object_create();
	tower_obj->pos=GLM_createVec3(-4, 7, -2);
	objects[num_objects++]=tower_obj;	
}

void load_level2(void){
	int i, j;

	/* clear previous level */
	for(i=0;i<num_objects;i++){
		objects[i]=NULL;
	}
	num_objects=0;
	for(i=0;i<GRID_SIZE;i++){
		for(j=0;j<GRID_SIZE;j++){
			platform_obj[i][j]=NULL;
		}
	}

	monkey_look=GLM_createVec3(0, 0, -1);
	monkey_movement=GLM_createVec3(0, 0, 0);

	/* light */
	Shader_update_Vec3(default_shader, LIGHT_U, GLM_createVec3(-13, 5, 2));
	Shader_update_Vec3(default_shader, TORCH_COLOR_U, GLM_createVec3(0,0,0));
	Shader_update_float(default_shader, TORCH_POWER_U, 5);
	
	/* heart */
	heart_obj=Object_create();
	heart_obj->shader=ui_shader;
	Object_attachMesh(heart_obj, heart_mesh);
	heart_obj->texture=heart_texture;
	heart_obj->pos.y=0.9;
	heart_obj->model=GLM_scaleMat4(0.05, 0.08, 1);
	

	/* monkey */
	monkey_obj=Object_create();
	Object_attachMesh(monkey_obj, monkey_mesh);
	monkey_obj->texture=monkey_texture;
	monkey_obj->model=GLM_scaleMat4(0.5, 0.7, -0.5);
	monkey_obj->spec_color=GLM_createVec3(0.01, 0.01, 0.01);
	Object_add_f(monkey_obj, Object_f_gravity);
	Object_add_f(monkey_obj, Object_f_collision);
	objects[num_objects++]=monkey_obj;

	/* platform */
	i=0;
	for(j=0;j<GRID_SIZE;j++){
			platform_obj[i][j]=Object_create();
			platform_obj[i][j]->model=GLM_rotateMat4(GLM_createVec3(0,1,0), (random()%4)*PI/2);
			platform_obj[i][j]->texture=platform_texture;
			platform_obj[i][j]->pos=GLM_createVec3(0, j/2.0, -j);
			Object_attachMesh(platform_obj[i][j], platform_mesh);
			objects[num_objects++]=platform_obj[i][j];
		
	}

	for(i=1;i<GRID_SIZE;i++){
		for(j=GRID_SIZE-1;j<GRID_SIZE;j++){
			platform_obj[i][j]=Object_create();
			platform_obj[i][j]->model=GLM_rotateMat4(GLM_createVec3(0,1,0), (random()%4)*PI/2);
			platform_obj[i][j]->texture=platform_texture;
			platform_obj[i][j]->pos=GLM_createVec3(i, j/2.0, -j);
			Object_attachMesh(platform_obj[i][j], platform_mesh);
			objects[num_objects++]=platform_obj[i][j];
		}
	}

	for(i=GRID_SIZE-2;i<GRID_SIZE;i++){
		for(j=2;j<GRID_SIZE-1;j++){
			platform_obj[i][j]=Object_create();
			platform_obj[i][j]->model=GLM_rotateMat4(GLM_createVec3(0,1,0), (random()%4)*PI/2);
			platform_obj[i][j]->texture=platform_texture;
			platform_obj[i][j]->pos=GLM_createVec3(i, (GRID_SIZE-1)/2.0, -j);
			Object_attachMesh(platform_obj[i][j], platform_mesh);
			objects[num_objects++]=platform_obj[i][j];
			platform_obj[i][j]->emit_color=GLM_createVec3(0.035, 0.035, 0);
			platform_obj[i][j]->spec_color=GLM_createVec3(0.5, 0.5, 0);
			Object_add_f(platform_obj[i][j], Object_f_dip);
		}
	}

	for(i=GRID_SIZE-2;i<GRID_SIZE;i++){
		for(j=0;j<2;j++){
			platform_obj[i][j]=Object_create();
			platform_obj[i][j]->model=GLM_rotateMat4(GLM_createVec3(0,1,0), (random()%4)*PI/2);
			platform_obj[i][j]->texture=platform_texture;
			platform_obj[i][j]->pos=GLM_createVec3(i, (GRID_SIZE-1)/2.0, -j);
			Object_attachMesh(platform_obj[i][j], platform_mesh);
			objects[num_objects++]=platform_obj[i][j];
		}
	}

	for(i=3;i<GRID_SIZE-2;i++){
		for(j=0;j<2;j++){
			platform_obj[i][j]=Object_create();
			platform_obj[i][j]->model=GLM_rotateMat4(GLM_createVec3(0,1,0), (random()%4)*PI/2);
			platform_obj[i][j]->texture=platform_texture;
			platform_obj[i][j]->pos=GLM_createVec3(i, (GRID_SIZE-1)/2.0+3-fabsf(i-(GRID_SIZE)/2.0), -j);
			Object_attachMesh(platform_obj[i][j], platform_mesh);
			objects[num_objects++]=platform_obj[i][j];
			Object_add_f(platform_obj[i][j], Object_f_oscillate);
		}
	}
	for(i=3;i<4;i++){
		for(j=2;j<GRID_SIZE-3;j++){
			platform_obj[i][j]=Object_create();
			platform_obj[i][j]->model=GLM_rotateMat4(GLM_createVec3(0,1,0), (random()%4)*PI/2);
			platform_obj[i][j]->texture=platform_texture;
			platform_obj[i][j]->pos=GLM_createVec3(i, (GRID_SIZE-1)/2.0, -j);
			Object_attachMesh(platform_obj[i][j], platform_mesh);
			objects[num_objects++]=platform_obj[i][j];
		}
	}

	for(i=3;i<4;i++){
		for(j=GRID_SIZE-3;j<GRID_SIZE-2;j++){
			platform_obj[i][j]=Object_create();
			platform_obj[i][j]->model=GLM_rotateMat4(GLM_createVec3(0,1,0), (random()%4)*PI/2);
			platform_obj[i][j]->texture=platform_texture;
			platform_obj[i][j]->pos=GLM_createVec3(i, (GRID_SIZE-1)/2.0+2, -j);
			Object_attachMesh(platform_obj[i][j], platform_mesh);
			objects[num_objects++]=platform_obj[i][j];
		}
	}


	/* for(i=0;i<GRID_SIZE;i++){ */
	/* 	for(j=0;j<GRID_SIZE;j++){ */
	/* 		platform_obj[i][j]=Object_create(); */
	/* 		platform_obj[i][j]->model=GLM_rotateMat4(GLM_createVec3(0,1,0), (random()%4)*PI/2); */
	/* 		platform_obj[i][j]->texture=platform_texture; */
	/* 		if(i>=2 && i<=5) */
	/* 			Object_add_f(platform_obj[i][j], Object_f_dip); */
	/* 		platform_obj[i][j]->pos=GLM_createVec3(i, 0, -j); */
	/* 		Object_attachMesh(platform_obj[i][j], platform_mesh); */
	/* 		objects[num_objects++]=platform_obj[i][j]; */
	/* 	} */
	/* } */
	/* Object_add_f(platform_obj[1][1], Object_f_burn); */
	/* platform_obj[1][1]->emit_color=GLM_createVec3(0.3, 0, 0); */

	platform_obj[3][GRID_SIZE-4]->emit_color=GLM_createVec3(0.2, 0.3, 0.2);
	Object_add_f(platform_obj[GRID_SIZE-1][GRID_SIZE-1], Object_f_levelup);


	/* tower */
	tower_obj=Object_create();
	tower_obj->pos=GLM_createVec3(-4, 7, -2);
	objects[num_objects++]=tower_obj;	
}

void draw(void){
	int i;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for(i=0;i<num_objects;i++){
		Object_render(objects[i]);
	}
	draw_ui();
}

void draw_ui(void){
	int i;
	float fract;
	Mat4 tmat;
	
	/* drawing hearts */
	for(i=1;i<=(int)(monkey_life/20);i++){
		heart_obj->pos.x=-1.05 + 0.09*i;
		Object_render_static(heart_obj);
	}
	i--;
	if(i*20<monkey_life){
		fract=(monkey_life/20-i)*0.4+0.6;
		tmat=heart_obj->model;
		heart_obj->model=GLM_mulMat4Mat4(tmat, GLM_scaleMat4(fract, fract, 1));
		heart_obj->pos.x=-1.05 + 0.09*(i+1);
		Object_render_static(heart_obj);
		heart_obj->model=tmat;
	}
}

GLFWwindow* initGLFW (void){
	GLFWwindow* window; // window desciptor/handle
	
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	/* plzz dont use. Major FPS loss for no apparent visual improvement */
	//	glfwWindowHint(GLFW_SAMPLES, AA_LEVEL);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	window = glfwCreateWindow(WIDTH, HEIGHT, GAME_NAME, NULL, NULL);
	
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	
	glfwSetWindowCloseCallback(window, quit);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_press_callback);
	glfwSetKeyCallback(window, key_callback);

	return window;
}

void initGL(void){
        // Background color of the scene
        glClearColor(0.05, 0.05, 0.093, 0.0); // R, G, B, A
        glClearDepth(1.0f);
	
	/* Enable depth test*/
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
	glDepthRange(1,-1);

	/* enable transparency */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	/* print OpenGL info  */
        printf("VENDOR:\t%s\n",glGetString(GL_VENDOR));
        printf("RENDERER:\t%s\n",glGetString(GL_RENDERER));
        printf("VERSION:\t%s\n",glGetString(GL_VERSION));
        printf("GLSL:\t%s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void initGLEW(void){
        glewExperimental = GL_TRUE;
        if(glewInit()!=GLEW_OK){
                fprintf(stderr,"Glew failed to initialize : %s\n", glewGetErrorString(glewInit()));
        }
        if(!GLEW_VERSION_3_3)
		fprintf(stderr, "3.3 version not available\n");
}

void error_callback(int error, const char* description){
	fprintf(stderr, "Error No. %d : %s\n", error, description);
}

void quit(GLFWwindow *window){
	Object *gobj;
	gobj=Object_create();
	Object_attachMesh(gobj, gameover_mesh);
	gobj->shader=ui_shader;
	gobj->model=GLM_scaleMat4(0.4, 1, 1);
	gobj->pos=GLM_createVec3(0.07, -0.4, 0);
	gobj->theta.x=PI/6;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Object_render_static(gobj);
	glfwSwapBuffers(window);
	sleep(3);
	glfwDestroyWindow(window);
	glfwTerminate();
	printf("Actual time %f\tGame time %f\n",last_time, game_time);
	printf("Avg. FPS : %f\n", num_frames/last_time);
	exit(EXIT_SUCCESS);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos){
	/* float normX, normY; */
	/* normX=(float)((16.0/WIDTH)*(xpos-WIDTH/2.0)); */
	/* normY=-(float)((9.0/HEIGHT)*(ypos-HEIGHT/2.0)); */
	/* normX/=zoomRatio;normY/=zoomRatio; */
	if(camera_type==5){
		if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS){
			Vec3 planar;
			planar=GLM_unitVec3(GLM_subVec3(heli_look, GLM_projectVec3(heli_look, GLM_createVec3(0, 1, 0))));
			heli_look=GLM_convertVec4Vec3(GLM_mulMat4Vec4(GLM_rotateMat4(GLM_createVec3(planar.z, 0, -planar.x), GLM_toRadian((mouseY-ypos)/20)), GLM_convertVec3Vec4(heli_look, 0)));
			heli_look=GLM_convertVec4Vec3(GLM_mulMat4Vec4(GLM_rotateMat4(GLM_createVec3(0, 1, 0), GLM_toRadian((mouseX-xpos)/20)), GLM_convertVec3Vec4(heli_look, 0)));
		}
		else{
			Vec3 planar;
			planar=GLM_unitVec3(GLM_subVec3(heli_look, GLM_projectVec3(heli_look, GLM_createVec3(0, 1, 0))));
			heli_pos=GLM_addVec3(heli_pos, GLM_mulScalarVec3((ypos-mouseY)/160, planar));
			heli_pos=GLM_addVec3(heli_pos, GLM_mulScalarVec3((mouseX-xpos)/90, GLM_createVec3(-planar.z, 0, planar.x)));
			if(heli_pos.x<-10){
				heli_pos.x=-10;
			}
			else if(heli_pos.x>20){
				heli_pos.x=20;
			}
			if(heli_pos.z>10){
				heli_pos.z=10;
			}
			else if(heli_pos.z<-20){
				heli_pos.z=-20;
			}
		}
	}
	mouseX=xpos;
	mouseY=ypos;
}

void mouse_scroll_callback(GLFWwindow *window, double xoff, double yoff){
	if(camera_type==5){
		if(yoff>0){
			heli_pos.y-=0.5;
		}
		else if(yoff<0){
			heli_pos.y+=0.5;
		}
		if(heli_pos.y<2){
			heli_pos.y=2;
		}
		else if(heli_pos.y>10){
			heli_pos.y=10;
		}
	}
}

void mouse_press_callback(GLFWwindow *window, int button, int action, int mods){
	return ;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	int i,j;

	/* camera control */
	switch(key){
	case GLFW_KEY_1:
	case GLFW_KEY_KP_1:
		camera_type=1;
		break;
	case GLFW_KEY_2:
	case GLFW_KEY_KP_2:
		camera_type=2;
		break;
	case GLFW_KEY_3:
	case GLFW_KEY_KP_3:
		camera_type=3;
		break;
	case GLFW_KEY_4:
	case GLFW_KEY_KP_4:
		camera_type=4;
		break;
	case GLFW_KEY_5:
	case GLFW_KEY_KP_5:
		camera_type=5;
		break;
	}

	/* player control */
	if(action==GLFW_PRESS){
		if(key==GLFW_KEY_SPACE){
				i=(int)(monkey_obj->pos.x+0.5);
				j=-(int)(monkey_obj->pos.z-0.5);
				if(i>=0 && i<GRID_SIZE && j>=0 && j<GRID_SIZE && platform_obj[i][j] && platform_obj[i][j]->pos.y==monkey_obj->pos.y){
					monkey_obj->vel.y=platform_obj[i][j]->vel.y+sqrtf(2.4*GRAVITY);
				}
		}
		if(key==GLFW_KEY_UP){
			monkey_movement.z=-1;
		}
		else if(key==GLFW_KEY_RIGHT){
			monkey_movement.x=1;
		}
		else if(key==GLFW_KEY_DOWN){
			monkey_movement.z=1;
		}
		else if(key==GLFW_KEY_LEFT){
			monkey_movement.x=-1;
		}
		else if(key==GLFW_KEY_F){
			float prev_speed;
			prev_speed=monkey_speed;
			monkey_speed=2.5;
			monkey_obj->vel.x*=monkey_speed/prev_speed;
			monkey_obj->vel.z*=monkey_speed/prev_speed;
		}
		else if(key==GLFW_KEY_S){
			float prev_speed;
			prev_speed=monkey_speed;
			monkey_speed=0.75;
			monkey_obj->vel.x*=monkey_speed/prev_speed;
			monkey_obj->vel.z*=monkey_speed/prev_speed;
		}
		else if(key==GLFW_KEY_A || key==GLFW_KEY_D){
			float angle;
			monkey_movement.y=key==GLFW_KEY_A?1:-1;
		}
		else if(key==GLFW_KEY_T){
			torch_mode=(torch_mode+1)%8;
			switch(torch_mode){
			case 0:
				Shader_update_Vec3(default_shader, TORCH_COLOR_U, GLM_createVec3(0, 0, 0));
				break;
			case 1:
				Shader_update_Vec3(default_shader, TORCH_COLOR_U, GLM_createVec3(0.7, 0, 0));
				break;
			case 2:
				Shader_update_Vec3(default_shader, TORCH_COLOR_U, GLM_createVec3(0, 0.7, 0));
				break;
			case 3:
				Shader_update_Vec3(default_shader, TORCH_COLOR_U, GLM_createVec3(0, 0, 0.7));
				break;
			case 4:
				Shader_update_Vec3(default_shader, TORCH_COLOR_U, GLM_createVec3(0, 0.7, 0.7));
				break;
			case 5:
				Shader_update_Vec3(default_shader, TORCH_COLOR_U, GLM_createVec3(0.7, 0, 0.7));
				break;
			case 6:
				Shader_update_Vec3(default_shader, TORCH_COLOR_U, GLM_createVec3(0.7, 0.7, 0));
				break;
			case 7:
				Shader_update_Vec3(default_shader, TORCH_COLOR_U, GLM_createVec3(0.7, 0.7, 0.7));
				break;
			}
		}
		else if(key==GLFW_KEY_H){
			torch_power=(torch_power+1)%2;
			if(torch_power){
				Shader_update_float(default_shader, TORCH_POWER_U, 13);
			}
			else{
				Shader_update_float(default_shader, TORCH_POWER_U, 5);
			}
		}
	}
	else if(action==GLFW_RELEASE){
		if(key==GLFW_KEY_UP || key==GLFW_KEY_DOWN){
			monkey_movement.z=0;
		}
		else if(key==GLFW_KEY_LEFT || key==GLFW_KEY_RIGHT){
			monkey_movement.x=0;
		}
		else if(key==GLFW_KEY_S || key==GLFW_KEY_F){
			float prev_speed;
			prev_speed=monkey_speed;
			monkey_speed=1.5;
			monkey_obj->vel.x*=monkey_speed/prev_speed;
			monkey_obj->vel.z*=monkey_speed/prev_speed;
		}
		else if(key==GLFW_KEY_A || key==GLFW_KEY_D){
			monkey_movement.y=0;
		}
	}
	return ;
}
