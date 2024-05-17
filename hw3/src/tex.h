#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
using namespace std;

struct SamPnt{
    float *X, *Y;
    SamPnt(){
        X = new float[6];
        Y = new float[6];
    }
};
class TEX {
public:
    TEX(vector<vector<vector<float>>>&);
    ~TEX();
    void creVOLE(const vector<vector<vector<float>>>&);
    void creTRAN();
    int* getColor(const int& choose); 
    SamPnt* getSamPnt(const int& choose); 
    void use()const;
private:
    GLuint texID[2];
    int* newColor(vector<pair<float, float>>&);
    SamPnt *SamR, *SamG, *SamB, *SamA;
    int *ColorR, *ColorG, *ColorB, *ColorA, *Color; 

};