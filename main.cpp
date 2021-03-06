#pragma warning(disable: 4996)

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm> 
#include <time.h>
#include <cstdio>
#include<glm/glm.hpp>
#include<glm/gtx/transform.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include<GL/wglew.h>
#include <GL/glut.h>
#include<GL/freeglut.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glut.h"
#include "imgui/imgui_impl_opengl2.h"
#include "pGNUPlotU.h"
#include "StdAfx.h"
#include <math.h>
#include <filesystem>


#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed
#endif

// Our state
static bool show_demo_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

#define WINDOW_HEIGHT 400
#define WINDOW_WIDTH 1600
#define SUBWINDOW_WIDTH 400
#define SUBWINDOW_HEIGHT 400
#define TO_RADIAN 0.01745329252f 
#define DRAG_STATE 0.5f // 드래그 할때 회전하는 정도

using namespace std;

GLuint programID;
GLuint programID2; //박스 프로그램
GLuint VertexArrayID;

clock_t start_time, end_time;
clock_t st1, st2, st3, ed1, ed2, ed3;
double tt1 = 0.0;
double tt2 = 0.0;

int mainWindow, subWindow1, subWindow2, subWindow3, subWindow4;

string filename = "piggybank.obj";
string filename2 = "piggybank.obj";

string change_filename = "cube.obj";    //바꿀 파일 이름
string mtlpath; //mtl 파일명 저장..

int changetype = 1; //sample = 1, 환자 = 2
float scale = 0.3f;
float scale2 = 0.3f;

int subwindow_num = 1;

vector<GLuint>vertexIndices, texIndices, normalIndices;
vector<GLuint>vertexIndices2, texIndices2, normalIndices2;
vector<glm::vec3>obj_vertices, obj_vertices2;
vector<glm::vec2>obj_texcoord, obj_texcoord2;
vector<glm::vec3>obj_normals, obj_normals2;
vector<glm::vec3>aColor, aColor_v2, aColor2, aColor2_v2;
vector<glm::vec3>bColor, bColor2; // box 후 컬러.
string sub_line[4];
string divide_slash[4];


//고광연 추가
glm::vec3 first_vec, second_vec;	//calc_sin에서 각에 따른 weight주기 위함
int second_nav_check = 0; //near vertex의 near vertex 탐색 여부
int color_step = 1, color_step2 = 1;
float threshold = 0.6;
int curv_distrib[10] = { 0 }, curv_distrib2[10] = { 0 };
int change_curv_distrib[10] = { 0 }; // curv_distrube 변경하기위한 용도
double curv_percent[10] = { 0 }, curv_percent2[10] = { 0 };
CpGnuplotU plot(_T("gnuplot\\bin\\wgnuplot.exe"));

float limitZ = -2.0f, limitZ2 = -2.0f;
float minZ, maxZ;



float box_vertices[12] = {
   0.0f, 0.0f, -1.0f,
   0.0f, 0.0f, -1.0f,
   0.0f, 0.0f, -1.0f,
   0.0f, 0.0f, -1.0f
};
float box_vertices2[12] = {
   0.0f, 0.0f, -1.0f,
   0.0f, 0.0f, -1.0f,
   0.0f, 0.0f, -1.0f,
   0.0f, 0.0f, -1.0f
};

float testbox[12] = {
   0.5f, 0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
   -0.5f, -0.5f, -0.5f,
   -0.5f, 0.5f, -0.5f
};


GLuint vbo_box; //박스 위치 버퍼
int mouseX, mouseX2;   // 마우스로 시점 이동시 전의 마우스좌표 저장  / 초기 설정 -1
int mouseY, mouseY2;
float StartmouseX, StartmouseX2; // 박스 시작 위치 x
float StartmouseY, StartmouseY2; // 박스 시작 위치 y
float EndmouseX, EndmouseX2; //q
float EndmouseY, EndmouseY2;
float varX, varX2; //박스 끝위치 x
float varY, varY2; //박스 끝위치 y
float MovevarX, MovevarY; //움직인 위치
float MovevarX2, MovevarY2;
glm::vec4 imMovevec;
bool Holding = FALSE;
bool BoxHolding = FALSE; // BOX DRAG
bool BoxMoving = FALSE; //박스 움직이는중
bool BoxMoving2 = FALSE; //박스 움직이는중
clock_t start_t, end_t; // 시간 계산 전역변수

struct Material
{
	glm::vec3 MTL_Kd; // Read diffuse
	glm::vec3 MTL_Ka; // Read ambient
	glm::vec3 MTL_Ks; // Read specular
	float Ni;
	int illum;
};
Material material; //생성


struct vertex_info {
	double near_vertex_count = 0;
	glm::vec3 normal_vec = glm::vec3(0., 0., 0.);
	float normal_count = 0;
	vector<int>near_vertex;   //주변 점
	vector<int> near_face;   //테스트용 : near_face_2와 값이 같은지 확인하기 위함

	double calc_curv = 0;
	double calc_count = 0;

	double calc_curv2 = 0;
	double calc_count2 = 0;
};
vertex_info* vertexInfo;
vertex_info* vertexInfo2;

int first_f_check = 0; //처음 f를 읽었는지 체크

int vertex_count = 0;
int vertex_count2 = 0;
int face_count = 0;
int face_count2 = 0;


float maxX = 1.0f, maxX2 = 1.0f;
float minX = -1.0f, minX2 = -1.0f;
float maxY = 1.0f, maxY2 = 1.0f;
float minY = -1.0f, minY2 = -1.0f;
float panRad = 0.f, panRad2 = 0.f;
float tiltRad = 0.f, tiltRad2 = 0.f;
float transX = 0.f, transX2 = 0.f;
float transY = 0.f, transY2 = 0.f;
float transY_indexZ = 0.f; //z index표시부분이동을 위함
float transZ = 2.0f, transZ2 = 2.0f;
float zNear = 0.01f, zNear2 = 0.01f;
float zFar = 10.0f, zFar2 = 10.0f;
int orthoOn = 1, orthoOn2 = 1;
int first = 1, first2 = 1;
float persRad = 60, persRad2 = 60;
int lightTurnOnOff = 1, lightTurnOnOff2 = 1;

glm::mat4 realMat = glm::mat4(1.0f), realMat2 = glm::mat4(1.0f); // 박스좌표 계산을 위한 매트릭스
glm::vec4 vPos, vPos2; // 박스좌표 계산을 위한 좌표
glm::mat4 wmat = glm::mat4(1.0f), wmat2 = glm::mat4(1.0f);
glm::mat4 viewmat = glm::mat4(1.0f), viewmat2 = glm::mat4(1.0f), viewmat4 = glm::mat4(1.0f);
glm::mat4 tiltmat = glm::mat4(1.0f), tiltmat2 = glm::mat4(1.0f), tiltmat4 = glm::mat4(1.0f);
glm::mat4 rotmat = glm::mat4(1.0f), rotmat2 = glm::mat4(1.0f);
glm::mat4 transmat = glm::mat4(1.0f), transmat2 = glm::mat4(1.0f), transmat4 = glm::mat4(1.0f);
glm::mat4 orthoMat = glm::mat4(1.0f), orthoMat2 = glm::mat4(1.0f);
glm::mat4 projectmat = glm::mat4(1.0f), projectmat2 = glm::mat4(1.0f);
glm::mat4 normalMat = glm::mat4(1.0f), normalMat2 = glm::mat4(1.0f);
glm::vec3 lightPos = glm::vec3(10.0f, 10.0f, 10.0f), lightPos2 = glm::vec3(10.0f, 10.0f, 10.0f);

glm::mat4 movemat = glm::mat4(1.0f);
glm::mat4 movemat2 = glm::mat4(1.0f);

GLuint vbo[2];
GLuint vao[2];
unsigned int ebo;

void filestreaminit(void); // 파일 리스트 읽는 함수
int returnCurvscope(double calc_curv); //박스 선택 후 curvdistrib 정보 변경.
void renderScene(void);
void renderSceneNew();
void renderScene2(void);
void renderScene3(void);
void renderScene4(void);
void renderSceneAll(void);
void show_graph(double percent[], double percent2[], int len);
void boxto_change_graph(); //박스 후 그래프 바꿔주는 것.
void boxto_change_graph2(); //박스 후 그래프 바꿔주는 것.
void exit_graph(CpGnuplotU* plot);
void SelectScale(); //  Scale 정해주는 함수 : 추후 구현 예정
void changeFile(char* file_str);
void changeFile2(char* file_str);
void changeFile3(char* file_str);
void changeFile4(char* file_str);
void parseMtllib(string line); //mtl 파일명 저장.
void onReadMTLFile(string path);
void parseKd(string line); // mtl파일의 Kd 저장.
void parseKa(string line); // mtl파일의 ka 저장.
void parseKs(string line); // mtl파일의 ks 저장.
int HowDrawBox(float start_X, float start_Y, float End_X, float End_Y);

