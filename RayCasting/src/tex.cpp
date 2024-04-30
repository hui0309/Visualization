#include "./tex.h"

void TEX::creVOLE(const vector<vector<vector<float>>>& intensity){
    unsigned char (*texpos)[4] = new unsigned char[256 * 256 * 256][4];
	for(int x = 0, index = 0; x < 256; ++x){
		for(int y = 0; y < 256; ++y){
			for(int z = 0; z < 256; ++z){
                glm::vec3 gradient;
				if(x == 0) gradient.x = intensity[x + 1][y][z] - intensity[x][y][z];
				else if(x == 255) gradient.x = intensity[x][y][z] - intensity[x - 1][y][z];
				else gradient.x = (intensity[x + 1][y][z] - intensity[x - 1][y][z]) / 2.0;

				if(y == 0) gradient.y = intensity[x][y + 1][z] - intensity[x][y][z];
				else if(y == 255) gradient.y = intensity[x][y][z] - intensity[x][y - 1][z];
				else gradient.y = (intensity[x][y + 1][z] - intensity[x][y - 1][z]) / 2.0;

				if(z == 0) gradient.z = intensity[x][y][z + 1] - intensity[x][y][z];
				else if(z == 255) gradient.z = intensity[x][y][z] - intensity[x][y][z - 1];
				else gradient.z = (intensity[x][y][z + 1] - intensity[x][y][z - 1]) / 2.0;
                
				if(glm::length(gradient))
                    gradient = (glm::normalize(gradient) + 1.0f) / 2.0f * 255.0f;
                texpos[index][0] = gradient.x;
                texpos[index][1] = gradient.y;
                texpos[index][2] = gradient.z;
                texpos[index][3] = intensity[x][y][z];
                ++index;
            }
		}
	}
    glGenTextures(1, &texID[0]);
    glBindTexture(GL_TEXTURE_3D, texID[0]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, 256, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texpos);
    cerr << "Succuss to generate TEX3D.\n";
    // delete[] texpos;
}
void TEX::creTRAN(){
    unsigned char (*color)[4] = new unsigned char[256][4];

    vector<pair<float, float>> tmpR, tmpG, tmpB, tmpA;
    set<int> isSam;

    //PreProcess sample pnt to poly
    tmpR.push_back({0, 0});
    isSam.clear(); isSam.insert(0);
    for(int i = 0; i < 6; ++i){
        if(!isSam.count(SamR -> X[i])){
            isSam.insert(SamR -> X[i]);
            tmpR.push_back({SamR -> X[i], SamR -> Y[i]});
        }
    }
    ColorR = newColor(tmpR);

    tmpG.push_back({0, 0});
    isSam.clear(); isSam.insert(0);
    for(int i = 0; i < 6; ++i){
        if(!isSam.count(SamG -> X[i])){
            isSam.insert(SamG -> X[i]);
            tmpG.push_back({SamG -> X[i], SamG -> Y[i]});
        }
    }
    ColorG = newColor(tmpG);

    tmpB.push_back({0, 0});
    isSam.clear(); isSam.insert(0);
    for(int i = 0; i < 6; ++i){
        if(!isSam.count(SamB -> X[i])){
            isSam.insert(SamB -> X[i]);
            tmpB.push_back({SamB -> X[i], SamB -> Y[i]});
        }
    }
    ColorB = newColor(tmpB);

    tmpA.push_back({0, 0});
    isSam.clear(); isSam.insert(0);
    for(int i = 0; i < 6; ++i){
        if(!isSam.count(SamA -> X[i])){
            isSam.insert(SamA -> X[i]);
            tmpA.push_back({SamA -> X[i], SamA -> Y[i]});
        }
    }
    ColorA = newColor(tmpA);

    for(int i = 0; i < 256; ++i){
        color[i][0] = ColorR[i];
        color[i][1] = ColorG[i];
        color[i][2] = ColorB[i];
        color[i][3] = ColorA[i];

    }
    glGenTextures(1, &texID[1]);
    glBindTexture(GL_TEXTURE_1D, texID[1]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, color);
}
TEX::TEX(vector<vector<vector<float>>>& intensity){
    creVOLE(intensity);
    SamR = new SamPnt;
    SamG = new SamPnt;
    SamB = new SamPnt;
    SamA = new SamPnt;
    SamR -> X[0] = 110.0f; SamR -> Y[0] = 60.0f;
    SamG -> X[0] = 128.0f; SamG -> Y[0] = 100.0f;
    SamB -> X[0] = 140.0f; SamB -> Y[0] = 200.0f;
    SamA -> X[0] = 128.0f; SamA -> Y[0] = 60.0f;
    for(int i = 1; i < 6; ++i){
        SamR -> X[i] = SamR -> Y[i] = 0.0;
        SamG -> X[i] = SamG -> Y[i] = 0.0;
        SamB -> X[i] = SamB -> Y[i] = 0.0;
        SamA -> X[i] = SamA -> Y[i] = 0.0;
    }
    creTRAN();
    Color = new int[256];
    for(int i = 0; i < 256; ++i) Color[i] = i;
    cerr << "success to load tex";
}
int* TEX::newColor(vector<pair<float, float>>& samPnt){
    int *color = new int[256];
    vector<vector<double>> _toget(256);
    for(const auto& pnt : samPnt){  
        _toget[pnt.first].push_back(pnt.second);
        if(pnt.first){
            for(int i = 1, value = pnt.second; i <= 20; ++i){
                value = value * (20 - i) / (20 - i + 1);
                if(pnt.first - i > 0) _toget[pnt.first - i].push_back(value);
                if(pnt.first + i < 256) _toget[pnt.first + i].push_back(value);
            }
        }
    }
    for(int i = 0; i < 256; ++i){
        double tmp = 0;
        for(const auto& t : _toget[i]) tmp += t;
        if(_toget[i].size()) tmp /= _toget[i].size();
        color[i] = tmp;
    }
    return color;
}

TEX::~TEX(){
    delete[] SamR; delete[] SamG; delete[] SamB; delete[] SamA;
    delete[] Color; delete[] ColorR; delete[] ColorG; delete[] ColorB; delete[] ColorA;
}
int* TEX::getColor(const int& choose){
    if(choose == 1) return ColorR;
    if(choose == 2) return ColorG;
    if(choose == 3) return ColorB;
    if(choose == 4) return ColorA;
    return Color;
} 
SamPnt* TEX::getSamPnt(const int& choose){
    if(choose == 1) return SamR;
    if(choose == 2) return SamG;
    if(choose == 3) return SamB;
    return SamA;
} 

void TEX::use()const{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, texID[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, texID[1]);
}
