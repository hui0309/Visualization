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
    const float vertices[6 * 36] = {
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        255.0f, 255.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        255.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        255.0f, 255.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 255.0f, 0.0f, 0.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 255.0f, 0.0f, 0.0f, 0.0f,
        255.0f, 0.0f, 255.0f, 1.0f, 0.0f, 0.0f,
        255.0f, 255.0f, 255.0f, 1.0f, 0.0f, 1.0f,

        255.0f, 255.0f, 255.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 255.0f, 255.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 255.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 255.0f, 255.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 255.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 255.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 255.0f, 255.0f, 0.0f, 0.0f, 1.0f,

        255.0f, 255.0f, 255.0f, 1.0f, 0.0f, 1.0f,
        255.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        255.0f, 255.0f, 0.0f, 1.0f, 1.0f, 1.0f,

        255.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        255.0f, 255.0f, 255.0f, 1.0f, 0.0f, 1.0f,
        255.0f, 0.0f, 255.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        255.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        255.0f, 0.0f, 255.0f, 1.0f, 0.0f, 0.0f,

        255.0f, 0.0f, 255.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 255.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 255.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        255.0f, 255.0f, 255.0f, 1.0f, 0.0f, 1.0f,
        255.0f, 255.0f, 0.0f, 1.0f, 1.0f, 1.0f,

        255.0f, 255.0f, 255.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 255.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 255.0f, 255.0f, 0.0f, 0.0f, 1.0f
    };
	int choose;
    const vector<string> dataName = {"mrt16_angio2.raw", "Carp.raw"};
    GLuint VBOID, VAOID;
};
