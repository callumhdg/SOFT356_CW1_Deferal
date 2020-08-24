

#include"GL/glew.h"
#include"GL/freeglut.h"
#include"GLFW/glfw3.h"
#include"Shader.h"

#include<glm/glm.hpp>
#include<glm/ext/matrix_transform.hpp>
#include<glm/ext/matrix_clip_space.hpp>
#include<glm/gtc/type_ptr.hpp> 

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

#include<iostream>
#include<sstream>
#include<fstream>
#include<string>
#include<vector>
#include<algorithm>



using namespace std;
using namespace glm;

enum VAOIDs { Cube, NumVAOs = 1 };
enum BufferIDs { Tri, Col, Norm, Text, Ind, NumBuff = 5 };

GLuint VAO[NumVAOs];
GLuint Buff[NumBuff];
GLuint texture1;
GLuint shader;
const GLuint NumOfVertices = 36;

const unsigned int windowHeight = 600;
const unsigned int windowWidth  = 800;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

//camera views
vec3 viewPosition = vec3(0.0f, 0.0f, 3.0f);//x, y, z
vec3 viewFront    = vec3(0.0f, 0.0f,-1.0f);
vec3 viewTop      = vec3(0.0f, 1.0f, 0.0f);

bool initMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0f / 2.0;
float yaw1 = -90.0f;//if set to 0 camera points right
float pitch1 = 0.0f;

float col1 = 0.0f;
float col2 = 0.0f;
float col3 = 0.0f;

float objXaxis = -2.0f;
float objYaxis = 0.0f;
float objZaxis = -4.0f;

void loadObjFile(string fileName, vector<GLfloat>& outVertices, vector<GLfloat>& outTextures, vector<GLfloat>& outNormals) {

	string line;
	ifstream file("Resources/" + fileName + ".obj");

	vector<GLfloat>vert;
	vector<GLfloat>text;
	vector<GLfloat>norm;

	vector<GLuint>vInd; 
	vector<GLuint>tInd;
	vector<GLuint>nInd;

	if (file.is_open()) {

		while (getline(file, line)) {

			cout << line << endl;

			//verticies
			if (line[0] == 'v' && line[1] == ' ') {

				GLfloat fl;

				istringstream data(line.substr(2));

				data >> fl;
				vert.push_back(fl);
				data >> fl;
				vert.push_back(fl);
				data >> fl;
				vert.push_back(fl);

			}  //end of verticies


			//textures & uv indice
			else if (line[0] == 'v' && line[1] == 't') {

				GLfloat fl;

				istringstream data(line.substr(2));

				data >> fl;
				text.push_back(fl);
				data >> fl;
				text.push_back(fl);

			}  //end of textures and uv


			//vector normals
			else if (line[0] == 'v' && line[1] == 'n') {

				GLfloat fl;

				istringstream data(line.substr(2));

				data >> fl;
				norm.push_back(fl);
				data >> fl;
				norm.push_back(fl);
				data >> fl;
				norm.push_back(fl);

			}  //end of normals


			//faces
			else if (line[0] == 'f' && line[1] == ' ') {

				replace(line.begin(), line.end(), '/', ' ');

				GLuint verInd[4], uvInd[4], normInd[4], num;
				istringstream data(line.substr(2));

				//push vertex, textures and normals
				data >> num;
				verInd[0] = num;
				data >> num;
				uvInd[0] = num;
				data >> num;
				normInd[0] = num;


				data >> num;
				verInd[1] = num;
				data >> num;
				uvInd[1] = num;
				data >> num;
				normInd[1] = num;


				data >> num;
				verInd[2] = num;
				data >> num;
				uvInd[2] = num;
				data >> num;
				normInd[2] = num;


				data >> num;
				verInd[3] = num;
				data >> num;
				uvInd[3] = num;
				data >> num;
				normInd[3] = num;


				//triangle 1
				vInd.push_back(verInd[0]);
				tInd.push_back(uvInd[0]);
				nInd.push_back(normInd[0]);

				vInd.push_back(verInd[1]);
				tInd.push_back(uvInd[1]);
				nInd.push_back(normInd[1]);

				vInd.push_back(verInd[2]);
				tInd.push_back(uvInd[2]);
				nInd.push_back(normInd[2]);


				//triangle 2
				vInd.push_back(verInd[0]);
				tInd.push_back(uvInd[0]);
				nInd.push_back(normInd[0]);

				vInd.push_back(verInd[2]);
				tInd.push_back(uvInd[2]);
				nInd.push_back(normInd[2]);

				vInd.push_back(verInd[3]);
				tInd.push_back(uvInd[3]);
				nInd.push_back(normInd[3]);
							   				 			  

			}  //end of faces
					   

		}  //end of while get line
			   		 	  	  	   	

	}  //end of if file open


	for (GLint i = 0; i < vInd.size(); i++) {
		
		GLint vertexIndex = (vInd[i] - 1) * 3;		

		outVertices.push_back(vert[vertexIndex]);
		outVertices.push_back(vert[vertexIndex + 1]);
		outVertices.push_back(vert[vertexIndex + 2]);

		GLint normalIndex = (nInd[i] - 1) * 3;
		outNormals.push_back(norm[normalIndex]);
		outNormals.push_back(norm[normalIndex + 1]);
		outNormals.push_back(norm[normalIndex + 2]);

		GLint textureIndex = (tInd[i] - 1) * 2;
		outTextures.push_back(text[textureIndex]);
		outTextures.push_back(text[textureIndex + 1]);


	}  //end of for
	   	  
	file.close();
}  //end of obj load file


