#include "./tex.h"


void TEX::creVOLE(const vector<vector<vector<double>>>& intensity){
    unsigned char ****color;
    glm::vec3 gradient;
    color = new unsigned char***[256];
	for(int x = 0; x < 256; ++x){
        color[x] = new unsigned char**[256];
		for(int y = 0; y < 256; ++y){
            color[x][y] = new unsigned char*[256];
			for(int z = 0; z < 256; ++z){
                color[x][y][z] = new unsigned char[4];
				if(x == 0) gradient.x = intensity[x + 1][y][z] - intensity[x][y][z];
				else if(x == 256 - 1) gradient.x = intensity[x][y][z] - intensity[x - 1][y][z];
				else gradient.x = (intensity[x + 1][y][z] - intensity[x - 1][y][z]) / 2.0;

				if(y == 0) gradient.y = intensity[x][y + 1][z] - intensity[x][y][z];
				else if(y == 256 - 1) gradient.y = intensity[x][y][z] - intensity[x][y - 1][z];
				else gradient.y = (intensity[x][y + 1][z] - intensity[x][y - 1][z]) / 2.0;

				if(z == 0) gradient.z = intensity[x][y][z + 1] - intensity[x][y][z];
				else if(z == 256 - 1) gradient.z = intensity[x][y][z] - intensity[x][y][z - 1];
				else gradient.z = (intensity[x][y][z + 1] - intensity[x][y][z - 1]) / 2.0;
                
				gradient = (glm::normalize(gradient) + 1.0f) / 2.0f * 255.0f;
                color[x][y][z][0] = (int)gradient.x;
                color[x][y][z][1] = (int)gradient.y;
                color[x][y][z][2] = (int)gradient.z;
                color[x][y][z][3] = (int)intensity[x][y][z];
            }
		}
	}
    cerr << "Succuss to caculate gradient.\n";
    
    glBindTexture(GL_TEXTURE_3D, texID[0]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, 256, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, color);
    cerr << "Succuss to generate TEX3D.\n";
    delete[] color;
}
void TEX::creTRAN(){
    unsigned char** color;
    color = new unsigned char*[256];
    for(int i = 0 ; i < 255; ++i){
        color[i] = new unsigned char[4];
        color[i][0] = i;
        color[i][1] = 128;
        color[i][2] = 255 - i;
        color[i][3] = (i % 64? 0 : 100);
    }
    glBindTexture(GL_TEXTURE_1D, texID[1]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, color);
    cerr << "Succuss to generate TEX1D.\n";
    delete[] color;
}
void TEX::loadTEX(const vector<vector<vector<double>>>& intensity){
    glGenTextures(2, texID);
    creVOLE(intensity);
    creTRAN();
}
TEX::TEX(vector<vector<vector<double>>> intensity){
    loadTEX(intensity);
}
void TEX::use()const{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, texID[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, texID[1]);
}
