#include "Angel.h"
#include "mat.h"
#include "vec.h"

#include<vector>
#include<fstream>
#include<iostream>
using namespace std;

#pragma comment(lib, "glew32.lib")

// 三角面片中的顶点序列
typedef struct vIndex {
	unsigned int a, b, c;
	vIndex(int ia, int ib, int ic) : a(ia), b(ib), c(ic) {}
} vec3i;

const int X_AXIS = 0;
const int Y_AXIS = 1;
const int Z_AXIS = 2;

const int TRANSFORM_SCALE = 0;
const int TRANSFORM_ROTATE = 1;
const int TRANSFORM_TRANSLATE = 2;

const double DELTA_DELTA = 0.1;    // Delta的变化率
const double DEFAULT_DELTA = 0.3;    // 默认的Delta值

double scaleDelta = DEFAULT_DELTA;
double rotateDelta = DEFAULT_DELTA;
double translateDelta = DEFAULT_DELTA;

vec3 scaleTheta(1.0, 1.0, 1.0);    // 缩放控制变量
vec3 rotateTheta(0.0, 0.0, 0.0);    // 旋转控制变量
vec3 translateTheta(0.0, 0.0, 0.0);    // 平移控制变量

GLint matrixLocation;
int currentTransform = TRANSFORM_TRANSLATE;    // 设置当前变换
int mainWindow;
int NUM_VERTICES = 8;

std::vector<vec3i> faces;

vec3* vertices;

void generateCube()
{
	ifstream fin;
	fin.open("cube.off");
	//fin.open("cow.off");
	string aa;
	int nump, numf, zzx;
	fin >> aa;
	fin >> nump >> numf >> zzx;
	NUM_VERTICES = nump;
	cout << nump;
	vertices = new vec3[nump];
	for (int i = 0; i < NUM_VERTICES; i++)
	{
		float a1, a2, a3;
		fin >> a1 >> a2 >> a3;
		vec3 az = vec3(a1, a2, a3);
		vertices[i] = vec3(a1, a2, a3);
	}
	while (numf--)
	{
		int a1, a2, a3, a4;
		fin >> a1 >> a2 >> a3 >> a4;
		vec3i az = vec3i(a2, a3, a4);
		faces.push_back(az);
	}
	fin.close();
}

