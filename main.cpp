
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm> 
#include<glm/glm.hpp>
#include<glm/gtx/transform.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include<GL/wglew.h>
#include <GL/glut.h>
#include<GL/freeglut.h>

#define WINDOWSIZE 600
#define TO_RADIAN 0.01745329252f 

using namespace std;

GLuint programID;
GLuint VertexArrayID;

string filename = "YuSample.obj";
string mtlpath; //mtl 파일명 저장.
float scale = 0.003f;

vector<GLuint>vertexIndices, texIndices, normalIndices;
vector<glm::vec3>obj_vertices;
vector<glm::vec2>obj_texcoord;
vector<glm::vec3>obj_normals;
vector<glm::vec3>aColor;
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
	vector<glm::vec2>near_face;
	vector<int>near_face_2;

	double calc_curv = 0;
	double calc_count = 0;
};
vertex_info*vertexInfo;
int first_f_check = 0; //처음 f를 읽었는지 체크

int vertex_count = 0;
int face_count = 0;


int mouseX = -1;	// 마우스로 시점 이동시 전의 마우스좌표 저장  / 초기 설정 -1
int mouseY = -1;
int windowCheck;

float maxX = 1.0f;
float minX = -1.0f;
float maxY = 1.0f;
float minY = -1.0f;
float panRad = 0.f;
float tiltRad = 0.f;
float transX = 0.f;
float transY = 0.f;
float transZ = 2.0f;
float zNear = 0.01f;
float zFar = 10.0f;
int orthoOn = 1;
int first = 1;
float randRad;
float persRad = 60;
int lightTurnOnOff = 1;

glm::mat4 wmat = glm::mat4(1.0f);
glm::mat4 viewmat = glm::mat4(1.0f);
glm::mat4 tiltmat = glm::mat4(1.0f);
glm::mat4 rotmat = glm::mat4(1.0f);
glm::mat4 transmat = glm::mat4(1.0f);
glm::mat4 orthoMat = glm::mat4(1.0f);
glm::mat4 projectmat = glm::mat4(1.0f);
glm::mat4 normalMat = glm::mat4(1.0f);
glm::vec3 lightPos = glm::vec3(10.0f, 10.0f, 10.0f);

GLuint vbo[2];
GLuint vao;
unsigned int ebo;

void parseMtllib(string line); //mtl 파일명 저장.
void onReadMTLFile(string path);
void parseKd(string line); // mtl파일의 Kd 저장.
void parseKa(string line); // mtl파일의 ka 저장.
void parseKs(string line); // mtl파일의 ks 저장.

