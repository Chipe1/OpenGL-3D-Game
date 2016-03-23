#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<GL/glew.h>
#include<GL/gl.h>
#include<GLFW/glfw3.h>
#include "stb_image.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define PI 3.14159265358979323846
#define WIDTH 1600
#define HEIGHT 900
#define FPS 60
#define GAME_NAME "Mystic Monkey"
#define GRAVITY 5
#define MAX_FUNCS 5
#define MAX_OBJECTS 500
#define GRID_SIZE 10
