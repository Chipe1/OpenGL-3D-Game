#include "texture.h"

GLuint Texture_load(const char *texName){
        GLuint toret;
        int width,height,numComponents;
        unsigned char *imgData=stbi_load(texName,&width,&height,&numComponents,STBI_rgb);
        if(imgData==NULL){
                fprintf(stderr,"Failed to load %s texture file\n",texName);
                exit(1);
        }
        glGenTextures(1,&toret);
        glBindTexture(GL_TEXTURE_2D,toret);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,imgData);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        stbi_image_free(imgData);
        return toret;
}

void Texture_bind(GLuint texture,int unit){
        if(unit<0||unit>31){
                fprintf(stderr,"unit=%d is out of range\n",unit);
                exit(1);
        }
        glActiveTexture(GL_TEXTURE0+unit);
        glBindTexture(GL_TEXTURE_2D,texture);
}
