#include "./obj.h"
#define fir first
#define sec second

OBJ::OBJ(const int& _choose): pos(0.0f, 0.0f, 0.0f), rot(0.0f, 0.0f, 0.0f), sca(1.0f, 1.0f, 1.0f){
    glGenVertexArrays(1, &vaoID);
    glGenBuffers(1, &vboID);
    glGenBuffers(1, &eboID);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vaoID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

    loadOBJ(_choose);
}
OBJ::~OBJ(){
    glDeleteVertexArrays(1, &vaoID);
    glDeleteBuffers(1, &vboID);
    glDeleteBuffers(1, &eboID);
    delete[] MyTex;
    delete[] valCnt;
}
void OBJ::loadOBJ(const int& _choose){
    choose = _choose;
    ifstream inputFile("./Scalar/" + dataName[choose] + ".raw", ios::binary);
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
    vector<vector<vector<float>>> int_data(256, vector<vector<float>>(256, vector<float>(256)));
    valCnt = new float[256];
    for(int i = 0, index = 0; i < 256; i++){
        for(int j = 0; j < 256; j++){
            for(int k = 0; k < 256; k++){
                int_data[k][j][i] = buffer[index++];
                ++valCnt[(int)int_data[k][j][i]];
            }
        }
    }
    for(int i = 0; i < 256; ++i)
        valCnt[i] = glm::log2(valCnt[i]);
    if(MyTex == nullptr) MyTex = new TEX(int_data);
    else MyTex -> creVOLE(int_data);
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
    return dataName[choose];
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

