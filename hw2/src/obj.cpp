#include "./obj.h"
#define fir first
#define sec second

OBJ::OBJ(): pos(0.0f, 0.0f, 0.0f), rot(0.0f, 0.0f, 0.0f), sca(1.0f, 1.0f, 1.0f), choose(0){
    glGenVertexArrays(1, &VAOID);
    glGenBuffers(1, &VBOID);
    glGenBuffers(1, &EBOID);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAOID);

    glBindBuffer(GL_ARRAY_BUFFER, VBOID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOID);
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

    loadOBJ();
}
OBJ::~OBJ(){
    glDeleteVertexArrays(1, &VAOID);
    glDeleteBuffers(1, &VBOID);
    glDeleteBuffers(1, &EBOID);
    delete MyTex;
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
	if (file.bad()) { cerr << "something error when reading the " << dataName[choose] << "\n"; return;}
	file.close();
	cerr << "Succuss to read date, file size =  " << sz << " bytes.\n";
	
	vector<vector<vector<double>>> int_data(256, vector<vector<double>>(256, vector<double>(256)));
	for(int i = 0; i < 256 * 256 * 256; ++i){
        int_data[i / 256 / 256][(i / 256) % 256][i % 256] = cha[i] + 128.0f;
    }
    cerr << "Succuss to get intensity.\n";
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
    glm::vec3 PNT;
    if(big){
        PNT.x = vertices[6 * 6];
        PNT.y = vertices[6 * 6 + 1];
        PNT.z = vertices[6 * 6 + 2];
    }
    else{
        PNT.x = vertices[0];
        PNT.y = vertices[1];
        PNT.z = vertices[2];
    }
    return PNT;
}

void OBJ::draw()const{
    MyTex -> use();
    glBindVertexArray(VAOID); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // no need to unbind it every time 
}

