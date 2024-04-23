#include "./tex.h"


void TEX::creVOLE(){
    unsigned char ****color;
    color = new unsigned char***[256];
    for(int x = 0; x < 256; ++x){
        color[x] = new unsigned char**[256];
        for(int y = 0; y < 256; ++y){
            color[x][y] = new unsigned char*[256];
            for(int z = 0; z < 256; ++z){
                color[x][y][z] = new unsigned char[4];
                color[x][y][z][0] = gradient[x][y][z][0];
                color[x][y][z][1] = gradient[x][y][z][1];
                color[x][y][z][2] = gradient[x][y][z][2];
                color[x][y][z][3] = intensity[x][y][z];
            }
        }
    }
    glGenTextures(1, &texID0);
    glBindTexture(GL_TEXTURE_3D, texID0);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_READ_COLOR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_READ_COLOR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_READ_COLOR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, 1, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, color);
    cerr << "Succuss to generate TEX3D.\n";
    delete(color);
}
void TEX::creTRAN(){
    unsigned char color[256 * 4];
    unsigned char tmp[4];
    for(int i = 0 ; i < 255; ++i){
        tmp[3] = i;
        if(i == 0){
            tmp[0] = 128;
            tmp[1] = tmp[2] = 0;
        }
        else if(i < 64){
            tmp[0] = min(255, tmp[0] + 2);
            tmp[1] = min(255, tmp[1] + 2);
        }
        else if(i == 64){
            tmp[0] = 255;
            tmp[1] = min(255, tmp[1] + 2);
        }
        else if(i < 128){
            tmp[0] = max(0, tmp[0] - 4);
            tmp[1] = min(255, tmp[1] + 2);
        }
        else if(i == 128){
            tmp[0] = 0;
            tmp[1] = 255;
        }
        else if(i < 192){
            tmp[1] = max(0, tmp[1] - 2);
            tmp[2] = min(255, tmp[2] + 4);
        }
        else if(i == 192){
            tmp[1] = max(0, tmp[1] - 2);
            tmp[2] = 255.0;
        }
        else if(i < 255){
            tmp[1] = max(0, tmp[1] - 2);
            tmp[2] = max(0, tmp[2] - 1);
        }
        else{
            tmp[1] = 0;
            tmp[2] = 192;
        }
        color[i * 4] = tmp[0];
        color[i * 4 + 1] = tmp[1];
        color[i * 4 + 2] = tmp[2];
        color[i * 4 + 3] = tmp[3];
    }
    glGenTextures(1, &texID1);
    glBindTexture(GL_TEXTURE_1D, texID1);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_READ_COLOR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, color);
    cerr << "Succuss to generate TEX1D.\n";
}
void TEX::loadTEX(){
    creVOLE();
    creTRAN();
}
TEX::TEX(vector<vector<vector<double>>> _intensity): intensity(_intensity){
    vector<vector<vector<glm::vec3>>> gra_data(256, vector<vector<glm::vec3>>(256, vector<glm::vec3>(256)));
    glm::vec3 grid;
	for(int i = 0; i < 256; ++i){
		for(int j = 0; j < 256; ++j){
			for(int k = 0; k < 256; ++k){
				if(i == 0) grid.x = intensity[i + 1][j][k] - intensity[i][j][k];
				else if(i == 256 - 1) grid.x = intensity[i][j][k] - intensity[i - 1][j][k];
				else grid.x = (intensity[i + 1][j][k] - intensity[i - 1][j][k]) / 2.0;
				if(j == 0) grid.y = intensity[i][j + 1][k] - intensity[i][j][k];
				else if(j == 256 - 1) grid.y = intensity[i][j][k] - intensity[i][j - 1][k];
				else grid.y = (intensity[i][j + 1][k] - intensity[i][j - 1][k]) / 2.0;
				if(k == 0) grid.z = intensity[i][j][k + 1] - intensity[i][j][k];
				else if(k == 256 - 1) grid.z = intensity[i][j][k] - intensity[i][j][k - 1];
				else grid.z = (intensity[i][j][k + 1] - intensity[i][j][k - 1]) / 2.0;
				gra_data[i][j][k] = (glm::normalize(grid) + 1.0f) / 2.0f;
			}
		}
	}
    gradient = gra_data;
    cerr << "Succuss to caculate gradient.\n";
    loadTEX();
}
void TEX::use()const{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, texID1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, texID0);
}
