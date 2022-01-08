/*  by Alun Evans 2016 LaSalle (aevanss@salleurl.edu) */

//librerias p3
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//include some standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

//include OpenGL libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//include some custom code files
#include "glfunctions.h" //include all OpenGL stuff
#include "Shader.h" // class to compile shaders
#include "imageloader.h";

using namespace std;
using namespace glm;

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "main.h"

const char teapot[] = "assets/teapot.obj";
const char sphere[] = "assets/sphere.obj";
const char esferaCielo[] = "assets/sphere.obj";

vector<tinyobj::shape_t> shapesCielo;

vector <tinyobj::shape_t> shapesTeapot;
vector <tinyobj::shape_t> shapesSphere;


//global variables to help us do things
int g_ViewportWidth = 1024; int g_ViewportHeight = 1024; // Default window size, in pixels
double mouse_x, mouse_y; //variables storing mouse position
const vec3 g_backgroundColor(0.1f, 0.1f, 0.1f); // background colour - a GLM 3-component vector

//comunes
GLuint g_simpleShader = 0; //shader identifier
GLuint g_simpleShader_sky = 0; //shaderIdentifier sky

GLuint g_Vao_sky = 0; //vao sky
GLuint g_NumTriangles_sky = 0; //triangulos cielo

vec3 center_sky = vec3(0.0f, 0.0f, -1.0f);
vec3 eye_sky = vec3(0.0f, 0.0f, 0.0f);

//vao teapot
GLuint g_Vao = 0; //vao
GLuint g_NumTriangles = 0; //  Numbre of triangles we are painting.

//vao sphere
GLuint g_VaoSphere = 0; //vao
GLuint g_NumTrianglesSphere = 0; //  Numbre of triangles we are painting.

//mover camara wasd
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//pitch and yaw
bool firstMouse = true;
float lastX = 512.0/2.0;
float lastY = 512.0/2.0;
float guiñada=-90.0f;
float inclinacion=0.0f;

GLuint texture_id_estrellas;

