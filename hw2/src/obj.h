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
 	const float vertices[48] = {
        // obj_pos          // text_pos          
        0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        255.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        255.0f, 255.0f, 0.0f,   1.0f, 1.0f, 0.0f,
        0.0f, 255.0f, 0.0f,   0.0f, 1.0f, 0.0f, 
        0.0f, 0.0f, 255.0f,   0.0f, 0.0f, 1.0f,  
        255.0f, 0.0f, 255.0f,   1.0f, 0.0f, 1.0f,  
        255.0f, 255.0f, 255.0f,   1.0f, 1.0f, 1.0f,   
        0.0f, 255.0f, 255.0f,   0.0f, 1.0f, 1.0f
    };
    const unsigned int indices[36] = {
        0, 1, 2, 2, 3, 0,
        6, 2, 1, 1, 5, 6,
        4, 6, 5, 6, 4, 7,
        3, 7, 0, 0, 7, 4,
        3, 2, 6, 6, 7, 3,
        5, 1, 0, 0, 4, 5 
    };
	int choose;
    const vector<string> dataName = {"mrt16_angio2.raw", "Carp.raw"};
    GLuint VBOID, VAOID, EBOID;
};
