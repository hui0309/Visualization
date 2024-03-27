#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
using namespace std;

class SURF{
public:
	// SURF();
	void draw()const;
	void initSURF(const int&, const int&);
	void loadSURF(const vector<float>&);
private:
	int sz, isoVal;
	GLuint VAOID;
};
class ISO {
public:
	ISO();
	void initISO(const int&, const string&);
	glm::mat4 getModel()const;
	void update(const int&);
	void draw(const int&)const;
	float* data[2];
	pair<double, double> dataOffe[2];
private:
	SURF Surf[8];
	// struct XYZ {float x, y, z;};
	struct TRIANGLE{ glm::vec3 pos[3]; glm::vec3 grid[3];};
	struct GRIDCELL{ glm::vec3 pos[8]; glm::vec3 grid[8]; double val[8];};
	vector<pair<short, short>> ins;
	vector<TRIANGLE> triangles;
	glm::vec3 pos, rot, sca;
	int seletRot = 0;
	float oldTime;
	void loadISO(const int&, const int&);
	void getTRIA(const GRIDCELL&, const double&);
	glm::vec3 getINTE(const double&, const glm::vec3&, const glm::vec3&, const double&, const double&)const;
};