void SelectScale() {
	if (changetype == 1) {
		scale = 0.003f;
		scale2 = 0.003f;
	}
	else if (changetype == 2) {
		scale = 0.3f;
		scale2 = 0.3f;
	}
}
void clear_subwindow_1() {
	obj_vertices.clear();
	vertexIndices.clear();
	aColor.clear();
	aColor_v2.clear();
	bColor.clear();

	delete[] vertexInfo;

	first_f_check = 0;
	vertex_count = 0;
	face_count = 0;
	subwindow_num = 1;
};
void clear_subwindow_2() {
	obj_vertices2.clear();
	vertexIndices2.clear();
	aColor2.clear();
	aColor2_v2.clear();
	bColor2.clear();

	delete[] vertexInfo2;

	first_f_check = 0;
	vertex_count2 = 0;
	face_count2 = 0;
	subwindow_num = 2;
};
void add_near_vertex_face(int vertex1, int vertex2, int vertex3, int face_num) {
	if (subwindow_num == 1) {

		vertexInfo[vertex1].near_vertex.push_back(vertex2);

		vertexInfo[vertex1].near_vertex.push_back(vertex3);

		vertexInfo[vertex2].near_vertex.push_back(vertex1);

		vertexInfo[vertex2].near_vertex.push_back(vertex3);

		vertexInfo[vertex3].near_vertex.push_back(vertex1);

		vertexInfo[vertex3].near_vertex.push_back(vertex2);

		//------------------near face에 넣기--------------------
		vertexInfo[vertex1].near_face.push_back(face_num);

		vertexInfo[vertex2].near_face.push_back(face_num);

		vertexInfo[vertex3].near_face.push_back(face_num);

	}
	else if (subwindow_num == 2) {												//subwindow 2

		vertexInfo2[vertex1].near_vertex.push_back(vertex2);

		vertexInfo2[vertex1].near_vertex.push_back(vertex3);

		vertexInfo2[vertex2].near_vertex.push_back(vertex1);

		vertexInfo2[vertex2].near_vertex.push_back(vertex3);

		vertexInfo2[vertex3].near_vertex.push_back(vertex1);

		vertexInfo2[vertex3].near_vertex.push_back(vertex2);


		//------------------near face에 넣기--------------------
		vertexInfo2[vertex1].near_face.push_back(face_num);

		vertexInfo2[vertex2].near_face.push_back(face_num);

		vertexInfo2[vertex3].near_face.push_back(face_num);
	}
};
void calc_normal(int num, int vertex_n1, int vertex_n2, int vertex_n3) {
	if (subwindow_num == 1) {
		glm::vec3 vec2_1, vec3_1;

		vec2_1 = obj_vertices[vertex_n2] - obj_vertices[vertex_n1];
		vec3_1 = obj_vertices[vertex_n3] - obj_vertices[vertex_n1];

		double vx = vec2_1.y * vec3_1.z - vec2_1.z * vec3_1.y;
		double vy = vec2_1.z * vec3_1.x - vec2_1.x * vec3_1.z;
		double vz = vec2_1.x * vec3_1.y - vec2_1.y * vec3_1.x;
		glm::vec3 normal = glm::normalize(glm::vec3(vx, vy, vz));

		//face각에 따라 sin값으로 weight를 줌
		double size_vec2_1 = sqrt(vec2_1.x * vec2_1.x + vec2_1.y * vec2_1.y + vec2_1.z * vec2_1.z);
		double size_vec3_1 = sqrt(vec3_1.x * vec3_1.x + vec3_1.y * vec3_1.y + vec3_1.z * vec3_1.z);
		double cos = glm::dot(vec2_1, vec3_1) / (size_vec2_1 * size_vec3_1);
		double sin = sqrt(1 - cos * cos);

		if (sin < 0) {
			sin = -sin;
		}

		vertexInfo[num].normal_vec += glm::vec3(normal.x * sin, normal.y * sin, normal.z * sin);     //각에 따른 weight 적용

		//vertexInfo[num].normal_vec += normal;      //기존
		vertexInfo[num].normal_count++;
	}
	else if (subwindow_num == 2) {
		glm::vec3 vec2_1, vec3_1;

		vec2_1 = obj_vertices2[vertex_n2] - obj_vertices2[vertex_n1];
		vec3_1 = obj_vertices2[vertex_n3] - obj_vertices2[vertex_n1];

		double vx = vec2_1.y * vec3_1.z - vec2_1.z * vec3_1.y;
		double vy = vec2_1.z * vec3_1.x - vec2_1.x * vec3_1.z;
		double vz = vec2_1.x * vec3_1.y - vec2_1.y * vec3_1.x;
		glm::vec3 normal = glm::normalize(glm::vec3(vx, vy, vz));

		double size_vec2_1 = sqrt(vec2_1.x * vec2_1.x + vec2_1.y * vec2_1.y + vec2_1.z * vec2_1.z);
		double size_vec3_1 = sqrt(vec3_1.x * vec3_1.x + vec3_1.y * vec3_1.y + vec3_1.z * vec3_1.z);
		double cos = glm::dot(vec2_1, vec3_1) / (size_vec2_1 * size_vec3_1);
		double sin = sqrt(1 - cos * cos);

		if (sin < 0) {
			sin = -sin;
		}

		vertexInfo2[num].normal_vec += glm::vec3(normal.x * sin, normal.y * sin, normal.z * sin);   //각에 따른 weight 적용

		//vertexInfo2[num].normal_vec += normal;    //기존
		vertexInfo2[num].normal_count++;
	}
};
void calc_sin(int num, int vertex, int face_num) {
	if (num != vertex) {
		if (subwindow_num == 1) {
			double tbta;

			glm::vec3 normal = vertexInfo[num].normal_vec;
			glm::vec3 vertexV = obj_vertices[num];   //기준 vertex
			glm::vec3 vertexX = obj_vertices[vertex]; //인접한 vertex

			glm::vec3 vertexX_V = vertexX - vertexV;
			double XdotN = vertexX.x * normal.x + vertexX.y * normal.y + vertexX.z * normal.z;
			double VdotN = vertexV.x * normal.x + vertexV.y * normal.y + vertexV.z * normal.z;

			double XdotN_VdotN = abs(XdotN - VdotN);

			double ta = sqrt(vertexX_V.x * vertexX_V.x + vertexX_V.y * vertexX_V.y + vertexX_V.z * vertexX_V.z);
			double tb = XdotN_VdotN;

			tbta = tb / ta;

			//near vertex가 속한 face의 각에 따라 weight를 줌     --수정중
		  /*  int vertex_n1 = vertexIndices[(face_num - 1) * 3];
			int vertex_n2 = vertexIndices[(face_num - 1) * 3 + 1];
			int vertex_n3 = vertexIndices[(face_num - 1) * 3 + 2];

			if (num == vertex_n1) {
				first_vec = obj_vertices[vertex_n2] - obj_vertices[vertex_n1];
				second_vec = obj_vertices[vertex_n3] - obj_vertices[vertex_n1];
			}
			else if (num == vertex_n2) {
				first_vec = obj_vertices[vertex_n1] - obj_vertices[vertex_n2];
				second_vec = obj_vertices[vertex_n3] - obj_vertices[vertex_n2];
			}
			else if (num == vertex_n3) {
				first_vec = obj_vertices[vertex_n1] - obj_vertices[vertex_n3];
				second_vec = obj_vertices[vertex_n2] - obj_vertices[vertex_n3];
			}
			double size_first_vec = sqrt(first_vec.x * first_vec.x + first_vec.y * first_vec.y + first_vec.z * first_vec.z);
			double size_second_vec = sqrt(second_vec.x * second_vec.x + second_vec.y * second_vec.y + second_vec.z * second_vec.z);
			double cos = glm::dot(first_vec, second_vec) / (size_first_vec * size_second_vec);
			double sin = sqrt(1 - cos * cos);*/

			if (ta != 0) {
				if (second_nav_check == 0) {
					vertexInfo[num].calc_curv += tbta;// *(sin * 1.5);  //점이 포함된 face의 각에 비례
					vertexInfo[num].calc_count++;
				}
				else {
					vertexInfo[num].calc_curv2 += tbta;
					vertexInfo[num].calc_count2++;
				}
			}

		}
		else if (subwindow_num == 2) {
			double tbta;

			glm::vec3 normal = vertexInfo2[num].normal_vec;
			glm::vec3 vertexV = obj_vertices2[num];   //기준 vertex
			glm::vec3 vertexX = obj_vertices2[vertex]; //인접한 vertex

			glm::vec3 vertexX_V = vertexX - vertexV;
			double XdotN = vertexX.x * normal.x + vertexX.y * normal.y + vertexX.z * normal.z;
			double VdotN = vertexV.x * normal.x + vertexV.y * normal.y + vertexV.z * normal.z;

			double XdotN_VdotN = abs(XdotN - VdotN);

			double ta = sqrt(vertexX_V.x * vertexX_V.x + vertexX_V.y * vertexX_V.y + vertexX_V.z * vertexX_V.z);
			double tb = XdotN_VdotN;

			tbta = tb / ta;

			if (ta != 0) {
				if (second_nav_check == 0) {
					vertexInfo2[num].calc_curv += tbta;// *(sin * 1.5);  //점이 포함된 face의 각에 비례
					vertexInfo2[num].calc_count++;
				}
				else {
					vertexInfo2[num].calc_curv2 += tbta;
					vertexInfo2[num].calc_count2++;
				}
			}
		}
	}
};
void calc_color() {
	if (subwindow_num == 1) {
		for (int i = 0; i < vertex_count; i++) {
			int near_vertex_count = vertexInfo[i].near_vertex.size();
			vertexInfo[i].near_vertex_count = near_vertex_count;

			int near_face_num;
			int near_face_count = vertexInfo[i].near_face.size();
			for (int j = 0; j < near_face_count; j++) {
				near_face_num = vertexInfo[i].near_face[j];
				int vertex_n1 = vertexIndices[(near_face_num - 1) * 3];
				int vertex_n2 = vertexIndices[(near_face_num - 1) * 3 + 1];
				int vertex_n3 = vertexIndices[(near_face_num - 1) * 3 + 2];

				calc_normal(i, vertex_n1, vertex_n2, vertex_n3);
			}

			float normal_count = vertexInfo[i].normal_count;
			vertexInfo[i].normal_vec = glm::normalize(glm::vec3(vertexInfo[i].normal_vec.x / normal_count,
				vertexInfo[i].normal_vec.y / normal_count, vertexInfo[i].normal_vec.z / normal_count));

			//-------------- calc sin --------------
			for (int j = 0; j < near_vertex_count; j++) {
				int k = vertexInfo[i].near_vertex[j];
				int face_num = vertexInfo[i].near_face[j / 2];
				second_nav_check = 0;
				calc_sin(i, k, face_num);

				//near vertex의 near vertex와의 sin값 계산
				for (int l = 0; l < vertexInfo[k].near_vertex.size(); l++) {
					int m = vertexInfo[k].near_vertex[l];
					int face_num2 = vertexInfo[k].near_face[l / 2];
					second_nav_check = 1;
					calc_sin(i, m, face_num2);
				}
			}/*
			vertexInfo[i].calc_curv2 -= vertexInfo[i].calc_curv;
			vertexInfo[i].calc_count2 -= vertexInfo[i].calc_count;
			*/
			if (vertexInfo[i].calc_count2 != 0) {
				vertexInfo[i].calc_curv2 = vertexInfo[i].calc_curv2 / (2 * vertexInfo[i].calc_count2);
			}

			vertexInfo[i].calc_curv = (vertexInfo[i].calc_curv / vertexInfo[i].calc_count);
			vertexInfo[i].calc_curv *= 3.;
			if (vertexInfo[i].calc_curv >= threshold) {
				aColor.push_back(glm::vec3(vertexInfo[i].calc_curv, 0, 0));
			}
			else {
				aColor.push_back(glm::vec3(1. - vertexInfo[i].calc_curv, 1. - vertexInfo[i].calc_curv, 1. - vertexInfo[i].calc_curv));
			}

			vertexInfo[i].calc_curv += vertexInfo[i].calc_curv2;
			if (vertexInfo[i].calc_curv >= threshold) {
				aColor_v2.push_back(glm::vec3(vertexInfo[i].calc_curv, 0, 0));

			}
			else {
				aColor_v2.push_back(glm::vec3(1. - vertexInfo[i].calc_curv, 1. - vertexInfo[i].calc_curv, 1. - vertexInfo[i].calc_curv));

			}
			if (vertexInfo[i].calc_curv >= 0.9) {
				curv_distrib[9]++;
			}
			else if (vertexInfo[i].calc_curv >= 0.8) {
				curv_distrib[8]++;
			}
			else if (vertexInfo[i].calc_curv >= 0.7) {
				curv_distrib[7]++;
			}
			else if (vertexInfo[i].calc_curv >= 0.6) {
				curv_distrib[6]++;
			}
			else if (vertexInfo[i].calc_curv >= 0.5) {
				curv_distrib[5]++;
			}
			else if (vertexInfo[i].calc_curv >= 0.4) {
				curv_distrib[4]++;
			}
			else if (vertexInfo[i].calc_curv >= 0.3) {
				curv_distrib[3]++;
			}
			else if (vertexInfo[i].calc_curv >= 0.2) {
				curv_distrib[2]++;
			}
			else if (vertexInfo[i].calc_curv >= 0.1) {
				curv_distrib[1]++;
			}
			else {
				curv_distrib[0]++;
			}
		}
	}
	else if (subwindow_num == 2) {

		for (int i = 0; i < vertex_count2; i++) {
			int near_vertex_count = vertexInfo2[i].near_vertex.size();
			vertexInfo2[i].near_vertex_count = near_vertex_count;

			int near_face_num;
			int near_face_count = vertexInfo2[i].near_face.size();
			for (int j = 0; j < near_face_count; j++) {
				near_face_num = vertexInfo2[i].near_face[j];
				int vertex_n1 = vertexIndices2[(near_face_num - 1) * 3];
				int vertex_n2 = vertexIndices2[(near_face_num - 1) * 3 + 1];
				int vertex_n3 = vertexIndices2[(near_face_num - 1) * 3 + 2];

				calc_normal(i, vertex_n1, vertex_n2, vertex_n3);
			}

			float normal_count = vertexInfo2[i].normal_count;
			vertexInfo2[i].normal_vec = glm::normalize(glm::vec3(vertexInfo2[i].normal_vec.x / normal_count,
				vertexInfo2[i].normal_vec.y / normal_count, vertexInfo2[i].normal_vec.z / normal_count));

			//-------------- calc sin --------------
			for (int j = 0; j < near_vertex_count; j++) {
				int k = vertexInfo2[i].near_vertex[j];
				int face_num = vertexInfo2[i].near_face[j / 2];
				second_nav_check = 0;
				calc_sin(i, k, face_num);

				//near vertex의 near vertex와의 sin값 계산
				for (int l = 0; l < vertexInfo2[k].near_vertex.size(); l++) {
					int m = vertexInfo2[k].near_vertex[l];
					int face_num2 = vertexInfo2[k].near_face[l / 2];
					second_nav_check = 1;
					calc_sin(i, m, face_num2);
				}
			}
			/*vertexInfo2[i].calc_curv2 -= vertexInfo2[i].calc_curv;
			vertexInfo2[i].calc_count2 -= vertexInfo2[i].calc_count;*/
			if (vertexInfo2[i].calc_count2 != 0) {
				vertexInfo2[i].calc_curv2 = vertexInfo2[i].calc_curv2 / (2 * vertexInfo2[i].calc_count2);
			}

			vertexInfo2[i].calc_curv = (vertexInfo2[i].calc_curv / vertexInfo2[i].calc_count);
			vertexInfo2[i].calc_curv *= 3.;

			if (vertexInfo2[i].calc_curv >= threshold) {
				aColor2.push_back(glm::vec3(vertexInfo2[i].calc_curv, 0, 0));
			}
			else {
				aColor2.push_back(glm::vec3(1 - vertexInfo2[i].calc_curv, 1 - vertexInfo2[i].calc_curv, 1 - vertexInfo2[i].calc_curv));
			}

			vertexInfo2[i].calc_curv += vertexInfo2[i].calc_curv2;
			vertexInfo2[i].calc_curv = abs(vertexInfo2[i].calc_curv);
			if (vertexInfo2[i].calc_curv >= threshold) {
				aColor2_v2.push_back(glm::vec3(vertexInfo2[i].calc_curv, 0, 0));

			}
			else {
				aColor2_v2.push_back(glm::vec3(1 - vertexInfo2[i].calc_curv, 1 - vertexInfo2[i].calc_curv, 1 - vertexInfo2[i].calc_curv));

			}

			if (vertexInfo2[i].calc_curv >= 0.9) {
				curv_distrib2[9]++;
			}
			else if (vertexInfo2[i].calc_curv >= 0.8) {
				curv_distrib2[8]++;
			}
			else if (vertexInfo2[i].calc_curv >= 0.7) {
				curv_distrib2[7]++;
			}
			else if (vertexInfo2[i].calc_curv >= 0.6) {
				curv_distrib2[6]++;
			}
			else if (vertexInfo2[i].calc_curv >= 0.5) {
				curv_distrib2[5]++;
			}
			else if (vertexInfo2[i].calc_curv >= 0.4) {
				curv_distrib2[4]++;
			}
			else if (vertexInfo2[i].calc_curv >= 0.3) {
				curv_distrib2[3]++;
			}
			else if (vertexInfo2[i].calc_curv >= 0.2) {
				curv_distrib2[2]++;
			}
			else if (vertexInfo2[i].calc_curv >= 0.1) {
				curv_distrib2[1]++;
			}
			else {
				curv_distrib2[0]++;
			}
		}
	}
};
void calc_box_color() {

	bColor.clear();
	int How = HowDrawBox(StartmouseX, StartmouseY, EndmouseX, EndmouseY);
	float a = abs((EndmouseX - StartmouseX) / 2);
	float b = abs((EndmouseY - StartmouseY) / 2);
	float movex = (EndmouseX + StartmouseX) / 2;
	float movey = (EndmouseY + StartmouseY) / 2;
	float apow = pow(a, 2.0);
	float bpow = pow(b, 2.0);

	for (int i = 0; i < vertex_count; i++) {
		vPos = glm::vec4(obj_vertices[i], 1.);
		float realY = glm::vec4(obj_vertices[i], 1.).y;
		vPos = realMat * vPos;
		float vPosxpow = pow((vPos.x - movex), 2.0);
		float vPosypow = pow((vPos.y - movey), 2.0);
		float lethow = (vPosxpow / apow) + (vPosypow / bpow);
		bool letZ;
		if (realY < (-limitZ)) {
			letZ = true;
		}
		else {
			letZ = false;
		}

		if (true) {
			if (lethow <= 1 && letZ) {
				bColor.push_back(glm::vec3(0.74 * aColor[i].r, 1.0 * aColor[i].r, 0.));
				change_curv_distrib[returnCurvscope(vertexInfo[i].calc_curv)]++;
			}
			else {
				bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
			}
		}

		//switch (How)
		//{
		//case 1: //우측 상단으로 선택

		//	if ((StartmouseX <= vPos.x) && (vPos.x <= EndmouseX)) {
		//		if ((StartmouseY <= vPos.y) && (vPos.y <= EndmouseY)) {
		//			bColor.push_back(glm::vec3(0.74 * aColor[i].r, 1.0 * aColor[i].r, 0.));
		//			change_curv_distrib[returnCurvscope(vertexInfo[i].calc_curv)]++;
		//		}
		//		else {
		//			bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
		//		}
		//	}
		//	else {
		//		bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
		//	}
		//	break;
		//case 2: //우측 하단으로 선택
		//	if ((StartmouseX <= vPos.x) && (vPos.x <= EndmouseX)) {
		//		if ((EndmouseY <= vPos.y) && (vPos.y <= StartmouseY)) {
		//			bColor.push_back(glm::vec3(0.74 * aColor[i].r, 1.0 * aColor[i].r, 0.));
		//			change_curv_distrib[returnCurvscope(vertexInfo[i].calc_curv)]++;
		//		}
		//		else {
		//			bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
		//		}
		//	}
		//	else {
		//		bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
		//	}
		//	break;
		//case 3: //좌측 상단으로 선택
		//	if ((EndmouseX <= vPos.x) && (vPos.x <= StartmouseX)) {
		//		if ((StartmouseY <= vPos.y) && (vPos.y <= EndmouseY)) {
		//			bColor.push_back(glm::vec3(0.74 * aColor[i].r, 1.0 * aColor[i].r, 0.));
		//			change_curv_distrib[returnCurvscope(vertexInfo[i].calc_curv)]++;
		//		}
		//		else {
		//			bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
		//		}
		//	}
		//	else {
		//		bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
		//	}
		//	break;
		//case 4: //좌측 하단으로 선택
		//	if ((EndmouseX <= vPos.x) && (vPos.x <= StartmouseX)) {
		//		if ((EndmouseY <= vPos.y) && (vPos.y <= StartmouseY)) {
		//			bColor.push_back(glm::vec3(0.74 * aColor[i].r, 1.0 * aColor[i].r, 0.));
		//			change_curv_distrib[returnCurvscope(vertexInfo[i].calc_curv)]++;
		//		}
		//		else {
		//			bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
		//		}
		//	}
		//	else {
		//		bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
		//	}
		//	break;
		//}
	}
}
void calc_box_color2() {

	bColor2.clear();
	int How = HowDrawBox(StartmouseX2, StartmouseY2, EndmouseX2, EndmouseY2);
	float a = abs((EndmouseX2 - StartmouseX2) / 2);
	float b = abs((EndmouseY2 - StartmouseY2) / 2);
	float movex = (EndmouseX2 + StartmouseX2) / 2;
	float movey = (EndmouseY2 + StartmouseY2) / 2;
	float apow = pow(a, 2.0);
	float bpow = pow(b, 2.0);

	for (int i = 0; i < vertex_count2; i++) {
		vPos2 = glm::vec4(obj_vertices2[i], 1.);
		vPos2 = realMat2 * vPos2;
		float vPosxpow = pow((vPos2.x - movex), 2.0);
		float vPosypow = pow((vPos2.y - movey), 2.0);
		float lethow = (vPosxpow / apow) + (vPosypow / bpow);

		if (true) {
			if (lethow <= 1) {
				bColor2.push_back(glm::vec3(0.74 * aColor2[i].r, 1.0 * aColor2[i].r, 0.));
				change_curv_distrib[returnCurvscope(vertexInfo2[i].calc_curv)]++;
			}
			else {
				bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
			}
		}

		//switch (How)
		//{
		//case 1: //우측 상단으로 선택
		//	if ((StartmouseX2 <= vPos2.x) && (vPos2.x <= EndmouseX2)) {
		//		if ((StartmouseY2 <= vPos2.y) && (vPos2.y <= EndmouseY2)) {
		//			bColor2.push_back(glm::vec3(0.74 * aColor2[i].r, 1.0 * aColor2[i].r, 0.));
		//			change_curv_distrib[returnCurvscope(vertexInfo2[i].calc_curv)]++;
		//		}
		//		else {
		//			bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
		//		}
		//	}
		//	else {
		//		bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
		//	}
		//	break;
		//case 2: //우측 하단으로 선택
		//	if ((StartmouseX2 <= vPos2.x) && (vPos2.x <= EndmouseX2)) {
		//		if ((EndmouseY2 <= vPos2.y) && (vPos2.y <= StartmouseY2)) {
		//			bColor2.push_back(glm::vec3(0.74 * aColor2[i].r, 1.0 * aColor2[i].r, 0.));
		//			change_curv_distrib[returnCurvscope(vertexInfo2[i].calc_curv)]++;
		//		}
		//		else {
		//			bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
		//		}
		//	}
		//	else {
		//		bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
		//	}
		//	break;
		//case 3: //좌측 상단으로 선택
		//	if ((EndmouseX2 <= vPos2.x) && (vPos2.x <= StartmouseX2)) {
		//		if ((StartmouseY2 <= vPos2.y) && (vPos2.y <= EndmouseY2)) {
		//			bColor2.push_back(glm::vec3(0.74 * aColor2[i].r, 1.0 * aColor2[i].r, 0.));
		//			change_curv_distrib[returnCurvscope(vertexInfo2[i].calc_curv)]++;
		//		}
		//		else {
		//			bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
		//		}
		//	}
		//	else {
		//		bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
		//	}
		//	break;
		//case 4: //좌측 하단으로 선택
		//	if ((EndmouseX2 <= vPos2.x) && (vPos2.x <= StartmouseX2)) {
		//		if ((EndmouseY2 <= vPos2.y) && (vPos2.y <= StartmouseY2)) {
		//			bColor2.push_back(glm::vec3(0.74 * aColor2[i].r, 1.0 * aColor2[i].r, 0.));
		//			change_curv_distrib[returnCurvscope(vertexInfo2[i].calc_curv)]++;
		//		}
		//		else {
		//			bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
		//		}
		//	}
		//	else {
		//		bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
		//	}
		//	break;
		//}
	}
}

