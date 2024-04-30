#include "./iso.h"
#include "./MarchingCubesTables.hpp"
#define fir first
#define sec second

void SURF::initSURF(const int& _sz, const int& _isoVal){
	sz = _sz; isoVal = _isoVal;
}
void SURF::loadSURF(const vector<float>& tmp){
//using VAO
	glGenVertexArrays(1, &VAOID);
	glBindVertexArray(VAOID);
	// //using VBO
	GLuint VBOID;
	glGenBuffers(1, &VBOID);
	glBindBuffer(GL_ARRAY_BUFFER, VBOID);
	glBufferData(GL_ARRAY_BUFFER, tmp.size() * sizeof(float), tmp.data(), GL_STATIC_DRAW);

	//loc, len, type, norm?,  sz of one part, head
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
 
	glBindVertexArray(0); 
}
void SURF::draw()const{
	//cerr << "start to draw\n";
	// glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this -> VAOID);
	glDrawArrays(GL_TRIANGLES, 0, sz);
	glBindVertexArray(0);   //解绑VAO
	//cerr << "succuss to draw";
}


ISO::ISO(): pos(glm::vec3(0.0f)), rot(glm::vec3(0.0f)), sca(glm::vec3(1.0f / 512.0f)){
	ins.push_back({0, 1}); ins.push_back({1, 2}); ins.push_back({2, 3}); ins.push_back({0, 3});
	ins.push_back({4, 5}); ins.push_back({5, 6}); ins.push_back({6, 7}); ins.push_back({4, 7});
	ins.push_back({0, 4}); ins.push_back({1, 5}); ins.push_back({2, 6});ins.push_back({3, 7});
}
glm::vec3 ISO::getINTE(const double& isovalue, const glm::vec3& pos1, const glm::vec3& pos2, const double& val1, const double& val2)const{
	if (fabs(val1 - val2) < 1e-5) return pos1;
	glm::vec3 TarPos = pos1 + (pos2 - pos1) * (float)((isovalue - val1) / (val2 - val1));
	return TarPos;
}
void ISO::getTRIA(const GRIDCELL& cell, const double& isovalue){
	int triindex = 0;
	// caculate insection ways
	for(int i = 0; i < 8; ++i)
		if (cell.val[i] < isovalue) triindex |= (1 << i);

	array<short, 16> kTri = cube::kCubeTriangles[triindex];
	for(int i = 0; kTri[i] != -1; i += 3){
		triangles.push_back({
			getINTE(isovalue, cell.pos[ins[kTri[i]].fir], cell.pos[ins[kTri[i]].sec], cell.val[ins[kTri[i]].fir], cell.val[ins[kTri[i]].sec]),
			getINTE(isovalue, cell.pos[ins[kTri[i + 1]].fir], cell.pos[ins[kTri[i + 1]].sec], cell.val[ins[kTri[i + 1]].fir], cell.val[ins[kTri[i + 1]].sec]),
			getINTE(isovalue, cell.pos[ins[kTri[i + 2]].fir], cell.pos[ins[kTri[i + 2]].sec], cell.val[ins[kTri[i + 2]].fir], cell.val[ins[kTri[i + 2]].sec]),
			glm::normalize(getINTE(isovalue, cell.grid[ins[kTri[i]].fir], cell.grid[ins[kTri[i]].sec], cell.val[ins[kTri[i]].fir], cell.val[ins[kTri[i]].sec])),
			glm::normalize(getINTE(isovalue, cell.grid[ins[kTri[i + 1]].fir], cell.grid[ins[kTri[i + 1]].sec], cell.val[ins[kTri[i + 1]].fir], cell.val[ins[kTri[i + 1]].sec])),
			glm::normalize(getINTE(isovalue, cell.grid[ins[kTri[i + 2]].fir], cell.grid[ins[kTri[i + 2]].sec], cell.val[ins[kTri[i + 2]].fir], cell.val[ins[kTri[i + 2]].sec]))
		});
	}
}
void ISO::initISO(const int& ind, const string& path){
	ifstream file("./Scalar/" + path, ios::binary);
	if (!file) { cerr << "something wrong to get " << path << "\n"; return; }
	// get data[ind] sz
	file.seekg(0, ios::end);
	int sz = file.tellg();
	file.seekg(0, ios::beg);
	vector<char> cha(sz);
	file.read(cha.data(), sz);
	if (file.bad()) { cerr << "something error when reading the data[ind]\n"; return;}
	file.close();
	cerr << "succuss to read date\n";
	cerr << "file size =  " << sz << " bytes\n";
	
	
	vector<vector<vector<double>>> raw_data(256, vector<vector<double>>(256, vector<double>(256)));
	vector<vector<vector<glm::vec3>>> gri_data(256, vector<vector<glm::vec3>>(256, vector<glm::vec3>(256)));
	dataOffe[ind] = {(double)INT_MIN, (double)INT_MAX};
	for(int z = 0; z < 256; ++z){
		for(int y = 0; y < 256; ++y){
			for(int x = 0; x < 256; ++x){
				raw_data[x][y][z] = cha[(z * 256 + y) * 256 + x];
				dataOffe[ind].fir = max(dataOffe[ind].fir, (double)raw_data[x][y][z]);
				dataOffe[ind].sec = min(dataOffe[ind].sec, (double)raw_data[x][y][z]);
			}
		}
	}
	printf("%.3lf, %.3lf\n", dataOffe[ind].fir, dataOffe[ind].sec);
	dataOffe[ind].fir -= dataOffe[ind].sec; ++dataOffe[ind].fir;
	data[ind] = new float[(int)dataOffe[ind].fir];
	memset(data[ind], 0, sizeof(float) * (int)dataOffe[ind].fir);
	for(int z = 0; z < 256; ++z)
		for(int y = 0; y < 256; ++y)
			for(int x = 0; x < 256; ++x)
				++data[ind][(int)(raw_data[x][y][z] - dataOffe[ind].sec)];
	glm::vec3 grid;
	for(int i = 0; i < 256; ++i){
		for(int j = 0; j < 256; ++j){
			for(int k = 0; k < 256; ++k){
				if(i == 0) grid.x = raw_data[i + 1][j][k] - raw_data[i][j][k];
				else if(i == 256 - 1) grid.x = raw_data[i][j][k] - raw_data[i - 1][j][k];
				else grid.x = (raw_data[i + 1][j][k] - raw_data[i - 1][j][k]) / 2.0;
				if(j == 0) grid.y = raw_data[i][j + 1][k] - raw_data[i][j][k];
				else if(j == 256 - 1) grid.y = raw_data[i][j][k] - raw_data[i][j - 1][k];
				else grid.y = (raw_data[i][j + 1][k] - raw_data[i][j - 1][k]) / 2.0;
				if(k == 0) grid.z = raw_data[i][j][k + 1] - raw_data[i][j][k];
				else if(k == 256 - 1) grid.z = raw_data[i][j][k] - raw_data[i][j][k - 1];
				else grid.z = (raw_data[i][j][k + 1] - raw_data[i][j][k - 1]) / 2.0;
				gri_data[i][j][k] = glm::normalize(grid);
			}
		}
	}

	double isolevel[4] = {-5.0, 20.0, 40.0, 70.0};
	for(int v = 0; v < 4; ++v){
		for(int i = 0; i < 256 - 1; ++i){
			for(int j = 0; j < 256 - 1; ++j){
				for(int k = 0; k < 256 - 1; ++k){
					GRIDCELL cell;
					cell.pos[0] = {(float)i, (float)j, (float)k};
					cell.grid[0] = gri_data[i][j][k];
					cell.pos[1] = {(float)i + 1, (float)j, (float)k};
					cell.grid[1] = gri_data[i + 1][j][k];
					cell.pos[2] = {(float)i + 1, (float)j + 1, (float)k};
					cell.grid[2] = gri_data[i + 1][j + 1][k];
					cell.pos[3] = {(float)i, (float)j + 1, (float)k};
					cell.grid[3] = gri_data[i][j + 1][k];
					cell.pos[4] = {(float)i, (float)j, (float)k + 1};
					cell.grid[4] = gri_data[i][j][k + 1];
					cell.pos[5] = {(float)i + 1, (float)j, (float)k + 1};
					cell.grid[5] = gri_data[i + 1][j][k + 1];
					cell.pos[6] = {(float)i + 1, (float)j + 1, (float)k + 1};
					cell.grid[6] = gri_data[i + 1][j + 1][k + 1];
					cell.pos[7] = {(float)i, (float)j + 1, (float)k + 1};
					cell.grid[7] = gri_data[i][j + 1][k + 1];
					for(int l = 0; l < 8; ++l)
						cell.val[l] = raw_data[cell.pos[l].x][cell.pos[l].y][cell.pos[l].z];
					getTRIA(cell, isolevel[v]);
				}
			}
		}
		loadISO(ind * 4 + v, isolevel[v]);
		triangles.clear();
	}
	raw_data.clear();
	gri_data.clear();
	oldTime = (float)glfwGetTime();
}
void ISO::loadISO(const int& vid, const int& val){
	vector<float> tmp;
	if(triangles.size()){
		for(const auto& tri : triangles){
			tmp.push_back(tri.pos[0].x - 126.0f); tmp.push_back(tri.pos[0].y - 126.0f); tmp.push_back(tri.pos[0].z - 126.0f);
			tmp.push_back(tri.grid[0].x); tmp.push_back(tri.grid[0].y); tmp.push_back(tri.grid[0].z);
			tmp.push_back(tri.pos[1].x - 126.0f); tmp.push_back(tri.pos[1].y - 126.0f); tmp.push_back(tri.pos[1].z - 126.0f);
			tmp.push_back(tri.grid[1].x); tmp.push_back(tri.grid[1].y); tmp.push_back(tri.grid[1].z);
			tmp.push_back(tri.pos[2].x - 126.0f); tmp.push_back(tri.pos[2].y - 126.0f); tmp.push_back(tri.pos[2].z - 126.0f);
			tmp.push_back(tri.grid[2].x); tmp.push_back(tri.grid[2].y); tmp.push_back(tri.grid[2].z);
		}
		cerr << "isolevel = " << val << ", create " << triangles.size() << " triangles.\n";
	}
	Surf[vid].initSURF(tmp.size() >> 1, val);
	Surf[vid].loadSURF(tmp);
	cerr << "succuss to load\n";
	tmp.clear();
}

glm::mat4 ISO::getModel()const{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
	model = glm::rotate(model, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, sca);
	return model;
}
void ISO::update(const int& _seleteRot){
	float currentTime = glfwGetTime();
	float dr = (float)(currentTime - oldTime) * glm::radians(50.0f);
	oldTime = currentTime;
	if(_seleteRot != 3) rot[_seleteRot] += dr;
}

void ISO::draw(const int& vid)const{
	Surf[vid].draw();
}
