#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <random>
#include <fstream>
#include <iterator>
#include <random>
#include <algorithm>

using namespace std;

random_device seeder;
const auto seed = seeder.entropy() ? seeder() : time(nullptr);
mt19937 eng(static_cast<mt19937::result_type>(seed));
uniform_int_distribution<int> left_right(0, 1);
uniform_real_distribution<float> location_y(-0.3, 0.5);
uniform_real_distribution<float> rand_velocity(0.01, 0.75);

GLvoid destroyFunc(int value);

const int WIN_X = 10, WIN_Y = 10;
const int WIN_W = 800, WIN_H = 800;

const glm::vec3 background_rgb = glm::vec3(0.0f, 0.0f, 0.0f);

bool isCulling = true;

GLfloat mx = 0.0f;
GLfloat my = 0.0f;

GLuint shaderProgramID;
GLuint VAO, EBO;
GLuint VBO;

int curindex = 0;

// 다각형의 정점 좌표들을 저장하는 벡터
std::vector<std::vector<glm::vec3>> polygons = {
	// 3각형
	{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-0.5f, 0.866f, 0.0f),
		glm::vec3(-0.5f, -0.866f, 0.0f)
	},
	// 4각형
	{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)
	},
	// 5각형
	{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.309f, 0.951f, 0.0f),
		glm::vec3(-0.809f, 0.588f, 0.0f),
		glm::vec3(-0.809f, -0.588f, 0.0f),
		glm::vec3(0.309f, -0.951f, 0.0f)
	},
	// 6각형
	{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.5f, 0.866f, 0.0f),
		glm::vec3(-0.5f, 0.866f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-0.5f, -0.866f, 0.0f),
		glm::vec3(0.5f, -0.866f, 0.0f)
	},
	// 8각형
	{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.707f, 0.707f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(-0.707f, 0.707f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-0.707f, -0.707f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.707f, -0.707f, 0.0f)
	},
	// 12각형
	{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.866f, 0.5f, 0.0f),
		glm::vec3(0.5f, 0.866f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(-0.5f, 0.866f, 0.0f),
		glm::vec3(-0.866f, 0.5f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-0.866f, -0.5f, 0.0f),
		glm::vec3(-0.5f, -0.866f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.5f, -0.866f, 0.0f),
		glm::vec3(0.866f, -0.5f, 0.0f)
	}
};
// 각 다각형의 인덱스 삼각형 리스트
std::vector<std::vector<unsigned int>> indexTriangles = {
	// 3각형
	{
		0, 1, 2,
		0, 2, 1
	},
	// 4각형
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 1
	},
	// 5각형
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 1
	},
	// 6각형
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 1
	},
	// 8각형
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 7,
		0, 7, 8,
		0, 8, 1
	},
	// 12각형
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 7,
		0, 7, 8,
		0, 8, 9,
		0, 9, 10,
		0, 10, 11,
		0, 11, 12,
		0, 12, 1
	}
};

std::vector<glm::vec3> color_ref = {
glm::vec3(0.72, 0.45, 0.88),
glm::vec3(0.15, 0.89, 0.34),
glm::vec3(0.90, 0.56, 0.12),
glm::vec3(0.22, 0.68, 0.91),
glm::vec3(0.47, 0.33, 0.75),
glm::vec3(0.84, 0.23, 0.65),
glm::vec3(0.10, 0.94, 0.29),
glm::vec3(0.31, 0.70, 0.54),
glm::vec3(0.61, 0.48, 0.88),
glm::vec3(0.27, 0.57, 0.43) };

uniform_int_distribution<int> rand_shape(0, polygons.size()-1);
uniform_int_distribution<int> rand_color(0, color_ref.size() - 1);

std::vector<std::vector<glm::vec3>> onShape = {};
std::vector<std::vector<unsigned int>> onShapeindex = {};
std::vector<glm::vec3> location = {};
std::vector<glm::vec3> velocity = {};
std::vector<glm::vec3> colors = {};