int HowDrawBox(float start_X, float start_Y, float End_X, float End_Y) {
	if (start_X < End_X) {
		if (start_Y < End_Y) {
			return 1;
		}
		else {
			return 2;
		}
	}
	else {
		if (start_Y < End_Y) {
			return 3;
		}
		else {
			return 4;
		}
	}
}
void onReadMTLFile(string path) {
	ifstream file(path);
	if (file.fail()) {
		material.MTL_Ka = glm::vec3(0., 0., 0.);
		material.MTL_Kd = glm::vec3(0.6, 0.6, 0.6);
		material.MTL_Ks = glm::vec3(0.5, 0.5, 0.5);
		material.Ni = 1.;
		material.illum = 2;
	}
	else {
		string line, currentMaterialName = "";
		while (file.peek() != EOF) {
			getline(file, line);

			int first_word_index = line.find(" ");
			string first_word = line.substr(0, first_word_index);

			if (first_word == "#") {
				continue;
			}
			else if (first_word == "newmtl") {
			}
			else if (first_word == "Kd") { //diffuse
				parseKd(line);
			}
			else if (first_word == "Ka") { //ambient
				parseKa(line);
			}
			else if (first_word == "Ks") { // specular
			}
			else if (first_word == "map_Kd") {
			}
		}
	}

}

void parseKs(string line) {
	string sub_line = line;
	string word;
	int blank_index;
	int count = 0;
	while (1) {
		blank_index = line.find(" ");
		if (blank_index != -1) {
			word = line.substr(0, blank_index);
			//cout << word << endl;
			line = line.substr(blank_index + 1);
			if (word == "Ks") {
				continue;
			}
			else if (count == 0) {
				material.MTL_Ks.x = stof(word);
				count++;
			}
			else if (count == 1) {
				material.MTL_Ks.y = stof(word);
				count++;
			}
		}
		else {
			blank_index = line.find("\n");
			word = line.substr(0, blank_index);
			//cout << word << endl;
			if (count == 2) {
				material.MTL_Ks.z = stof(word);
			}
			break;
		}
	}
}
void parseKa(string line) {
	string sub_line = line;
	string word;
	int blank_index;
	int count = 0;
	while (1) {
		blank_index = line.find(" ");
		if (blank_index != -1) {
			word = line.substr(0, blank_index);
			//cout << word << endl;
			line = line.substr(blank_index + 1);
			if (word == "Ka") {
				continue;
			}
			else if (count == 0) {
				material.MTL_Ka.x = stof(word);
				count++;
			}
			else if (count == 1) {
				material.MTL_Ka.y = stof(word);
				count++;
			}
		}
		else {
			blank_index = line.find("\n");
			word = line.substr(0, blank_index);
			//cout << word << endl;
			if (count == 2) {
				material.MTL_Ka.z = stof(word);
			}
			break;
		}
	}
}
void parseKd(string line) {
	string sub_line = line;
	string word;
	int blank_index;
	int count = 0;
	while (1) {
		blank_index = line.find(" ");
		if (blank_index != -1) {
			word = line.substr(0, blank_index);
			//cout << word << endl;
			line = line.substr(blank_index + 1);
			if (word == "Kd") {
				continue;
			}
			else if (count == 0) {
				material.MTL_Kd.x = stof(word);
				count++;
			}
			else if (count == 1) {
				material.MTL_Kd.y = stof(word);
				count++;
			}
		}
		else {
			blank_index = line.find("\n");
			word = line.substr(0, blank_index);
			//cout << word << endl;
			if (count == 2) {
				material.MTL_Kd.z = stof(word);
			}
			break;
		}
	}
}
void parseMtllib(string line) {
	string word;
	int blank_index;

	blank_index = line.find(" ");
	word = line.substr(0, blank_index);
	line = line.substr(blank_index + 1); //xxx.mtl
	mtlpath = line;
	onReadMTLFile(mtlpath);
}
void parseVertex(string line, float scale) {
	if (subwindow_num == 1) {
		vertex_count++;
	}
	else if (subwindow_num == 2) {
		vertex_count2++;
	}

	short blank_index;
	short now_index = 2; //이 전에 어디서부터 찾았는지, v때기
	glm::vec3 vertex;
	int count = 0;

	while (1) {
		blank_index = line.find(" ", now_index);
		if (blank_index != -1) {

			if (count == 0) {
				vertex.x = stof(line.substr(now_index, blank_index)) * scale;
				now_index = blank_index + 1;
				count++;
			}
			else if (count == 1) {
				vertex.y = stof(line.substr(now_index, (blank_index - now_index))) * scale;
				count++;
				now_index = blank_index + 1;
			}
		}
		else {
			if (count == 2) {
				vertex.z = stof(line.substr(now_index)) * scale;
				if (subwindow_num == 1) {
					obj_vertices.push_back(vertex);
				}
				else {
					obj_vertices2.push_back(vertex);
				}
			}
			break;
		}
	}
}
void parseNormal(string line) {
	string sub_line = line;
	string word;
	int blank_index;
	glm::vec3 normal;
	int count = 0;
	while (1) {
		blank_index = line.find(" ");
		if (blank_index != -1) {
			word = line.substr(0, blank_index);
			//cout << word << endl;
			line = line.substr(blank_index + 1);
			if (word == "vn") {
				continue;
			}
			if (count == 0) {
				normal.x = stof(word);
				count++;
			}
			else if (count == 1) {
				normal.y = stof(word);
				count++;
			}
		}
		else {
			blank_index = line.find("\n");
			word = line.substr(0, blank_index);
			//cout << word << endl;
			if (count == 2) {
				normal.z = stof(word);
				if (subwindow_num == 1) {
					obj_normals.push_back(normal);
				}
				else {
					obj_normals2.push_back(normal);
				}
			}
			break;
		}
	}
}
void parseTexcoord(string line) {
	string sub_line = line;
	string word;
	int blank_index;
	glm::vec2 texcoord;
	int count = 0;
	while (1) {
		blank_index = line.find(" ");
		if (blank_index != -1) {
			word = line.substr(0, blank_index);
			//cout << word << endl;
			line = line.substr(blank_index + 1);
			if (word == "vt") {
				continue;
			}
			if (count == 0) {
				texcoord.x = stof(word);
				count++;
			}
		}
		else {
			blank_index = line.find("\n");
			word = line.substr(0, blank_index);
			//cout << word << endl;
			if (count == 1) {
				texcoord.y = stof(word);
				if (subwindow_num == 1) {
					obj_texcoord.push_back(texcoord);
				}
				else {
					obj_texcoord2.push_back(texcoord);
				}
			}
			break;
		}
	}
}
void parseFace(string line, string currentMaterialName) {

	int blank_index;
	vector<int> now_blank_index(4);
	int start_index[4] = { 2, 0 }; // 처음 start = 2(f 제외하기 위함)
	int start_divide_index[4] = { 2, }; //divide용 start
	int pusha, pushb, pushc, pushd;
	int word_count = 0;
	int slash_index;

	//-------------------------------5.514----------------------------------
	while (1) {                              //divide blank
		blank_index = line.find(" ", start_index[word_count]);
		if (blank_index != -1) {
			word_count++;
			start_index[word_count] = blank_index + 1;
			start_divide_index[word_count] = blank_index + 1;
		}
		else
			break;

	}
	//----------------------------------------------------------------------------


	if (word_count == 2) {         //3 face

	   //sub_line[3].length() ==0 -> word_count ==2로 변경
		slash_index = line.find("/");   //check "/"
		if (slash_index == -1) {            //not exist "/"
		   //cout << "no slash" << endl;
			pusha = stoi(line.substr(start_index[0], (start_index[1] - 1 - start_index[0]))) - 1;
			pushb = stoi(line.substr(start_index[1], (start_index[2] - 1 - start_index[1]))) - 1;
			pushc = stoi(line.substr(start_index[2])) - 1;

			if (subwindow_num == 1) {
				face_count += 1;
				vertexIndices.push_back(pusha);
				vertexIndices.push_back(pushb);
				vertexIndices.push_back(pushc);
				add_near_vertex_face(pusha, pushb, pushc, face_count);

			}
			else {
				face_count2 += 1;
				vertexIndices2.push_back(pusha);
				vertexIndices2.push_back(pushb);
				vertexIndices2.push_back(pushc);
				add_near_vertex_face(pusha, pushb, pushc, face_count2);
			}
		}
		else {
			int end_check = 0;
			int j = 0;
			//-----------------------------------------16.175---------------------------------------------
			for (int i = 0; i < 3; i++) {
				//slash_index = (line.substr(start_divide_index[i])).find("/");
				slash_index = line.find("/", start_divide_index[i]);
				if (slash_index != -1) {
					now_blank_index[i] = slash_index;
				}
				else {
					now_blank_index[i] = 0;
				}
			}
			//--------------------------------------------------------------------------------------------
			pusha = stoi(line.substr(start_index[0], now_blank_index[0])) - 1;
			pushb = stoi(line.substr(start_index[1], now_blank_index[1])) - 1;
			pushc = stoi(line.substr(start_index[2], now_blank_index[2])) - 1;

			if (now_blank_index[0] != 0) {
				if (subwindow_num == 1) {
					face_count += 1;
					vertexIndices.push_back(pusha);
					vertexIndices.push_back(pushb);
					vertexIndices.push_back(pushc);

					add_near_vertex_face(pusha, pushb, pushc, face_count);

				}
				else {
					face_count2 += 1;
					vertexIndices2.push_back(pusha);
					vertexIndices2.push_back(pushb);
					vertexIndices2.push_back(pushc);
					add_near_vertex_face(pusha, pushb, pushc, face_count2);
				}
			}
		}
	}
	else {                              //4 face      (need to change 3 face)
	//   //cout << "4 face" << endl;
		slash_index = line.find("/");
		if (slash_index == -1) {            //not exist "/"
		   //cout << "no slash" << endl;
			pusha = stoi(line.substr(start_index[0], (start_index[1] - 1 - start_index[0]))) - 1;
			pushb = stoi(line.substr(start_index[1], (start_index[2] - 1 - start_index[1]))) - 1;
			pushc = stoi(line.substr(start_index[2], (start_index[3] - 1 - start_index[2]))) - 1;
			pushd = stoi(line.substr(start_index[3])) - 1;
			if (subwindow_num == 1) {
				face_count += 1;
				vertexIndices.push_back(pusha);
				vertexIndices.push_back(pushb);
				vertexIndices.push_back(pushc);
				add_near_vertex_face(pusha, pushb, pushc, face_count);
			}

			else {
				face_count2 += 1;
				vertexIndices2.push_back(pusha);
				vertexIndices2.push_back(pushb);
				vertexIndices2.push_back(pushc);
				add_near_vertex_face(pusha, pushb, pushc, face_count2);
			}

			if (subwindow_num == 1) {
				face_count += 1;
				vertexIndices.push_back(pusha);
				vertexIndices.push_back(pushc);
				vertexIndices.push_back(pushd);
				add_near_vertex_face(pusha, pushc, pushd, face_count);
			}
			else {
				face_count2 += 1;
				vertexIndices2.push_back(pusha);
				vertexIndices2.push_back(pushc);
				vertexIndices2.push_back(pushd);
				add_near_vertex_face(pusha, pushc, pushd, face_count2);
			}
		}
		else {
			//cout << "slash" << endl;
			int j = 0;
			for (int i = 0; i < 4; i++) {
				slash_index = line.find("/", start_divide_index[i]);
				if (slash_index != -1) {
					now_blank_index[i] = slash_index;

				}
				else {
					now_blank_index[i] = 0;
				}
			}
			pusha = stoi(line.substr(start_index[0], now_blank_index[0])) - 1;
			pushb = stoi(line.substr(start_index[1], now_blank_index[1])) - 1;
			pushc = stoi(line.substr(start_index[2], now_blank_index[2])) - 1;
			pushd = stoi(line.substr(start_index[3], now_blank_index[3])) - 1;

			if (subwindow_num == 1) {
				face_count += 1;
				vertexIndices.push_back(pusha);
				vertexIndices.push_back(pushb);
				vertexIndices.push_back(pushc);
				add_near_vertex_face(pusha, pushb, pushc, face_count);
			}
			else {
				face_count2 += 1;
				vertexIndices2.push_back(pusha);
				vertexIndices2.push_back(pushb);
				vertexIndices2.push_back(pushc);
				add_near_vertex_face(pusha, pushb, pushc, face_count2);
			}

			if (subwindow_num == 1) {
				face_count += 1;
				vertexIndices.push_back(pusha);
				vertexIndices.push_back(pushc);
				vertexIndices.push_back(pushd);
				add_near_vertex_face(pusha, pushc, pushd, face_count);
			}
			else {
				face_count2 += 1;
				vertexIndices2.push_back(pusha);
				vertexIndices2.push_back(pushc);
				vertexIndices2.push_back(pushd);
				add_near_vertex_face(pusha, pushc, pushd, face_count2);
			}
		}
	}
	now_blank_index.clear();
}
void loadObj(string path, float scale) {
	ifstream file(path);
	string line, currentMaterialName = "";
	while (file.peek() != EOF) {
		getline(file, line);

		int first_word_index = line.find(" ");
		string first_word = line.substr(0, first_word_index);

		if (first_word == "#") {
			continue;
		}
		else if (first_word == "mtllib") {
			parseMtllib(line);
		}
		else if (first_word == "o") {

		}
		else if (first_word == "g") {

		}
		else if (first_word == "v") {
			parseVertex(line, scale);
		}
		else if (first_word == "vn") {
			parseNormal(line);
		}
		else if (first_word == "vt") {
			parseTexcoord(line);
		}
		else if (first_word == "usemtl") {
			currentMaterialName = line.substr(first_word_index + 1);
		}
		else if (first_word == "f") {

			if (first_f_check == 0) {
				if (subwindow_num == 1) {
					vertexInfo = new vertex_info[vertex_count];
					first_f_check = 1;
				}
				else {
					vertexInfo2 = new vertex_info[vertex_count2];
					first_f_check = 1;
				}
			}
			parseFace(line, currentMaterialName);
		}
	}
}
void loadObj2(string path, float scale) {
	const char* path_char = path.c_str();
	FILE* fp = fopen(path_char, "r");
	char aline[100];
	char* pLine;
	int first_word_index;

	string line, currentMaterialName = "";
	char FirstChar;


	while (1) {
		pLine = fgets(aline, 100, fp);
		if (feof(fp)) {
			break;
		}

		line = pLine;
		if (!line.find_first_not_of("\n"))
			line = line.erase(line.find_last_not_of("\n") + 1);

		first_word_index = line.find(" ");
		FirstChar = line.front();

		switch (FirstChar) {
		case 'v':
			if (first_word_index == 1) {
				st2 = clock();
				parseVertex(line, scale);
				ed2 = clock();
				tt2 += (double)(ed2 - st2);
				break;
			}
			else
				break;
		case 'f':
			if (first_f_check == 0) {
				if (subwindow_num == 1) {
					vertexInfo = new vertex_info[vertex_count];
					first_f_check = 1;
				}
				else {
					vertexInfo2 = new vertex_info[vertex_count2];
					first_f_check = 1;
				}
			}
			st1 = clock();
			parseFace(line, currentMaterialName);
			ed1 = clock();
			tt1 += (double)(ed1 - st1);
			continue;
		case '#':
			continue;
		case 'm': //mtllib
			parseMtllib(line);
		case 'o':
			continue;
		case 'g':
			continue;
		case 'u': //usemtl
			currentMaterialName = line.substr(first_word_index + 1);
			break;
		}
	}
	std::fclose(fp);

}

