#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>

using namespace std;

class TEX {
public:
    const char *dataName[3];
    TEX();
    ~TEX();
    void use();
	string chooseName; // dataName with raw data
    int selectData;
private:
    GLuint texID[3]; // tecture ID
};