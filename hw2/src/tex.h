#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
using namespace std;

class TEX {
private:
    GLuint texID[2];
    void creVOLE(const vector<vector<vector<double>>>&);
    void creTRAN();
public:
    TEX(vector<vector<vector<double>>>);
    void loadTEX(const vector<vector<vector<double>>>&);
    void use()const;
};