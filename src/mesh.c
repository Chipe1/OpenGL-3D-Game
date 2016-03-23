#include "mesh.h"

Mesh *Mesh_create(const char *fileName){
	Vec3 *positionArray, *normalArray;
	float px, py, pz, uvx, uvy, nx, ny, nz, *uvArray;
	int numVertices, numTriangles, i, j, *indexArray;
	FILE *fp;
	Mesh *toret;

	fp=fopen(fileName,"r");
	if(fp==NULL){
		fprintf(stderr,"Mesh not found : %s\n",fileName);
		exit(1);
	}
	
	toret=(Mesh *)malloc(sizeof(Mesh));

	fscanf(fp,"%d %d",&numVertices,&numTriangles);

	/* get vertex information */
	positionArray=(Vec3 *)malloc(sizeof(Vec3)*numVertices);
	normalArray=(Vec3 *)malloc(sizeof(Vec3)*numVertices);
	uvArray=(float *)malloc(sizeof(float)*numVertices*2);
	for(i=0;i<numVertices;i++){
		fscanf(fp,"%f %f %f %f %f %f %f %f", &px, &py, &pz, &uvx, &uvy, &nx, &ny, &nz);
		positionArray[i]=GLM_createVec3(px, py, pz);
		normalArray[i]=GLM_createVec3(nx, ny, nz);
		uvArray[i<<1]=uvx;
		uvArray[(i<<1)+1]=uvy;
	}
	
	indexArray=malloc(sizeof(int)*3*numTriangles);
	for(i=0;i<numTriangles;i++)
		fscanf(fp,"%d %d %d",indexArray+3*i,indexArray+3*i+1,indexArray+3*i+2);
	toret->drawCount=3*numTriangles;

	glGenBuffers(1,&(toret->buffers[POSITION_VB]));
	glBindBuffer(GL_ARRAY_BUFFER,toret->buffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER,numVertices*sizeof(Vec3),positionArray,GL_STATIC_DRAW);
	free(positionArray);

	glGenBuffers(1,&(toret->buffers[UV_VB]));
	glBindBuffer(GL_ARRAY_BUFFER,toret->buffers[UV_VB]);
	glBufferData(GL_ARRAY_BUFFER,numVertices*sizeof(float)*2,uvArray,GL_STATIC_DRAW);
	free(uvArray);

	glGenBuffers(1,&(toret->buffers[NORMAL_VB]));
	glBindBuffer(GL_ARRAY_BUFFER,toret->buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER,numVertices*sizeof(Vec3),normalArray,GL_STATIC_DRAW);
	free(normalArray);

	glGenBuffers(1, &(toret->buffers[INDEX_VB]));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, toret->buffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*numTriangles*sizeof(int), indexArray, GL_STATIC_DRAW);
	free(indexArray);

	return toret;
}
