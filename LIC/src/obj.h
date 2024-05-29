#include <glad/glad.h>
#include <GLFW/glfw3.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <bits/stdc++.h>
using namespace std;

class OBJ {
public:
	OBJ();
    ~OBJ();
    glm::mat4 getModel()const;
	void draw()const;
private:
    glm::vec3 rot, pos, sca;
    GLuint vboID, vaoID;
    int pntSZ;
};