std::vector<glm::vec3> intersectionPoints;

glm::vec3 gravity = { 0,-0.3,0 };

class Mouse_Line
{
public:
	void set_start(float x,float y) {
		start_x = x;
		start_y = y;
	}
	void set_end(float x, float y) {
		end_x = x;
		end_y = y;
	}
	void Draw_Line() {
		std::vector<glm::vec3> Line = { {start_x,start_y,0},{end_x,end_y,0} };

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, Line.size() * sizeof(glm::vec3), Line.data(), GL_STATIC_DRAW);

		GLint positionAttribute = glGetAttribLocation(shaderProgramID, "positionAttribute");
		glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(positionAttribute);
		glBindVertexArray(0);

		glBindVertexArray(VAO);

		glm::mat4 TR = glm::mat4(1.0f);
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "transform");
		unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "colorAttribute");

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
		glUniform3f(colorLocation, 0.0, 0.0, 1.0);

		glDrawArrays(GL_LINES, 0, Line.size());
	}
	void Debug_print_xy() {
		cout << "(" << start_x << ", " << start_y << ") -> (" << end_x << ", " << end_y << ")" << endl;
	}
	glm::vec2 start() {
		return glm::vec2(start_x, start_y);
	}
	glm::vec2 end() {
		return glm::vec2(end_x, end_y);
	}
private:
	float start_x, start_y;
	float end_x, end_y;
};

Mouse_Line ML1;
bool b_ML1;