void init()
{
	generateCube();

	// 创建顶点数组对象
	GLuint vao[1];
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	// 创建并初始化顶点缓存对象
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, NUM_VERTICES * sizeof(vec3), vertices, GL_STATIC_DRAW);

	// 创建并初始化顶点索引缓存对象
	GLuint vertexIndexBuffer;
	glGenBuffers(1, &vertexIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(vec3i), faces.data(), GL_STATIC_DRAW);

	// 读取着色器并使用
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// 从顶点着色器中初始化顶点的位置
	GLuint pLocation = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(pLocation);
	glVertexAttribPointer(pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// 获得矩阵存储位置
	matrixLocation = glGetUniformLocation(program, "matrix");

	// 白色背景
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//mat4 m(1.0, 0.0, 0.0, 0.0,
//	0.0, 1.0, 0.0, 0.0,
//	0.0, 0.0, 1.0, 0.0,
//	0.0, 0.0, 0.0, 1.0);

void display()
{
	// 清理窗口
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 生成变换矩阵
	mat4 m(1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);

	// 可使用Scale(),Translate(),RotateX(),RotateY(),RotateZ()等函数。函数定义在mat.h
	m = Translate(translateTheta) * m;

	m = RotateZ(rotateTheta[2])*m;
	m = RotateY(rotateTheta[1])*m;
	m = RotateX(rotateTheta[0])*m;

	m = Scale(scaleTheta) * m;

	// 从指定位置matrixLocation中传入变换矩阵m
	glUniformMatrix4fv(matrixLocation, 1, GL_TRUE, m);
	// 绘制立方体中的各个三角形
	glDrawElements(GL_TRIANGLES, int(faces.size() * 3), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glutSwapBuffers();
}

bool Kguan = false;
int bz = 0;
int fx = 1;
void idleFunction()
{
	switch (currentTransform)
	{
	case TRANSFORM_SCALE:
		scaleTheta[bz] += fx * scaleDelta / 100;
		//scaleTheta[bz] = fx * scaleDelta / 100;
		break;
	case TRANSFORM_ROTATE:
		rotateTheta[bz] += fx * rotateDelta;
		/*if (bz == 0)
			m = RotateX(fx * rotateDelta) * m;
		else if (bz == 1)
			m = RotateY(fx * rotateDelta) * m;
		else if (bz == 2)
			m = RotateZ(fx * rotateDelta) * m;*/
		break;
	case TRANSFORM_TRANSLATE:
		translateTheta[bz] += fx * translateDelta / 100;
		//translateTheta[bz] = fx * translateDelta / 100;
		break;
	}
	glutPostWindowRedisplay(mainWindow);
}

// 通过Delta值更新Theta
void updateTheta(int axis) {
	bz = axis;
}

// 复原Theta和Delta
void resetTheta()
{
	/*m = (1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);*/
	scaleTheta = vec3(1.0, 1.0, 1.0);
	rotateTheta = vec3(0.0, 0.0, 0.0);
	translateTheta = vec3(0.0, 0.0, 0.0);
	scaleDelta = DEFAULT_DELTA;
	rotateDelta = DEFAULT_DELTA;
	translateDelta = DEFAULT_DELTA;
}

// 更新变化Delta值
void updateDelta(int sign)
{
	switch (currentTransform) {
	case TRANSFORM_SCALE:
		scaleDelta += sign * DELTA_DELTA;
		break;
	case TRANSFORM_ROTATE:
		rotateDelta += sign * DELTA_DELTA;
		break;
	case TRANSFORM_TRANSLATE:
		translateDelta += sign * DELTA_DELTA;
		break;
	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'x':
		updateTheta(0);
		break;
	case 'y':
		updateTheta(1);
		break;
	case 'z':
		updateTheta(2);
		break;
	case 'r':
		fx = 1;
		break;
	case 'f':
		fx = -1;
		break;
	case 'w':
		updateDelta(1);
		break;
	case 's':
		updateDelta(-1);
		break;
	case 't':
		resetTheta();
		break;
	case 033:
		// Esc按键
		exit(EXIT_SUCCESS);
		break;
	}
	glutPostWindowRedisplay(mainWindow);
}


void menuEvents(int menuChoice)
{
	currentTransform = menuChoice;
}

void setupMenu()
{
	glutCreateMenu(menuEvents);
	glutAddMenuEntry("Scale", TRANSFORM_SCALE);
	glutAddMenuEntry("Rotate", TRANSFORM_ROTATE);
	glutAddMenuEntry("Translate", TRANSFORM_TRANSLATE);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void printHelp() {
	printf("%s\n\n", "3D Transfomations");
	printf("Keyboard options:\n");
	printf("q: Increase x\n");
	printf("a: Decrease x\n");
	printf("w: Increase y\n");
	printf("s: Decrease y\n");
	printf("e: Increase z\n");
	printf("d: Decrease z\n");
	printf("r: Increase delta of currently selected transform\n");
	printf("f: Decrease delta of currently selected transform\n");
	printf("t: Reset all transformations and deltas\n");
}

void mainWindowMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (Kguan)
		{
			Kguan = false;
			glutIdleFunc(NULL);
		}
		else
		{
			Kguan = true;
			glutIdleFunc(idleFunction);
		}
	}

}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);    // 窗口支持双重缓冲和深度测试
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	mainWindow = glutCreateWindow("实验二_梁贵皓");

	glewExperimental = GL_TRUE;
	glewInit();

	init();
	setupMenu();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mainWindowMouse);
	// 输出帮助信息
	printHelp();
	// 启用深度测试
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	return 0;
}
