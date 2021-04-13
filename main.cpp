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

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 1200
#define TO_RADIAN 0.01745329252f 

using namespace std;

GLuint programID;
GLuint programID2; //박스 프로그램
GLuint VertexArrayID;

int mainWindow, subWindow1, subWindow2;

string filename = "PiggyBank.obj";
string filename2 = "PiggyBank.obj";
string mtlpath; //mtl 파일명 저장..
float scale = 0.3f;
float scale2 = 0.3f;

int subwindow_num = 1;

vector<GLuint>vertexIndices, texIndices, normalIndices;
vector<GLuint>vertexIndices2, texIndices2, normalIndices2;
vector<glm::vec3>obj_vertices, obj_vertices2;
vector<glm::vec2>obj_texcoord, obj_texcoord2;
vector<glm::vec3>obj_normals, obj_normals2;
vector<glm::vec3>aColor, aColor2;
vector<glm::vec3>bColor, bColor2; // box 후 컬러.
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
GLuint vbo_box; //박스 위치 버퍼
int mouseX,mouseX2;	// 마우스로 시점 이동시 전의 마우스좌표 저장  / 초기 설정 -1
int mouseY,mouseY2;
float StartmouseX, StartmouseX2; // 박스 시작 위치 x
float StartmouseY, StartmouseY2; // 박스 시작 위치 y
float EndmouseX, EndmouseX2; //q
float EndmouseY, EndmouseY2;
float varX, varX2; //박스 끝위치 x
float varY, varY2; //박스 끝위치 y
bool Holding = FALSE;
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
	glm::vec3 normal_vec = glm::vec3(0.,0.,0.);
	float normal_count = 0;
	vector<int>near_vertex;	//주변 점
	vector<int> near_face;	//테스트용 : near_face_2와 값이 같은지 확인하기 위함

	double calc_curv = 0;
	double calc_count = 0;
};
vertex_info*vertexInfo;
vertex_info*vertexInfo2;

int first_f_check = 0; //처음 f를 읽었는지 체크

int vertex_count = 0;
int vertex_count2 = 0;
int face_count = 0;
int face_count2 = 0;

int windowCheck;

float maxX = 1.0f,maxX2 = 1.0f;
float minX = -1.0f, minX2 = -1.0f;
float maxY = 1.0f, maxY2 = 1.0f;
float minY = -1.0f, minY2 = -1.0f;
float panRad = 0.f, panRad2 = 0.f;
float tiltRad = 0.f, tiltRad2 = 0.f;
float transX = 0.f, transX2 = 0.f;
float transY = 0.f, transY2 = 0.f;
float transZ = 2.0f, transZ2 = 2.0f;
float zNear = 0.01f, zNear2 = 0.01f;
float zFar = 10.0f, zFar2 = 10.0f;
int orthoOn = 1, orthoOn2 = 1;
int first = 1, first2 = 1;
float randRad, randRad2;
float persRad = 60, persRad2 = 60;
int lightTurnOnOff = 1, lightTurnOnOff2 = 1;

glm::mat4 realMat = glm::mat4(1.0f), realMat2 = glm::mat4(1.0f); // 박스좌표 계산을 위한 매트릭스
glm::vec4 vPos,vPos2; // 박스좌표 계산을 위한 좌표
glm::mat4 wmat = glm::mat4(1.0f), wmat2 = glm::mat4(1.0f);
glm::mat4 viewmat = glm::mat4(1.0f), viewmat2 = glm::mat4(1.0f);
glm::mat4 tiltmat = glm::mat4(1.0f), tiltmat2 = glm::mat4(1.0f);
glm::mat4 rotmat = glm::mat4(1.0f), rotmat2 = glm::mat4(1.0f);
glm::mat4 transmat = glm::mat4(1.0f), transmat2 = glm::mat4(1.0f);
glm::mat4 orthoMat = glm::mat4(1.0f), orthoMat2 = glm::mat4(1.0f);
glm::mat4 projectmat = glm::mat4(1.0f), projectmat2 = glm::mat4(1.0f);
glm::mat4 normalMat = glm::mat4(1.0f), normalMat2 = glm::mat4(1.0f);
glm::vec3 lightPos = glm::vec3(10.0f, 10.0f, 10.0f), lightPos2 = glm::vec3(10.0f, 10.0f, 10.0f);

GLuint vbo[2];
GLuint vao[2];
unsigned int ebo;


void SelectScale(); // Scale 정해주는 함수 : 추후 구현 예정
void parseMtllib(string line); //mtl 파일명 저장.
void onReadMTLFile(string path);
void parseKd(string line); // mtl파일의 Kd 저장.
void parseKa(string line); // mtl파일의 ka 저장.
void parseKs(string line); // mtl파일의 ks 저장.
int HowDrawBox(float start_X, float start_Y, float End_X, float End_Y);

