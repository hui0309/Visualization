#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
using namespace std;

class TEX {
private:
    GLuint texID0, texID1;
    vector<vector<vector<double>>> intensity;
    vector<vector<vector<glm::vec3>>> gradient;
    void creVOLE();
    void creTRAN();
public:
    TEX(vector<vector<vector<double>>>);
    const glm::vec3 vert[8] = {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
                    {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}};
    void loadTEX();
    void use()const;
};