#define GLEW_STATIC
#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "SOIL.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
const GLuint WIDTH = 800, HEIGHT =600;

//camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -100.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 model(1.f);
glm::mat4 view(1.f);
glm::mat4 projection(1.f);


void openfile(string file);
void vertical_v();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_pos(GLFWwindow* window, GLdouble x, GLdouble y);
void mouse_callback(GLFWwindow* window, int key, int action, int hold);
void movement();

GLdouble mouse_x = 0.0;
GLdouble mouse_y = 0.0;
GLfloat mouse_z = 0;

GLfloat init_x = 0;
GLfloat init_y = 0;
GLfloat init_z = 0;




GLfloat points[34834 * 3] = {0.0f};
GLuint indices[69451 *3];
GLfloat points_ver[34834 * 3];
GLfloat points_comp[34834 * 6];

int point_index = 0;
int indice_index = 0;

bool keys[1024];
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void openfile(string file){
	ifstream infile;
	infile.open(file.data());
	assert(infile.is_open());
	string s;
	int countline = 0;
	int totalpoint = 0;
	int totalsrf = 0;
	char *p;
	const char* split = " ";
	while(getline(infile, s)){
		if(countline == 0)
		{
			totalpoint = atoi(s.c_str());
			countline ++;
			totalpoint += 2;
		}
		else if(countline == 1)
		{
			totalsrf = atoi(s.c_str());
			countline++;
		}
		else
		{
			if(countline < totalpoint)
			{
				p = strtok(const_cast<char*>(s.c_str()), split);
				points[point_index ++] = atof(p);
				p = strtok(NULL, split);
				points[point_index ++] = atof(p);
				p = strtok(NULL, split);
				points[point_index ++] = atof(p);
			}
			else
			{
				p = strtok(const_cast<char*>(s.c_str()), split);
				p = strtok(NULL, split);
				indices[indice_index ++] = atof(p);
				p = strtok(NULL, split);
				indices[indice_index ++] = atof(p);
				p = strtok(NULL, split);
				indices[indice_index ++] = atof(p);
		
			}
			countline++;
		}
	}
	infile.close();
}

void vertical_v() {
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 v3;
	int i;
	int a;
	int b;
	int c;
	int indice_i = indice_index / 3;
	for (int i = 0; i < indice_i; i++) {
		a = indices[3 * i];
		b = indices[3 * i + 1];
		c = indices[3 * i + 2];
		v1 = glm::vec3(points[3 * b] - points[3 * a], points[3 * b + 1] - points[3 * a + 1], points[3 * b + 2] - points[3 * a + 2]);
		v2 = glm::vec3(points[3 * c] - points[3 * a], points[3 * c + 1] - points[3 * a + 1], points[3 * c + 2] - points[3 * a + 2]);
		v3 = glm::cross(v1, v2);
		points_ver[3 * a] += v3.x;
		points_ver[3 * a + 1] += v3.y;
		points_ver[3 * a + 2] += v3.z;
		points_ver[3 * b] += v3.x;
		points_ver[3 * b + 1] += v3.y;
		points_ver[3 * b + 2] += v3.z;
		points_ver[3 * c] += v3.x;
		points_ver[3 * c + 1] += v3.y;
		points_ver[3 * c + 2] += v3.z;
	}
	int point_i = point_index / 3;
	for (i = 0; i < point_i; i++) {
		v1 = glm::vec3(points_ver[3 * i], points_ver[3 * i + 1], points_ver[3 * i + 2]);
		v1 = glm::normalize(v1);
		points_comp[6 * i] = points[3 * i];
		points_comp[6 * i + 1] = points[3 * i + 1];
		points_comp[6 * i + 2] = points[3 * i + 2];
		points_comp[6 * i + 3] = v1.x;
		points_comp[6 * i + 4] = v1.y;
		points_comp[6 * i + 5] = v1.z;
	
	}
	
}

int main(){
	std::cout << "Starting" << std::endl;
	openfile("happy\\bunny_iH.ply2");
	vertical_v();
	//for (int i = 0; i < 100; i++) {
	//	std::cout << indices[i] << std::endl;
	//}

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "rabbit", NULL, NULL);
	if (window == NULL){
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);//键盘的回调函数
	glfwSetCursorPosCallback(window, mouse_pos);//鼠标坐标的回调函数
	glfwSetMouseButtonCallback(window, mouse_callback);//鼠标点击的回调函数
	glewExperimental = GL_TRUE;
	glewInit();
	if(glewInit() != GLEW_OK){
		std::cout << "Fail" << std::endl;
		return -1;
	}
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);

	//shader
	Shader ourShader("happy\\vertex.vs", "happy\\fragment.frag");
	

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points_comp), points_comp, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

	

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	
	

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	while (!glfwWindowShouldClose(window)){
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		movement();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		ourShader.Use();

		GLint objectColorLoc = glGetUniformLocation(ourShader.Program, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(ourShader.Program, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(ourShader.Program, "lightPos");
		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		glUniform3f(lightColorLoc, 1.0f, 0.5f, 1.0f);
		glUniform3f(lightPosLoc, -50.0f, 50.0f, -50.0f);

	
		
		


		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		view = glm::lookAt(cameraPos, cameraFront, cameraUp);
		projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 10000.0f);
		

		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indice_index, GL_UNSIGNED_INT, 0);
		
		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}


