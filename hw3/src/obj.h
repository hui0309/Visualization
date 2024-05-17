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
	OBJ(const unsigned int&, const unsigned int&, const unsigned int&, const double&);
    ~OBJ();
    void loadGrid(const unsigned int&, const unsigned int&, const unsigned int&, const double&);
    void loadIMG(const unsigned int&, const unsigned int&, const double&);
    void loadFlow(const double&);
    glm::mat4 getModel()const;
    float* getCnt()const;
    int getChoose()const;
    string getDataName()const;
	void draw()const;
	TEX *MyTex = nullptr;
private:
    unsigned int GridX, GridY;
    unsigned int IMGX, IMGY;
    double h;
    glm::vec3 rot, pos, sca;
    vector<vector<double>> GridData;
    vector<vector<double>> IMGData;
    vector<vector<double>> IMGGrad;
	int choose;
    float *valCnt;
    int dataSZ = 23;
    // const vector<string> dataName = {"aneurism", "BluntFin", "Body_SPECT", "bonsai", "Carp", "foot", 
    //                                 // "golfball", "linearaneurism", "mrt16_angio2", "skull", "testing_engine"};
    GLuint vboID, vaoID, eboID;
};