void add_near_vertex(int vertex1, int vertex2, int vertex3, int face_num) {
	auto vertex1_begin = vertexInfo[vertex1].near_vertex.begin();
	auto vertex1_end = vertexInfo[vertex1].near_vertex.end();
	vector<int>::iterator iter12 = find(vertex1_begin, vertex1_end, vertex2);
	int vertex12 = distance(vertex1_begin, iter12);
	
	if (vertex12 == vertexInfo[vertex1].near_vertex.size()) {	//기존에 vertex1의 near vertex에 vertex2가 없을때
		vertexInfo[vertex1].near_vertex.push_back(vertex2);
		vertexInfo[vertex1].near_face.push_back(glm::vec2(face_num, -1));
	}
	else {										//있을때
		vertexInfo[vertex1].near_face[vertex12] =
			glm::vec2(vertexInfo[vertex1].near_face[vertex12].x, face_num);
	}

	auto vertex1_begin2 = vertexInfo[vertex1].near_vertex.begin();
	auto vertex1_end2 = vertexInfo[vertex1].near_vertex.end();
	vector<int>::iterator iter13 = find(vertex1_begin2, vertex1_end2, vertex3);
	int vertex13 = distance(vertex1_begin2, iter13);
	if (vertex13 == vertexInfo[vertex1].near_vertex.size()) {	//1 3 x
		vertexInfo[vertex1].near_vertex.push_back(vertex3);
		vertexInfo[vertex1].near_face.push_back(glm::vec2(face_num, -1));
	}
	else {										//있을때
		vertexInfo[vertex1].near_face[vertex13] =
			glm::vec2(vertexInfo[vertex1].near_face[vertex13].x, face_num);
	}



	auto vertex2_begin = vertexInfo[vertex2].near_vertex.begin();
	auto vertex2_end = vertexInfo[vertex2].near_vertex.end();
	vector<int>::iterator iter21 = find(vertex2_begin, vertex2_end, vertex1);
	int vertex21 = distance(vertex2_begin, iter21);

	if (vertex21 == vertexInfo[vertex2].near_vertex.size()) {	//기존에 vertex2의 near vertex에 vertex1가 없을때
		vertexInfo[vertex2].near_vertex.push_back(vertex1);
		vertexInfo[vertex2].near_face.push_back(glm::vec2(face_num, -1));
	}
	else {										//있을때
		vertexInfo[vertex2].near_face[vertex21] =
			glm::vec2(vertexInfo[vertex2].near_face[vertex21].x, face_num);
	}

	auto vertex2_begin2 = vertexInfo[vertex2].near_vertex.begin();
	auto vertex2_end2 = vertexInfo[vertex2].near_vertex.end();
	vector<int>::iterator iter23 = find(vertex2_begin2, vertex2_end2, vertex3);
	int vertex23 = distance(vertex2_begin2, iter23);

	if (vertex23 == vertexInfo[vertex2].near_vertex.size()) {	//2 3 x
		vertexInfo[vertex2].near_vertex.push_back(vertex3);
		vertexInfo[vertex2].near_face.push_back(glm::vec2(face_num, -1));
	}
	else {										//있을때
		vertexInfo[vertex2].near_face[vertex23] =
			glm::vec2(vertexInfo[vertex2].near_face[vertex23].x, face_num);
	}



	auto vertex3_begin = vertexInfo[vertex3].near_vertex.begin();
	auto vertex3_end = vertexInfo[vertex3].near_vertex.end();
	vector<int>::iterator iter31 = find(vertex3_begin, vertex3_end, vertex1);
	int vertex31 = distance(vertex3_begin, iter31);

	if (vertex31 == vertexInfo[vertex3].near_vertex.size()) {	//기존에 vertex3의 near vertex에 vertex1가 없을때
		vertexInfo[vertex3].near_vertex.push_back(vertex1);
		vertexInfo[vertex3].near_face.push_back(glm::vec2(face_num, -1));
	}
	else {										//있을때
		vertexInfo[vertex3].near_face[vertex31] =
			glm::vec2(vertexInfo[vertex3].near_face[vertex31].x, face_num);
	}

	auto vertex3_begin2 = vertexInfo[vertex3].near_vertex.begin();
	auto vertex3_end2 = vertexInfo[vertex3].near_vertex.end();
	vector<int>::iterator iter32 = find(vertex3_begin2, vertex3_end2, vertex2);
	int vertex32 = distance(vertex3_begin2, iter32);

	if (vertex32 == vertexInfo[vertex3].near_vertex.size()) {	//3 2 x
		vertexInfo[vertex3].near_vertex.push_back(vertex2);
		vertexInfo[vertex3].near_face.push_back(glm::vec2(face_num, -1));
	}
	else {										//있을때
		vertexInfo[vertex3].near_face[vertex32] =
			glm::vec2(vertexInfo[vertex3].near_face[vertex32].x, face_num);
	}

};
void calc_normal(int num,int vertex_n1, int vertex_n2, int vertex_n3) {
	glm::vec3 vec2_1, vec3_1;
	//cout << "num : " << num << "  vertex_n1 : " << vertex_n1 << "  vertex_n2 : " << vertex_n2 << "  vertex_n3 : " << vertex_n3 << endl;
	vec2_1 = obj_vertices[vertex_n2] - obj_vertices[vertex_n1];
	vec3_1 = obj_vertices[vertex_n3] - obj_vertices[vertex_n1];
	
	double vx = vec2_1.y*vec3_1.z - vec2_1.z*vec3_1.y;
	double vy = vec2_1.z*vec3_1.x - vec2_1.x*vec3_1.z;
	double vz = vec2_1.x*vec3_1.y - vec2_1.y*vec3_1.x;
	glm::vec3 normal=glm::normalize(glm::vec3(vx,vy,vz));


	vertexInfo[num].normal_vec += normal;
	vertexInfo[num].normal_count++;
};
void calc_sin(int num, int vertex) {
	glm::vec3 normal = vertexInfo[num].normal_vec;
	glm::vec3 vertexV = obj_vertices[num];	//기준 vertex
	glm::vec3 vertexX = obj_vertices[vertex]; //인접한 vertex

	glm::vec3 vertexX_V = vertexX - vertexV;
	double XdotN = vertexX.x*normal.x + vertexX.y*normal.y + vertexX.z*normal.z;
	double VdotN = vertexV.x*normal.x + vertexV.y*normal.y + vertexV.z*normal.z;

	double XdotN_VdotN = abs(XdotN - VdotN);
	
	double ta = sqrt(vertexX_V.x*vertexX_V.x + vertexX_V.y*vertexX_V.y + vertexX_V.z*vertexX_V.z);
	double tb = XdotN_VdotN;
	vertexInfo[num].calc_curv += tb / ta;
	vertexInfo[num].calc_count++;


};
void calc_sin2(int num, int face) {
	glm::vec3 normal = glm::normalize(vertexInfo[num].normal_vec);

	int vertex_n1 = vertexIndices[(face - 1) * 3];
	int vertex_n2 = vertexIndices[(face - 1) * 3 + 1];
	int vertex_n3 = vertexIndices[(face - 1) * 3 + 2];

	glm::vec3 vec2_1, vec3_1;
	vec2_1 = obj_vertices[vertex_n2] - obj_vertices[vertex_n1];
	vec3_1 = obj_vertices[vertex_n3] - obj_vertices[vertex_n1];

	double vx = vec2_1.y*vec3_1.z - vec2_1.z*vec3_1.y;
	double vy = vec2_1.z*vec3_1.x - vec2_1.x*vec3_1.z;
	double vz = vec2_1.x*vec3_1.y - vec2_1.y*vec3_1.x;

	glm::vec3 face_normal = glm::normalize(glm::vec3(vx, vy, vz));

	double normal1 = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
	double normal2 = sqrt(face_normal.x*face_normal.x + face_normal.y*face_normal.y
		+ face_normal.z*face_normal.z);

	



	vertexInfo[num].calc_curv += glm::dot(normal, face_normal) / (normal1*normal2);
	vertexInfo[num].calc_count++;
};
void calc_color() {
	for (int i = 0; i < vertex_count; i++) {
		int near_vertex_count = vertexInfo[i].near_vertex.size();
		vertexInfo[i].near_vertex_count = near_vertex_count;	
		for (int j = 0; j < near_vertex_count; j++) {
			int first_face = (int)vertexInfo[i].near_face[j].x;
			int second_face = (int)vertexInfo[i].near_face[j].y;
			auto vertexInfo_face_begin = vertexInfo[i].near_face_2.begin();
			auto vertexInfo_face_end = vertexInfo[i].near_face_2.end();
			if (first_face != -1 ) {
				vector<int>::iterator first_face_check = find(vertexInfo_face_begin, vertexInfo_face_end, first_face);
				vector<int>::iterator second_face_check = find(vertexInfo_face_begin, vertexInfo_face_end, second_face);
				int check_face_1 = distance(vertexInfo_face_begin, first_face_check);
				int check_face_2 = distance(vertexInfo_face_begin, second_face_check);

				if (check_face_1 == vertexInfo[i].near_face_2.size()) {	//1번째 face가 이미 계산되었는지 확인
					vertexInfo[i].near_face_2.push_back(first_face);

					int vertex_n11 = vertexIndices[(first_face - 1) * 3];
					int vertex_n12 = vertexIndices[(first_face - 1) * 3 + 1];
					int vertex_n13 = vertexIndices[(first_face - 1) * 3 + 2];

					calc_normal(i, vertex_n11, vertex_n12, vertex_n13);

					continue;
				}
				else if(check_face_2 == vertexInfo[i].near_face_2.size() && second_face != -1){
					vertexInfo[i].near_face_2.push_back(second_face);

					int vertex_n11 = vertexIndices[(second_face - 1) * 3];
					int vertex_n12 = vertexIndices[(second_face - 1) * 3 + 1];
					int vertex_n13 = vertexIndices[(second_face - 1) * 3 + 2];

					calc_normal(i, vertex_n11, vertex_n12, vertex_n13);

					continue;
				}
			}
			if (second_face != -1) {
				vector<int>::iterator second_face_check = find(vertexInfo_face_begin, vertexInfo_face_end, second_face);
				int check_face_2 = distance(vertexInfo_face_begin, second_face_check);
				if (check_face_2 == vertexInfo[i].near_face_2.size()) {

					vertexInfo[i].near_face_2.push_back(second_face);

					int vertex_n21 = vertexIndices[(second_face - 1) * 3];
					int vertex_n22 = vertexIndices[(second_face - 1) * 3 + 1];
					int vertex_n23 = vertexIndices[(second_face - 1) * 3 + 2];

					calc_normal(i, vertex_n21, vertex_n22, vertex_n23);
					
					continue;
				}
				
			}
		}
		float normal_count = vertexInfo[i].normal_count;
		vertexInfo[i].normal_vec = glm::normalize(glm::vec3 (vertexInfo[i].normal_vec.x/normal_count ,
			vertexInfo[i].normal_vec.y / normal_count, vertexInfo[i].normal_vec.z / normal_count));

//		for (int j = 0; j < near_vertex_count; j++) {
//			int first_face = (int)vertexInfo[i].near_face[j].x;
//			int second_face = (int)vertexInfo[i].near_face[j].y;
//			
//			calc_sin(i, j);
//			
//		}
		for (int j = 0; j < vertexInfo[i].near_face_2.size(); j++) {
			//cout << (vertexInfo[i].near_face_2[j]) << endl;
			calc_sin2(i, vertexInfo[i].near_face_2[j]);
		}
		//cout << vertexInfo[i].calc_count << endl;
		vertexInfo[i].calc_curv = (vertexInfo[i].calc_curv / vertexInfo[i].calc_count);
		vertexInfo[i].calc_curv = sqrt(1 - vertexInfo[i].calc_curv*vertexInfo[i].calc_curv);
		//int a = int(vertexInfo[i].calc_curv*100);
		vertexInfo[i].calc_curv = vertexInfo[i].calc_curv * 3;
		//cout << vertexInfo[i].calc_curv << endl;
		aColor.push_back(glm::vec3(vertexInfo[i].calc_curv,0.,0.));
	}
};
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
	vertex_count++;
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
				obj_vertices.push_back(vertex);
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
				obj_normals.push_back(normal);
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
				obj_texcoord.push_back(texcoord);
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
			vertexIndices.push_back(stoi(sub_line[0]) - 1);
			vertexIndices.push_back(stoi(sub_line[1]) - 1);
			vertexIndices.push_back(stoi(sub_line[2]) - 1);
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
						face_count += 1;
						vertexIndices.push_back(stoi(divide_slash[0]) - 1);
						vertexIndices.push_back(stoi(divide_slash[1]) - 1);
						vertexIndices.push_back(stoi(divide_slash[2]) - 1);
						add_near_vertex(stoi(divide_slash[0]) - 1, stoi(divide_slash[1]) - 1, stoi(divide_slash[2]) - 1, face_count);
					}
					break;
				case 1:
					if (divide_slash[0].length() != 0) {
						texIndices.push_back(stoi(divide_slash[0]) - 1);
						texIndices.push_back(stoi(divide_slash[1]) - 1);
						texIndices.push_back(stoi(divide_slash[2]) - 1);
					}
					break;
				case 2:
					if (divide_slash[0].length() != 0) {
						normalIndices.push_back(stoi(divide_slash[0]) - 1);
						normalIndices.push_back(stoi(divide_slash[1]) - 1);
						normalIndices.push_back(stoi(divide_slash[2]) - 1);
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
			vertexIndices.push_back(stoi(sub_line[0]) - 1);
			vertexIndices.push_back(stoi(sub_line[1]) - 1);
			vertexIndices.push_back(stoi(sub_line[2]) - 1);

			vertexIndices.push_back(stoi(sub_line[0]) - 1);
			vertexIndices.push_back(stoi(sub_line[2]) - 1);
			vertexIndices.push_back(stoi(sub_line[3]) - 1);
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
					face_count += 1;
					vertexIndices.push_back(stoi(divide_slash[0]) - 1);
					vertexIndices.push_back(stoi(divide_slash[1]) - 1);
					vertexIndices.push_back(stoi(divide_slash[2]) - 1);
					add_near_vertex(stoi(divide_slash[0]) - 1, stoi(divide_slash[1]) - 1, stoi(divide_slash[2]) - 1, face_count);

					face_count += 1;
					vertexIndices.push_back(stoi(divide_slash[0]) - 1);
					vertexIndices.push_back(stoi(divide_slash[2]) - 1);
					vertexIndices.push_back(stoi(divide_slash[3]) - 1);
					add_near_vertex(stoi(divide_slash[0]) - 1, stoi(divide_slash[2]) - 1, stoi(divide_slash[3]) - 1, face_count);
					break;
				case 1:
					texIndices.push_back(stoi(divide_slash[0]) - 1);
					texIndices.push_back(stoi(divide_slash[1]) - 1);
					texIndices.push_back(stoi(divide_slash[2]) - 1);

					texIndices.push_back(stoi(divide_slash[0]) - 1);
					texIndices.push_back(stoi(divide_slash[2]) - 1);
					texIndices.push_back(stoi(divide_slash[3]) - 1);
					break;
				case 2:
					normalIndices.push_back(stoi(divide_slash[0]) - 1);
					normalIndices.push_back(stoi(divide_slash[1]) - 1);
					normalIndices.push_back(stoi(divide_slash[2]) - 1);

					normalIndices.push_back(stoi(divide_slash[0]) - 1);
					normalIndices.push_back(stoi(divide_slash[2]) - 1);
					normalIndices.push_back(stoi(divide_slash[3]) - 1);
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

			if (first_f_check == 0) {
				vertexInfo = new vertex_info[vertex_count];
				first_f_check = 1;
			}
			parseFace(line, currentMaterialName);
		}
	}
}