// ------------------------------------------------------------------------------------------
// This function manually creates a square geometry (defined in the array vertices[])
// ------------------------------------------------------------------------------------------
void load()
{
	//**********************
	// CODE TO SET GEOMETRY
	//**********************

	//check teapot
	bool ret = tinyobj::LoadObj(shapesTeapot, teapot);
	if (ret)
		cout << "OBJ File: " << teapot << "sucessfully loaded\n ";
	else
		cout << "OBJ File: " << teapot << "error!\n";
	//check sphere
	bool ret2 = tinyobj::LoadObj(shapesSphere, sphere);
	if (ret2)
		cout << "OBJ File: " << sphere << "sucessfully loaded\n ";
	else
		cout << "OBJ File: " << sphere << "error!\n";
	
	bool ret3 = tinyobj::LoadObj(shapesCielo, esferaCielo);
	if (ret2)
		cout << "OBJ File: " << esferaCielo << "sucessfully loaded\n ";
	else
		cout << "OBJ File: " << esferaCielo << "error!\n";

 	//**********************
	// CODE TO LOAD EVERYTHING INTO MEMORY
	//**********************

	//load shader
	Shader simpleShader("src/shader.vert", "src/shader.frag");
	g_simpleShader = simpleShader.program;

	//load sky shader
	Shader simpleShader_sky("src/shader_sky.vert", "src/shader_sky.frag");
	g_simpleShader_sky = simpleShader_sky.program;

	Image* image = loadBMP("assets/milkyway.bmp");

	glGenTextures(1, &texture_id_estrellas);
	glBindTexture(GL_TEXTURE_2D, texture_id_estrellas);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);


	/*
	//vao esfera cielo
	g_Vao_sky = gl_createAndBindVAO();
	gl_createAndBindAttribute(&(shapesCielo[0].mesh.positions[0]), shapesCielo[0].mesh.positions.size()*sizeof(float), g_simpleShader_sky, "a_vertex", 3);
	gl_createIndexBuffer(&(shapesCielo[0].mesh.indices[0]), shapesCielo[0].mesh.indices.size()*sizeof(unsigned int));
	gl_createAndBindAttribute(&(shapesCielo[0].mesh.texcoords[0]), shapesCielo[0].mesh.texcoords.size()*sizeof(GLfloat), g_simpleShader_sky, "a_uv", 2);
	gl_unbindVAO;
	g_NumTriangles_sky = shapesCielo[0].mesh.indices.size() * 3;
	*/

	//VAO teapot
	// Create the VAO where we store all geometry (stored in g_Vao)
	g_Vao = gl_createAndBindVAO();
	//create vertex buffer for positions, colors, and indices, and bind them to shader
	gl_createAndBindAttribute(&(shapesTeapot[0].mesh.positions[0]), sizeof(float) * shapesTeapot[0].mesh.positions.size(), g_simpleShader, "a_vertex", 3);
	gl_createIndexBuffer(&(shapesTeapot[0].mesh.indices[0]), sizeof(unsigned int) * shapesTeapot[0].mesh.indices.size());
	//unbind everything
	gl_unbindVAO();
	//store number of triangles (use in draw())
	g_NumTriangles = shapesTeapot[0].mesh.indices.size() / 3;



	//VAO shpere
	g_VaoSphere = gl_createAndBindVAO();
	//load the shader
	//create vertex buffer for positions, colors, and indices, and bind them to shader
	gl_createAndBindAttribute(&(shapesSphere[0].mesh.positions[0]), sizeof(float) * shapesSphere[0].mesh.positions.size(), g_simpleShader, "a_vertex", 3);
	gl_createIndexBuffer(&(shapesSphere[0].mesh.indices[0]), sizeof(unsigned int) * shapesSphere[0].mesh.indices.size());
	//unbind everything
	gl_unbindVAO();
	//store number of triangles (use in draw())
	g_NumTrianglesSphere = shapesSphere[0].mesh.indices.size() / 3;

}

// ------------------------------------------------------------------------------------------
// This function actually draws to screen and called non-stop, in a loop
// ------------------------------------------------------------------------------------------
void draw()
{
	// set background color
	glClearColor(g_backgroundColor.x, g_backgroundColor.y, g_backgroundColor.z, 1.0);
	//clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//desactivar depth test
	glDisable(GL_DEPTH_TEST);
	//
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glUseProgram(g_simpleShader_sky);
	gl_bindVAO(g_Vao_sky);
	// activate shader
	glUseProgram(g_simpleShader);


	//CIELO
	GLuint u_model = glGetUniformLocation(g_simpleShader_sky, "u_model");
	GLuint u_view = glGetUniformLocation(g_simpleShader_sky, "u_view");
	GLuint u_projection = glGetUniformLocation(g_simpleShader_sky, "u_projection");

	//sertMVP
	mat4 model_matrix = translate(mat4(1.0f), eye_sky);
	mat4 view_matrix = lookAt(eye_sky, center_sky, vec3(0,1,0));
	mat4 projection_matrix = glm::perspective(120.0f, (float)(g_ViewportWidth / g_ViewportHeight), 0.1f, 50.0f);

	//send values to sahder
	glUniformMatrix4fv(u_model, 1, GL_FALSE, glm::value_ptr(model_matrix));
	glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(u_projection, 1, GL_FALSE, glm::value_ptr(projection_matrix));

	GLuint u_texture = glGetUniformLocation(g_simpleShader_sky, "u_texture");
	glUniform1i(u_texture, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id_estrellas);

	gl_bindVAO(g_Vao_sky);
	glDrawElements(g_NumTriangles_sky, 3 * g_NumTriangles_sky, GL_UNSIGNED_INT, 0);


	//dibujar teapot
	gl_bindVAO(g_Vao);
	GLuint colorLoc = glGetUniformLocation(g_simpleShader, "u_color");
	glUniform3f(colorLoc, 0.0, 1.0, 0.0);
	GLuint model_loc = glGetUniformLocation(g_simpleShader, "u_model");
	mat4 model2 = rotate(mat4(1.0f), 0.0f, vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model2));
	// Draw to screen
	glDrawElements(GL_TRIANGLES, 3 * g_NumTriangles, GL_UNSIGNED_INT, 0);

	/*
	//proyecciones
	mat4 projection_matrix = perspective(90.f, 1.0f, 0.1f, 50.0f);
	GLuint projection_loc = glGetUniformLocation(g_simpleShader, "u_projection");
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
	*/

	//camara
	mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	GLuint view_loc = glGetUniformLocation(g_simpleShader, "u_view");
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
	
	//dibujar sphere
	gl_bindVAO(g_VaoSphere);
	GLuint colorLocSphere = glGetUniformLocation(g_simpleShader, "u_color");
	glUniform3f(colorLocSphere, 0.0, 0.0, 1.0);
	GLuint model_locSphere = glGetUniformLocation(g_simpleShader, "u_model");
	//matrices para hacer pequeña i mover + las dos juntas
	mat4 modelSpherePequeña = scale(mat4(1.0f), vec3(0.8f, 0.8f, 0.8f)); // x y z
	mat4 modelSphereMover = translate(mat4(1.0f), vec3(0.0, -0.5, -2.0f)); // x y z
	mat4 juntos = modelSphereMover * modelSpherePequeña;
	glUniformMatrix4fv(model_locSphere, 1, GL_FALSE, glm::value_ptr(juntos)); 
	// Draw to screen
	glDrawElements(GL_TRIANGLES, 3 * g_NumTrianglesSphere, GL_UNSIGNED_INT, 0);
}

