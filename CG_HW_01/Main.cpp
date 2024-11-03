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

using namespace std;

random_device seeder;
const auto seed = seeder.entropy() ? seeder() : time(nullptr);
mt19937 eng(static_cast<mt19937::result_type>(seed));

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

// 다각형의 정점 좌표들을 저장하는 벡터 생성
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
	},
	// 24각형
	{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.966f, 0.259f, 0.0f),
		glm::vec3(0.866f, 0.5f, 0.0f),
		glm::vec3(0.707f, 0.707f, 0.0f),
		glm::vec3(0.5f, 0.866f, 0.0f),
		glm::vec3(0.259f, 0.966f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(-0.259f, 0.966f, 0.0f),
		glm::vec3(-0.5f, 0.866f, 0.0f),
		glm::vec3(-0.707f, 0.707f, 0.0f),
		glm::vec3(-0.866f, 0.5f, 0.0f),
		glm::vec3(-0.966f, 0.259f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-0.966f, -0.259f, 0.0f),
		glm::vec3(-0.866f, -0.5f, 0.0f),
		glm::vec3(-0.707f, -0.707f, 0.0f),
		glm::vec3(-0.5f, -0.866f, 0.0f),
		glm::vec3(-0.259f, -0.966f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.259f, -0.966f, 0.0f),
		glm::vec3(0.5f, -0.866f, 0.0f),
		glm::vec3(0.707f, -0.707f, 0.0f),
		glm::vec3(0.866f, -0.5f, 0.0f),
		glm::vec3(0.966f, -0.259f, 0.0f)
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
	},
	// 24각형
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
		0, 12, 13,
		0, 13, 14,
		0, 14, 15,
		0, 15, 16,
		0, 16, 17,
		0, 17, 18,
		0, 18, 19,
		0, 19, 20,
		0, 20, 21,
		0, 21, 22,
		0, 22, 23,
		0, 23, 24,
		0, 24, 1
	}
};

uniform_int_distribution<int> rand_shape(0, polygons.size()-1);

std::vector<std::vector<glm::vec3>> onShape = {};
std::vector<std::vector<unsigned int>> onShapeindex = {};
std::vector<glm::vec3> location = {};
std::vector<glm::vec3> velocity = {};

std::vector<glm::vec3> intersectionPoints;

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

		glDrawArrays(GL_LINES, 0, Line.size() * 3);
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
	TR = glm::scale(TR, glm::vec3(.1, .1, .1));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
	glUniform3f(colorLocation, 1.0, 0.0, 0.0);

	glDrawElements(GL_TRIANGLES, onShapeindex[polygonindex].size() * 3, GL_UNSIGNED_INT, (void*)(0));
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
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(shaderProgramID);

	for (int i = 0; i < onShape.size(); i++)
	{
		Set_VAO(i);
		Draw_Shape(i);
	}

	if (b_ML1)
	{
		ML1.Draw_Line();
	}
	Draw_points();

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
}

GLvoid TimerFunction1(int value)
{
	int newindex = -1;
	glutPostRedisplay();
	switch (value)
	{
	case 0: //도형 생성 타이머
		newindex = rand_shape(eng);
		onShape.push_back(polygons[newindex]);
		onShapeindex.push_back(indexTriangles[newindex]);
		location.push_back(glm::vec3{ -1,-1,0 });
		velocity.push_back(glm::vec3{ 0.5,0.8,0 });
		//glutTimerFunc(100, TimerFunction1, 0);
		break;
	case 1:
		for (int i = 0; i < onShape.size(); i++)
		{
			velocity[i] += glm::vec3(0,-0.3,0) * 0.016f;
			location[i] += velocity[i] * 0.016f;
		}
		glutTimerFunc(16, TimerFunction1, 1);
		break;
	default:
		break;
	}


}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	vector<int> new_opnenface = {};
	switch (key) {
	case 'r':
		intersectionPoints.clear();
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

bool doIntersect(glm::vec2 p1, glm::vec2 p2, glm::vec2 q1, glm::vec2 q2, glm::vec2& intersection,int index) {
	glm::vec2 r = p2 - p1;
	glm::vec2 s = q2 - q1;

	float rxs = r.x * s.y - r.y * s.x; // Cross product
	float qp_r = (q1 - p1).x * r.y - (q1 - p1).y * r.x; // Cross product

	// Check if the lines are parallel
	if (rxs == 0) {
		return false; // Collinear or parallel
	}

	float t = ((q1 - p1).x * s.y - (q1 - p1).y * s.x) / rxs;
	float u = qp_r / rxs;

	// Check if the intersection point is within both segments
	if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
		intersection = p1 + t * r; // Calculate the intersection point
		return true;
	}
	return false;
}