void updateViewmat() {
	tiltmat = glm::rotate(tiltRad * TO_RADIAN, glm::vec3(1.0f, 0, 0));
	rotmat = glm::rotate(panRad * TO_RADIAN, glm::vec3(0, 1.0f, 0));
	transmat = glm::translate(glm::vec3(transX, transY, transZ));

	viewmat = glm::inverse(transmat) * glm::transpose(tiltmat * rotmat);
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
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

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
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	//Link the program
	fprintf(stdout, "Linking program\n");
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
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

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

	GLuint lightPosID = glGetUniformLocation(programID, "uLightPos");
	glUniform3fv(lightPosID, 1, &lightPos[0]);

	GLuint matLoc = glGetUniformLocation(programID, "worldMat");
	glUniformMatrix4fv(matLoc, 1, GL_FALSE, &wmat[0][0]);

	GLuint viewID = glGetUniformLocation(programID, "viewmat");
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewmat[0][0]);

	GLuint projectID = glGetUniformLocation(programID, "projectmat");
	glUniformMatrix4fv(projectID, 1, GL_FALSE, &projectmat[0][0]);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glDrawElements(GL_TRIANGLES, vertexIndices.size(), GL_UNSIGNED_INT, (void*)0);
	//glDrawArrays(GL_POINTS, 0, obj_vertices.size());

	//Double buffer
	glutSwapBuffers();
}
void myKeyboard(unsigned char key, int x, int y) {
	switch (key) {
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
		else if (orthoOn = 1) {
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
void myMouse(int x, int y) {
	if (mouseX == -1) {	//마우스 위치 초기화
		mouseX = x;
	}
	if (mouseY == -1) {
		mouseY = y;
	}

	if (mouseX < x) {	//pan right
		panRad -= 0.3f;
		updateViewmat();
		mouseX = x;
	}
	else {				//pan left
		panRad += 0.3f;
		updateViewmat();
		mouseX = x;
	}
	if (mouseY < y) {	//tilt up
		tiltRad -= 0.3f;
		mouseY = y;
	}
	else {				//tilt down
		tiltRad += 0.3f;
		mouseY = y;
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
		else if (orthoOn = 1) {
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
void init()
{
	//initilize the glew and check the errors.
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
	}

	//select the background color
	glClearColor(1.f, 1.f, 1.f, 1.0);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	loadObj(filename, scale);
	
	calc_color();

	int a = 0;
	for (int i = 0; i < vertexInfo[a].near_vertex.size(); i++) {
		cout << vertexInfo[a].near_vertex[i] << "  " << vertexInfo[a].near_face[i].x << "  " << vertexInfo[a].near_face[i].y << endl;
	}
	cout << endl;
	cout << vertexInfo[a].near_vertex_count <<"  "<<vertexInfo[a].normal_count<<"  "<<vertexInfo[a].calc_count<< endl;
	for (int i = 0; i < vertexInfo[a].near_face_2.size(); i++) {
		cout << vertexInfo[a].near_face_2[i] << endl;
	}

	programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
	glUseProgram(programID);

	glGenVertexArrays(1, &vao);

	glGenBuffers(2, vbo);

	glBindVertexArray(vao);

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
}
int main(int argc, char** argv)
{
	//init GLUT and create Window
	//initialize the GLUT
	glutInit(&argc, argv);
	//GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
	glutInitDisplayMode(/* GLUT_3_2_CORE_PROFILE | */ GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//These two functions are used to define the position and size of the window. 
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
	//This is used to define the name of the window.
	glutCreateWindow("Simple OpenGL Window");

	//call initization function
	init();

	//1.
	//Generate VAO
	//3. 

	glutDisplayFunc(renderScene);
	//enter GLUT event processing cycle
	glutKeyboardFunc(myKeyboard);
	glutMotionFunc(myMouse);
	glutMouseWheelFunc(MyMouseWheelFunc);

	glutMainLoop();

	glDeleteVertexArrays(1, &vao);

	return 1;
}