// ------------------------------------------------------------------------------------------
// This function is called every time you press a screen
// ------------------------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //quit
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
	//reload
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		load();
	
	//mover con wasd camara
	const float cameraSpeed = 0.3f; // velocidad camara
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// ------------------------------------------------------------------------------------------
// This function is called every time you click the mouse
// ------------------------------------------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        cout << "Left mouse down at" << mouse_x << ", " << mouse_y << endl;
    }
	//codigo para saber si click abajo
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		cout << "Left mouse down" << endl;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		cout << "Left mmouse up" << endl;
	}
}

void onMouseMove(GLFWwindow* window, double x, double y) {
	cout << "Mouse at" << x << ", " << y << endl;
	
		if (firstMouse)
		{
			lastX = x;
			lastY = y;
			firstMouse = false;
		}

		float xoffset = x - lastX;
		float yoffset = lastY - y;
		lastX = x;
		lastY = y;

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		guiñada += xoffset;
		inclinacion += yoffset;

		if (inclinacion > 89.0f)
			inclinacion = 89.0f;
		if (inclinacion < -89.0f)
			inclinacion = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(guiñada)) * cos(glm::radians(inclinacion));
		direction.y = sin(glm::radians(inclinacion));
		direction.z = sin(glm::radians(guiñada)) * cos(glm::radians(inclinacion));
		cameraFront = glm::normalize(direction);
	
}

int main(void)
{

	//setup window and boring stuff, defined in glfunctions.cpp
	GLFWwindow* window;
	if (!glfwInit())return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(g_ViewportWidth, g_ViewportHeight, "Hello OpenGL!", NULL, NULL);
	if (!window) {glfwTerminate();	return -1;}
	glfwMakeContextCurrent(window);	
	glewExperimental = GL_TRUE;
	glewInit();

	//input callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

	//fijar cursor en el medio
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(window, onMouseMove);

	//load all the resources
	load();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
		draw();
        
        // Swap front and back buffers
        glfwSwapBuffers(window);
        
        // Poll for and process events
        glfwPollEvents();
        
        //mouse position must be tracked constantly (callbacks do not give accurate delta)
        glfwGetCursorPos(window, &mouse_x, &mouse_y);

    }

    //terminate glfw and exit
    glfwTerminate();
    return 0;
}