void loadMtlFile(string fileName, vector<GLfloat>& colour, vector<GLfloat>& diffuse, vector<GLfloat>& specular, GLfloat& ns, string& textureName) {

	ifstream file("Resources/" + fileName + ".mtl");
	string line;
	GLfloat transparency = -1;

	if (file.is_open()) {

		GLfloat fl;

		while (getline(file, line)) {

			cout << line << endl;

			//colour
			if (line[0] == 'K' && line[1] == 'a') {

				istringstream data(line.substr(2));

				data >> fl;
				colour.push_back(fl);
				data >> fl;
				colour.push_back(fl);
				data >> fl;
				colour.push_back(fl);
				data >> fl;
				colour.push_back(fl);

			}

			//transparency
			else if (line[0] == 'd' && line[1] == ' ') {

				istringstream data(line.substr(2));

				data >> fl;
				transparency = fl;

				if (transparency == -1){
					transparency = 1;
				}

				colour.push_back(transparency);
			}
			
			//diffuse
			else if (line[0] == 'K' && line[1] == 'd') {

				istringstream data(line.substr(2));

				data >> fl;
				diffuse.push_back(fl);
				data >> fl;
				diffuse.push_back(fl);
				data >> fl;
				diffuse.push_back(fl);
			}

			//specular
			else if (line[0] == 'K' && line[1] == 's') {

				istringstream data(line.substr(2));

				data >> fl;
				specular.push_back(fl);
				data >> fl;
				specular.push_back(fl);
				data >> fl;
				specular.push_back(fl);
			}

			//ns
			else if (line[0] == 'N' && line[1] == 's') {

				istringstream data(line.substr(2));

				data >> fl;
				ns = fl;
			}

			//texture name
			else if (line.substr(0, 6) == "map_Kd") {

				istringstream data(line.substr(7));
				textureName = data.str();
			}


		}  // end of while
		
		textureName = "Resources/textures/" + textureName;

	}  // end of if file open
	
}  //  end of load mtl file

#define BUFFER_OFFSET(a) ((void*)(a))