std::vector<unsigned int> CreateTriangleIndexList(const std::vector<glm::vec3>& vertices) {
	std::vector<unsigned int> indices;

	// 주어진 정점의 개수가 3개 미만이면 삼각형을 만들 수 없음
	if (vertices.size() < 3) {
		std::cerr << "Not enough vertices to form triangles." << std::endl;
		return indices;
	}

	size_t vertexCount = vertices.size();

	// 모든 정점에서 다른 정점으로 삼각형 인덱스 생성
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
	// 기존 도형의 정점 및 인덱스 가져오기
	std::vector<glm::vec3>& originalVertices = onShape[shapeIndex];

	// 분리된 두 도형을 저장할 새로운 벡터 초기화
	std::vector<glm::vec3> newVertices1, newVertices2;
	std::vector<unsigned int> newIndices1, newIndices2;

	// Ensure there are two intersection points to define the line segment
	if (intersectionPoints.size() < 2) {
		std::cerr << "Error: Less than two intersection points found." << std::endl;
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
			newVertices1.push_back(vertex); // Left or above
		}
		else {
			newVertices2.push_back(vertex); // Right or below
		}
	}

	newVertices1.push_back(p1);
	newVertices2.push_back(p1);
	newVertices1.push_back(p2);
	newVertices2.push_back(p2);

	// 새로운 인덱스 리스트 생성
	newIndices1 = CreateTriangleIndexList(newVertices1);
	newIndices2 = CreateTriangleIndexList(newVertices2);
	glm::vec3 cur_location = location[shapeIndex];
	glm::vec3 cur_velocity = velocity[shapeIndex];

	// 기존 도형 삭제
	destroyFunc(shapeIndex);

	// 분리된 도형을 onShape와 onShapeindex에 추가
	onShape.push_back(newVertices1);
	onShapeindex.push_back(newIndices1);
	location.push_back(cur_location);
	velocity.push_back(glm::vec3(-.1, cur_velocity.y +.1, cur_velocity.z));

	onShape.push_back(newVertices2);
	onShapeindex.push_back(newIndices2);
	location.push_back(cur_location);
	velocity.push_back(glm::vec3(.1, cur_velocity.y -.1, cur_velocity.z));

	intersectionPoints.clear();
}

// Function to check for intersections with all edges of a polygon
bool checkIntersections(const glm::vec2& lineStart, const glm::vec2& lineEnd, const std::vector<glm::vec3>& polygon, int index) {
	glm::vec2 intersection;
	int Intersections = 0;
	for (size_t i = 0; i < polygon.size(); ++i) {
		glm::vec2 p1 = glm::vec2((polygon[i].x * 0.1f) + location[index].x, (polygon[i].y * 0.1f) + location[index].y);
		glm::vec2 p2 = glm::vec2((polygon[(i + 1) % polygon.size()].x * 0.1f) + location[index].x, (polygon[(i + 1) % polygon.size()].y * 0.1f) + location[index].y);

		if (doIntersect(lineStart, lineEnd, p1, p2, intersection, index)) {
			intersection = (intersection - glm::vec2(location[index].x, location[index].y)) * 10.0f;
			cout << "Intersection at: (" << intersection.x << ", " << intersection.y << ")" << endl;
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

		ML1.Debug_print_xy();
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

	glutTimerFunc(100, TimerFunction1, 0);
	glutTimerFunc(16, TimerFunction1, 1);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutMainLoop();
}