void updateViewmat() {

	tiltmat = glm::rotate(tiltRad * TO_RADIAN, glm::vec3(1.0f, 0, 0));
	rotmat = glm::rotate(panRad * TO_RADIAN, glm::vec3(0, 1.0f, 0));
	transmat = glm::translate(glm::vec3(transX, transY, transZ));
	viewmat = glm::inverse(transmat) * glm::transpose(tiltmat * rotmat);

	realMat = viewmat * wmat;
	realMat = projectmat * realMat;

	realMat2 = realMat;

	//0711 추가
	transmat4 = glm::translate(glm::vec3(transX, transY + transY_indexZ, transZ));
	viewmat4 = glm::inverse(transmat4) * glm::transpose(tiltmat4);

}
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	//create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;


	//Read the vertex shader code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);
	if (VertexShaderStream.is_open())
	{
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	//Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	//Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	vector<char> VertexShaderErrorMessage(max(InfoLogLength, int(1)));
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	std::fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	//Read the Tessellation Control Shader code

	//Read the Tessellation Evaluation Shader code

	//Read the Geometry shader code

	//Read the fragment shader code from the file
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if (FragmentShaderStream.is_open())
	{
		string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	//Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	//Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	vector<char> FragmentShaderErrorMessage(max(InfoLogLength, int(1)));
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, FragmentShaderErrorMessage.data());
	std::fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	//Link the program
	std::fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();

	glAttachShader(ProgramID, VertexShaderID);
	//glAttachShader(ProgramID, TessControlShaderID);
	//glAttachShader(ProgramID, TessEvaluationShaderID);
	glAttachShader(ProgramID, FragmentShaderID);

	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, ProgramErrorMessage.data());
	std::fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	//glDeleteShader(TessControlShaderID);
	//glDeleteShader(TessEvaluationShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void my_display_code()
{
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
}
void renderSceneNew()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGLUT_NewFrame();

	my_display_code();

	if (curvature_1 == 1) {
		color_step = 1;
		color_step2 = 1;
		curvature_1 = 0;
	}
	if (curvature_2 == 1) {
		color_step = 2;
		color_step2 = 2;
		curvature_2 = 0;
	}

	// Rendering
	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();
	glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
	glClearColor(1., 1., 1., 1.);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (changeclick == 1) {
		renderSceneAll();
	}

	if (changelimit2) {
		limitZ2 = limittestZ2;
		changelimit2 = false;
		renderSceneAll();

	}

	if (changelimit) {
		limitZ = limittestZ;
		changelimit = false;
		renderSceneAll();

	}

	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	glutSwapBuffers();
	glutPostRedisplay();
}
void show_graph(double percent[], double percent2[], int len, CpGnuplotU* plot)
{

	double min1 = *min_element(percent, percent + len);
	double min2 = *min_element(percent2, percent2 + len);
	double min = min1 <= min2 ? min1 : min2;

	double max1 = *max_element(percent, percent + len);
	double max2 = *max_element(percent2, percent2 + len);
	double max = max1 >= max2 ? max1 : max2;

	double boxwidth = 0.3;
	double height_space = boxwidth / 10;

	// CwpGnuplot�� ���ڿ��� ���ڷ� wgnuplot.exe�� ��ü ��θ� �Ѱ��ش�.
	// Gnuplot� ��ġ�� ��ο� ��� �� ��� �ٲ�� �Ѵ�.


	FILE* fp = _wfopen(_T("curv_percent.dat"), _T("wt"));
	if (fp) {
		for (int x = 0; x < len; x++) {
			fwprintf(fp, _T("%f, %f \n"), (double)x, percent[x]);
		}
		fclose(fp);
	}

	FILE* fp2 = _wfopen(_T("curv_percent2.dat"), _T("wt"));
	if (fp2) {
		for (int x = 0; x < len; x++) {
			fwprintf(fp2, _T("%f, %f \n"), (double)x + boxwidth, percent2[x]);
		}
		fclose(fp2);
	}

	(*plot).cmd(_T("set title 'Before And After'"));
	(*plot).cmd(_T("set boxwidth 0.3"));   //boxwidth
	(*plot).cmd(_T("set style fill solid 1.00 border lt - 1"));


	(*plot).cmd(_T("plot 'curv_percent.dat' with boxes, 'curv_percent2.dat' with boxes"));
	//plot.cmd (_T("plot [x=-3:3] sin(x), cos(x)"));



}
void boxto_change_graph() {
	int numOfElements = sizeof(change_curv_distrib) / sizeof(int);
	int sumofvertex = 0;
	for (int i = 0; i < numOfElements; i++) {
		sumofvertex += change_curv_distrib[i];
	}

	for (int i = 0; i < numOfElements; i++) {
		curv_percent[i] = change_curv_distrib[i] / (float)sumofvertex;
	}

	//ㅡㅡㅡㅡㅡ왼쪽 화면 계산 cout
	cout << "------------------------------------------왼쪽-----------------------------------------------------------" << endl;
	cout << "\t0.0~0.1\t\t0.1~0.2\t\t0.2~0.3\t\t0.3~0.4\t\t0.4~0.5\t\t0.5~0.6\t\t0.6~0.7\t\t0.7~0.8\t\t0.8~0.9\t\t0.9~1.0\t\ttotal" << endl;
	cout << "count\t" << change_curv_distrib[0]
		<< "\t\t" << change_curv_distrib[1]
		<< "\t\t" << change_curv_distrib[2]
		<< "\t\t" << change_curv_distrib[3]
		<< "\t\t" << change_curv_distrib[4]
		<< "\t\t" << change_curv_distrib[5]
		<< "\t\t" << change_curv_distrib[6]
		<< "\t\t" << change_curv_distrib[7]
		<< "\t\t" << change_curv_distrib[8]
		<< "\t\t" << change_curv_distrib[9]

		<< "\t\t" << sumofvertex << endl;
	cout << "percent\t" << curv_percent[0]
		<< "\t" << curv_percent[1]
		<< "\t" << curv_percent[2]
		<< "\t" << curv_percent[3]
		<< "\t" << curv_percent[4]
		<< "\t" << curv_percent[5]
		<< "\t" << curv_percent[6]
		<< "\t" << curv_percent[7]
		<< "\t" << curv_percent[8]
		<< "\t" << curv_percent[9]

		<< endl << endl;

	for (int i = 0; i < numOfElements; i++) {
		change_curv_distrib[i] = 0;
	}
}
void boxto_change_graph2() {
	int numOfElements = sizeof(change_curv_distrib) / sizeof(int);
	int sumofvertex = 0;
	for (int i = 0; i < numOfElements; i++) {
		sumofvertex += change_curv_distrib[i];
	}

	for (int i = 0; i < numOfElements; i++) {
		curv_percent2[i] = change_curv_distrib[i] / (float)sumofvertex;
	}

	//ㅡㅡㅡㅡㅡ오른쪽 화면 계산 cout
	cout << "------------------------------------------오른쪾-----------------------------------------------------------"<<endl;
	cout << "\t0.0~0.1\t\t0.1~0.2\t\t0.2~0.3\t\t0.3~0.4\t\t0.4~0.5\t\t0.5~0.6\t\t0.6~0.7\t\t0.7~0.8\t\t0.8~0.9\t\t0.9~1.0\t\t1.0\t\ttotal" << endl;
	cout << "count\t" << change_curv_distrib[0]
		<< "\t\t" << change_curv_distrib[1]
		<< "\t\t" << change_curv_distrib[2]
		<< "\t\t" << change_curv_distrib[3]
		<< "\t\t" << change_curv_distrib[4]
		<< "\t\t" << change_curv_distrib[5]
		<< "\t\t" << change_curv_distrib[6]
		<< "\t\t" << change_curv_distrib[7]
		<< "\t\t" << change_curv_distrib[8]
		<< "\t\t" << change_curv_distrib[9]

		<< "\t\t" << sumofvertex << endl;
	cout << "percent\t" << curv_percent2[0]
		<< "\t" << curv_percent2[1] 
		<< "\t" << curv_percent2[2] 
		<< "\t" << curv_percent2[3] 
		<< "\t" << curv_percent2[4] 
		<< "\t" << curv_percent2[5] 
		<< "\t" << curv_percent2[6] 
		<< "\t" << curv_percent2[7]
		<< "\t" << curv_percent2[8] 
		<< "\t" << curv_percent2[9] 

		<< endl << endl;

	for (int i = 0; i < numOfElements; i++) {
		change_curv_distrib[i] = 0;
	}
}
int returnCurvscope(double calc_curv) {
	if (calc_curv >= 0.9) {
		return 9;
	}
	else if (calc_curv >= 0.8) {
		return 8;
	}
	else if (calc_curv >= 0.7) {
		return 7;
	}
	else if (calc_curv >= 0.6) {
		return 6;
	}
	else if (calc_curv >= 0.5) {
		return 5;
	}

	else if (calc_curv >= 0.4) {
		return 4;
	}
	else if (calc_curv >= 0.3) {
		return 3;
	}
	else if (calc_curv >= 0.2) {
		return 2;
	}
	else if (calc_curv >= 0.1) {
		return 1;
	}
	else {
		return 0;
	}
}
void exit_graph(CpGnuplotU* plot) {
	(*plot).cmd(_T("exit"));

}
void renderScene(void)
{

	glutSetWindow(subWindow1);
	glUseProgram(programID);
	//Clear all pixels
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Let's draw something here

	//define the size of point and draw a point.


	if (changeclick == 1) {
		changeFile(clickname);
	}
	glUniform1f(glGetUniformLocation(programID, "limitZ"), limitZ);

	GLuint lightPosID = glGetUniformLocation(programID, "uLightPos");
	glUniform3fv(lightPosID, 1, &lightPos[0]);

	GLuint matLoc = glGetUniformLocation(programID, "worldMat");
	glUniformMatrix4fv(matLoc, 1, GL_FALSE, &wmat[0][0]);

	GLuint viewID = glGetUniformLocation(programID, "viewmat");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewmat[0][0]);

	GLuint projectID = glGetUniformLocation(programID, "projectmat");
	glUniformMatrix4fv(projectID, 1, GL_FALSE, &projectmat[0][0]);

	glUniform1i(glGetUniformLocation(programID, "lightTurnOnOff"), lightTurnOnOff);

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	if (color_step == 1) {  //근처 1단계 탐색한 색
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor.size(), aColor.data(), GL_STATIC_DRAW);
		GLint bColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(bColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(bColor);
	}
	else if (color_step == 2) { //근처 2단계 탐색한 색
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor_v2.size(), aColor_v2.data(), GL_STATIC_DRAW);
		GLint bColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(bColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(bColor);
	}

	if (!bColor.empty()) { // 박스를 지정했을 경우
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * bColor.size(), bColor.data(), GL_STATIC_DRAW);
		GLint bColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(bColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(bColor);
	}

	glDrawElements(GL_TRIANGLES, vertexIndices.size(), GL_UNSIGNED_INT, (void*)0);
	//glDrawArrays(GL_POINTS, 0, obj_vertices.size());

	glUseProgram(programID2);

	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_box);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, box_vertices, GL_STATIC_DRAW);
	GLuint vtxPosition2 = glGetAttribLocation(programID2, "vtxPosition");
	glVertexAttribPointer(vtxPosition2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));

	GLuint moveID = glGetUniformLocation(programID2, "movemat");
	glUniformMatrix4fv(moveID, 1, GL_FALSE, &movemat[0][0]);

	glDrawArrays(GL_LINE_LOOP, 0, 4);

	glUseProgram(programID);
	//Double buffer
	glutSwapBuffers();

}
void renderScene2(void) {
	glutSetWindow(subWindow2);
	//Clear all pixels
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Let's draw something here

	//define the size of point and draw a point.
	if (changeclick == 1) {
		changeFile2(clickname);
	}
	glUniform1f(glGetUniformLocation(programID, "limitZ"), limitZ2);


	GLuint lightPosID = glGetUniformLocation(programID, "uLightPos");
	glUniform3fv(lightPosID, 1, &lightPos[0]);

	GLuint matLoc = glGetUniformLocation(programID, "worldMat");
	glUniformMatrix4fv(matLoc, 1, GL_FALSE, &wmat[0][0]);

	GLuint viewID = glGetUniformLocation(programID, "viewmat");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewmat[0][0]);

	GLuint projectID = glGetUniformLocation(programID, "projectmat");
	glUniformMatrix4fv(projectID, 1, GL_FALSE, &projectmat[0][0]);


	glUniform1i(glGetUniformLocation(programID, "lightTurnOnOff"), lightTurnOnOff);

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	if (color_step == 1) { //근처 1단계 탐색한 색

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor2.size(), aColor2.data(), GL_STATIC_DRAW);
		GLint bColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(bColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(bColor);
	}
	else if (color_step == 2) { //근처 2단계 탐색한 색
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor2_v2.size(), aColor2_v2.data(), GL_STATIC_DRAW);
		GLint bColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(bColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(bColor);
	}

	if (!bColor2.empty()) { // 박스를 지정했을 경우
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * bColor2.size(), bColor2.data(), GL_STATIC_DRAW);
		GLint bColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(bColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(bColor);
	}

	glDrawElements(GL_TRIANGLES, vertexIndices2.size(), GL_UNSIGNED_INT, (void*)0);
	//glDrawArrays(GL_POINTS, 0, obj_vertices.size());

	glUseProgram(programID2);

	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_box);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, box_vertices2, GL_STATIC_DRAW);
	GLuint vtxPosition2 = glGetAttribLocation(programID2, "vtxPosition");
	glVertexAttribPointer(vtxPosition2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));

	GLuint moveID = glGetUniformLocation(programID2, "movemat");
	glUniformMatrix4fv(moveID, 1, GL_FALSE, &movemat2[0][0]);

	glDrawArrays(GL_LINE_LOOP, 0, 4);

	glUseProgram(programID);
	//Double buffer
	glutSwapBuffers();
}
void renderScene3(void) {

	glutSetWindow(subWindow3);
	glUseProgram(programID);
	//Clear all pixels
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Let's draw something here

	//define the size of point and draw a point.


	if (changeclick == 1) {
		changeFile3(clickname);
	}

	glUseProgram(programID);

	glUniform1f(glGetUniformLocation(programID, "limitZ"), limitZ);

	GLuint lightPosID = glGetUniformLocation(programID, "uLightPos");
	glUniform3fv(lightPosID, 1, &lightPos[0]);

	GLuint matLoc = glGetUniformLocation(programID, "worldMat");
	glUniformMatrix4fv(matLoc, 1, GL_FALSE, &wmat[0][0]);

	GLuint viewID = glGetUniformLocation(programID, "viewmat");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewmat4[0][0]);

	GLuint projectID = glGetUniformLocation(programID, "projectmat");
	glUniformMatrix4fv(projectID, 1, GL_FALSE, &projectmat[0][0]);

	glUniform1i(glGetUniformLocation(programID, "lightTurnOnOff"), lightTurnOnOff);

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	if (color_step == 1) {  //근처 1단계 탐색한 색
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor.size(), aColor.data(), GL_STATIC_DRAW);
		GLint bColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(bColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(bColor);
	}
	else if (color_step == 2) { //근처 2단계 탐색한 색
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor_v2.size(), aColor_v2.data(), GL_STATIC_DRAW);
		GLint bColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(bColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(bColor);
	}

	if (!bColor.empty()) { // 박스를 지정했을 경우
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * bColor.size(), bColor.data(), GL_STATIC_DRAW);
		GLint bColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(bColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(bColor);
	}

	glDrawElements(GL_TRIANGLES, vertexIndices.size(), GL_UNSIGNED_INT, (void*)0);


	glUseProgram(programID);
	//Double buffer
	glutSwapBuffers();
}
void renderScene4(void) {
	glutSetWindow(subWindow4);
	//Clear all pixels
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//define the size of point and draw a point.

	if (changeclick == 1) {
		changeFile4(clickname);
		changeclick = 0;
	}

	glUseProgram(programID);

	glUniform1f(glGetUniformLocation(programID, "limitZ"), limitZ2);

	GLuint lightPosID = glGetUniformLocation(programID, "uLightPos");
	glUniform3fv(lightPosID, 1, &lightPos[0]);

	GLuint matLoc = glGetUniformLocation(programID, "worldMat");
	glUniformMatrix4fv(matLoc, 1, GL_FALSE, &wmat[0][0]);

	GLuint viewID = glGetUniformLocation(programID, "viewmat");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewmat4[0][0]);

	GLuint projectID = glGetUniformLocation(programID, "projectmat");
	glUniformMatrix4fv(projectID, 1, GL_FALSE, &projectmat[0][0]);

	glUniform1i(glGetUniformLocation(programID, "lightTurnOnOff"), lightTurnOnOff);

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	if (color_step == 1) { //근처 1단계 탐색한 색

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor2.size(), aColor2.data(), GL_STATIC_DRAW);
		GLint bColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(bColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(bColor);
	}
	else if (color_step == 2) { //근처 2단계 탐색한 색
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor2_v2.size(), aColor2_v2.data(), GL_STATIC_DRAW);
		GLint bColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(bColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(bColor);
	}

	if (!bColor2.empty()) { // 박스를 지정했을 경우
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * bColor2.size(), bColor2.data(), GL_STATIC_DRAW);
		GLint bColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(bColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(bColor);
	}

	glDrawElements(GL_TRIANGLES, vertexIndices2.size(), GL_UNSIGNED_INT, (void*)0);

	glUseProgram(programID);
	//Double buffer
	glutSwapBuffers();
}

