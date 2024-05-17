#include "./obj.h"
#define X first
#define fir first
#define Y second
#define sec second

OBJ::OBJ(const unsigned int& _choose, const unsigned int& _IMGX, const unsigned int& _IMGY, const double& _h): pos(0.0f, 0.0f, 0.0f), rot(0.0f, 0.0f, 0.0f), sca(1.0f, 1.0f, 1.0f){
    loadOBJ(_choose, _IMGX, _IMGY, _h);
    
    glGenVertexArrays(1, &vaoID);
    glGenBuffers(1, &vboID);
    glGenBuffers(1, &eboID);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vaoID);

    // glBindBuffer(GL_ARRAY_BUFFER, vboID);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //vertpos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //textpos
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    // glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0); 

}
OBJ::~OBJ(){
    glDeleteVertexArrays(1, &vaoID);
    glDeleteBuffers(1, &vboID);
    glDeleteBuffers(1, &eboID);
    delete[] MyTex;
    delete[] valCnt;
}
void OBJ::loadGrid(const unsigned int& _choose, const unsigned int& _IMGX, const unsigned int& _IMGY, const double& _h){
    choose = _choose;
    ifstream inputFile("./Vector/" + to_string(choose) + ".vec", ios::binary);
    if (!inputFile.is_open()) {
        cerr << "Unable to open file.\n";
        return ;
    }

    inputFile.seekg(0, ios::end);
    streampos fileSize = inputFile.tellg();
    inputFile.seekg(0, ios::beg);

    vector<unsigned char> buffer(fileSize);

    inputFile.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    inputFile.close();
    GridX = buffer[0]; GridY = buffer[1];
    GridData.resize(GridX);
    // valCnt = new float[256];
    for(int x = 0, index = 2; x < GridX; ++x){
        GridData[x].resize(GridY);
        for(int y = 0; y < GridY; ++y){
                GridData[x][y] = buffer[index++];
                // ++valCnt[(int)int_data[k][j][x]];
        }
    }
    loadIMG(_IMGX, _IMGY, _h);
    // for(int i = 0; i < 256; ++i)
    //     valCnt[i] = glm::log2(valCnt[i]);
    // if(MyTex == nullptr) MyTex = new TEX(int_data);
    // else MyTex -> creVOLE(int_data);
}
void OBJ::loadIMG(const unsigned int& _IMGX, const unsigned int& _IMGY, const double& _h){
    IMGX = _IMGX, IMGY = IMGY;
    IMGData.resize(IMGX);
    double BLKX = GridX / IMGX, BLKY = GridY / IMGY;
    for(int x = 0; x < IMGX; ++x){
        pair<double, double> tmpX;
        tmpX.sec = modf(BLKX * x, &tmpX.fir);
        IMGData[x].resize(IMGY);
        for(int y = 0; y < IMGY; ++y){
            pair<double, double> tmpY;
            tmpY.sec = modf(BLKY * x, &tmpY.fir);
            IMGData[x][y] = GridData[(int)tmpX.fir][(int)tmpY.fir] * (1 - tmpX.sec) * (1 - tmpY.sec) +
                            GridData[(int)tmpX.fir + 1][(int)tmpY.fir] * tmpX.sec * (1 - tmpY.sec) +
                            GridData[(int)tmpX.fir][(int)tmpY.fir + 1] * (1 - tmpX.sec) * tmpY.sec +
                            GridData[(int)tmpX.fir + 1][(int)tmpY.fir + 1] * tmpX.sec * tmpY.sec;
        }
	}
    IMGGrad.resize(IMGX);
    for(int x = 0; x < IMGX; ++x){
        IMGGrad[x].resize(IMGY);
        for(int y = 0; y < IMGY; ++y){
            if(x == 0) IMGGrad[x][y] = IMGData[x + 1][y] - IMGData[x][y];
            else if(x == IMGX - 1) IMGGrad[x][y] = IMGData[x][y] - IMGData[x - 1][y];
            else IMGGrad[x][y] = (IMGData[x + 1][y] - IMGData[x - 1][y]) / 2.0;
            if(y == 0) IMGGrad[x][y] = IMGData[x][y + 1] - IMGData[x][y];
            else if(y == IMGY - 1) IMGGrad[x][y]= IMGData[x][y] - IMGData[x][y - 1];
            else IMGGrad[x][y] = (IMGData[x][y + 1] - IMGData[x][y - 1]) / 2.0;
        }
    }
    loadFlow(_h);
}
void OBJ::loadFlow(const double& _h){
    h = _h;
    vector<vector<double>> vis(IMGX, vector<double>(IMGY, false));
    for(int x = 0; x < IMGX; ++x){
        for(int y = 0; y < IMGY; ++y){
            if(vis[x][y]) continue;
            pair<glm::vec3, glm::vec3> line;
            line.fir = {x, y, 0};
            while(true){
                
            }
        }
    }
}
glm::mat4 OBJ::getModel()const{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
	model = glm::rotate(model, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
	// model = glm::scale(model, sca);
	model = glm::scale(model, sca);
	return model;
}
float* OBJ::getCnt()const{
    return valCnt;
}
int OBJ::getChoose()const{
    return choose;
}
string OBJ::getDataName()const{
    return to_string(choose);
}
void OBJ::draw()const{
    MyTex -> use();
    // default: front, COUNTERCLOCKWISE
    // glCullFace(GL_FRONT);
    // glFrontFace(GL_CCW);
    glBindVertexArray(vaoID); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // no need to unbind it every time 
}