class Bucket
{
public:
	void init() {
		glGenVertexArrays(1, &Bucket_VAO);
		glBindVertexArray(Bucket_VAO);

		glGenBuffers(1, &Bucket_VBO);
		glGenBuffers(1, &Bucket_EBO);


		glGenVertexArrays(1, &inBucket_VAO);
		glBindVertexArray(inBucket_VAO);

		glGenBuffers(1, &inBucket_VBO);
		glGenBuffers(1, &inBucket_EBO);
	}
	void set_bucket() {
		glBindVertexArray(Bucket_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, Bucket_VBO);
		glBufferData(GL_ARRAY_BUFFER, Bucket_vertex.size() * sizeof(glm::vec3), Bucket_vertex.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Bucket_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Bucket_index.size() * sizeof(unsigned int), Bucket_index.data(), GL_STATIC_DRAW);

		GLint positionAttribute = glGetAttribLocation(shaderProgramID, "positionAttribute");
		glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(positionAttribute);
		glBindVertexArray(0);
	}
	void Draw_bucket() {
		set_bucket();
		glBindVertexArray(Bucket_VAO);

		glm::mat4 TR = glm::mat4(1.0f);
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "transform");
		unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "colorAttribute");

		TR = glm::translate(TR, bucket_location);
		TR = glm::scale(TR, bucket_size);

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
		glUniform3f(colorLocation, 1.0, 0.0, 0.0);

		glDrawElements(GL_TRIANGLES, Bucket_index.size(), GL_UNSIGNED_INT, (void*)(0));
	}
	void Move_bucket() {
		bucket_location += bucket_velocity * 0.016f;
		for (int i = 0; i < inBucket_location.size(); i++)
		{
			inBucket_location[i] += bucket_velocity * 0.016f;
			Move_inBucket(i);
		}

		if ((bucket_location.x + bucket_size.x >= 1.0f) || (bucket_location.x - bucket_size.x <= -1.0f))
			bucket_velocity.x = -bucket_velocity.x;
	}

	bool is_in_bucket(glm::vec3 location,int index) {
		if (location.x > bucket_location.x - (0.9f * bucket_size.x) && location.x < bucket_location.x + (0.9f * bucket_size.x)) //x좌표 상에는 안쪽
		{
			if (location.y > bucket_location.y - (0.9f * bucket_size.y)) //y좌표가 버켓의 바닥보다 위에있다. 천장보다 아래인지는 호출할때 확인
			{
				//cout << "It's in Bucket" << endl;
				this->push_in_bucket(index);
			}
			return true;
		}
		return false;
	}

	void push_in_bucket(int index) {
		inBucket_vertex.push_back(onShape[index]);
		inBucket_index.push_back(onShapeindex[index]);
		inBucket_location.push_back(location[index]);
		velocity[index].x = 0;
		inBucket_velocity.push_back(velocity[index]);
		inBucket_colors.push_back(colors[index]);
		destroyFunc(index);
	}

	bool toched_boundary(glm::vec3 location, int index) {
		if (location.x + 0.1 > bucket_location.x - (0.9f * bucket_size.x) && location.x - 0.1 < bucket_location.x + (0.9f * bucket_size.x))
			return true;
		return false;
	}

	void add_velocity(int index) {
		velocity[index].x += bucket_velocity.x * 1.1;
	}

	void Set_inBucket(int inBucketindex) {
		glBindVertexArray(inBucket_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, Bucket_VBO);
		glBufferData(GL_ARRAY_BUFFER, inBucket_vertex[inBucketindex].size() * sizeof(glm::vec3), inBucket_vertex[inBucketindex].data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, inBucket_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, inBucket_index[inBucketindex].size() * sizeof(unsigned int), inBucket_index[inBucketindex].data(), GL_STATIC_DRAW);

		GLint positionAttribute = glGetAttribLocation(shaderProgramID, "positionAttribute");
		glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(positionAttribute);
		glBindVertexArray(0);
	}

	void Draw_inBucket() {
		for (int i = 0; i < inBucket_index.size(); i++)
		{
			Set_inBucket(i);

			glBindVertexArray(inBucket_VAO);

			glm::mat4 TR = glm::mat4(1.0f);
			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "transform");
			unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "colorAttribute");

			TR = glm::translate(TR, inBucket_location[i]);
			TR = glm::scale(TR, glm::vec3(.1, .1, .1));

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
			glUniform3f(colorLocation, inBucket_colors[i].x, inBucket_colors[i].y, inBucket_colors[i].z);

			glDrawElements(GL_TRIANGLES, inBucket_index[i].size(), GL_UNSIGNED_INT, (void*)(0));
		}
	}

	void Move_inBucket(int inBucketindex) {
		if (inBucket_location[inBucketindex].x > bucket_location.x - (0.9f * bucket_size.x) && 
			inBucket_location[inBucketindex].x < bucket_location.x + (0.9f * bucket_size.x)) //x좌표 상에는 안쪽
		{
			if (inBucket_location[inBucketindex].y > bucket_location.y - (0.9f * bucket_size.y))
			{
				inBucket_velocity[inBucketindex] += gravity * 0.016f;
			}
			else
			{
				inBucket_velocity[inBucketindex].y = 0.0f;
			}
		}
		inBucket_location[inBucketindex] += inBucket_velocity[inBucketindex] * 0.016f;
	}

private:
	std::vector<glm::vec3> Bucket_vertex = {
		glm::vec3(-1.1,1.0,0),
		glm::vec3(-0.9,1.0,0),

		glm::vec3(-1.1,-0.9,0),

		glm::vec3(-1.1,-1.1,0),
		glm::vec3(-0.9,-1.1,0),

		glm::vec3(1.1,-0.9,0),

		glm::vec3(1.1,-1.1,0),
		glm::vec3(0.9,-1.1,0),

		glm::vec3(1.1,1.0,0),
		glm::vec3(0.9,1.0,0)
	};
	std::vector<unsigned int> Bucket_index = {
		3,1,0,
		3,4,1,
		3,6,2,
		6,5,2,
		9,7,6,
		9,6,8
	};
	glm::vec3 bucket_location = { 0,-0.8,0 };
	glm::vec3 bucket_velocity = { 0.5,0,0 };
	glm::vec3 bucket_size = { .15,.15,.1 };

	std::vector<std::vector<glm::vec3>> inBucket_vertex = {};
	std::vector<std::vector<unsigned int>> inBucket_index = {};
	std::vector<glm::vec3> inBucket_location = {};
	std::vector<glm::vec3> inBucket_velocity = {};
	std::vector<glm::vec3> inBucket_colors = {};

	GLuint inBucket_VAO, inBucket_EBO;
	GLuint inBucket_VBO;

	GLuint Bucket_VAO, Bucket_EBO;
	GLuint Bucket_VBO;
};