void add_near_vertex_face(int vertex1, int vertex2, int vertex3, int face_num) {
	if (subwindow_num == 1) {	
		auto vertex1_begin = vertexInfo[vertex1].near_vertex.begin();
		auto vertex1_end = vertexInfo[vertex1].near_vertex.end();
		vector<int>::iterator iter12 = find(vertex1_begin, vertex1_end, vertex2);
		int vertex12 = distance(vertex1_begin, iter12);
		if (vertex12 == vertexInfo[vertex1].near_vertex.size()) {	//기존에 vertex1의 near vertex에 vertex2가 없을때
			vertexInfo[vertex1].near_vertex.push_back(vertex2);
		}

		auto vertex1_begin2 = vertexInfo[vertex1].near_vertex.begin();
		auto vertex1_end2 = vertexInfo[vertex1].near_vertex.end();
		vector<int>::iterator iter13 = find(vertex1_begin2, vertex1_end2, vertex3);
		int vertex13 = distance(vertex1_begin2, iter13);
		if (vertex13 == vertexInfo[vertex1].near_vertex.size()) {	//1 3 x
			vertexInfo[vertex1].near_vertex.push_back(vertex3);
		}

		auto vertex2_begin = vertexInfo[vertex2].near_vertex.begin();
		auto vertex2_end = vertexInfo[vertex2].near_vertex.end();
		vector<int>::iterator iter21 = find(vertex2_begin, vertex2_end, vertex1);
		int vertex21 = distance(vertex2_begin, iter21);
		if (vertex21 == vertexInfo[vertex2].near_vertex.size()) {	//기존에 vertex2의 near vertex에 vertex1가 없을때
			vertexInfo[vertex2].near_vertex.push_back(vertex1);
		}

		auto vertex2_begin2 = vertexInfo[vertex2].near_vertex.begin();
		auto vertex2_end2 = vertexInfo[vertex2].near_vertex.end();
		vector<int>::iterator iter23 = find(vertex2_begin2, vertex2_end2, vertex3);
		int vertex23 = distance(vertex2_begin2, iter23);
		if (vertex23 == vertexInfo[vertex2].near_vertex.size()) {	//2 3 x
			vertexInfo[vertex2].near_vertex.push_back(vertex3);
		}



		auto vertex3_begin = vertexInfo[vertex3].near_vertex.begin();
		auto vertex3_end = vertexInfo[vertex3].near_vertex.end();
		vector<int>::iterator iter31 = find(vertex3_begin, vertex3_end, vertex1);
		int vertex31 = distance(vertex3_begin, iter31);
		if (vertex31 == vertexInfo[vertex3].near_vertex.size()) {	//기존에 vertex3의 near vertex에 vertex1가 없을때
			vertexInfo[vertex3].near_vertex.push_back(vertex1);
		}

		auto vertex3_begin2 = vertexInfo[vertex3].near_vertex.begin();
		auto vertex3_end2 = vertexInfo[vertex3].near_vertex.end();
		vector<int>::iterator iter32 = find(vertex3_begin2, vertex3_end2, vertex2);
		int vertex32 = distance(vertex3_begin2, iter32);
		if (vertex32 == vertexInfo[vertex3].near_vertex.size()) {	//3 2 x
			vertexInfo[vertex3].near_vertex.push_back(vertex2);
		}

		//------------------near face에 넣기--------------------
		auto vertex1_face_begin = vertexInfo[vertex1].near_face.begin();
		auto vertex1_face_end = vertexInfo[vertex1].near_face.end();
		vector<int>::iterator iter1 = find(vertex1_face_begin, vertex1_face_end, face_num);
		if (iter1 == vertex1_face_end) {
			vertexInfo[vertex1].near_face.push_back(face_num);
		}

		auto vertex2_face_begin = vertexInfo[vertex2].near_face.begin();
		auto vertex2_face_end = vertexInfo[vertex2].near_face.end();
		vector<int>::iterator iter2 = find(vertex2_face_begin, vertex2_face_end, face_num);
		if (iter2 == vertex2_face_end) {
			vertexInfo[vertex2].near_face.push_back(face_num);
		}

		auto vertex3_face_begin = vertexInfo[vertex3].near_face.begin();
		auto vertex3_face_end = vertexInfo[vertex3].near_face.end();
		vector<int>::iterator iter3 = find(vertex3_face_begin, vertex3_face_end, face_num);
		if (iter3 == vertex3_face_end) {
			vertexInfo[vertex3].near_face.push_back(face_num);
		}

	}
	else if (subwindow_num == 2) {												//subwindow 2
		auto vertex1_begin = vertexInfo2[vertex1].near_vertex.begin();
		auto vertex1_end = vertexInfo2[vertex1].near_vertex.end();
		vector<int>::iterator iter12 = find(vertex1_begin, vertex1_end, vertex2);
		int vertex12 = distance(vertex1_begin, iter12);
		if (vertex12 == vertexInfo2[vertex1].near_vertex.size()) {	//기존에 vertex1의 near vertex에 vertex2가 없을때
			vertexInfo2[vertex1].near_vertex.push_back(vertex2);
		}

		auto vertex1_begin2 = vertexInfo2[vertex1].near_vertex.begin();
		auto vertex1_end2 = vertexInfo2[vertex1].near_vertex.end();
		vector<int>::iterator iter13 = find(vertex1_begin2, vertex1_end2, vertex3);
		int vertex13 = distance(vertex1_begin2, iter13);
		if (vertex13 == vertexInfo2[vertex1].near_vertex.size()) {	//1 3 x
			vertexInfo2[vertex1].near_vertex.push_back(vertex3);
		}



		auto vertex2_begin = vertexInfo2[vertex2].near_vertex.begin();
		auto vertex2_end = vertexInfo2[vertex2].near_vertex.end();
		vector<int>::iterator iter21 = find(vertex2_begin, vertex2_end, vertex1);
		int vertex21 = distance(vertex2_begin, iter21);
		if (vertex21 == vertexInfo2[vertex2].near_vertex.size()) {	//기존에 vertex2의 near vertex에 vertex1가 없을때
			vertexInfo2[vertex2].near_vertex.push_back(vertex1);
		}

		auto vertex2_begin2 = vertexInfo2[vertex2].near_vertex.begin();
		auto vertex2_end2 = vertexInfo2[vertex2].near_vertex.end();
		vector<int>::iterator iter23 = find(vertex2_begin2, vertex2_end2, vertex3);
		int vertex23 = distance(vertex2_begin2, iter23);
		if (vertex23 == vertexInfo2[vertex2].near_vertex.size()) {	//2 3 x
			vertexInfo2[vertex2].near_vertex.push_back(vertex3);
		}


		auto vertex3_begin = vertexInfo2[vertex3].near_vertex.begin();
		auto vertex3_end = vertexInfo2[vertex3].near_vertex.end();
		vector<int>::iterator iter31 = find(vertex3_begin, vertex3_end, vertex1);
		int vertex31 = distance(vertex3_begin, iter31);
		if (vertex31 == vertexInfo2[vertex3].near_vertex.size()) {	//기존에 vertex3의 near vertex에 vertex1가 없을때
			vertexInfo2[vertex3].near_vertex.push_back(vertex1);
		}

		auto vertex3_begin2 = vertexInfo2[vertex3].near_vertex.begin();
		auto vertex3_end2 = vertexInfo2[vertex3].near_vertex.end();
		vector<int>::iterator iter32 = find(vertex3_begin2, vertex3_end2, vertex2);
		int vertex32 = distance(vertex3_begin2, iter32);
		if (vertex32 == vertexInfo2[vertex3].near_vertex.size()) {	//3 2 x
			vertexInfo2[vertex3].near_vertex.push_back(vertex2);
		}

		//------------------near face에 넣기--------------------
		auto vertex1_face_begin = vertexInfo2[vertex1].near_face.begin();
		auto vertex1_face_end = vertexInfo2[vertex1].near_face.end();
		vector<int>::iterator iter1 = find(vertex1_face_begin, vertex1_face_end, face_num);
		if (iter1 == vertex1_face_end) {
			vertexInfo2[vertex1].near_face.push_back(face_num);
		}

		auto vertex2_face_begin = vertexInfo2[vertex2].near_face.begin();
		auto vertex2_face_end = vertexInfo2[vertex2].near_face.end();
		vector<int>::iterator iter2 = find(vertex2_face_begin, vertex2_face_end, face_num);
		if (iter2 == vertex2_face_end) {
			vertexInfo2[vertex2].near_face.push_back(face_num);
		}

		auto vertex3_face_begin = vertexInfo2[vertex3].near_face.begin();
		auto vertex3_face_end = vertexInfo2[vertex3].near_face.end();
		vector<int>::iterator iter3 = find(vertex3_face_begin, vertex3_face_end, face_num);
		if (iter3 == vertex3_face_end) {
			vertexInfo2[vertex3].near_face.push_back(face_num);
		}

	}
	

};
void calc_normal(int num,int vertex_n1, int vertex_n2, int vertex_n3) {
	if (subwindow_num == 1) {
		glm::vec3 vec2_1, vec3_1;

		vec2_1 = obj_vertices[vertex_n2] - obj_vertices[vertex_n1];
		vec3_1 = obj_vertices[vertex_n3] - obj_vertices[vertex_n1];

		double vx = vec2_1.y*vec3_1.z - vec2_1.z*vec3_1.y;
		double vy = vec2_1.z*vec3_1.x - vec2_1.x*vec3_1.z;
		double vz = vec2_1.x*vec3_1.y - vec2_1.y*vec3_1.x;
		glm::vec3 normal = glm::normalize(glm::vec3(vx, vy, vz));


		vertexInfo[num].normal_vec += normal;
		vertexInfo[num].normal_count++;
	}
	else if (subwindow_num == 2) {

		glm::vec3 vec2_1, vec3_1;

		vec2_1 = obj_vertices2[vertex_n2] - obj_vertices2[vertex_n1];
		vec3_1 = obj_vertices2[vertex_n3] - obj_vertices2[vertex_n1];

		double vx = vec2_1.y*vec3_1.z - vec2_1.z*vec3_1.y;
		double vy = vec2_1.z*vec3_1.x - vec2_1.x*vec3_1.z;
		double vz = vec2_1.x*vec3_1.y - vec2_1.y*vec3_1.x;
		glm::vec3 normal = glm::normalize(glm::vec3(vx, vy, vz));


		vertexInfo2[num].normal_vec += normal;
		vertexInfo2[num].normal_count++;
	}
};
void calc_sin(int num, int vertex) {
	if (subwindow_num == 1) {
		glm::vec3 normal = vertexInfo[num].normal_vec;
		glm::vec3 vertexV = obj_vertices[num];	//기준 vertex
		glm::vec3 vertexX = obj_vertices[vertex]; //인접한 vertex

		glm::vec3 vertexX_V = vertexX - vertexV;
		double XdotN = vertexX.x*normal.x + vertexX.y*normal.y + vertexX.z*normal.z;
		double VdotN = vertexV.x*normal.x + vertexV.y*normal.y + vertexV.z*normal.z;

		double XdotN_VdotN = abs(XdotN - VdotN);

		double ta = sqrt(vertexX_V.x*vertexX_V.x + vertexX_V.y*vertexX_V.y + vertexX_V.z*vertexX_V.z);
		double tb = XdotN_VdotN;
		double tbta = tb / ta;

		vertexInfo[num].calc_curv += tbta;
		vertexInfo[num].calc_count++;
	}
	else if (subwindow_num == 2) {
		glm::vec3 normal = vertexInfo2[num].normal_vec;
		glm::vec3 vertexV = obj_vertices2[num];	//기준 vertex
		glm::vec3 vertexX = obj_vertices2[vertex]; //인접한 vertex

		glm::vec3 vertexX_V = vertexX - vertexV;
		double XdotN = vertexX.x*normal.x + vertexX.y*normal.y + vertexX.z*normal.z;
		double VdotN = vertexV.x*normal.x + vertexV.y*normal.y + vertexV.z*normal.z;

		double XdotN_VdotN = abs(XdotN - VdotN);

		double ta = sqrt(vertexX_V.x*vertexX_V.x + vertexX_V.y*vertexX_V.y + vertexX_V.z*vertexX_V.z);
		double tb = XdotN_VdotN;
		double tbta = tb / ta;

		vertexInfo2[num].calc_curv += tbta;
		vertexInfo2[num].calc_count++;
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
				int vertex_n2 = vertexIndices[(near_face_num - 1) * 3 +1];
				int vertex_n3 = vertexIndices[(near_face_num - 1) * 3 +2];

				calc_normal(i, vertex_n1, vertex_n2, vertex_n3);
			}

			float normal_count = vertexInfo[i].normal_count;
			vertexInfo[i].normal_vec = glm::normalize(glm::vec3(vertexInfo[i].normal_vec.x / normal_count,
				vertexInfo[i].normal_vec.y / normal_count, vertexInfo[i].normal_vec.z / normal_count));

			//-------------- calc sin --------------
			for (int j = 0; j < near_vertex_count; j++) {
				int k = vertexInfo[i].near_vertex[j];
				calc_sin(i, k);
				
			}
			vertexInfo[i].calc_curv = (vertexInfo[i].calc_curv / vertexInfo[i].calc_count);	
			vertexInfo[i].calc_curv *= 2.;

			aColor.push_back(glm::vec3(1-vertexInfo[i].calc_curv, 1-vertexInfo[i].calc_curv, 1-vertexInfo[i].calc_curv));
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
				calc_sin(i, k);
			}
			vertexInfo2[i].calc_curv = (vertexInfo2[i].calc_curv / vertexInfo2[i].calc_count);
			vertexInfo2[i].calc_curv *= 2.;

			aColor2.push_back(glm::vec3(1-vertexInfo2[i].calc_curv, 1-vertexInfo2[i].calc_curv, 1-vertexInfo2[i].calc_curv));
		}
	}
};
void calc_box_color() {

	bColor.clear();
	int How = HowDrawBox(StartmouseX, StartmouseY, EndmouseX, EndmouseY);

	for (int i = 0; i < vertex_count; i++) {
		vPos = glm::vec4(obj_vertices[i], 1.);
		vPos = realMat * vPos;
		
		switch (How)
		{
		case 1: //우측 상단으로 선택
			if ((StartmouseX <= vPos.x) && (vPos.x <= EndmouseX)) {
				if ((StartmouseY <= vPos.y) && (vPos.y <= EndmouseY)) {
					bColor.push_back(glm::vec3(0.74, 1., 0.));
				}
				else {
					bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
				}
			}
			else {
				bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
			}
			break;
		case 2: //우측 하단으로 선택
			if ((StartmouseX <= vPos.x) && (vPos.x <= EndmouseX)) {
				if ((EndmouseY <= vPos.y) && (vPos.y <= StartmouseY)) {
					bColor.push_back(glm::vec3(0.74, 1., 0.));
				}
				else {
					bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
				}
			}
			else {
				bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
			}
			break;
		case 3: //좌측 상단으로 선택
			if ((EndmouseX <= vPos.x) && (vPos.x <= StartmouseX)) {
				if ((StartmouseY <= vPos.y) && (vPos.y <= EndmouseY)) {
					bColor.push_back(glm::vec3(0.74, 1., 0.));
				}
				else {
					bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
				}
			}
			else {
				bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
			}
			break;
		case 4: //좌측 하단으로 선택
			if ((EndmouseX <= vPos.x) && (vPos.x <= StartmouseX)) {
				if ((EndmouseY <= vPos.y) && (vPos.y <= StartmouseY)) {
					bColor.push_back(glm::vec3(0.74, 1., 0.));
				}
				else {
					bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
				}
			}
			else {
				bColor.push_back(glm::vec3(aColor[i].r, aColor[i].g, aColor[i].b));
			}
			break;
		}
	}
}
void calc_box_color2() {

	bColor2.clear();
	int How = HowDrawBox(StartmouseX2, StartmouseY2, EndmouseX2, EndmouseY2);

	for (int i = 0; i < vertex_count2; i++) {
		vPos2 = glm::vec4(obj_vertices2[i], 1.);
		vPos2 = realMat2 * vPos2;

		switch (How)
		{
		case 1: //우측 상단으로 선택
			if ((StartmouseX2 <= vPos2.x) && (vPos2.x <= EndmouseX2)) {
				if ((StartmouseY2 <= vPos2.y) && (vPos2.y <= EndmouseY2)) {
					bColor2.push_back(glm::vec3(0.74, 1., 0.));
				}
				else {
					bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
				}
			}
			else {
				bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
			}
			break;
		case 2: //우측 하단으로 선택
			if ((StartmouseX2 <= vPos2.x) && (vPos2.x <= EndmouseX2)) {
				if ((EndmouseY2 <= vPos2.y) && (vPos2.y <= StartmouseY2)) {
					bColor2.push_back(glm::vec3(0.74, 1., 0.));
				}
				else {
					bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
				}
			}
			else {
				bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
			}
			break;
		case 3: //좌측 상단으로 선택
			if ((EndmouseX2 <= vPos2.x) && (vPos2.x <= StartmouseX2)) {
				if ((StartmouseY2 <= vPos2.y) && (vPos2.y <= EndmouseY2)) {
					bColor2.push_back(glm::vec3(0.74, 1., 0.));
				}
				else {
					bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
				}
			}
			else {
				bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
			}
			break;
		case 4: //좌측 하단으로 선택
			if ((EndmouseX2 <= vPos2.x) && (vPos2.x <= StartmouseX2)) {
				if ((EndmouseY2 <= vPos2.y) && (vPos2.y <= StartmouseY2)) {
					bColor2.push_back(glm::vec3(0.74, 1., 0.));
				}
				else {
					bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
				}
			}
			else {
				bColor2.push_back(glm::vec3(aColor2[i].r, aColor2[i].g, aColor2[i].b));
			}
			break;
		}
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
		else{
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
	string sub_line = line;
	string word;
	int blank_index;
	glm::vec3 vertex;
	int count = 0;
	while (1) {
		blank_index = line.find(" ");
		if (blank_index != -1) {
			word = line.substr(0, blank_index);
			//cout << word << endl;
			line = line.substr(blank_index + 1);
			if (word == "v") {
				continue;
			}
			if (count == 0) {
				vertex.x = stof(word) * scale;
				count++;
			}
			else if (count == 1) {
				vertex.y = stof(word) * scale;
				count++;
			}
		}
		else {
			blank_index = line.find("\n");
			word = line.substr(0, blank_index);
			//cout << word << endl;
			if (count == 2) {
				vertex.z = stof(word) * scale;
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
				}else{
					obj_texcoord2.push_back(texcoord);
				}
			}
			break;
		}
	}
}
void parseFace(string line, string currentMaterialName) {
	string sub_line[4];
	string word, divide_slash[4];
	int blank_index;
	int word_count = 0;
	while (1) {										//divide blank
		blank_index = line.find(" ");
		if (blank_index != -1) {
			sub_line[word_count] = line.substr(0, blank_index);
			//cout << sub_line[word_count]+" ";
			line = line.substr(blank_index + 1);
			if (sub_line[word_count] == "f") {
				continue;
			}
			else {
				word_count++;
			}
		}
		else {
			blank_index = line.find("\n");
			sub_line[word_count] = line.substr(0, blank_index);
			//cout << sub_line[word_count] << endl;
			break;
		}
	}
	int slash_index;
	if (sub_line[3].length() == 0) {			//3 face
		//cout << "3 face" << endl;

		slash_index = sub_line[0].find("/");	//check "/"
		if (slash_index == -1) {				//not exist "/"
			//cout << "no slash" << endl;
			if (subwindow_num == 1) {
				face_count += 1;
				vertexIndices.push_back(stoi(sub_line[0]) - 1);
				vertexIndices.push_back(stoi(sub_line[1]) - 1);
				vertexIndices.push_back(stoi(sub_line[2]) - 1);
				add_near_vertex_face( stoi(sub_line[0]) - 1, stoi(sub_line[1]) - 1, stoi(sub_line[2]) - 1, face_count);
				
			}
			else {
				face_count2 += 1;
				vertexIndices2.push_back(stoi(sub_line[0]) - 1);
				vertexIndices2.push_back(stoi(sub_line[1]) - 1);
				vertexIndices2.push_back(stoi(sub_line[2]) - 1);
				add_near_vertex_face( stoi(sub_line[0]) - 1, stoi(sub_line[1]) - 1, stoi(sub_line[2]) - 1, face_count2);
			}
		}
		else {
			//cout << "slash" << endl;



			int end_check = 0;
			int j = 0;
			while (end_check == 0) {
				for (int i = 0; i < 3; i++) {
					if (sub_line[i].find("/") != -1) {
						slash_index = sub_line[i].find("/");
						divide_slash[i] = sub_line[i].substr(0, slash_index);
						sub_line[i] = sub_line[i].substr(slash_index + 1);
					}
					else {
						divide_slash[i] = sub_line[i];
						end_check = 1;
					}
				}
				switch (j) {
				case 0:
					if (divide_slash[0].length() != 0) {
						if (subwindow_num == 1) {
							face_count += 1;
							vertexIndices.push_back(stoi(divide_slash[0]) - 1);
							vertexIndices.push_back(stoi(divide_slash[1]) - 1);
							vertexIndices.push_back(stoi(divide_slash[2]) - 1);
							add_near_vertex_face( stoi(divide_slash[0]) - 1, stoi(divide_slash[1]) - 1, stoi(divide_slash[2]) - 1, face_count);
						}
						else {
							face_count2 += 1;
							vertexIndices2.push_back(stoi(divide_slash[0]) - 1);
							vertexIndices2.push_back(stoi(divide_slash[1]) - 1);
							vertexIndices2.push_back(stoi(divide_slash[2]) - 1);
							add_near_vertex_face( stoi(divide_slash[0]) - 1, stoi(divide_slash[1]) - 1, stoi(divide_slash[2]) - 1, face_count2);
						}
					}
					break;
				case 1:
					if (divide_slash[0].length() != 0) {
						if (subwindow_num == 1) {
							texIndices.push_back(stoi(divide_slash[0]) - 1);
							texIndices.push_back(stoi(divide_slash[1]) - 1);
							texIndices.push_back(stoi(divide_slash[2]) - 1);
						}
						else {
							texIndices2.push_back(stoi(divide_slash[0]) - 1);
							texIndices2.push_back(stoi(divide_slash[1]) - 1);
							texIndices2.push_back(stoi(divide_slash[2]) - 1);
						}
					}
					break;
				case 2:
					if (divide_slash[0].length() != 0) {
						if (subwindow_num == 1) {
							normalIndices.push_back(stoi(divide_slash[0]) - 1);
							normalIndices.push_back(stoi(divide_slash[1]) - 1);
							normalIndices.push_back(stoi(divide_slash[2]) - 1);
						}
						else {
							normalIndices2.push_back(stoi(divide_slash[0]) - 1);
							normalIndices2.push_back(stoi(divide_slash[1]) - 1);
							normalIndices2.push_back(stoi(divide_slash[2]) - 1);
						}
					}
					break;
				}
				j++;
			}
		}
	}
	else {										//4 face		(need to change 3 face)
		//cout << "4 face" << endl;

		slash_index = sub_line[0].find("/");
		if (slash_index == -1) {				//not exist "/"
			//cout << "no slash" << endl;
			if (subwindow_num == 1) {
				face_count += 1;
				vertexIndices.push_back(stoi(sub_line[0]) - 1);
				vertexIndices.push_back(stoi(sub_line[1]) - 1);
				vertexIndices.push_back(stoi(sub_line[2]) - 1);
				add_near_vertex_face(stoi(sub_line[0]) - 1, stoi(sub_line[1]) - 1, stoi(sub_line[2]) - 1, face_count);
			}
			else {
				face_count2 += 1;
				vertexIndices2.push_back(stoi(sub_line[0]) - 1);
				vertexIndices2.push_back(stoi(sub_line[1]) - 1);
				vertexIndices2.push_back(stoi(sub_line[2]) - 1);
				add_near_vertex_face(stoi(sub_line[0]) - 1, stoi(sub_line[1]) - 1, stoi(sub_line[2]) - 1, face_count2);
			}

			if (subwindow_num == 1) {
				face_count += 1;
				vertexIndices.push_back(stoi(sub_line[0]) - 1);
				vertexIndices.push_back(stoi(sub_line[2]) - 1);
				vertexIndices.push_back(stoi(sub_line[3]) - 1);
				add_near_vertex_face(stoi(sub_line[0]) - 1, stoi(sub_line[2]) - 1, stoi(sub_line[3]) - 1, face_count);
			}
			else {
				face_count2 += 1;
				vertexIndices2.push_back(stoi(sub_line[0]) - 1);
				vertexIndices2.push_back(stoi(sub_line[2]) - 1);
				vertexIndices2.push_back(stoi(sub_line[3]) - 1);
				add_near_vertex_face(stoi(sub_line[0]) - 1, stoi(sub_line[2]) - 1, stoi(sub_line[3]) - 1, face_count2);
			}
		}
		else {
			//cout << "slash" << endl;

			int j = 0;
			int end_check = 0;
			while (end_check == 0) {
				for (int i = 0; i < 4; i++) {
					if (sub_line[i].find("/") != -1) {
						slash_index = sub_line[i].find("/");
						divide_slash[i] = sub_line[i].substr(0, slash_index);
						sub_line[i] = sub_line[i].substr(slash_index + 1);
						//cout << divide_slash[i].length() << endl;
					}
					else {
						divide_slash[i] = sub_line[i];
						end_check = 1;
					}
				}
				switch (j) {
				case 0:
					if (subwindow_num == 1) {
						face_count += 1;
						vertexIndices.push_back(stoi(divide_slash[0]) - 1);
						vertexIndices.push_back(stoi(divide_slash[1]) - 1);
						vertexIndices.push_back(stoi(divide_slash[2]) - 1);
						add_near_vertex_face(stoi(divide_slash[0]) - 1, stoi(divide_slash[1]) - 1, stoi(divide_slash[2]) - 1, face_count);
					}
					else {
						face_count2 += 1;
						vertexIndices2.push_back(stoi(divide_slash[0]) - 1);
						vertexIndices2.push_back(stoi(divide_slash[1]) - 1);
						vertexIndices2.push_back(stoi(divide_slash[2]) - 1);
						add_near_vertex_face(stoi(divide_slash[0]) - 1, stoi(divide_slash[1]) - 1, stoi(divide_slash[2]) - 1, face_count2);
					}

					if (subwindow_num == 1) {
						face_count += 1;
						vertexIndices.push_back(stoi(divide_slash[0]) - 1);
						vertexIndices.push_back(stoi(divide_slash[2]) - 1);
						vertexIndices.push_back(stoi(divide_slash[3]) - 1);
						add_near_vertex_face(stoi(divide_slash[0]) - 1, stoi(divide_slash[2]) - 1, stoi(divide_slash[3]) - 1, face_count);
					}
					else {
						face_count2 += 1;
						vertexIndices2.push_back(stoi(divide_slash[0]) - 1);
						vertexIndices2.push_back(stoi(divide_slash[2]) - 1);
						vertexIndices2.push_back(stoi(divide_slash[3]) - 1);
						add_near_vertex_face( stoi(divide_slash[0]) - 1, stoi(divide_slash[2]) - 1, stoi(divide_slash[3]) - 1, face_count2);
					}
					break;
				case 1:
					if (subwindow_num == 1) {
						texIndices.push_back(stoi(divide_slash[0]) - 1);
						texIndices.push_back(stoi(divide_slash[1]) - 1);
						texIndices.push_back(stoi(divide_slash[2]) - 1);

						texIndices.push_back(stoi(divide_slash[0]) - 1);
						texIndices.push_back(stoi(divide_slash[2]) - 1);
						texIndices.push_back(stoi(divide_slash[3]) - 1);
					}
					else {
						texIndices2.push_back(stoi(divide_slash[0]) - 1);
						texIndices2.push_back(stoi(divide_slash[1]) - 1);
						texIndices2.push_back(stoi(divide_slash[2]) - 1);

						texIndices2.push_back(stoi(divide_slash[0]) - 1);
						texIndices2.push_back(stoi(divide_slash[2]) - 1);
						texIndices2.push_back(stoi(divide_slash[3]) - 1);
					}
					break;
				case 2:
					if (subwindow_num == 1) {
						normalIndices.push_back(stoi(divide_slash[0]) - 1);
						normalIndices.push_back(stoi(divide_slash[1]) - 1);
						normalIndices.push_back(stoi(divide_slash[2]) - 1);

						normalIndices.push_back(stoi(divide_slash[0]) - 1);
						normalIndices.push_back(stoi(divide_slash[2]) - 1);
						normalIndices.push_back(stoi(divide_slash[3]) - 1);
					}
					else {
						normalIndices2.push_back(stoi(divide_slash[0]) - 1);
						normalIndices2.push_back(stoi(divide_slash[1]) - 1);
						normalIndices2.push_back(stoi(divide_slash[2]) - 1);

						normalIndices2.push_back(stoi(divide_slash[0]) - 1);
						normalIndices2.push_back(stoi(divide_slash[2]) - 1);
						normalIndices2.push_back(stoi(divide_slash[3]) - 1);
					}
					break;
				}
				j++;
			}
		}
	}
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

			if (first_f_check == 0 ) {
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

	while (1) {
		pLine = fgets(aline, 100, fp);
		if (feof(fp)) {
			break;
		}

		line = pLine;
		if (!line.find_first_not_of("\n"))
			line = line.erase(line.find_last_not_of("\n") + 1);

		first_word_index = line.find(" ");

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
	fclose(fp);

}

void updateViewmat() {

	tiltmat = glm::rotate(tiltRad * TO_RADIAN, glm::vec3(1.0f, 0, 0));
	rotmat = glm::rotate(panRad * TO_RADIAN, glm::vec3(0, 1.0f, 0));
	transmat = glm::translate(glm::vec3(transX, transY, transZ));

	viewmat = glm::inverse(transmat) * glm::transpose(tiltmat * rotmat);

	realMat = viewmat * wmat;
	realMat = projectmat * realMat;



	tiltmat2 = glm::rotate(tiltRad2 * TO_RADIAN, glm::vec3(1.0f, 0, 0));
	rotmat2 = glm::rotate(panRad2 * TO_RADIAN, glm::vec3(0, 1.0f, 0));
	transmat2 = glm::translate(glm::vec3(transX2, transY2, transZ2));

	viewmat2 = glm::inverse(transmat2) * glm::transpose(tiltmat2 * rotmat2);

	realMat2 = viewmat2 * wmat2;
	realMat2 = projectmat2 * realMat2;

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


void renderScene(void)
{
		//Clear all pixels
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Let's draw something here

		//define the size of point and draw a point.


		glUseProgram(programID);

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
		glDrawArrays(GL_LINE_LOOP, 0, 4);

		glUseProgram(programID);
		//Double buffer
		glutSwapBuffers();
	
		
}
void renderScene2(void) {
		//Clear all pixels
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Let's draw something here

		//define the size of point and draw a point.


		glUseProgram(programID);

		GLuint lightPosID = glGetUniformLocation(programID, "uLightPos");
		glUniform3fv(lightPosID, 1, &lightPos2[0]);

		GLuint matLoc = glGetUniformLocation(programID, "worldMat");
		glUniformMatrix4fv(matLoc, 1, GL_FALSE, &wmat2[0][0]);

		GLuint viewID = glGetUniformLocation(programID, "viewmat");
		glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewmat2[0][0]);

		GLuint projectID = glGetUniformLocation(programID, "projectmat");
		glUniformMatrix4fv(projectID, 1, GL_FALSE, &projectmat2[0][0]);

		glUniform1i(glGetUniformLocation(programID, "lightTurnOnOff"), lightTurnOnOff2);

		glBindVertexArray(vao[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

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
		glDrawArrays(GL_LINE_LOOP, 0, 4);

		glUseProgram(programID);
		//Double buffer
		glutSwapBuffers();
}
void myKeyboard(unsigned char key, int x, int y) {
		switch (key) {
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

		case '1':				//light move
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

		case 'l':				//light on/off
			if (lightTurnOnOff == 1) {
				lightTurnOnOff = 0;
			}
			else {
				lightTurnOnOff = 1;
			}
			glUniform1i(glGetUniformLocation(programID, "lightTurnOnOff"), lightTurnOnOff);
			break;
		}		
	glutPostRedisplay();
}
void myKeyboard2(unsigned char key, int x, int y) {
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

		case '1':				//light move
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

		case 'l':				//light on/off
			if (lightTurnOnOff2 == 1) {
				lightTurnOnOff2 = 0;
			}
			else {
				lightTurnOnOff2 = 1;
			}
			glUniform1i(glGetUniformLocation(programID, "lightTurnOnOff"), lightTurnOnOff2);
			break;
		}
	glutPostRedisplay();

}
void myMouseClick(GLint Button, GLint State, int x, int y) {
	if ((Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) && Holding == TRUE) { //왼쪽 마우스 버튼을 눌렀을 때, 기준점잡기.
		glUseProgram(programID2);
		glVertexAttrib3f(glGetAttribLocation(programID2, "a_Color"), 1, 0, 0); //빨간 점으로 표시.
		glUseProgram(programID);
		mouseX = x;
		mouseY = y;
		StartmouseX = (float)((x - 0.5 * (WINDOW_WIDTH / 2)) / (0.5 * (WINDOW_WIDTH / 2)));
		StartmouseY = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
	}
	else if ((Button == GLUT_LEFT_BUTTON && State == GLUT_UP) && Holding == TRUE) { //왼쪽 마우스 뗐을 경우, z값 초기화.

		box_vertices[2] = 1;
		box_vertices[5] = 1;
		box_vertices[8] = 1;
		box_vertices[11] = 1;
		EndmouseX = (float)((x - 0.5 * (WINDOW_WIDTH / 2)) / (0.5 * (WINDOW_WIDTH / 2)));
		EndmouseY = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
		calc_box_color();

	}
}
void myMouseClick2(GLint Button, GLint State, int x, int y) {
	if ((Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) && Holding == TRUE) { //왼쪽 마우스 버튼을 눌렀을 때, 기준점잡기.
		glUseProgram(programID2);
		glVertexAttrib3f(glGetAttribLocation(programID2, "a_Color"), 1, 0, 0); //빨간 점으로 표시.
		glUseProgram(programID);
		mouseX2 = x;
		mouseY2 = y;
		StartmouseX2 = (float)((x - 0.5 * (WINDOW_WIDTH/2)) / (0.5 * (WINDOW_WIDTH / 2)));
		StartmouseY2 = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
	}
	else if ((Button == GLUT_LEFT_BUTTON && State == GLUT_UP) && Holding == TRUE) { //왼쪽 마우스 뗐을 경우, z값 초기화.

		box_vertices2[2] = 1;
		box_vertices2[5] = 1;
		box_vertices2[8] = 1;
		box_vertices2[11] = 1;
		EndmouseX2 = (float)((x - 0.5 * (WINDOW_WIDTH / 2)) / (0.5 * (WINDOW_WIDTH / 2)));
		EndmouseY2 = -((float)((y - 0.5 * WINDOW_HEIGHT) / (0.5 * WINDOW_HEIGHT)));
		calc_box_color2();

	}
}
void myMouseDrag(int x, int y) {

	if (Holding == FALSE) { //'/' 안누를 시.
			if (mouseX < x) {	//pan right
				if (mouseY < y) {
					tiltRad -= 0.3f;
				}
				else if (mouseY > y) {
					tiltRad += 0.3f;
				}
				panRad -= 0.3f;
				updateViewmat();
				mouseX = x;
				mouseY = y;
			}
			else if (mouseX > x) {
				if (mouseY < y) {	//tilt up
					tiltRad -= 0.3f;
				}
				else if (mouseY > y) {				//tilt down
					tiltRad += 0.3f;
				}
				panRad += 0.3f;
				updateViewmat();
				mouseX = x;
				mouseY = y;
			}
			else {
				if (mouseY < y) {	//tilt up
					tiltRad -= 0.3f;
				}
				else if (mouseY > y) {				//tilt down
					tiltRad += 0.3f;
				}
				updateViewmat();
				mouseX = x;
				mouseY = y;
			}
		
	}
	else {

		varX = (float)((x - 0.5 * (WINDOW_WIDTH/2)) / (0.5 * (WINDOW_WIDTH/2)));
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

	}

	glutPostRedisplay();
}
void myMouseDrag2(int x, int y) {

	if (Holding == FALSE) { //'/' 안누를 시.

		if (mouseX2 < x) {	//pan right
			if (mouseY2 < y) {
				tiltRad2 -= 0.3f;
			}
			else if (mouseY2 > y) {
				tiltRad2 += 0.3f;
			}
			panRad2 -= 0.3f;
			updateViewmat();
			mouseX2 = x;
			mouseY2 = y;
		}
		else if (mouseX2 > x) {
			if (mouseY2 < y) {	//tilt up
				tiltRad2 -= 0.3f;
			}
			else if (mouseY2 > y) {				//tilt down
				tiltRad2 += 0.3f;
			}
			panRad2 += 0.3f;
			updateViewmat();
			mouseX2 = x;
			mouseY2 = y;
		}
		else {
			if (mouseY2 < y) {	//tilt up
				tiltRad2 -= 0.3f;
			}
			else if (mouseY2 > y) {				//tilt down
				tiltRad2 += 0.3f;
			}
			updateViewmat();
			mouseX2 = x;
			mouseY2 = y;
		}

	}
	else {

		varX2 = (float)((x - 0.5 * (WINDOW_WIDTH/2)) / (0.5 * (WINDOW_WIDTH/2)));
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

	}

	glutPostRedisplay();
}
void MyMouseWheelFunc(int wheel, int direction, int x, int y) {
	if (direction > 0) {		//zoom in
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

	}
	else {					//zoom out
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
	}
	glutPostRedisplay();
}
void MyMouseWheelFunc2(int wheel, int direction, int x, int y) {
	if (direction > 0) {		//zoom in
		if (orthoOn2 == 0) {
			persRad2 -= 1.0f;
			glm::mat4 perspectiveMat = glm::perspective(persRad2 * TO_RADIAN, 1.0f, zNear2, zFar2);
			projectmat2 = perspectiveMat;

			updateViewmat();
		}
		else if (orthoOn == 1) {
			maxX2 -= 0.1f;
			minX2 += 0.1f;
			maxY2 -= 0.1f;
			minY2 += 0.1f;
			orthoMat2 = glm::ortho(minX2, maxX2, minY2, maxY2, zNear2, zFar2);
			projectmat2 = orthoMat2;
		}

	}
	else {					//zoom out
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
	}
	glutPostRedisplay();
}
void init(string file_name, float obj_scale)
{
	if (subwindow_num == 1) {
		cout <<endl<< "========== SubWindow 1 ==========" << endl;
		//initilize the glew and check the errors.
		GLenum res = glewInit();
		if (res != GLEW_OK)
		{
			std::fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
		}

		//select the background color
		glClearColor(1.f, 1.f, 1.f, 1.0);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		loadObj2(file_name, obj_scale);


		calc_color();


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

		// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡBox drawingㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
		glUseProgram(programID2);
		glBindVertexArray(vao[1]);
		glGenBuffers(1, &vbo_box);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_box);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, box_vertices, GL_STATIC_DRAW);

		GLuint vtxPosition2 = glGetAttribLocation(programID2, "vtxPosition");
		glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(vtxPosition);

		//glVertexAttrib3f(glGetAttribLocation(programID2, "a_Color"), 1, 0, 0);
	}
	else if(subwindow_num == 2){													//subwindow 2

		cout << endl<<"========== SubWindow 2	 ==========" << endl;
		//initilize the glew and check the errors.
		GLenum res = glewInit();
		if (res != GLEW_OK)
		{
			std::fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
		}

		//select the background color
		glClearColor(1.f, 1.f, 1.f, 1.0);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		loadObj2(file_name, obj_scale);
		

		calc_color();


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


		tiltmat2 = glm::rotate(tiltRad2 * TO_RADIAN, glm::vec3(1.0f, 0, 0));
		rotmat2 = glm::rotate(panRad2 * TO_RADIAN, glm::vec3(0, 1.0f, 0));
		transmat2 = glm::translate(glm::vec3(transX2, transY2, transZ2));

		viewmat2 = glm::transpose(tiltmat2) * glm::transpose(rotmat2) * glm::inverse(transmat2);

		GLuint viewID = glGetUniformLocation(programID, "viewmat");
		glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewmat2[0][0]);


		glm::mat4 orthoMat = glm::ortho(minX, maxX2, minY2, maxY2, zNear2, zFar2);

		projectmat2 = orthoMat;

		GLuint projectID = glGetUniformLocation(programID, "projectmat");
		glUniformMatrix4fv(projectID, 1, GL_FALSE, &projectmat2[0][0]);

		GLuint lightPosID = glGetUniformLocation(programID, "uLightPos");
		glUniform3fv(lightPosID, 1, &lightPos2[0]);

		glUniform3fv(glGetUniformLocation(programID, "material.diffuse"), 1, &material.MTL_Kd[0]);
		glUniform3fv(glGetUniformLocation(programID, "material.specular"), 1, &material.MTL_Kd[0]);

		glUniform1i(glGetUniformLocation(programID, "lightTurnOnOff"), lightTurnOnOff2);

		// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡBox drawingㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
		glUseProgram(programID2);
		glBindVertexArray(vao[1]);
		glGenBuffers(1, &vbo_box);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_box);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, box_vertices2, GL_STATIC_DRAW);

		GLuint vtxPosition2 = glGetAttribLocation(programID2, "vtxPosition");
		glVertexAttribPointer(vtxPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(0));
		glEnableVertexAttribArray(vtxPosition);

		//glVertexAttrib3f(glGetAttribLocation(programID2, "a_Color"), 1, 0, 0);
	}
	

}
int main(int argc, char** argv)
{

	clock_t start_time, end_time;
	start_time = clock();


	//init GLUT and create Window
	//initialize the GLUT
	glutInit(&argc, argv);
	//GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
	glutInitDisplayMode(/* GLUT_3_2_CORE_PROFILE | */ GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//These two functions are used to define the position and size of the window. 
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//This is used to define the name of the window.
	mainWindow = glutCreateWindow("Simple OpenGL Window");

	//call initization function
	//init();

	//1.
	//Generate VAO
	//3. 

	glutDisplayFunc(renderScene);
	//enter GLUT event processing cycle

	subWindow1 = glutCreateSubWindow(mainWindow, 0,0, WINDOW_WIDTH / 2, WINDOW_HEIGHT);
	subwindow_num = 1;
	first_f_check = 0;
	init(filename,scale);
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouseClick);
	glutMotionFunc(myMouseDrag);
	glutMouseWheelFunc(MyMouseWheelFunc);
	
	
	subWindow2 = glutCreateSubWindow(mainWindow, WINDOW_WIDTH / 2, 0, WINDOW_WIDTH/2, WINDOW_HEIGHT);
	subwindow_num = 2;
	first_f_check = 0;
	init(filename2,scale2);
	glutDisplayFunc(renderScene2);
	glutKeyboardFunc(myKeyboard2);
	glutMouseFunc(myMouseClick2);
	glutMotionFunc(myMouseDrag2);
	glutMouseWheelFunc(MyMouseWheelFunc2);

	end_time = clock();

	double total_t = (double)(end_time - start_time);
	cout << "Time : " << total_t << endl;
	cout << "\t" << total_t / 1000.<<endl;



	glutMainLoop();

	glDeleteVertexArrays(2, vao);

	return 1;
}