void test_mat( ) {//测试每个点经过变化后的投影
	glm::vec4 test = glm::vec4(1.f);
	glm::vec3 test1 = glm::vec3(1.f);
	double data = 0;
	double data_min = 10;
	int index = -1;
	for (int i = 0; i < 34834; i++) {
		test = glm::vec4(points[3 * i], points[3 * i + 1], points[3 * i + 2], 1.0f);
		test = projection * view * model * test;
		test1 = glm::vec3(test.x / test.w * 400 + 400, test.y / test.w * 300 + 300, test.z / test.w);
		data = pow((test1.x - mouse_x), 2) + pow((test1.y - mouse_y), 2) + pow((test1.z - mouse_z), 2);
		if (data < 10)
			if (data < data_min) {
				data_min = data;
				index = i;
			}
	}
	if (index < 0) {
		std::cout << "Haven't choose any valid pots" << std::endl;
		std::cout << "\n" << std::endl;
	}
	else {
		std::cout << "The index of chosen pot is: " << std::endl;
		std::cout << index << std::endl;
		std::cout << "Chosen pot's cor_x is: " << std::endl;
		std::cout << points[index * 3] << std::endl;
		std::cout << "Chosen pot's cor_y is: " << std::endl;
		std::cout << points[index * 3 + 1] << std::endl;
		std::cout << "Chosen pot's cor_z is: " << std::endl;
		std::cout << points[index * 3 + 2] << std::endl;
		std::cout << "\n" << std::endl;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
	//std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void mouse_pos(GLFWwindow* window, GLdouble x, GLdouble y){
	
	mouse_x = x;
	mouse_y = HEIGHT - y;
}

void mouse_callback(GLFWwindow* window, int key, int action, int hold) {
	if (key == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
		glReadPixels((int)mouse_y, (int)mouse_x, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &mouse_z);
		glm::vec4 now_vec = glm::vec4(1.f);
		now_vec = glm::vec4(float(mouse_x), float(mouse_y), mouse_z, 1.0f);
		now_vec = glm::inverse(model) * glm::inverse(view) * glm::inverse(projection) * now_vec;

		std::cout << "the corrod_x is:" << std::endl;
		std::cout << mouse_x << std::endl;
		std::cout << "; the corrod_y is:" << std::endl;
		std::cout << mouse_y << std::endl;
		std::cout << "; the corrod_z is:" << std::endl;
		std::cout << mouse_z << std::endl;
		test_mat();
	}
}



void movement() {
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_I]) {//拉远视角
		cameraSpeed /= 10;
		cameraFront -= cameraPos;
		cameraPos += cameraSpeed * cameraFront;
		cameraFront += cameraPos;
	}

	if (keys[GLFW_KEY_U]) {//拉近视角
		cameraSpeed /= 10;
		cameraFront -= cameraPos;
		cameraPos -= cameraSpeed * cameraFront;
		cameraFront += cameraPos;
	}

	if (keys[GLFW_KEY_W]) {//上移
		cameraFront -= cameraPos;
		cameraPos -= glm::normalize(cameraUp) * cameraSpeed * 5.0f;
		cameraFront += cameraPos;
	}

	if (keys[GLFW_KEY_S]) {//下移
		cameraFront -= cameraPos;
		cameraPos += glm::normalize(cameraUp) * cameraSpeed * 5.0f;
		cameraFront += cameraPos;
}
	if (keys[GLFW_KEY_A]) {//左移
		cameraFront -= cameraPos;
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * 5.0f;
		cameraFront += cameraPos;
	}
	if (keys[GLFW_KEY_D]) {//右移
		cameraFront -= cameraPos;
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * 5.0f;
		cameraFront += cameraPos;
	}
		if (keys[GLFW_KEY_E]) {//顺时针旋转
			glm::mat4 trans(1.f);
			glm::vec4 cam = glm::vec4(cameraUp.x, cameraUp.y, cameraUp.z, 1.0f);
			trans = glm::rotate(trans, glm::radians(-0.5f), glm::normalize(cameraPos));
			cam = cam * trans;
			cameraUp = glm::vec3(cam.x, cam.y, cam.z);

			cam = glm::vec4(cameraFront.x, cameraFront.y, cameraFront.z, 1.0f);
			trans = glm::mat4(1.f);
			trans = glm::rotate(trans, glm::radians(-0.5f), glm::normalize(cameraPos));
			cam = cam * trans;
			cameraFront = glm::vec3(cam.x, cam.y, cam.z);
	}
		if (keys[GLFW_KEY_Q]) {//逆时针旋转
		glm::mat4 trans(1.f);
		glm::vec4 cam = glm::vec4(cameraUp.x, cameraUp.y, cameraUp.z, 1.0f);
		trans = glm::rotate(trans, glm::radians(0.5f), glm::normalize(cameraPos));
		cam = cam * trans;
		cameraUp = glm::vec3(cam.x, cam.y, cam.z);

		cam = glm::vec4(cameraFront.x, cameraFront.y, cameraFront.z, 1.0f);
		trans = glm::mat4(1.f);
		trans = glm::rotate(trans, glm::radians(0.5f), glm::normalize(cameraPos));
		cam = cam * trans;
		cameraFront = glm::vec3(cam.x, cam.y, cam.z);

		//

		}
		
		if (keys[GLFW_KEY_C]) {//向右转
			glm::mat4 trans(1.f);
			glm::vec4 cam = glm::vec4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
			trans = glm::rotate(trans, glm::radians(0.5f), glm::normalize(cameraUp));
			cam = cam * trans;
			cameraPos = glm::vec3(cam.x, cam.y, cam.z);
			cam = glm::vec4(cameraFront.x, cameraFront.y, cameraFront.z, 1.0f);
			trans = glm::mat4(1.f);
			trans = glm::rotate(trans, glm::radians(0.5f), glm::normalize(cameraUp));
			cam = cam * trans;
			cameraFront = glm::vec3(cam.x, cam.y, cam.z);
		}
		if (keys[GLFW_KEY_Z]) {//向左转
			glm::mat4 trans(1.f);
			glm::vec4 cam = glm::vec4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
			trans = glm::rotate(trans, glm::radians(-0.5f), glm::normalize(cameraUp));
			cam = cam * trans;
			cameraPos = glm::vec3(cam.x, cam.y, cam.z);
			cam = glm::vec4(cameraFront.x, cameraFront.y, cameraFront.z, 1.0f);
			trans = glm::mat4(1.f);
			trans = glm::rotate(trans, glm::radians(-0.5f), glm::normalize(cameraUp));
			cam = cam * trans;
			cameraFront = glm::vec3(cam.x, cam.y, cam.z);
		}

		if (keys[GLFW_KEY_J]) {//向上转
			//cameraFront -= cameraPos;
			glm::mat4 trans(1.f);
			glm::vec3 nor = glm::normalize(glm::cross(cameraPos - cameraFront, cameraUp));//摄像机所有坐标绕着这个向量旋转，保证视线内物体位置不变
			glm::vec4 cam = glm::vec4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
			trans = glm::rotate(trans, glm::radians(0.05f), nor);
			cam = cam * trans;
			cameraPos = glm::vec3(cam.x, cam.y, cam.z);
			trans = glm::mat4(1.f);
			cam = glm::vec4(cameraFront.x, cameraFront.y, cameraFront.z, 1.0f);
			trans = glm::rotate(trans, glm::radians(0.05f), nor);
			cam = cam * trans;
			cameraFront = glm::vec3(cam.x, cam.y, cam.z);
			trans = glm::mat4(1.f);
			cam = glm::vec4(cameraUp.x, cameraUp.y, cameraUp.z, 1.0f);
			trans = glm::rotate(trans, glm::radians(0.05f), nor);
			cam = cam * trans;
			cameraUp = glm::vec3(cam.x, cam.y, cam.z);
		}
		if (keys[GLFW_KEY_K]) {// 向下转
			glm::mat4 trans(1.f);
			glm::vec3 nor = glm::normalize(glm::cross(cameraPos - cameraFront, cameraUp));
			glm::vec4 cam = glm::vec4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
			trans = glm::rotate(trans, glm::radians(-0.05f), nor);
			cam = cam * trans;
			cameraPos = glm::vec3(cam.x, cam.y, cam.z);
			trans = glm::mat4(1.f);
			cam = glm::vec4(cameraFront.x, cameraFront.y, cameraFront.z, 1.0f);
			trans = glm::rotate(trans, glm::radians(-0.05f), nor);
			cam = cam * trans;
			cameraFront = glm::vec3(cam.x, cam.y, cam.z);
			trans = glm::mat4(1.f);
			cam = glm::vec4(cameraUp.x, cameraUp.y, cameraUp.z, 1.0f);
			trans = glm::rotate(trans, glm::radians(-0.05f), nor);
			cam = cam * trans;
			cameraUp = glm::vec3(cam.x, cam.y, cam.z);
		}
		

}