Bucket bucket;

class Path
{
public:
	void set_point(glm::vec3 newpoint) {
		Line.push_back(newpoint);
	}
	void Draw_Line(int index) {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, Line.size() * sizeof(glm::vec3), Line.data(), GL_STATIC_DRAW);

		GLint positionAttribute = glGetAttribLocation(shaderProgramID, "positionAttribute");
		glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(positionAttribute);
		glBindVertexArray(0);

		glBindVertexArray(VAO);

		glm::mat4 TR = glm::mat4(1.0f);
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "transform");
		unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "colorAttribute");

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
		glUniform3f(colorLocation, colors[index].x, colors[index].y, colors[index].z);

		glDrawArrays(GL_LINE_STRIP, 0, Line.size());
	}

private:
	std::vector<glm::vec3> Line;
};

std::vector<Path> paths;

bool is_Fill = true;
bool draw_path = false;

float Speed = 1.0f;
float rotation = 1.0f;

GLvoid InitBuffer()
{
	//--- VAO와 VBO 객체 생성
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
}

char* File_To_Buf(const char* file)
{
	ifstream in(file, ios_base::binary);

	if (!in) {
		cerr << file << "파일 못찾음";
		exit(1);
	}

	in.seekg(0, ios_base::end);
	long len = in.tellg();
	char* buf = new char[len + 1];
	in.seekg(0, ios_base::beg);

	int cnt = -1;
	while (in >> noskipws >> buf[++cnt]) {}
	buf[len] = 0;

	return buf;
}

bool Make_Shader_Program() {
	//세이더 코드 파일 불러오기
	const GLchar* vertexShaderSource = File_To_Buf("vertex.glsl");
	const GLchar* fragmentShaderSource = File_To_Buf("fragment.glsl");

	//세이더 객체 만들기
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//세이더 객체에 세이더 코드 붙이기
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//세이더 객체 컴파일하기
	glCompileShader(vertexShader);

	GLint result;
	GLchar errorLog[512];

	//세이더 상태 가져오기
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << endl;
		return false;
	}

	//세이더 객체 만들기
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//세이더 객체에 세이더 코드 붙이기
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	//세이더 객체 컴파일하기
	glCompileShader(fragmentShader);
	//세이더 상태 가져오기
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << endl;
		return false;
	}

	//세이더 프로그램 생성
	shaderProgramID = glCreateProgram();
	//세이더 프로그램에 세이더 객체들을 붙이기
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	//세이더 프로그램 링크
	glLinkProgram(shaderProgramID);

	//세이더 객체 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//프로그램 상태 가져오기
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderProgramID, 512, NULL, errorLog);
		cerr << "ERROR: shader program 연결 실패\n" << errorLog << endl;
		return false;
	}
	//세이더 프로그램 활성화
	glUseProgram(shaderProgramID);

	return true;
}

void Set_VAO(int polygonindex) {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, onShape[polygonindex].size() * sizeof(glm::vec3), onShape[polygonindex].data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, onShapeindex[polygonindex].size() * sizeof(unsigned int), onShapeindex[polygonindex].data(), GL_STATIC_DRAW);

	GLint positionAttribute = glGetAttribLocation(shaderProgramID, "positionAttribute");
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionAttribute);
	glBindVertexArray(0);
}

