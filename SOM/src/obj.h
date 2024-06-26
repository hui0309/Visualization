#include <glad/glad.h>
#include <GLFW/glfw3.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <bits/stdc++.h>
using namespace std;

class OBJ {
public:
    const char *dataName[3];
    int selectData;
    string chooseName;
    struct DATA{
        double x, y, z;
    };
	OBJ();
    ~OBJ();
    glm::mat4 getModel()const;
    void getData();
    void genPnt();
    void som();
    void initSom();
    void update();
	void drawLine();
    void drawPoly();
    void drawNorm();
    int dataSZ, pntSZ, drawSZ, iterSZ, upSZ;
    int flag;
    bool isNorm, isPoly, isTex, isCly;
    vector<DATA> data;
    vector<vector<DATA>> pnt;
    vector<vector<double>> tarDis;
    double accDis, lr, accE, vary;
    glm::vec3 rot, pos, sca;
private:
    double delta = 1e-6;
    GLuint vboID[3], vaoID[3];
};