void initalise(vector<GLfloat>& vertices, vector<GLfloat>& textures, vector<GLfloat>& normals, string textureName, vector<GLfloat>& colour, vector<GLfloat>& diffuse, vector<GLfloat>& specular, GLfloat& ns) {

	glGenVertexArrays(NumVAOs, VAO);
	glBindVertexArray(VAO[Cube]);

	ShaderInfo shaders[] =
	{
		{GL_VERTEX_SHADER,   "Resources/triangles.vert"},
		{GL_FRAGMENT_SHADER, "Resources/triangles.frag"},
		{GL_NONE, NULL}
	};

	shader = LoadShaders(shaders);
	glUseProgram(shader);


	//lighting
	vec4 ambient = vec4(colour[0], colour[1], colour[2], colour[3]);//ambient colour
	GLuint ambientLocation = glGetUniformLocation(shader, "ambient");
 	glUniform4fv(ambientLocation, 1, value_ptr(ambient));//location, count, value
	
	vec3 lightPos = vec3(100.0f, 25.0f, 100.0f);
	GLuint lightSourceLocation = glGetUniformLocation(shader, "lightPos");
	glUniform3fv(lightSourceLocation, 1, value_ptr(lightPos));

	vec3 diff = vec3(diffuse[0], diffuse[1], diffuse[2]);
	GLuint diffuesLocation = glGetUniformLocation(shader, "dLight");
	glUniform3fv(diffuesLocation, 1, value_ptr(diff));

	vec3 spec = vec3(specular[0], specular[1], specular[2]);
	GLfloat shine = ns;
	GLuint specularLightLocation = glGetUniformLocation(shader, "sLight");
	glUniform3fv(specularLightLocation, 1, value_ptr(spec));
	GLuint specularShineLocation = glGetUniformLocation(shader, "sShine");
	glUniform1fv(specularShineLocation, 1, &shine);


	//colours
	GLfloat colours[][4] = {
		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },

		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },

		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },

		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },

		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },

		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },

	};
	   

	//binding
	glGenBuffers(NumBuff, Buff);
	glBindBuffer(GL_ARRAY_BUFFER, Buff[Tri]);//target, buffer
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(Tri, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, Buff[Col]);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(colours), colours, 0);//target, size, data, flags
	glVertexAttribPointer(Col, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));//index, size, type, normalized, stride, pointer

	glBindBuffer(GL_ARRAY_BUFFER, Buff[Norm]);
	glBufferStorage(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), 0);
	glVertexAttribPointer(Norm, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, Buff[Text]);
	glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(GLfloat), textures.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(Text, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


	//texture wrap
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);//puts object the right way up
	GLint width;
	GLint height;
	GLint channels;

	unsigned char* data = stbi_load(textureName.c_str(), &width, &height, &channels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	stbi_image_free(data);
	glUniform1i(glGetUniformLocation(shader, "texture1"), 0);


	mat4 modle = mat4(1.0f);
	modle = scale(modle, vec3(2.0f, 2.0f, 2.0f));

	mat4 view = mat4(1.0f);
	view = translate(view, vec3(0.0f, 0.0f, 1.0f));

	mat4 projection = perspective(45.0f, 4.0f / 3, 0.1f, 10.0f);
	//mat4 modleView = projection * view * modle;
	mat4 modleView = view * modle;

	int mvLocation = glGetUniformLocation(shader, "mvMatrix");
	glUniformMatrix4fv(mvLocation, 1, GL_FALSE, value_ptr(modleView));
	int projectionLocation = glGetUniformLocation(shader, "pMatrix");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(projection));


	glEnableVertexAttribArray(Tri);
	glEnableVertexAttribArray(Col);
	glEnableVertexAttribArray(Text);
	glEnableVertexAttribArray(Norm);

}  //  end of initalise



