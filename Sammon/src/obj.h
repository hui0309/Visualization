#include <glad/glad.h>
#include <GLFW/glfw3.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <bits/stdc++.h>
using namespace std;

class OBJ {
public:
    struct DATA{
        vector<double> attribute;
        bool iserror;
    };
    pair<pair<double, double>, double> priAxisT, priAxisF;
    pair<double, double> clyCenT, clyCenF;
	OBJ();
    ~OBJ();
    glm::mat4 getModel()const;
    void genPnt();
    int sammon();
    void pca();
    pair<int, int> jacobi(const vector<vector<double>>&, const int&,const int&);
    void update_P(vector<vector<double>>&, const pair<pair<int, int>, double>&, const double&, const int&);
    void update_A(vector<vector<double>>&, const pair<pair<int, int>, double>&, const double&, const int&);
    pair<pair<int, int>, double> max_off_diag_entry(vector<vector<double>>&, const int&);
    void update();
	void drawPnt(const float*, const float*);
	void drawCly(const float*, const float*);
    int dataSZ, pntSZ, drawSZ, attSZ, iterSZ;
    pair<int, int> cnt;
    int flag;
    vector<DATA> data;
    vector<vector<double>> tarDis;
    vector<pair<double, double>> samPos;
    double accDis, lr, accE, vary;
private:
    double delta = 1e-9;
    glm::vec3 rot, pos, sca;
    GLuint vboID[2], vaoID[2];
};