void renderSceneAll(void) {
	renderScene();
	renderScene2();
	renderScene3();
	renderScene4();
};
void myKeyboard(unsigned char key, int x, int y) {

	GLint vtxPosition = glGetAttribLocation(programID, "vtxPosition");
	GLint Color = glGetAttribLocation(programID, "a_Color");
	switch (key) {
	case '.':
		BoxHolding = !BoxHolding;
		break;
	case '/':
		Holding = !Holding;
		break;
	case 'o':
		orthoOn = 1;
		maxX = 1.0f;
		minX = -1.0f;
		maxY = 1.0f;
		minY = -1.0f;
		glm::mat4 orthoMat = glm::ortho(minX, maxX, minY, maxY, zNear, zFar);

		projectmat = orthoMat;

		transX = 0.0f;
		transY = 0.0f;
		transZ = 2.0f;
		tiltRad = 0.0f;
		panRad = 0.0f;

		updateViewmat();

		break;
	case 'p':
		orthoOn = 0;
		persRad = 60;

		glm::mat4 perspectivemat = glm::perspective(persRad * TO_RADIAN, 1.0f, zNear, zFar);

		projectmat = perspectivemat;

		transX = 0.0f;
		transY = 0.0f;
		transZ = 2.0f;
		tiltRad = 0.0f;
		panRad = 0.0f;

		updateViewmat();

		break;
	case 'f':   //pan right
		panRad -= 0.7f;
		updateViewmat();
		break;
	case 'r':   //pan left
		panRad += 0.7f;
		updateViewmat();
		break;
	case 't':   //tilt up
		tiltRad += 0.7f;
		updateViewmat();
		break;
	case 'g':   //tilt down
		tiltRad -= 0.7f;
		updateViewmat();
		break;
	case 'a':   //crab right
		transX += 0.1f;
		updateViewmat();
		break;
	case 'q':   //crab left
		transX -= 0.1f;
		updateViewmat();
		break;
	case 'w':   //ped up
		transY += 0.1f;
		updateViewmat();
		break;
	case 's':   //ped down
		transY -= 0.1f;
		updateViewmat();
		break;
	case 'i':
		transY_indexZ += 0.1f;
		updateViewmat();
		break;
	case 'k':
		transY_indexZ -= 0.1f;
		updateViewmat();
		break;
	case 'y':   //zoom in
		if (orthoOn == 0) {
			persRad -= 1.0f;
			glm::mat4 perspectiveMat = glm::perspective(persRad * TO_RADIAN, 1.0f, zNear, zFar);
			projectmat = perspectiveMat;

			updateViewmat();
		}
		else if (orthoOn == 1) {
			maxX -= 0.1f;
			minX += 0.1f;
			maxY -= 0.1f;
			minY += 0.1f;
			orthoMat = glm::ortho(minX, maxX, minY, maxY, zNear, zFar);
			projectmat = orthoMat;
		}

		break;
	case 'h':   //zoom out
		if (orthoOn == 0) {
			persRad += 1.0f;
			glm::mat4 perspectiveMat = glm::perspective(persRad * TO_RADIAN, 1.0f, zNear, zFar);
			projectmat = perspectiveMat;
			updateViewmat();
		}
		else if (orthoOn == 1) {
			maxX += 0.1f;
			minX -= 0.1f;
			maxY += 0.1f;
			minY -= 0.1f;
			orthoMat = glm::ortho(minX, maxX, minY, maxY, zNear, zFar);

			projectmat = orthoMat;
		}
		break;
	case 'e':   //track in
		transZ -= 0.05f;
		updateViewmat();
		break;
	case 'd':   //track out
		transZ += 0.05f;
		updateViewmat();
		break;

	case '1':            //light move
		lightPos.x += 0.2f;
		break;
	case '2':
		lightPos.x -= 0.2f;
		break;
	case '3':
		lightPos.y += 0.2f;
		break;
	case '4':
		lightPos.y -= 0.2f;
		break;
	case '5':
		lightPos.z += 0.2f;
		break;
	case '6':
		lightPos.z -= 0.2f;
		break;

	case 'l':            //light on/off
		if (lightTurnOnOff == 1) {
			lightTurnOnOff = 0;
		}
		else {
			lightTurnOnOff = 1;
		}
		glUniform1i(glGetUniformLocation(programID, "lightTurnOnOff"), lightTurnOnOff);
		break;
	case 'z':
		color_step = 1;
		break;
	case 'x':
		color_step = 2;
		break;
		/*case 'c':  //수정예정
			threshold -= 0.1;
			aColor.clear();
			aColor2.clear();
			aColor_v2.clear();
			aColor2_v2.clear();
			calc_color();
			break;
		case 'v':
			threshold += 0.1;
			aColor.clear();
			aColor2.clear();
			aColor_v2.clear();
			aColor2_v2.clear();
			calc_color();
			break;*/
	}
	glutPostRedisplay();
}
void changeFile(char* file_str) {
	GLint vtxPosition = glGetAttribLocation(programID, "vtxPosition");
	GLint Color = glGetAttribLocation(programID, "a_Color");

	filename = file_str;

	clear_subwindow_1();
	loadObj2(filename, scale);
	calc_color();

	glBindVertexArray(vao[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertexIndices.size(), vertexIndices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * obj_vertices.size(), obj_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
	glEnableVertexAttribArray(vtxPosition);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor.size(), aColor.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(Color, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
	glEnableVertexAttribArray(Color);

}
void changeFile2(char* file_str) {
	GLint vtxPosition = glGetAttribLocation(programID, "vtxPosition");
	GLint Color = glGetAttribLocation(programID, "a_Color");

	filename = file_str;


	clear_subwindow_2();
	loadObj2(filename, scale);
	calc_color();

	glBindVertexArray(vao[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertexIndices2.size(), vertexIndices2.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * obj_vertices2.size(), obj_vertices2.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
	glEnableVertexAttribArray(vtxPosition);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor2.size(), aColor2.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(Color, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
	glEnableVertexAttribArray(Color);


}
void changeFile3(char* file_str) {
	GLint vtxPosition = glGetAttribLocation(programID, "vtxPosition");
	GLint Color = glGetAttribLocation(programID, "a_Color");

	filename = file_str;

	glBindVertexArray(vao[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertexIndices.size(), vertexIndices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * obj_vertices.size(), obj_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
	glEnableVertexAttribArray(vtxPosition);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor.size(), aColor.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(Color, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
	glEnableVertexAttribArray(Color);
}
void changeFile4(char* file_str) {
	GLint vtxPosition = glGetAttribLocation(programID, "vtxPosition");
	GLint Color = glGetAttribLocation(programID, "a_Color");

	filename = file_str;

	glBindVertexArray(vao[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertexIndices2.size(), vertexIndices2.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * obj_vertices2.size(), obj_vertices2.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
	glEnableVertexAttribArray(vtxPosition);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor2.size(), aColor2.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(Color, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
	glEnableVertexAttribArray(Color);

}
void myKeyboard2(unsigned char key, int x, int y) {
	GLint vtxPosition = glGetAttribLocation(programID, "vtxPosition");
	GLint Color = glGetAttribLocation(programID, "a_Color");
	switch (key) {
	case '/':
		Holding = !Holding;
		break;
	case 'o':
		orthoOn2 = 1;
		maxX2 = 1.0f;
		minX2 = -1.0f;
		maxY2 = 1.0f;
		minY2 = -1.0f;
		glm::mat4 orthoMat = glm::ortho(minX2, maxX2, minY2, maxY2, zNear2, zFar2);

		projectmat2 = orthoMat;

		transX2 = 0.0f;
		transY2 = 0.0f;
		transZ2 = 2.0f;
		tiltRad2 = 0.0f;
		panRad2 = 0.0f;

		updateViewmat();

		break;
	case 'p':
		orthoOn2 = 0;
		persRad2 = 60;

		glm::mat4 perspectivemat = glm::perspective(persRad2 * TO_RADIAN, 1.0f, zNear2, zFar2);

		projectmat2 = perspectivemat;

		transX2 = 0.0f;
		transY2 = 0.0f;
		transZ2 = 2.0f;
		tiltRad2 = 0.0f;
		panRad2 = 0.0f;

		updateViewmat();

		break;
	case 'f':   //pan right
		panRad2 -= 0.7f;
		updateViewmat();
		break;
	case 'r':   //pan left
		panRad2 += 0.7f;
		updateViewmat();
		break;
	case 't':   //tilt up
		tiltRad2 += 0.7f;
		updateViewmat();
		break;
	case 'g':   //tilt down
		tiltRad2 -= 0.7f;
		updateViewmat();
		break;
	case 'a':   //crab right
		transX2 += 0.1f;
		updateViewmat();
		break;
	case 'q':   //crab left
		transX2 -= 0.1f;
		updateViewmat();
		break;
	case 'w':   //ped up
		transY2 += 0.1f;
		updateViewmat();
		break;
	case 's':   //ped down
		transY2 -= 0.1f;
		updateViewmat();
		break;
	case 'y':   //zoom in
		if (orthoOn2 == 0) {
			persRad2 -= 1.0f;
			glm::mat4 perspectiveMat = glm::perspective(persRad2 * TO_RADIAN, 1.0f, zNear2, zFar2);
			projectmat2 = perspectiveMat;

			updateViewmat();
		}
		else if (orthoOn2 == 1) {
			maxX2 -= 0.1f;
			minX2 += 0.1f;
			maxY2 -= 0.1f;
			minY2 += 0.1f;
			orthoMat2 = glm::ortho(minX2, maxX2, minY2, maxY2, zNear2, zFar2);
			projectmat2 = orthoMat2;
		}

		break;
	case 'h':   //zoom out
		if (orthoOn2 == 0) {
			persRad2 += 1.0f;
			glm::mat4 perspectiveMat = glm::perspective(persRad2 * TO_RADIAN, 1.0f, zNear2, zFar2);
			projectmat2 = perspectiveMat;
			updateViewmat();
		}
		else if (orthoOn2 == 1) {
			maxX2 += 0.1f;
			minX2 -= 0.1f;
			maxY2 += 0.1f;
			minY2 -= 0.1f;
			orthoMat2 = glm::ortho(minX2, maxX2, minY2, maxY2, zNear2, zFar2);

			projectmat2 = orthoMat2;
		}
		break;
	case 'e':   //track in
		transZ2 -= 0.05f;
		updateViewmat();
		break;
	case 'd':   //track out
		transZ2 += 0.05f;
		updateViewmat();
		break;

	case '1':            //light move
		lightPos2.x += 0.2f;
		break;
	case '2':
		lightPos2.x -= 0.2f;
		break;
	case '3':
		lightPos2.y += 0.2f;
		break;
	case '4':
		lightPos2.y -= 0.2f;
		break;
	case '5':
		lightPos2.z += 0.2f;
		break;
	case '6':
		lightPos2.z -= 0.2f;
		break;

	case 'l':            //light on/off
		if (lightTurnOnOff2 == 1) {
			lightTurnOnOff2 = 0;
		}
		else {
			lightTurnOnOff2 = 1;
		}
		glUniform1i(glGetUniformLocation(programID, "lightTurnOnOff"), lightTurnOnOff2);
		break;
	case 'z':
		color_step2 = 1;
		break;
	case 'x':
		color_step2 = 2;
		break;
	case 'c':
		clear_subwindow_2();
		change_filename = "cube.obj";
		filename = change_filename;
		loadObj2(filename, scale);
		calc_color();

		glBindVertexArray(vao[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertexIndices2.size(), vertexIndices2.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * obj_vertices2.size(), obj_vertices2.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(vtxPosition);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor2.size(), aColor2.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(Color, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(Color);

		break;
	case 'v':
		clear_subwindow_2();
		change_filename = "PiggyBank.obj";
		filename = change_filename;
		loadObj2(filename, scale);
		calc_color();

		glBindVertexArray(vao[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertexIndices2.size(), vertexIndices2.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * obj_vertices2.size(), obj_vertices2.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(vtxPosition);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor2.size(), aColor2.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(Color, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(Color);

		break;
	}

	glutPostRedisplay();

}
void myMouseClick(GLint Button, GLint State, int x, int y) {
	if ((Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) && Holding == TRUE && BoxHolding == FALSE) { //왼쪽 마우스 버튼을 눌렀을 때, 기준점잡기.
		glUseProgram(programID2);
		glVertexAttrib3f(glGetAttribLocation(programID2, "a_Color"), 1, 0, 0); //빨간 점으로 표시.
		glUseProgram(programID);
		mouseX = x;
		mouseY = y;
		StartmouseX = (float)((x - 0.5 * SUBWINDOW_WIDTH) / (0.5 * SUBWINDOW_WIDTH));
		StartmouseY = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));

		mouseX2 = x;
		mouseY2 = y;
		StartmouseX2 = StartmouseX;
		StartmouseY2 = StartmouseY;
	}
	else if ((Button == GLUT_LEFT_BUTTON && State == GLUT_UP) && Holding == TRUE && BoxHolding == FALSE) { //왼쪽 마우스 뗐을 경우, z값 초기화.

		box_vertices[2] = 1;
		box_vertices[5] = 1;
		box_vertices[8] = 1;
		box_vertices[11] = 1;
		EndmouseX = (float)((x - 0.5 * SUBWINDOW_WIDTH) / (0.5 * SUBWINDOW_WIDTH));
		EndmouseY = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
		calc_box_color();
		boxto_change_graph();


		box_vertices2[2] = 1;
		box_vertices2[5] = 1;
		box_vertices2[8] = 1;
		box_vertices2[11] = 1;
		EndmouseX2 = EndmouseX;
		EndmouseY2 = EndmouseY;
		calc_box_color2();
		boxto_change_graph2();

		show_graph(curv_percent, curv_percent2, 10, &plot);

		//추가



	}

	if ((Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) && Holding == TRUE && BoxHolding == TRUE) {
		BoxMoving = TRUE;
		StartmouseX = (float)((x - 0.5 * SUBWINDOW_WIDTH) / (0.5 * SUBWINDOW_WIDTH));
		StartmouseY = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
		box_vertices[2] = -1;
		box_vertices[5] = -1;
		box_vertices[8] = -1;
		box_vertices[11] = -1;
		glutPostRedisplay();

	}
	else if ((Button == GLUT_LEFT_BUTTON && State == GLUT_UP) && Holding == TRUE && BoxHolding == TRUE) {
		EndmouseX = (float)((x - 0.5 * SUBWINDOW_WIDTH) / (0.5 * SUBWINDOW_WIDTH));
		EndmouseY = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
		BoxMoving = FALSE;


		imMovevec = movemat * glm::vec4(box_vertices[0], box_vertices[1], box_vertices[2], 1.0f);
		box_vertices[0] = imMovevec.x;
		box_vertices[1] = imMovevec.y;
		imMovevec = movemat * glm::vec4(box_vertices[3], box_vertices[4], box_vertices[5], 1.0f);
		box_vertices[3] = imMovevec.x;
		box_vertices[4] = imMovevec.y;
		imMovevec = movemat * glm::vec4(box_vertices[6], box_vertices[7], box_vertices[8], 1.0f);
		box_vertices[6] = imMovevec.x;
		box_vertices[7] = imMovevec.y;
		imMovevec = movemat * glm::vec4(box_vertices[9], box_vertices[10], box_vertices[11], 1.0f);
		box_vertices[9] = imMovevec.x;
		box_vertices[10] = imMovevec.y;
		//-----------박스 숨기기
		box_vertices[2] = 1;
		box_vertices[5] = 1;
		box_vertices[8] = 1;
		box_vertices[11] = 1;
		//-----------박스 숨기기
		//-----------movemat 초기화
		movemat = glm::mat4(1.0f);
		//-----------movemat 초기화
		//-----------calc box color 위해 start, end좌표 다시 넣어주기
		StartmouseX = box_vertices[0];
		StartmouseY = box_vertices[1];
		EndmouseX = box_vertices[6];
		EndmouseY = box_vertices[7];
		//-----------calc box color 위해 start, end좌표 다시 넣어주기
		calc_box_color();
		boxto_change_graph();
		glutPostRedisplay();
		show_graph(curv_percent, curv_percent2, 10, &plot);
	}
}
void myMouseClick2(GLint Button, GLint State, int x, int y) {
	if ((Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) && Holding == TRUE && BoxHolding == FALSE) { //왼쪽 마우스 버튼을 눌렀을 때, 기준점잡기.
		glUseProgram(programID2);
		glVertexAttrib3f(glGetAttribLocation(programID2, "a_Color"), 1, 0, 0); //빨간 점으로 표시.
		glUseProgram(programID);
		mouseX2 = x;
		mouseY2 = y;
		StartmouseX2 = (float)((x - 0.5 * SUBWINDOW_WIDTH) / (0.5 * SUBWINDOW_WIDTH));
		StartmouseY2 = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
		//추가
		mouseX = mouseX2;
		mouseY = mouseY2;
		StartmouseX = StartmouseX2;
		StartmouseY = StartmouseY2;
	}
	else if ((Button == GLUT_LEFT_BUTTON && State == GLUT_UP) && Holding == TRUE && BoxHolding == FALSE) { //왼쪽 마우스 뗐을 경우, z값 초기화.

		box_vertices[2] = 1;
		box_vertices[5] = 1;
		box_vertices[8] = 1;
		box_vertices[11] = 1;
		EndmouseX = (float)((x - 0.5 * SUBWINDOW_WIDTH) / (0.5 * SUBWINDOW_WIDTH));
		EndmouseY = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
		calc_box_color();
		boxto_change_graph();


		//추가
		box_vertices2[2] = 1;
		box_vertices2[5] = 1;
		box_vertices2[8] = 1;
		box_vertices2[11] = 1;
		EndmouseX2 = EndmouseX;
		EndmouseY2 = EndmouseY;
		calc_box_color2();
		boxto_change_graph2();

		show_graph(curv_percent, curv_percent2, 10, &plot);

	}
	if ((Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) && Holding == TRUE && BoxHolding == TRUE) {
		BoxMoving2 = TRUE;
		StartmouseX2 = (float)((x - 0.5 * SUBWINDOW_WIDTH) / (0.5 * SUBWINDOW_WIDTH));
		StartmouseY2 = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
		box_vertices2[2] = -1;
		box_vertices2[5] = -1;
		box_vertices2[8] = -1;
		box_vertices2[11] = -1;
		glutPostRedisplay();

	}
	else if ((Button == GLUT_LEFT_BUTTON && State == GLUT_UP) && Holding == TRUE && BoxHolding == TRUE) {
		EndmouseX2 = (float)((x - 0.5 * SUBWINDOW_WIDTH) / (0.5 * SUBWINDOW_WIDTH));
		EndmouseY2 = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
		BoxMoving2 = FALSE;


		imMovevec = movemat2 * glm::vec4(box_vertices2[0], box_vertices2[1], box_vertices2[2], 1.0f);
		box_vertices2[0] = imMovevec.x;
		box_vertices2[1] = imMovevec.y;
		imMovevec = movemat2 * glm::vec4(box_vertices2[3], box_vertices2[4], box_vertices2[5], 1.0f);
		box_vertices2[3] = imMovevec.x;
		box_vertices2[4] = imMovevec.y;
		imMovevec = movemat2 * glm::vec4(box_vertices2[6], box_vertices2[7], box_vertices2[8], 1.0f);
		box_vertices2[6] = imMovevec.x;
		box_vertices2[7] = imMovevec.y;
		imMovevec = movemat2 * glm::vec4(box_vertices2[9], box_vertices2[10], box_vertices2[11], 1.0f);
		box_vertices2[9] = imMovevec.x;
		box_vertices2[10] = imMovevec.y;
		//-----------박스 숨기기
		box_vertices2[2] = 1;
		box_vertices2[5] = 1;
		box_vertices2[8] = 1;
		box_vertices2[11] = 1;
		//-----------박스 숨기기
		//-----------movemat 초기화
		movemat2 = glm::mat4(1.0f);
		//-----------movemat 초기화
		//-----------calc box color 위해 start, end좌표 다시 넣어주기
		StartmouseX2 = box_vertices2[0];
		StartmouseY2 = box_vertices2[1];
		EndmouseX2 = box_vertices2[6];
		EndmouseY2 = box_vertices2[7];
		//-----------calc box color 위해 start, end좌표 다시 넣어주기
		calc_box_color2();
		boxto_change_graph2();
		glutPostRedisplay();
		show_graph(curv_percent, curv_percent2, 10, &plot);
	}
}
void myMouseDrag(int x, int y) {
	if (Holding == FALSE) { //'/' 안누를 시.
		if (mouseX < x) {   //pan right
			if (mouseY < y) {
				tiltRad -= DRAG_STATE;
			}
			else if (mouseY > y) {
				tiltRad += DRAG_STATE;
			}
			panRad -= DRAG_STATE;
			updateViewmat();
			mouseX = x;
			mouseY = y;
		}
		else if (mouseX > x) {
			if (mouseY < y) {   //tilt up
				tiltRad -= DRAG_STATE;
			}
			else if (mouseY > y) {            //tilt down
				tiltRad += DRAG_STATE;
			}
			panRad += DRAG_STATE;
			updateViewmat();
			mouseX = x;
			mouseY = y;
		}
		else {
			if (mouseY < y) {   //tilt up
				tiltRad -= DRAG_STATE;
			}
			else if (mouseY > y) {            //tilt down
				tiltRad += DRAG_STATE;
			}
			updateViewmat();
			mouseX = x;
			mouseY = y;
		}

	}
	else if (BoxHolding == FALSE) {
		varX = (float)((x - 0.5 * SUBWINDOW_WIDTH) / (0.5 * SUBWINDOW_WIDTH));
		varY = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
		box_vertices[0] = StartmouseX;
		box_vertices[1] = StartmouseY;
		box_vertices[3] = varX;
		box_vertices[4] = StartmouseY;
		box_vertices[6] = varX;
		box_vertices[7] = varY;
		box_vertices[9] = StartmouseX;
		box_vertices[10] = varY;
		box_vertices[2] = -1;
		box_vertices[5] = -1;
		box_vertices[8] = -1;
		box_vertices[11] = -1;
		//추가

		box_vertices2[0] = StartmouseX;
		box_vertices2[1] = StartmouseY;
		box_vertices2[3] = varX;
		box_vertices2[4] = StartmouseY;
		box_vertices2[6] = varX;
		box_vertices2[7] = varY;
		box_vertices2[9] = StartmouseX;
		box_vertices2[10] = varY;
		box_vertices2[2] = -1;
		box_vertices2[5] = -1;
		box_vertices2[8] = -1;
		box_vertices2[11] = -1;
	}
	else if (BoxMoving == TRUE) {
		varX = (float)((x - 0.5 * SUBWINDOW_WIDTH) / (0.5 * SUBWINDOW_WIDTH));
		varY = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
		MovevarX = StartmouseX - varX;
		MovevarY = StartmouseY - varY;
		movemat = glm::translate(glm::vec3(-MovevarX, -MovevarY, 0));


	}

	glutPostRedisplay();
}
void myMouseDrag2(int x, int y) {
	if (Holding == FALSE) { //'/' 안누를 시.
		if (mouseX < x) {   //pan right
			if (mouseY < y) {
				tiltRad -= DRAG_STATE;
			}
			else if (mouseY > y) {
				tiltRad += DRAG_STATE;
			}
			panRad -= DRAG_STATE;
			updateViewmat();
			mouseX = x;
			mouseY = y;
		}
		else if (mouseX > x) {
			if (mouseY < y) {   //tilt up
				tiltRad -= DRAG_STATE;
			}
			else if (mouseY > y) {            //tilt down
				tiltRad += DRAG_STATE;
			}
			panRad += DRAG_STATE;
			updateViewmat();
			mouseX = x;
			mouseY = y;
		}
		else {
			if (mouseY < y) {   //tilt up
				tiltRad -= DRAG_STATE;
			}
			else if (mouseY > y) {            //tilt down
				tiltRad += DRAG_STATE;
			}
			updateViewmat();
			mouseX = x;
			mouseY = y;
		}

	}
	else if (BoxHolding == FALSE) {

		varX2 = (float)((x - 0.5 * SUBWINDOW_WIDTH) / (0.5 * SUBWINDOW_WIDTH));
		varY2 = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
		box_vertices2[0] = StartmouseX2;
		box_vertices2[1] = StartmouseY2;
		box_vertices2[3] = varX2;
		box_vertices2[4] = StartmouseY2;
		box_vertices2[6] = varX2;
		box_vertices2[7] = varY2;
		box_vertices2[9] = StartmouseX2;
		box_vertices2[10] = varY2;
		box_vertices2[2] = -1;
		box_vertices2[5] = -1;
		box_vertices2[8] = -1;
		box_vertices2[11] = -1;

		//추가 
		box_vertices[0] = StartmouseX2;
		box_vertices[1] = StartmouseY2;
		box_vertices[3] = varX2;
		box_vertices[4] = StartmouseY2;
		box_vertices[6] = varX2;
		box_vertices[7] = varY2;
		box_vertices[9] = StartmouseX2;
		box_vertices[10] = varY2;
		box_vertices[2] = -1;
		box_vertices[5] = -1;
		box_vertices[8] = -1;
		box_vertices[11] = -1;
		//추가

	}
	else if (BoxMoving2 == TRUE) {
		varX2 = (float)((x - 0.5 * SUBWINDOW_WIDTH) / (0.5 * SUBWINDOW_WIDTH));
		varY2 = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
		MovevarX2 = StartmouseX2 - varX2;
		MovevarY2 = StartmouseY2 - varY2;
		movemat2 = glm::translate(glm::vec3(-MovevarX2, -MovevarY2, 0));


	}

	glutPostRedisplay();
}
void MyMouseWheelFunc(int wheel, int direction, int x, int y) {
	if (direction > 0) {      //zoom in
		if (orthoOn == 0) {
			persRad -= 1.0f;
			glm::mat4 perspectiveMat = glm::perspective(persRad * TO_RADIAN, 1.0f, zNear, zFar);
			projectmat = perspectiveMat;

			updateViewmat();
		}
		else if (orthoOn == 1) {
			maxX -= 0.02f;
			minX += 0.02f;
			maxY -= 0.02f;
			minY += 0.02f;
			orthoMat = glm::ortho(minX, maxX, minY, maxY, zNear, zFar);
			projectmat = orthoMat;
			updateViewmat();
		}

	}
	else {               //zoom out
		if (orthoOn == 0) {
			persRad += 1.0f;
			glm::mat4 perspectiveMat = glm::perspective(persRad * TO_RADIAN, 1.0f, zNear, zFar);
			projectmat = perspectiveMat;
			updateViewmat();
		}
		else if (orthoOn == 1) {
			maxX += 0.02f;
			minX -= 0.02f;
			maxY += 0.02f;
			minY -= 0.02f;
			orthoMat = glm::ortho(minX, maxX, minY, maxY, zNear, zFar);

			projectmat = orthoMat;
			updateViewmat();
		}
	}
	glutPostRedisplay();
}
void MyMouseWheelFunc2(int wheel, int direction, int x, int y) {
	if (direction > 0) {      //zoom in
		if (orthoOn2 == 0) {
			persRad2 -= 1.0f;
			glm::mat4 perspectiveMat = glm::perspective(persRad2 * TO_RADIAN, 1.0f, zNear2, zFar2);
			projectmat2 = perspectiveMat;

			updateViewmat();
		}
		else if (orthoOn == 1) {
			maxX2 -= 0.02f;
			minX2 += 0.02f;
			maxY2 -= 0.02f;
			minY2 += 0.02f;
			orthoMat2 = glm::ortho(minX2, maxX2, minY2, maxY2, zNear2, zFar2);
			projectmat2 = orthoMat2;
		}

	}
	else {               //zoom out
		if (orthoOn2 == 0) {
			persRad2 += 1.0f;
			glm::mat4 perspectiveMat = glm::perspective(persRad2 * TO_RADIAN, 1.0f, zNear2, zFar2);
			projectmat2 = perspectiveMat;


			;
		}
		else if (orthoOn2 == 1) {
			maxX2 += 0.1f;
			minX2 -= 0.1f;
			maxY2 += 0.1f;
			minY2 -= 0.1f;
			orthoMat2 = glm::ortho(minX2, maxX2, minY2, maxY2, zNear2, zFar2);

			projectmat2 = orthoMat2;
		}
	}
	glutPostRedisplay();
}
void init(string file_name, float obj_scale)
{

	if (subwindow_num == 1) {
		cout << endl << "========== SubWindow 1 ==========" << endl;
		//initilize the glew and check the errors.
		GLenum res = glewInit();
		if (res != GLEW_OK)
		{
			std::fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
		}

		//select the background color
		glClearColor(0.85f, 0.85f, 0.85f, 1.0);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		start_time = clock();
		loadObj2(file_name, obj_scale);
		calc_color();
		end_time = clock();

		double total_t = (double)(end_time - start_time);
		cout << "Time : " << total_t / 1000. << endl;

		cout << endl;


		programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
		programID2 = LoadShaders("VertexShader2_box.txt", "FragmentShader2_box.txt");

		glUseProgram(programID);

		glGenVertexArrays(2, vao);

		glGenBuffers(2, vbo);

		glBindVertexArray(vao[0]);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertexIndices.size(), vertexIndices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * obj_vertices.size(), obj_vertices.data(), GL_STATIC_DRAW);

		GLint vtxPosition = glGetAttribLocation(programID, "vtxPosition");
		glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));

		glEnableVertexAttribArray(vtxPosition);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor.size(), aColor.data(), GL_STATIC_DRAW);
		GLint aColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(aColor);

		GLuint matLoc = glGetUniformLocation(programID, "worldMat");
		glUniformMatrix4fv(matLoc, 1, GL_FALSE, &wmat[0][0]);


		tiltmat = glm::rotate(tiltRad * TO_RADIAN, glm::vec3(1.0f, 0, 0));
		rotmat = glm::rotate(panRad * TO_RADIAN, glm::vec3(0, 1.0f, 0));
		transmat = glm::translate(glm::vec3(transX, transY, transZ));


		viewmat = glm::transpose(tiltmat) * glm::transpose(rotmat) * glm::inverse(transmat);

		GLuint viewID = glGetUniformLocation(programID, "viewmat");
		glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewmat[0][0]);


		glm::mat4 orthoMat = glm::ortho(minX, maxX, minY, maxY, zNear, zFar);

		projectmat = orthoMat;

		GLuint projectID = glGetUniformLocation(programID, "projectmat");
		glUniformMatrix4fv(projectID, 1, GL_FALSE, &projectmat[0][0]);

		GLuint lightPosID = glGetUniformLocation(programID, "uLightPos");
		glUniform3fv(lightPosID, 1, &lightPos[0]);

		glUniform3fv(glGetUniformLocation(programID, "material.diffuse"), 1, &material.MTL_Kd[0]);
		glUniform3fv(glGetUniformLocation(programID, "material.specular"), 1, &material.MTL_Kd[0]);

		glUniform1i(glGetUniformLocation(programID, "lightTurnOnOff"), lightTurnOnOff);
		glUniform1f(glGetUniformLocation(programID, "limitZ"), limitZ);

		// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡBox drawingㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
		glUseProgram(programID2);
		glBindVertexArray(vao[1]);
		glGenBuffers(1, &vbo_box);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_box);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, box_vertices, GL_STATIC_DRAW);

		GLuint vtxPosition2 = glGetAttribLocation(programID2, "vtxPosition");
		glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(vtxPosition);

		GLuint moveID = glGetUniformLocation(programID2, "movemat");
		glUniformMatrix4fv(moveID, 1, GL_FALSE, &movemat[0][0]);

		//glVertexAttrib3f(glGetAttribLocation(programID2, "a_Color"), 1, 0, 0);
	}
	else if (subwindow_num == 2) {                                       //subwindow 2

		cout << endl << "========== SubWindow 2    ==========" << endl;
		//initilize the glew and check the errors.
		GLenum res = glewInit();
		if (res != GLEW_OK)
		{
			std::fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
		}

		//select the background color
		glClearColor(0.85f, 0.85f, 0.85f, 1.0);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		start_time = clock();
		loadObj2(file_name, obj_scale);
		calc_color();
		end_time = clock();

		double total_t = (double)(end_time - start_time);
		cout << "Time : " << total_t / 1000. << endl;


		programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
		programID2 = LoadShaders("VertexShader2_box.txt", "FragmentShader2_box.txt");

		glUseProgram(programID);

		glGenVertexArrays(2, vao);

		glGenBuffers(2, vbo);

		glBindVertexArray(vao[0]);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertexIndices2.size(), vertexIndices2.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * obj_vertices2.size(), obj_vertices2.data(), GL_STATIC_DRAW);

		GLint vtxPosition = glGetAttribLocation(programID, "vtxPosition");
		glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));

		glEnableVertexAttribArray(vtxPosition);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor2.size(), aColor2.data(), GL_STATIC_DRAW);
		GLint aColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(aColor);

		GLuint matLoc = glGetUniformLocation(programID, "worldMat");
		glUniformMatrix4fv(matLoc, 1, GL_FALSE, &wmat2[0][0]);


		tiltmat = glm::rotate(tiltRad * TO_RADIAN, glm::vec3(1.0f, 0, 0));
		rotmat = glm::rotate(panRad * TO_RADIAN, glm::vec3(0, 1.0f, 0));
		transmat = glm::translate(glm::vec3(transX, transY, transZ));
		viewmat = glm::transpose(tiltmat) * glm::transpose(rotmat) * glm::inverse(transmat);

		tiltmat4 = glm::rotate(-90 * TO_RADIAN, glm::vec3(1.0f, 0, 0));
		transmat4 = glm::translate(glm::vec3(transX, transY - 1, transZ));

		GLuint viewID = glGetUniformLocation(programID, "viewmat");
		glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewmat[0][0]);


		glm::mat4 orthoMat = glm::ortho(minX, maxX, minY, maxY, zNear, zFar);

		projectmat = orthoMat;

		GLuint projectID = glGetUniformLocation(programID, "projectmat");
		glUniformMatrix4fv(projectID, 1, GL_FALSE, &projectmat[0][0]);

		GLuint lightPosID = glGetUniformLocation(programID, "uLightPos");
		glUniform3fv(lightPosID, 1, &lightPos[0]);

		glUniform3fv(glGetUniformLocation(programID, "material.diffuse"), 1, &material.MTL_Kd[0]);
		glUniform3fv(glGetUniformLocation(programID, "material.specular"), 1, &material.MTL_Kd[0]);

		glUniform1i(glGetUniformLocation(programID, "lightTurnOnOff"), lightTurnOnOff);
		glUniform1f(glGetUniformLocation(programID, "limitZ"), limitZ2);

		// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡBox drawingㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
		glUseProgram(programID2);
		glBindVertexArray(vao[1]);
		glGenBuffers(1, &vbo_box);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_box);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, box_vertices2, GL_STATIC_DRAW);

		GLuint vtxPosition2 = glGetAttribLocation(programID2, "vtxPosition");
		glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(vtxPosition);

		GLuint moveID = glGetUniformLocation(programID2, "movemat");
		glUniformMatrix4fv(moveID, 1, GL_FALSE, &movemat2[0][0]);

		//glVertexAttrib3f(glGetAttribLocation(programID2, "a_Color"), 1, 0, 0);
	}
	else if (subwindow_num == 3) {                                       //subwindow 3
		cout << endl << "========== SubWindow 3 ==========" << endl;
		//initilize the glew and check the errors.
		GLenum res = glewInit();
		if (res != GLEW_OK)
		{
			std::fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
		}

		//select the background color
		glClearColor(0.85f, 0.85f, 0.85f, 1.0);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);


		programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");

		glUseProgram(programID);

		glGenVertexArrays(2, vao);

		glGenBuffers(2, vbo);

		glBindVertexArray(vao[0]);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertexIndices.size(), vertexIndices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * obj_vertices.size(), obj_vertices.data(), GL_STATIC_DRAW);

		GLint vtxPosition = glGetAttribLocation(programID, "vtxPosition");
		glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));

		glEnableVertexAttribArray(vtxPosition);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor.size(), aColor.data(), GL_STATIC_DRAW);
		GLint aColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(aColor);

		GLuint matLoc = glGetUniformLocation(programID, "worldMat");
		glUniformMatrix4fv(matLoc, 1, GL_FALSE, &wmat[0][0]);


		tiltmat = glm::rotate(tiltRad * TO_RADIAN, glm::vec3(1.0f, 0, 0));
		rotmat = glm::rotate(panRad * TO_RADIAN, glm::vec3(0, 1.0f, 0));
		transmat = glm::translate(glm::vec3(transX, transY, transZ));
		viewmat = glm::transpose(tiltmat) * glm::transpose(rotmat) * glm::inverse(transmat);


		tiltmat4 = glm::rotate(TO_RADIAN, glm::vec3(1.0f, 0, 0));
		transmat4 = glm::translate(glm::vec3(transX, transY, transZ));
		viewmat4 = glm::inverse(transmat4) * glm::transpose(tiltmat4);

		GLuint viewID = glGetUniformLocation(programID, "viewmat");
		glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewmat[0][0]);


		glm::mat4 orthoMat = glm::ortho(minX, maxX, minY, maxY, zNear, zFar);

		projectmat = orthoMat;

		GLuint projectID = glGetUniformLocation(programID, "projectmat");
		glUniformMatrix4fv(projectID, 1, GL_FALSE, &projectmat[0][0]);

		GLuint lightPosID = glGetUniformLocation(programID, "uLightPos");
		glUniform3fv(lightPosID, 1, &lightPos[0]);

		glUniform3fv(glGetUniformLocation(programID, "material.diffuse"), 1, &material.MTL_Kd[0]);
		glUniform3fv(glGetUniformLocation(programID, "material.specular"), 1, &material.MTL_Kd[0]);

		glUniform1i(glGetUniformLocation(programID, "lightTurnOnOff"), lightTurnOnOff);
		glUniform1f(glGetUniformLocation(programID, "limitZ"), limitZ);
	}

	else if (subwindow_num == 4) {                                       //subwindow 4

		cout << endl << "========== SubWindow 4    ==========" << endl;
		//initilize the glew and check the errors.
		GLenum res = glewInit();
		if (res != GLEW_OK)
		{
			std::fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
		}

		//select the background color
		glClearColor(0.85f, 0.85f, 0.85f, 1.0);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);


		programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");

		glUseProgram(programID);

		glGenVertexArrays(2, vao);

		glGenBuffers(2, vbo);

		glBindVertexArray(vao[0]);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertexIndices2.size(), vertexIndices2.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * obj_vertices2.size(), obj_vertices2.data(), GL_STATIC_DRAW);

		GLint vtxPosition = glGetAttribLocation(programID, "vtxPosition");
		glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));

		glEnableVertexAttribArray(vtxPosition);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * aColor2.size(), aColor2.data(), GL_STATIC_DRAW);
		GLint aColor = glGetAttribLocation(programID, "a_Color");
		glVertexAttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(aColor);

		GLuint matLoc = glGetUniformLocation(programID, "worldMat");
		glUniformMatrix4fv(matLoc, 1, GL_FALSE, &wmat2[0][0]);


		tiltmat = glm::rotate(tiltRad * TO_RADIAN, glm::vec3(1.0f, 0, 0));
		rotmat = glm::rotate(panRad * TO_RADIAN, glm::vec3(0, 1.0f, 0));
		transmat = glm::translate(glm::vec3(transX, transY, transZ));
		viewmat = glm::transpose(tiltmat) * glm::transpose(rotmat) * glm::inverse(transmat);

		tiltmat4 = glm::rotate(TO_RADIAN, glm::vec3(1.0f, 0, 0));
		transmat4 = glm::translate(glm::vec3(transX, transY, transZ));
		viewmat4 = glm::inverse(transmat4) * glm::transpose(tiltmat4);

		GLuint viewID = glGetUniformLocation(programID, "viewmat");
		glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewmat4[0][0]);


		glm::mat4 orthoMat = glm::ortho(minX, maxX, minY, maxY, zNear, zFar);

		projectmat = orthoMat;

		GLuint projectID = glGetUniformLocation(programID, "projectmat");
		glUniformMatrix4fv(projectID, 1, GL_FALSE, &projectmat[0][0]);

		GLuint lightPosID = glGetUniformLocation(programID, "uLightPos");
		glUniform3fv(lightPosID, 1, &lightPos[0]);

		glUniform3fv(glGetUniformLocation(programID, "material.diffuse"), 1, &material.MTL_Kd[0]);
		glUniform3fv(glGetUniformLocation(programID, "material.specular"), 1, &material.MTL_Kd[0]);

		glUniform1i(glGetUniformLocation(programID, "lightTurnOnOff"), lightTurnOnOff);
		glUniform1f(glGetUniformLocation(programID, "limitZ"), limitZ2);
	}


}
void filestreaminit() {
	//-------------------------FileStream-----------------------------
	std::string dir = "patient/";
	std::vector<std::string> filelist_paths;
	int pathnum = 0; //파일 개수
	int path_count = 0;

	namespace fs = std::filesystem;
	for (auto& p : fs::recursive_directory_iterator(dir)) {
	 filelist_paths.push_back(p.path().string());
	}

	pathnum = filelist_paths.size();

	for (auto str : filelist_paths) {
		int slashindex;
		slashindex = str.find("/");
		str = str.substr(slashindex + 1);
	
		std::cout << str << std::endl;
    
	}

}
int main(int argc, char** argv)
{
	filestreaminit();

	// Create GLUT window
	glutInit(&argc, argv);
#ifdef __FREEGLUT_EXT_H__
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS | GLUT_DEPTH);
#endif
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT * 2);
	mainWindow = glutCreateWindow("Medical 3D Project");


	glutDisplayFunc(renderSceneNew);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGLUT_Init();
	ImGui_ImplGLUT_InstallFuncs();
	ImGui_ImplOpenGL2_Init();

	subWindow1 = glutCreateSubWindow(mainWindow, 0, 0, SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
	subwindow_num = 1;
	first_f_check = 0;
	init(filename, scale);
	glutDisplayFunc(renderSceneAll);
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouseClick);
	glutMotionFunc(myMouseDrag);
	glutMouseWheelFunc(MyMouseWheelFunc);

	subWindow3 = glutCreateSubWindow(mainWindow, 0, SUBWINDOW_HEIGHT, SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
	subwindow_num = 3;
	init(filename, scale);
	glutDisplayFunc(renderSceneAll);

	subWindow2 = glutCreateSubWindow(mainWindow, SUBWINDOW_WIDTH, 0, SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
	subwindow_num = 2;
	first_f_check = 0;
	init(filename2, scale2);
	glutDisplayFunc(renderSceneAll);
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouseClick2);
	glutMotionFunc(myMouseDrag2);
	glutMouseWheelFunc(MyMouseWheelFunc);

	subWindow4 = glutCreateSubWindow(mainWindow, SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT, SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT);
	subwindow_num = 4;
	init(filename2, scale2);
	glutDisplayFunc(renderSceneAll);



	for (int i = 0; i < 10; i++) {
		curv_percent[i] = curv_distrib[i] / (float)vertex_count;
		curv_percent2[i] = curv_distrib2[i] / (float)vertex_count2;
	}
	cout << "\t0.0~0.1\t\t0.1~0.2\t\t0.2~0.3\t\t0.3~0.4\t\t0.4~0.5\t\t0.5~0.6\t\t0.6~0.7\t\t0.7~0.8\t\t0.8~0.9\t\t0.9~1.0\t\ttotal" << endl;
	cout << "count\t" << curv_distrib[0]
		<< "\t\t" << curv_distrib[1]
		<< "\t\t" << curv_distrib[2]
		<< "\t\t" << curv_distrib[3]
		<< "\t\t" << curv_distrib[4]
		<< "\t\t" << curv_distrib[5]
		<< "\t\t" << curv_distrib[6]
		<< "\t\t" << curv_distrib[7]
		<< "\t\t" << curv_distrib[8]
		<< "\t\t" << curv_distrib[9]

		<< "\t\t" << vertex_count << endl;
	cout << "percent\t" <<
		curv_percent[0] << "\t" <<
		curv_percent[1] << "\t" <<
		curv_percent[2] << "\t" <<
		curv_percent[3] << "\t" <<
		curv_percent[4] << "\t" <<
		curv_percent[5] << "\t" <<
		curv_percent[6] << "\t" <<
		curv_percent[7] << "\t" <<
		curv_percent[8] << "\t" <<
		curv_percent[9] << "\t" <<

		endl << endl;

	cout << "count\t" << curv_distrib2[0]
		<< "\t\t" << curv_distrib2[1]
		<< "\t\t" << curv_distrib2[2]
		<< "\t\t" << curv_distrib2[3]
		<< "\t\t" << curv_distrib2[4]
		<< "\t\t" << curv_distrib2[5]
		<< "\t\t" << curv_distrib2[6]
		<< "\t\t" << curv_distrib2[7]
		<< "\t\t" << curv_distrib2[8]
		<< "\t\t" << curv_distrib2[9]

		<< "\t\t" << vertex_count2 << endl;
	cout << "percent\t" << curv_percent2[0]
		<< "\t" << curv_percent2[1]
		<< "\t" << curv_percent2[2]
		<< "\t" << curv_percent2[3]
		<< "\t" << curv_percent2[4]
		<< "\t" << curv_percent2[5]
		<< "\t" << curv_percent2[6]
		<< "\t" << curv_percent2[7]
		<< "\t" << curv_percent2[8]
		<< "\t" << curv_percent2[9]

		<< endl << endl;

	int numOfElements = sizeof(curv_percent) / sizeof(double);

	show_graph(curv_percent, curv_percent2, numOfElements, &plot);


	glutMainLoop();

	exit_graph(&plot); //그래프 종료


	glDeleteVertexArrays(2, vao);

	return 1;
}