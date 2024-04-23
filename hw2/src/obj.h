#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
#include "./tex.h"
using namespace std;

class OBJ {
public:
	const glm::vec3 vert[8] = {{0.0f, 0.0f, 0.0f}, {256.0f, 0.0f, 0.0f}, {256.0f, 256.0f, 0.0f}, {0.0f, 256.0f, 0.0f},
					{0.0f, 0.0f, 256.0f}, {256.0f, 0.0f, 256.0f}, {256.0f, 256.0f, 256.0f}, {0.0f, 256.0f, 256.0f}};
	const vector<string> dataName = {"mrt16_angio2.raw", "Carp.raw"};
	OBJ();
	void loadOBJ();
	// vector<vector<vector<double>>> getINTE()const;
	glm::mat4 getModel()const;
	glm::vec3 getWorldPNT(const bool&)const;
	string getName()const;

	void draw()const;
private:
	short tri[6][4] {{0, 1, 2, 3}, {0, 3, 6, 7}, {0, 7, 4, 1}, {5, 4, 7, 6}, {5, 2, 1, 4}, {5, 6, 3, 2}};
	int choose;
	glm::vec3 pos, rot, sca;
	TEX *MyTex;
	GLuint VAOID;
};
