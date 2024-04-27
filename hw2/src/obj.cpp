#include "./obj.h"
#define fir first
#define sec second

OBJ::OBJ(): pos(0.0f, 0.0f, 0.0f), rot(0.0f, 0.0f, 0.0f), sca(1.0f, 1.0f, 1.0f), choose(1){
    glGenVertexArrays(1, &VAOID);
    glGenBuffers(1, &VBOID);
    // glGenBuffers(1, &EBOID);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAOID);

    glBindBuffer(GL_ARRAY_BUFFER, VBOID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOID);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //vertpos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //textpos
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    loadOBJ();
}
OBJ::~OBJ(){
    glDeleteVertexArrays(1, &VAOID);
    glDeleteBuffers(1, &VBOID);
    // glDeleteBuffers(1, &EBOID);
    delete MyTex;
}
void OBJ::loadOBJ(){
  std::ifstream inputFile("./Scalar/" + dataName[choose], std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Unable to open file." << std::endl;
        return ;
    }

    inputFile.seekg(0, std::ios::end);
    std::streampos fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    vector<unsigned char> buffer(fileSize);

    inputFile.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    inputFile.close();
    vector<vector<vector<float>>> int_data(256, vector<vector<float>>(256, vector<float>(256)));

    for(int i = 0, index = 0; i < 256; i++){
        for(int j = 0; j < 256; j++){
            for(int k = 0; k < 256; k++){
                int_data[i][j][k] = buffer[index++];
            }
        }
    }


	MyTex = new TEX(int_data);
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

glm::vec3 OBJ::getBoard(const bool& big)const{
    return (big? glm::vec3(255.0f) : glm::vec3(0.0f));
}

void OBJ::draw()const{
    MyTex -> use();
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);
    // //glFrontFace(GL_CCW);
    glBindVertexArray(VAOID); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // no need to unbind it every time 
}