void Draw_Shape(int polygonindex) {
	glBindVertexArray(VAO);

	glm::mat4 TR = glm::mat4(1.0f);
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "transform");
	unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "colorAttribute");

	TR = glm::translate(TR, location[polygonindex]);
	TR = glm::rotate(TR, glm::radians(rotation), glm::vec3(0, 0, 1));
	TR = glm::scale(TR, glm::vec3(.1, .1, .1));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	glUniform3f(colorLocation, colors[polygonindex].x, colors[polygonindex].y, colors[polygonindex].z);

	glDrawElements(GL_TRIANGLES, onShapeindex[polygonindex].size(), GL_UNSIGNED_INT, (void*)(0));
}

void Draw_points() {glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, intersectionPoints.size() * sizeof(glm::vec3), intersectionPoints.data(), GL_STATIC_DRAW);

	GLint positionAttribute = glGetAttribLocation(shaderProgramID, "positionAttribute");
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionAttribute);
	glBindVertexArray(0);

	glBindVertexArray(VAO);

	glm::mat4 TR = glm::mat4(1.0f);
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "transform");
	unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "colorAttribute");

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	glUniform3f(colorLocation, 0.0, 1.0, 1.0);
	glPointSize(5);
	glDrawArrays(GL_POINTS, 0, intersectionPoints.size());
}