void display(GLfloat delta) {

	float thisFrame = glfwGetTime();
	deltaTime = thisFrame - lastFrame;
	lastFrame = thisFrame;

	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };//background colour

	glClearBufferfv(GL_COLOR, 0, black);//buffer, draw buffer, const glFloat * value
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(col1, col2, col3, 0.0f);//debug/testing  //changes  background colour
	glEnable(GL_CULL_FACE);

	
	mat4 modleMatrix = mat4(1.0f);
	modleMatrix = scale(modleMatrix, vec3(2.0f, 2.0f, 2.0f));//modle size
	modleMatrix = rotate(modleMatrix, radians(delta), vec3(0.2f, 1.0f, 0.0f));//modle orientation and spin

	mat4 viewMatrix = lookAt(viewPosition, viewPosition + viewFront, viewTop);
	viewMatrix = translate(viewMatrix, vec3(objXaxis, objYaxis, objZaxis));//move the modle back 
	mat4 projectionMatrix = perspective(radians(50.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);//fov, aspect, near, far/n
	mat4 modelView = viewMatrix * modleMatrix;

	int mvLoc = glGetUniformLocation(shader, "mvMatrix");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(modelView));

	int pLoc = glGetUniformLocation(shader, "pMatrix");
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, value_ptr(projectionMatrix));

	glBindVertexArray(VAO[Cube]);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glDrawArrays(GL_TRIANGLES, 0, NumOfVertices);//mode, first num, size
	

	//second stationary object
	mat4 modleMatrix1 = mat4(1.0f);
	mat4 viewMatrix1 = lookAt(viewPosition, viewPosition + viewFront, viewTop);
	viewMatrix1 = translate(viewMatrix1, vec3(0.0f, 0.0f, -3.0f));
	modleMatrix1 = translate(modleMatrix1, vec3(-1.5f, 0.0f, 0.0f));
	mat4 projectionMatrix1 = perspective(radians(50.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

	viewMatrix1 = rotate(viewMatrix1, radians(180.0f), vec3(0.0, 1.0, 0.0));//makes second object face forwards

	mat4 modelView1 = viewMatrix1 * modleMatrix1;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(modelView1));
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, value_ptr(projectionMatrix1));
	glDrawArrays(GL_TRIANGLES, 0, NumOfVertices);

}  //  end of display

void buffCallback(GLFWwindow* window, int width, int height) {

	glViewport(0, 0, width, height);

}


//keyboard movement
void handleInput1(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { // Close on exit
		glfwSetWindowShouldClose(window, true);
		cout << "Window closed" << endl;
	}

	float viewSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {//forward
		viewPosition += viewSpeed * viewFront;
	}
		
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {//backward
		viewPosition += normalize(cross(viewFront, viewTop)) * viewSpeed;
	}

}

void handleInput2(GLFWwindow* window) {
	float viewSpeed = 2.5 * deltaTime;

if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {//left
		viewPosition -= normalize(cross(viewFront, viewTop)) * viewSpeed;
	}
	
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {//right
		viewPosition -= viewSpeed * viewFront;
	}
}

void handleInput3(GLFWwindow* window) {
	float viewSpeed = 2.5 * deltaTime;

if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {//up
		viewPosition += viewTop * viewSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {//down
		viewPosition -= viewTop * viewSpeed;
	}
}


//change background colours
void changeBackgroundColourUp(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && col1 != 1.0f) {//red
		col1 = col1 + 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && col2 != 1.0f) {//green
		col2 = col2 + 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && col3 != 1.0f) {//blue
		col3 = col3 + 0.01f;
	}
}

void changeBackgroundColourDown(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && col1 != 0.0f) {//red
		col1 = col1 - 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && col2 != 0.0f) {//green
		col2 = col2 - 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && col3 != 0.0f) {//blue
		col3 = col3 - 0.01f;
	}
}


//move object
void moveObj1(GLFWwindow* window) {//z inverted (left and right from users perspective)

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		objZaxis = objZaxis - 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		objZaxis = objZaxis + 0.01f;
	}

}

void moveObj2(GLFWwindow* window) {//x inverted (left and right from users perspective)

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		objXaxis = objXaxis - 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		objXaxis = objXaxis + 0.01f;
	}

}

void moveObj3(GLFWwindow* window) {//y 

	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
		objYaxis = objYaxis + 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
		objYaxis = objYaxis - 0.01f;
	}

}


//mouse movement
void mouseInput(GLFWwindow* window, double xPos, double yPos) {

	if (initMouse) {

		lastX = xPos;
		lastY = yPos;
		initMouse = false;
	}

	float xoffset = xPos - lastX;
	float yoffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw1 += xoffset;
	pitch1 += yoffset;

	if (pitch1 > 89.0f) {
		pitch1 = 89.0f;
	}

	if (pitch1 < -89.0f) {
		pitch1 = -89.0f;
	}

	vec3 front;
	front.x = cos(radians(yaw1)) * cos(radians(pitch1));
	front.y = sin(radians(pitch1));
	front.z = sin(radians(yaw1)) * cos(radians(pitch1));
	viewFront = normalize(front);

}


