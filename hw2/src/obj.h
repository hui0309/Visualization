#include <glad/glad.h>
#include <GLFW/glfw3.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <bits/stdc++.h>
using namespace std;

#include "./tex.h"

class OBJ {
public:
	OBJ();
    ~OBJ();
    void loadOBJ();
    glm::mat4 getModel()const;
	glm::vec3 getBoard(const bool&)const;
	void draw()const;
	TEX *MyTex;
private:
    glm::vec3 rot, pos, sca;
    const float vertices[8 * 6] = {
        -127.5f, -127.5f, -127.5f, 0.0f, 0.0f, 0.0f,
        127.5f, -127.5f, -127.5f, 1.0f, 0.0f, 0.0f,
        127.5f, 127.5f, -127.5f, 1.0f, 1.0f, 0.0f,
        -127.5f, 127.5f, -127.5f, 0.0f, 1.0f, 0.0f,
        -127.5f, -127.5f, 127.5f, 0.0f, 0.0f, 1.0f,
        127.5f, -127.5f, 127.5f, 1.0f, 0.0f, 1.0f,
        127.5f, 127.5f, 127.5f, 1.0f, 1.0f, 1.0f,
        -127.5f, 127.5f, 127.5f, 0.0f, 1.0f, 1.0f
    };
    const unsigned int indices[12 * 3] = {
        5, 6, 4, 4, 6, 7,
        1, 0, 2, 2, 0, 3,
        0, 4, 3, 3, 4, 7,
        1, 2, 5, 5, 2, 6,
        6, 2, 7, 7, 2, 3,
        5, 4, 1, 1, 4, 0,
    };
	int choose;
    const vector<string> dataName = {"testing_engine.raw", "Carp.raw", "golfball.raw", "bonsai.raw"};
    GLuint VBOID, VAOID, EBOID;
};
