#include "shader.h"

extern Shader *current_shader;

Shader *Shader_create(const char *vsname,const char *fsname){
	Shader *toret;
	int i;
	toret=(Shader *)malloc(sizeof(Shader));
	toret->prog=glCreateProgram();
	toret->shaders[0]=Shader_load(vsname,GL_VERTEX_SHADER);
	toret->shaders[1]=Shader_load(fsname,GL_FRAGMENT_SHADER);	
	for(i=0;i<NUM_SHADERS;i++)
		glAttachShader(toret->prog,toret->shaders[i]);
	glLinkProgram(toret->prog);Shader_checkError(toret->prog,GL_LINK_STATUS,1,"Program Linking Failed : ");
	glValidateProgram(toret->prog);Shader_checkError(toret->prog,GL_VALIDATE_STATUS,1,"Program is Invalid : ");
	toret->uniforms[MVP_U]=glGetUniformLocation(toret->prog, "MVP");
	toret->uniforms[MODEL_U]=glGetUniformLocation(toret->prog, "M");
	toret->uniforms[VIEW_U]=glGetUniformLocation(toret->prog, "V");
	toret->uniforms[LIGHT_U]=glGetUniformLocation(toret->prog, "light_pos");
	toret->uniforms[TORCH_POS_U]=glGetUniformLocation(toret->prog, "torch_pos");
	toret->uniforms[TORCH_DIR_U]=glGetUniformLocation(toret->prog, "torch_direction");
	toret->uniforms[TORCH_COLOR_U]=glGetUniformLocation(toret->prog, "torch_color");
	toret->uniforms[TORCH_POWER_U]=glGetUniformLocation(toret->prog, "torch_power");
	toret->uniforms[EMIT_U]=glGetUniformLocation(toret->prog, "emit_color");
	toret->uniforms[SPEC_U]=glGetUniformLocation(toret->prog, "spec_color");
	
	/* debug for unused uniforms */
	/* for(i=0;i<NUM_UNIFORMS;i++){ */
	/* 	if(toret->uniforms[i]==-1){ */
	/* 		fprintf(stderr, "Uniform number %d failed to get value\n",i); */
	/* 	} */
	/* } */
	return toret;
}

GLuint Shader_load(const char *name,GLenum stype){
	GLuint toret;
	char *source;
	int len;
	FILE *fp;
	toret=glCreateShader(stype);
	if(toret==0)
		fprintf(stderr,"Shader creation failed\n");
	fp=fopen(name,"r");
	if(fp==NULL){
		fprintf(stderr,"Cant open file : %s\n", name);
		exit(1);
	}
	fseek(fp,0,SEEK_END);
	len=ftell(fp);
	source=(char *)malloc(sizeof(char)*(len+1));
	rewind(fp);
	fread(source,len,sizeof(char),fp);
	source[len]='\0';
	fclose(fp);

	/* paranoid error check */
	if(strlen(source)!=len){
		printf("Different lengths\nstrlen=%d\tlen=%d\nstring = %s\n",(int)strlen(source),len,source);
	}
	
	glShaderSource(toret,1,(const GLchar **)&source,NULL);
	glCompileShader(toret);
	sprintf(source,"Failed to compile the file %s\n",name);
	Shader_checkError(toret,GL_COMPILE_STATUS,0,source);
	free(source);
	return toret;
}

void Shader_bind(Shader *shader){
	if(current_shader!=shader){
		glUseProgram(shader->prog);
		current_shader=shader;
	}
}

void Shader_checkError(GLuint shader,GLuint flag,char isProg,const char *mesg){
	GLint success=0;
	GLchar error[1024]={0};
	if(isProg)
		glGetProgramiv(shader,flag,&success);
	else
		glGetShaderiv(shader,flag,&success);
	if(success==GL_FALSE){
		if(isProg)
			glGetProgramInfoLog(shader,sizeof(error),NULL,error);
		else
			glGetShaderInfoLog(shader,sizeof(error),NULL,error);
		fprintf(stderr,"%s %s\n",mesg,error);
	}
}

void Shader_update_Mat4(Shader *shader, int uniform, Mat4 MVPMatrix){
	Shader_bind(shader);
	glUniformMatrix4fv(shader->uniforms[uniform],1,GL_TRUE,&MVPMatrix.e[0][0]);
}

void Shader_update_Vec3(Shader *shader, int uniform, Vec3 vec){
	Shader_bind(shader);
	glUniform3f(shader->uniforms[uniform], vec.x, vec.y, vec.z);
}

void Shader_update_float(Shader * shader, int uniform,  float f){
	Shader_bind(shader);
	glUniform1f(shader->uniforms[uniform], f);	
}