void printControlInfo() {
	cout << "" << endl;
	cout << "'Escape' key terminates the window" << endl;

	cout << "" << endl;
	cout << "'W' moves camera forwards" << endl;
	cout << "'A' moves camera Left" << endl;
	cout << "'S' moves camera backwards" << endl;
	cout << "'D' moves camera right" << endl;
	cout << "'Space' moves camera up" << endl;
	cout << "'C' moves camera down" << endl;

	cout << "" << endl;
	cout << "'R' increases red background" << endl;
	cout << "'F' decreases red background" << endl;
	cout << "'T' increases green background" << endl;
	cout << "'G' decreases green background" << endl;
	cout << "'Y' increases blue background" << endl;
	cout << "'H' decreases blue background" << endl;

	cout << "" << endl;
	cout << "'Up arrow' moves object forwards" << endl;
	cout << "'Left arrow' moves object Left" << endl;
	cout << "'Down arrow' moves object backwards" << endl;
	cout << "'Right arrow' moves object right" << endl;
	cout << "'Page up' moves object up" << endl;
	cout << "'Page down' moves object down" << endl;

	cout << "" << endl;
	cout << "Please enter 'creeper' to load object" << endl;
	cout << "or enter 'info' for control information" << endl;
}


int main(int argc, char** argv) {
	
	string file;

	vector<GLfloat>verticies;
	vector<GLfloat>textures;
	vector<GLfloat>normals;

	vector<GLfloat>colour;
	vector<GLfloat>diffuse;
	vector<GLfloat>specular;
	GLfloat ns;
	string textureName;

	cout << "Please enter 'creeper' to load object" << endl;
	cout << "or enter 'info' for control information" << endl;
	//loop or '2nd file' or '3rd file' etc.



	while (true) {

		cin >> file;

		if (file == "info") {
			printControlInfo();
		}
		else if (file == "creeper") {
					   
			loadObjFile(file, verticies, textures, normals);
			loadMtlFile(file, colour, diffuse, specular, ns, textureName);

			glewExperimental = GL_TRUE;
			glfwInit();

			GLFWwindow* window = glfwCreateWindow(800, 600, "Module Loader", NULL, NULL);//width, height, title

			glfwMakeContextCurrent(window);
			glfwSetFramebufferSizeCallback(window, buffCallback); //resizing window wont crash
			glfwSetCursorPosCallback(window, mouseInput);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glewInit();

			initalise(verticies, textures, normals, textureName, colour, diffuse, specular, ns);

			//reset camera view when loading object
			viewPosition = vec3(0.0f, 0.0f, 3.0f);
			viewFront = vec3(0.0f, 0.0f, -1.0f);
			viewTop = vec3(0.0f, 1.0f, 0.0f);

			//reset movable object location
			objXaxis = -2.0f;
			objYaxis = 0.0f;
			objZaxis = -4.0f;

			//reset background colour
			col1 = 0.0f;
			col2 = 0.0f;
			col3 = 0.0f;
					   
			GLfloat time = 0.0f;

			while (!glfwWindowShouldClose(window)) {

				//breaking these up allows for angled movement (left & up) instead of only one direction 
				handleInput1(window);//z camera movement
				handleInput2(window);//x camera movement
				handleInput3(window);//y camera movement

				changeBackgroundColourUp(window);//increace colour values
				changeBackgroundColourDown(window);//decrease colour values

				moveObj1(window);//z object movement
				moveObj2(window);//x object movement
				moveObj3(window);//y object movement

				display(time);

				glfwSwapBuffers(window);
				glfwPollEvents();

				time += 0.1f;
			}

			glfwDestroyWindow(window);

		}  
		else {
			cout << "Invalid input please try again" << endl;
		}

	}

}  //  end of main

//------------------------------------------------------- end ----------------------------------------------------------------------











