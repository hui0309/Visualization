#include "./obj.h"
#define fir first
#define sec second

OBJ::OBJ(): pos(glm::vec3(0.0f)), rot(glm::vec3(0.0f)), sca(glm::vec3(1.0f)), choose(0){
	loadOBJ();
}
void OBJ::loadOBJ(){
	ifstream file("./Scalar/" + dataName[choose], ios::binary);
	if (!file) { cerr << "Something wrong to get " << dataName[choose] << ".\n"; return; }
	// get data[ind] sz
	file.seekg(0, ios::end);
	int sz = file.tellg();
	file.seekg(0, ios::beg);
	vector<char> cha(sz);
	file.read(cha.data(), sz);
	if (file.bad()) { cerr << "something error when reading the data[ind]\n"; return;}
	file.close();
	cerr << "Succuss to read date, file size =  " << sz << " bytes.\n";
	
	vector<vector<vector<double>>> int_data(256, vector<vector<double>>(256, vector<double>(256)));
	vector<vector<vector<glm::vec3>>> gra_data(256, vector<vector<glm::vec3>>(256, vector<glm::vec3>(256)));
	for(int i = 0; i < 256 * 256 * 256; ++i){
        int_data[i % 256][(i / 256) % 256][i / 256 / 256] = cha[i] + 128.0f;
    }
    cerr << "Succuss to get intensity.\n";
	MyTex = new TEX(int_data);

	
	vector<float> quad(6 * 4 * 6);
	for(int i = 0; i < 6; ++i){
		for(int j = 0; j < 4; ++j){
			quad.push_back(vert[tri[i][j]].x);
			quad.push_back(vert[tri[i][j]].y);
			quad.push_back(vert[tri[i][j]].z);
			quad.push_back(MyTex -> vert[tri[i][j]].x);
			quad.push_back(MyTex -> vert[tri[i][j]].y);
			quad.push_back(MyTex -> vert[tri[i][j]].z);
		}
	}
	// using VAO
	glGenVertexArrays(1, &VAOID);
	glBindVertexArray(VAOID);
	//using VBO
	GLuint VBOID;
	glGenBuffers(1, &VBOID);
	glBindBuffer(GL_ARRAY_BUFFER, VBOID);
	glBufferData(GL_ARRAY_BUFFER, quad.size() * sizeof(float), quad.data(), GL_STATIC_DRAW);

	// loc, len, type, norm?,  sz of one part, head
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(30 * sizeof(float)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); 

}

glm::mat4 OBJ::getModel()const{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
	model = glm::rotate(model, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, sca);
	model = glm::translate(model, -pos);
	return model;
}
// void ISO::update(const int& _seleteRot){
// 	float currentTime = glfwGetTime();
// 	float dr = (float)(currentTime - oldTime) * glm::radians(50.0f);
// 	oldTime = currentTime;
// 	if(_seleteRot != 3) rot[_seleteRot] += dr;
// }

glm::vec3 OBJ::getWorldPNT(const bool& big)const{
	return (big? vert[6] : vert[0]);
}

void OBJ::draw()const{
	MyTex -> use();
	glBindVertexArray(VAOID);
	glDrawElements(GL_QUADS, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
