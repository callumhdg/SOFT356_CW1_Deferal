

#include"GL/glew.h"
#include"GL/freeglut.h"
#include"GLFW/glfw3.h"

#include"Shader.h"

#include<glm/glm.hpp>
#include<glm/ext/matrix_transform.hpp>
#include<glm/ext/matrix_clip_space.hpp>
#include<glm/gtc/type_ptr.hpp> 

#include<iostream>
#include<sstream>
#include<fstream>
#include<string>
#include<vector>



using namespace std;


enum VAO_IDs { Cube, NumVAOs = 1 };
enum Buffer_IDs { Tri, Col, Norm, Text, Ind, NumBuff = 5 };

GLuint VAOs[NumVAOs];
GLuint Buff[NumBuff];
GLuint texture1;

GLuint shader;
const GLuint  NumOfVertices = 36;


















