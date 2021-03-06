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
//摄像机位置
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -100.0f);
//摄像机朝向
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
//摄像机法线（垂直）
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 model(1.f);
glm::mat4 view(1.f);
glm::mat4 projection(1.f);
glm::vec3 chosenPos(1.f);


//打开兔子模型
void openfile(string file);
//对点信息初始化处理
void vertical_v();
//处理键盘事件
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
//处理鼠标位置
void mouse_pos(GLFWwindow* window, GLdouble x, GLdouble y);
//处理鼠标点击事件
void mouse_callback(GLFWwindow* window, int key, int action, int hold);
//进行兔子的移动
void movement();
//打印点信息
void test_mat();
//鼠标点击屏幕位置，x是x轴，y是y轴，z是深度
GLdouble mouse_x = 0.0;
GLdouble mouse_y = 0.0;
GLfloat mouse_z = 0;



//灯光设置，x，z为光源坐标，ra为选装角度（灯光环绕着模型）
GLfloat light_x = 50.0f;
GLfloat light_z = 50.0f;
GLfloat ra = 0.0f;
//点和切片信息
GLfloat points[34834 * 3] = {0.0f};
GLuint indices[69451 *3];
GLfloat points_ver[34834 * 3];
GLfloat points_comp[34834 * 6];

int point_index = 0;
int indice_index = 0;
//键盘事件监听
bool keys[1024];
//移动速度信息
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void openfile(string file){
	//读取兔子模型文件，将点的信息和三角切片的信息储存到数组中
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

	
	//根据自己的设置读取不同长度
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	
	

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window)){
	
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		movement();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);



		ourShader.Use();

		GLint objectColorLoc = glGetUniformLocation(ourShader.Program, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(ourShader.Program, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(ourShader.Program, "lightPos");
		GLint chosenPosLoc = glGetUniformLocation(ourShader.Program, "chosenPos");

		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		glUniform3f(lightColorLoc, 1.0f, 0.5f, 1.0f);
		glUniform3f(lightPosLoc, light_x , 50.0f, light_z);
		glUniform3f(chosenPosLoc, chosenPos.x, chosenPos.y, chosenPos.z);
		

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
		//glDrawElements(GL_LINES, indice_index, GL_UNSIGNED_INT, 0);
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
	double data_z_min = 10;
	int index = -1;
	for (int i = 0; i < 34834; i++) {
		test = glm::vec4(points[3 * i], points[3 * i + 1], points[3 * i + 2], 1.0f);
		test = projection * view * model * test;//将原坐标转换为最终坐标
		test1 = glm::vec3(test.x / test.w * 400 + 400, test.y / test.w * 300 + 300, test.z / test.w);//最终坐标转换为屏幕坐标
		data = pow((test1.x - mouse_x), 2) + pow((test1.y - mouse_y), 2) + pow((test1.z - mouse_z), 2);//与鼠标点击的屏幕坐标做比较
		if (data < 10) {
			if (test1.z < data_z_min) {//深度信息越小，越近，避免点到对面的面
				if (data < data_min) {
					data_min = data;
					data_z_min = test1.z;
					index = i;
				}
			}
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
		chosenPos = glm::vec3(points[index * 3], points[index * 3 + 1], points[index * 3 + 2]);
		chosenPos = glm::vec3(model * glm::vec4(chosenPos, 1.0f));
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
		glReadPixels((int)mouse_x, (int)mouse_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &mouse_z);//求mouse_z，为深度信息
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
			//摄像机所有坐标绕着这个向量旋转，保证视线内物体位置不变
			//先让pos沿着弧线转，再将front，up也沿着转，达到物体核心相对位置不变的效果
			glm::mat4 trans(1.f);
			glm::vec3 nor = glm::normalize(glm::cross(cameraPos - cameraFront, cameraUp));//求pos绕着旋转的轴
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
		if (keys[GLFW_KEY_V]) {//灯光位置变化
			ra += 0.01;
			light_x = 50.0f * sin(ra);
			light_z = 50.0f * cos(ra);
		}
		if (keys[GLFW_KEY_P]) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		
		
}