GLvoid drawScene()
{
	glClearColor(background_rgb.x, background_rgb.y, background_rgb.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	is_Fill ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram(shaderProgramID);

	for (int i = 0; i < onShape.size(); i++)
	{
		Set_VAO(i);
		Draw_Shape(i);
		if (draw_path)
			paths[i].Draw_Line(i);
	}

	if (b_ML1)
	{
		ML1.Draw_Line();
	}
	//Draw_points();

	bucket.Draw_inBucket();
	bucket.Draw_bucket();

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid destroyFunc(int value) {
	onShape.erase(onShape.begin() + value);
	onShapeindex.erase(onShapeindex.begin() + value);
	location.erase(location.begin() + value);
	velocity.erase(velocity.begin() + value);
	colors.erase(colors.begin() + value);
	paths.erase(paths.begin() + value);
}

void throw_new_shape() {
	int newindex = -1;
	glm::vec3 newlocation = glm::vec3{ 0,location_y(eng),0 };
	newindex = rand_shape(eng);
	onShape.push_back(polygons[newindex]);
	onShapeindex.push_back(indexTriangles[newindex]);
	if (left_right(eng))
	{
		newlocation.x = -1;
		location.push_back(newlocation);
		velocity.push_back(glm::vec3{ rand_velocity(eng),rand_velocity(eng),0});
	}
	else
	{
		newlocation.x = 1;
		location.push_back(newlocation);
		velocity.push_back(glm::vec3{ -rand_velocity(eng),rand_velocity(eng),0});
	}
	colors.push_back(color_ref[rand_color(eng)]);
	Path newpath;
	newpath.set_point(newlocation);
	paths.push_back(newpath);
}

bool doIntersect(glm::vec2 p1, glm::vec2 p2, glm::vec2 q1, glm::vec2 q2, glm::vec2& intersection,int index) {
	glm::vec2 r = p2 - p1;
	glm::vec2 s = q2 - q1;
	
	float rxs = r.x * s.y - r.y * s.x;

	if (rxs == 0) return false;
	
	float t = ((q1 - p1).x * s.y - (q1 - p1).y * s.x) / rxs;
	float u = ((q1 - p1).x * r.y - (q1 - p1).y * r.x) / rxs;

	if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
		intersection = p1 + t * r;
		return true;
	}
	return false;
}

std::vector<unsigned int> CreateTriangleIndexList(const std::vector<glm::vec3>& vertices) {
	std::vector<unsigned int> indices;

	if (vertices.size() < 3) {
		std::cerr << "Error: 정점이 3개 미만 - 삼각형을 만들 수 없음." << std::endl;
		return indices;
	}

	size_t vertexCount = vertices.size();

	for (size_t i = 0; i < vertexCount; ++i) {
		for (size_t j = i + 1; j < vertexCount; ++j) {
			for (size_t k = j + 1; k < vertexCount; ++k) {
				indices.push_back(i);
				indices.push_back(j);
				indices.push_back(k);
			}
		}
	}

	return indices;
}

void SplitPolygonByIntersection(int shapeIndex) {
	std::vector<glm::vec3>& originalVertices = onShape[shapeIndex];

	std::vector<glm::vec3> newVertices1, newVertices2;
	std::vector<unsigned int> newIndices1, newIndices2;

	if (intersectionPoints.size() < 2) {
		std::cerr << "Error: 정점이 2개 미만 - 분리 불가능한 도형임." << std::endl;
		return;
	}

	glm::vec3 p1 = intersectionPoints[0];
	glm::vec3 p2 = intersectionPoints[1];

	auto isLeftOrAbove = [&](const glm::vec3& vertex) {
		float position = (p2.x - p1.x) * (vertex.y - p1.y) - (p2.y - p1.y) * (vertex.x - p1.x);
		return position > 0;
		};

	// intersectionPoints를 기준으로 기존 정점 리스트를 두 개로 분리
	for (const auto& vertex : originalVertices) {
		if (isLeftOrAbove(vertex)) {
			newVertices1.push_back(vertex); // 왼쪽 or 위
		}
		else {
			newVertices2.push_back(vertex); // 오른쪽 or 아래
		}
	}

	newVertices1.push_back(p1);
	newVertices2.push_back(p1);
	newVertices1.push_back(p2);
	newVertices2.push_back(p2);

	// 새로운 인덱스 리스트 생성
	newIndices1 = CreateTriangleIndexList(newVertices1);
	newIndices2 = CreateTriangleIndexList(newVertices2);

	// 기존 도형의 정보를 복사
	glm::vec3 cur_location = location[shapeIndex];
	glm::vec3 cur_velocity = velocity[shapeIndex];
	glm::vec3 cur_color = colors[shapeIndex];
	Path cur_path = paths[shapeIndex];
	cur_path.set_point(cur_location);

	// 기존 도형 삭제
	destroyFunc(shapeIndex);

	// 분리된 도형에 기존 도형의 정보를 추가
	onShape.push_back(newVertices1);
	onShapeindex.push_back(newIndices1);
	location.push_back(cur_location);
	velocity.push_back(glm::vec3(-.1, cur_velocity.y +.1, cur_velocity.z));
	colors.push_back(cur_color);
	paths.push_back(cur_path);

	onShape.push_back(newVertices2);
	onShapeindex.push_back(newIndices2);
	location.push_back(cur_location);
	velocity.push_back(glm::vec3(.1, cur_velocity.y -.1, cur_velocity.z));
	colors.push_back(cur_color);
	paths.push_back(cur_path);

	// 교차점 vector 초기화
	intersectionPoints.clear();
}

bool checkIntersections(const glm::vec2& lineStart, const glm::vec2& lineEnd, const std::vector<glm::vec3>& polygon, int index) {
	glm::vec2 intersection;
	int Intersections = 0;
	
	for (size_t i = 0; i < polygon.size(); ++i) {
		glm::vec2 p1 = glm::vec2((polygon[i].x * 0.1f) + location[index].x, (polygon[i].y * 0.1f) + location[index].y);
		glm::vec2 p2 = glm::vec2((polygon[(i + 1) % polygon.size()].x * 0.1f) + location[index].x, (polygon[(i + 1) % polygon.size()].y * 0.1f) + location[index].y);

		if (doIntersect(lineStart, lineEnd, p1, p2, intersection, index)) {
			intersection = (intersection - glm::vec2(location[index].x, location[index].y)) * 10.0f;
			//cout << "Intersection at: (" << intersection.x << ", " << intersection.y << ")" << endl;
			intersectionPoints.push_back(glm::vec3(intersection.x, intersection.y, 0));
			Intersections++;
		}
	}

	if (Intersections < 2)
	{
		intersectionPoints.clear();
		return false;
	}
	else
	{
		return true;
	}
}

GLvoid TimerFunction(int value)
{
	glutPostRedisplay();
	switch (value)
	{
	case 0: //도형 생성 타이머
		throw_new_shape();
		glutTimerFunc(3000, TimerFunction, 0);
		break;
	case 1:
		for (int i = 0; i < onShape.size(); i++)
		{
			velocity[i] += (gravity * Speed) * 0.016f;
			location[i] += velocity[i] * Speed * 0.016f;
			if (location[i].y <= -0.6)
				if (!bucket.is_in_bucket(location[i], i)) //true면 x범위 안쪽 y가 버켓보다 아래임 / false확인이니 x범위가 밖인 것들만 if문 실행
					if (bucket.toched_boundary(location[i], i)) //경계에 닿았으면
						bucket.add_velocity(i);
		}

		for (int i = 0; i < onShape.size(); i++)
			if (location[i].y < -1.0)
				destroyFunc(i);

		bucket.Move_bucket();

		for (int i = 0; i < onShape.size(); i++)
		{
			for (size_t j = 0; j < onShape[i].size(); j++)
			{
				glm::mat4 TR = glm::mat4(1.0f);
				TR = glm::rotate(TR, glm::radians(rotation), glm::vec3(0, 0, 1));
				glm::vec4 newpoint = TR * glm::vec4(onShape[i][j], 1.0);
				onShape[i][j] = glm::vec3(newpoint.x, newpoint.y, newpoint.z);
			}
		}
		glutTimerFunc(16, TimerFunction, 1);
		break;
	case 2:
		for (int i = 0; i < onShape.size(); i++)
		{
			paths[i].set_point(location[i]);
		}
		glutTimerFunc(300, TimerFunction, 2);
		break;
	default:
		break;
	}


}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	vector<int> new_opnenface = {};
	switch (key) {
	case 'm':
		is_Fill = !is_Fill;
		break;
	case 'p':
		draw_path = !draw_path;
		break;
	case '+':
		if (Speed < 10.0f)
			Speed += 0.1f;
		break;
	case '-':
		if (Speed > 0.1f)
			Speed -= 0.1f;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	GLfloat half_w = WIN_W / 2.0f;
	mx = (x - half_w) / half_w;
	my = (half_w - y) / half_w;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		ML1.set_start(mx, my);
		ML1.set_end(mx, my);
		b_ML1 = true;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		ML1.set_end(mx, my);
		b_ML1 = false;

		for (int i = 0; i < onShape.size(); i++)
		{
			if (checkIntersections(ML1.start(), ML1.end(), onShape[i], i)) {
				SplitPolygonByIntersection(i);
				break;
			}
		}

		//ML1.Debug_print_xy();
	}
	glutPostRedisplay();
}

void MouseMotion(int x, int y) {
	GLfloat half_w = WIN_W / 2.0f;
	mx = (x - half_w) / half_w;
	my = (half_w - y) / half_w;

	if (b_ML1 == true) {
		ML1.set_end(mx, my);
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	//윈도우 생성
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(WIN_X, WIN_Y);
	glutInitWindowSize(WIN_W, WIN_H);
	glutCreateWindow("Example1");

	//GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	if (!Make_Shader_Program()) {
		cerr << "Error: Shader Program 생성 실패" << endl;
		std::exit(EXIT_FAILURE);
	}

	InitBuffer();
	bucket.init();
	glutTimerFunc(100, TimerFunction, 0);
	glutTimerFunc(16, TimerFunction, 1);
	glutTimerFunc(300, TimerFunction, 2);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutMainLoop();
}