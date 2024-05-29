#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
using namespace std;

class TEX {
public:
    TEX(const string&, const glm::vec2&, const float&, const int&, const int&, unsigned char*);
    ~TEX();
    void loadGrid(const string&);
    void creFlow(const glm::vec2&, const float&);
    void creNoise(const int&);
    void loadFlow(const int&);
    void creTRAN(const unsigned char*);
    void update(const string&, const glm::vec2&, const float&, const int&, const int&, unsigned char*);
    void use()const;
    float *speedCnt = NULL;
private:
    glm::vec2 interpolate(const glm::vec2&, const glm::vec2&)const;
    void filter2D(unsigned char* , const glm::vec2&);
    void filter1D(float*, const int&);
    int noiseMethod, filMethod;  // white, spot | box, tent, gausi
    vector<vector<pair<glm::vec2, float>>> flow; // streamline
    vector<vector<glm::vec2>> gridData; // raw data
    unsigned char *noise = NULL; // raw noise texture
    glm::vec2 texSZ, gridSZ; // texture size | raw data size 
    glm::vec2 flowRange; // raw data V range
	string dataName; // dataName with raw data
    GLuint texID[3]; // tecture ID
    int minFlowCNT; // len with texture size per 1 trace pnt
    int filSZ; // filter size
    float h; // delta 
};