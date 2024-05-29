#include "./obj.h"

#define X first
#define Y second
#define fir first
#define sec second
#define INT x
#define DEC y
#define MN x
#define MXN y

// construct OBJ
OBJ::OBJ() : pos(0.0f), rot(0.0f), sca(1.0f){
    vector<float> vertices = {  -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f,
                                -0.5f,  0.5f, 0.5f, -0.5f,  0.5f, 0.5f  };
    glGenVertexArrays(1, &vaoID);
    glGenBuffers(1, &vboID);
    glBindVertexArray(vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    pntSZ = vertices.size() / 2;
}

// deconstruct OBJ
OBJ::~OBJ() {
    // release space
    glDeleteVertexArrays(1, &vaoID);
    glDeleteBuffers(1, &vboID);
}

// get world space mat
glm::mat4 OBJ::getModel() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, sca);
    return model;
}

// draw the obj
void OBJ::draw() const {
    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLES, 0, pntSZ);
    glBindVertexArray(0);
}