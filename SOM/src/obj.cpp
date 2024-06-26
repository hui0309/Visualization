#include "./obj.h"

#define X first
#define Y second
#define fir first
#define sec second
#define INT x
#define DEC y
#define MN x
#define MXN y

// construct OBJ
OBJ::OBJ() : pos(-0.5, -0.5, 0.0), rot(0.0f), sca(1.0f), pntSZ(32), iterSZ(500), flag(0), selectData(2), isCly(false){
    dataName[0] = "vaseSurface.txt";
    dataName[1] = "bunnySurface.txt";
    dataName[2] = "teapotSurface.txt";
    getData();
}

void OBJ::getData(){
    string tmp = "";
    for(int i = 0; isprint(dataName[selectData][i]); ++i)
        tmp = tmp + dataName[selectData][i];
    if(tmp == chooseName) return;
    chooseName = tmp;
    // read the data
    ifstream inputFile("./TXT/" + string(dataName[selectData]), ios::binary | fstream::in);
    if (!inputFile.is_open())
    {
        cerr << "Unable to open file.\n";
        return;
    }
    int space;
    double mxn = INT_MIN, mn = INT_MAX;
    inputFile >> dataSZ >> space;
    cerr << dataSZ << "\n";
    data.clear();
    data.resize(dataSZ);
    for (int i = 0; i < dataSZ; ++i)
    {
        inputFile >> data[i].x >> data[i].y >> data[i].z;
        mxn = max({mxn, data[i].x, data[i].y, data[i].z});
        mn = min({mn, data[i].x, data[i].y, data[i].z});
    }
    inputFile.close();
    for(int i = 0; i < dataSZ; ++i){
        data[i].x = (data[i].x - mn) / (mxn - mn);
        data[i].y = (data[i].y - mn) / (mxn - mn);
        data[i].z = (data[i].z - mn) / (mxn - mn);
    }
    cerr << "Successfully loaded original data\n";

    genPnt();
}

void OBJ::genPnt()
{
    accDis = 0;
    drawSZ = pntSZ;
    flag = 0;
    isTex = isPoly = isNorm = false;
    vary = numeric_limits<double>::infinity();
    //tarDis.clear();    tarDis.resize(pntSZ, vector<double>(pntSZ, 0));
    pnt.clear(); pnt.resize(drawSZ);
    // Randomly get the sample points in the data
    for(int i = 0; i < drawSZ; ++i){
        pnt[i].resize(drawSZ);
        for(int j = 0; j < drawSZ; ++j){
            pnt[i][j].x = 1.0 * i / drawSZ;
            pnt[i][j].y = 1.0 * (drawSZ - j) / drawSZ;
            pnt[i][j].z = 0.0;
        }
    }
}

void OBJ::initSom(){
    lr = 0.97;
    upSZ = drawSZ / 2 - 1; // update neightbor size
    for (int i = 0; i < drawSZ; ++i){
        double theta = 2.0 * 3.1415926 / drawSZ;
        double angle = 0.0f;
        for(int j = 0; j < drawSZ; ++j){
            std::random_device rd;
            std::mt19937 gen(rd());
            uniform_real_distribution<> dist(0, 1);
            pnt[i][j].z = (1? dist(gen) : 0);
            pnt[i][j].x = dist(gen);
            pnt[i][j].y = dist(gen);
            angle += theta;
        }
    }
}

// sommon method to update 2d_space distance
void OBJ::som()
{
    double newE = 0;
    // vector<pair<double, double>> grid(pntSZ, {0, 0});
    std::random_device rd;
    std::mt19937 gen(rd());
    uniform_int_distribution<> dist(0, dataSZ);
    int IndP = dist(gen);
    pair<pair<int, double>, int> minDiff = {{-1, INT_MAX}, 1};
    for (int i = 0; i < drawSZ; ++i){
        for(int j = 0; j < drawSZ; ++j){
            double diffX = pnt[i][j].x - data[IndP].x;
            double diffY = pnt[i][j].y - data[IndP].y;
            double diffZ = (1? pnt[i][j].z - data[IndP].z : 0.0);
            double diff = sqrt(diffX * diffX + diffY * diffY + diffZ * diffZ);
            if(fabs(diff - minDiff.fir.sec) < delta){
                ++minDiff.sec;
                uniform_real_distribution<> dic(0.0, 1.0);
                if(dic(gen) > 1.0 / minDiff.sec){
                    minDiff.fir.fir = i * drawSZ + j;
                }
            }
            else if(diff < minDiff.fir.sec){
                minDiff = {{i * drawSZ + j, diff}, 1};
            }
        }
    }
    pair<int, int> chK = {minDiff.fir.fir / drawSZ, minDiff.fir.fir % drawSZ};
    pnt[chK.X][chK.Y].x += lr * (data[IndP].x - pnt[chK.X][chK.Y].x);
    pnt[chK.X][chK.Y].y += lr * (data[IndP].y - pnt[chK.X][chK.Y].y); 
    pnt[chK.X][chK.Y].z += lr * (1? (data[IndP].z - pnt[chK.X][chK.Y].z) : 0.0); 
    double alpha = lr * 0.6;
    for(int i = 1; i <= max(1, upSZ); ++i){
        int tmp;
        if(chK.Y - i >= 0){
            pnt[chK.X][chK.Y - i].x += alpha * (data[IndP].x - pnt[chK.X][chK.Y - i].x);
            pnt[chK.X][chK.Y - i].y += alpha * (data[IndP].y - pnt[chK.X][chK.Y - i].y); 
            pnt[chK.X][chK.Y - i].z += alpha * (1? (data[IndP].z - pnt[chK.X][chK.Y - i].z):0); 
            tmp = (chK.X + drawSZ - i) % drawSZ;
            pnt[tmp][chK.Y - i].x += alpha * (data[IndP].x - pnt[tmp][chK.Y - i].x);
            pnt[tmp][chK.Y - i].y += alpha * (data[IndP].y - pnt[tmp][chK.Y - i].y); 
            pnt[tmp][chK.Y - i].z += alpha * (1? (data[IndP].z - pnt[tmp][chK.Y - i].z):0); 
            tmp = (chK.X + i) % drawSZ;
            pnt[tmp][chK.Y - i].x += alpha * (data[IndP].x - pnt[tmp][chK.Y - i].x);
            pnt[tmp][chK.Y - i].y += alpha * (data[IndP].y - pnt[tmp][chK.Y - i].y); 
            pnt[tmp][chK.Y - i].z += alpha * (1? (data[IndP].z - pnt[tmp][chK.Y - i].z):0); 
        }
        if(chK.Y + i < drawSZ){
            pnt[chK.X][chK.Y + i].x += alpha * (data[IndP].x - pnt[chK.X][chK.Y + i].x);
            pnt[chK.X][chK.Y + i].y += alpha * (data[IndP].y - pnt[chK.X][chK.Y + i].y); 
            pnt[chK.X][chK.Y + i].z += alpha * (1? (data[IndP].z - pnt[chK.X][chK.Y + i].z):0); 
            tmp = (chK.X + drawSZ - i) % drawSZ;
            pnt[tmp][chK.Y + i].x += alpha * (data[IndP].x - pnt[tmp][chK.Y + i].x);
            pnt[tmp][chK.Y + i].y += alpha * (data[IndP].y - pnt[tmp][chK.Y + i].y); 
            pnt[tmp][chK.Y + i].z += alpha * (1? (data[IndP].z - pnt[tmp][chK.Y + i].z):0); 
            tmp = (chK.X + i) % drawSZ;
            pnt[tmp][chK.Y + i].x += alpha * (data[IndP].x - pnt[tmp][chK.Y + i].x);
            pnt[tmp][chK.Y + i].y += alpha * (data[IndP].y - pnt[tmp][chK.Y + i].y); 
            pnt[tmp][chK.Y + i].z += alpha * (1? (data[IndP].z - pnt[tmp][chK.Y + i].z):0); 
        }
        tmp = (chK.X + drawSZ - i) % drawSZ;
        pnt[chK.X][chK.Y].x += alpha * (data[IndP].x - pnt[chK.X][chK.Y].x);
        pnt[chK.X][chK.Y].y += alpha * (data[IndP].y - pnt[chK.X][chK.Y].y); 
        pnt[chK.X][chK.Y].z += alpha * (1? (data[IndP].z - pnt[chK.X][chK.Y].z):0); 
        tmp = (chK.X + i) % drawSZ;
        pnt[tmp][chK.Y].x += alpha * (data[IndP].x - pnt[tmp][chK.Y].x);
        pnt[tmp][chK.Y].y += alpha * (data[IndP].y - pnt[tmp][chK.Y].y); 
        pnt[tmp][chK.Y].z += alpha * (1? (data[IndP].z - pnt[tmp][chK.Y].z) :0); 
    }
    alpha *= 0.6;
}

// update the iter 
void OBJ::update()
{
    glm::vec3 cen(0.0f);
    for(int i = 0; i < drawSZ; ++i){
        for(int j = 0; j < drawSZ; ++j){
            cen.x += pnt[i][j].x;
            cen.y += pnt[i][j].y;
            cen.z += pnt[i][j].z;
        }
    }
    cen /= (drawSZ * drawSZ);
    pos = -cen;
    getData();
    if(flag != 1 && flag != 2) return;
    if(flag == 1){
        initSom(); flag = 2;
    }
    for (int i = 0; i < iterSZ; ++i){
        som();
        if(!flag) return;
    }
    lr *= 0.97;
    upSZ *= 0.8;
    if(lr < delta) flag = 4;
}

// deconstruct OBJ
OBJ::~OBJ()
{
    // release space
    glDeleteVertexArrays(1, &vaoID[0]);
    glDeleteBuffers(1, &vboID[0]);
    glDeleteVertexArrays(1, &vaoID[1]);
    glDeleteBuffers(1, &vboID[1]);
}

// get world space mat
glm::mat4 OBJ::getModel() const
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, sca);
    return model;
}

// draw the sample data obj
void OBJ::drawLine()
{
    vector<float> vert;
    for (int i = 0; i < drawSZ; ++i)
    {
        for(int j = 0; j < drawSZ - 1; ++j){
            vert.push_back(pnt[i][j].x);
            vert.push_back(pnt[i][j].y);
            vert.push_back((isCly?pnt[i][j].z:0));
            vert.push_back(pnt[i][j + 1].x);
            vert.push_back(pnt[i][j + 1].y);
            vert.push_back((isCly?pnt[i][j + 1].z:0));  
        }
    }
    for (int i = 0; i < drawSZ; ++i)
    {
        for(int j = 0; j < drawSZ; ++j){
            vert.push_back(pnt[i][j].x);
            vert.push_back(pnt[i][j].y);
            vert.push_back((isCly?pnt[i][j].z:0));
            vert.push_back(pnt[(i + 1) % drawSZ][j].x);
            vert.push_back(pnt[(i + 1) % drawSZ][j].y);
            vert.push_back((isCly?pnt[(i + 1) % drawSZ][j].z:0));  
        }
    }
    glDeleteVertexArrays(1, &vaoID[0]);
    glDeleteBuffers(1, &vboID[0]);

    glGenVertexArrays(1, &vaoID[0]);
    glGenBuffers(1, &vboID[0]);
    glBindVertexArray(vaoID[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), vert.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_LINES, 0, vert.size() / 3);
    glBindVertexArray(0);
}

void OBJ::drawPoly()
{
    vector<float> vert;
    for (int i = 0; i < drawSZ; ++i)
    {
        for(int j = 0; j < drawSZ - 1; ++j){
            glm::vec3 a = glm::vec3(pnt[(i + 1) % drawSZ][j].x - pnt[i][j].x, pnt[(i + 1) % drawSZ][j].y - pnt[i][j].y, pnt[(i + 1) % drawSZ][j].z - pnt[i][j].z);
            glm::vec3 b = glm::vec3(pnt[i][j + 1].x - pnt[i][j].x, pnt[i][j + 1].y - pnt[i][j].y, pnt[i][j + 1].z - pnt[i][j].z);
            glm::vec3 c = glm::normalize(glm::cross(a, b));
            vert.push_back(pnt[i][j].x);
            vert.push_back(pnt[i][j].y);
            vert.push_back((isCly? pnt[i][j].z : 0));
            vert.push_back(c.x);
            vert.push_back(c.y);
            vert.push_back((isCly?c.z:0));
            vert.push_back(1.0 * i / drawSZ);
            vert.push_back(1.0 * (drawSZ - j) / drawSZ);
            vert.push_back(pnt[(i + 1) % drawSZ][j].x);
            vert.push_back(pnt[(i + 1) % drawSZ][j].y);
            vert.push_back((isCly? pnt[(i + 1) % drawSZ][j].z : 0));  
            vert.push_back(c.x);
            vert.push_back(c.y);
            vert.push_back((isCly?c.z:0));
            vert.push_back(1.0 * ((i + 1) % drawSZ) / drawSZ);
            vert.push_back(1.0 * (drawSZ - j) / drawSZ);
            vert.push_back(pnt[i][j + 1].x);
            vert.push_back(pnt[i][j + 1].y);
            vert.push_back((isCly? pnt[i][j + 1].z : 0));  
            vert.push_back(c.x);
            vert.push_back(c.y);
            vert.push_back((isCly?c.z:0));
            vert.push_back(1.0 * i / drawSZ);
            vert.push_back(1.0 * (drawSZ - j - 1) / drawSZ);
            a = glm::vec3(pnt[i][j + 1].x - pnt[(i + 1) % drawSZ][j + 1].x, pnt[i][j + 1].y - pnt[(i + 1) % drawSZ][j + 1].y, pnt[i][j + 1].z - pnt[(i + 1) % drawSZ][j + 1].z);
            b = glm::vec3(pnt[(i + 1) % drawSZ][j].x - pnt[(i + 1) % drawSZ][j + 1].x, pnt[(i + 1) % drawSZ][j].y - pnt[(i + 1) % drawSZ][j + 1].y, pnt[(i + 1) % drawSZ][j].z - pnt[(i + 1) % drawSZ][j + 1].z);
            c = glm::normalize(glm::cross(a, b));            
            vert.push_back(pnt[(i + 1) % drawSZ][j + 1].x);
            vert.push_back(pnt[(i + 1) % drawSZ][j + 1].y);
            vert.push_back((isCly? pnt[(i + 1) % drawSZ][j + 1].z:0));
            vert.push_back(c.x);
            vert.push_back(c.y);
            vert.push_back((isCly?c.z:0));
            vert.push_back(1.0 * ((i + 1) % drawSZ) / drawSZ);
            vert.push_back(1.0 * (drawSZ - j - 1) / drawSZ);
            vert.push_back(pnt[i][j + 1].x);
            vert.push_back(pnt[i][j + 1].y);
            vert.push_back((isCly? pnt[i][j + 1].z:0));  
            vert.push_back(c.x);
            vert.push_back(c.y);
            vert.push_back((isCly?c.z:0));
            vert.push_back(1.0 * i / drawSZ);
            vert.push_back(1.0 * (drawSZ - j - 1) / drawSZ);
            vert.push_back(pnt[(i + 1) % drawSZ][j].x);
            vert.push_back(pnt[(i + 1) % drawSZ][j].y);
            vert.push_back((isCly? pnt[(i + 1) % drawSZ][j].z:0));  
            vert.push_back(c.x);
            vert.push_back(c.y);
            vert.push_back((isCly?c.z:0));
            vert.push_back(1.0 * ((i + 1) % drawSZ) / drawSZ);
            vert.push_back(1.0 * (drawSZ - j) / drawSZ);
        }
    }
    glDeleteVertexArrays(1, &vaoID[1]);
    glDeleteBuffers(1, &vboID[1]);

    glGenVertexArrays(1, &vaoID[1]);
    glGenBuffers(1, &vboID[1]);
    glBindVertexArray(vaoID[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), vert.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(0 * sizeof(float)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glDrawArrays(GL_TRIANGLES, 0, vert.size() / 8);
    glBindVertexArray(0);
}

void OBJ::drawNorm()
{
    vector<float> vert;
    for (int i = 0; i < drawSZ; ++i)
    {
        for(int j = 0; j < drawSZ - 1; ++j){
            glm::vec3 a = glm::vec3(pnt[(i + 1) % drawSZ][j].x - pnt[i][j].x, pnt[(i + 1) % drawSZ][j].y - pnt[i][j].y, pnt[(i + 1) % drawSZ][j].z - pnt[i][j].z);
            glm::vec3 b = glm::vec3(pnt[i][j + 1].x - pnt[i][j].x, pnt[i][j + 1].y - pnt[i][j].y, pnt[i][j + 1].z - pnt[i][j].z);
            glm::vec3 c = glm::normalize(glm::cross(a, b));
            glm::vec3 cen = glm::vec3(
                (pnt[i][j].x + pnt[(i + 1) % drawSZ][j].x + pnt[i][j + 1].x) / 3,
                (pnt[i][j].y + pnt[(i + 1) % drawSZ][j].y + pnt[i][j + 1].y) / 3,
                (pnt[i][j].z + pnt[(i + 1) % drawSZ][j].z + pnt[i][j + 1].z) / 3
            );
            vert.push_back(cen.x);
            vert.push_back(cen.y);
            vert.push_back(cen.z);
            vert.push_back(cen.x + c.x * 0.025);
            vert.push_back(cen.y + c.y * 0.025);
            vert.push_back(cen.z + c.z * 0.025);
            a = glm::vec3(pnt[i][j + 1].x - pnt[(i + 1) % drawSZ][j + 1].x, pnt[i][j + 1].y - pnt[(i + 1) % drawSZ][j + 1].y, pnt[i][j + 1].z - pnt[(i + 1) % drawSZ][j + 1].z);
            b = glm::vec3(pnt[(i + 1) % drawSZ][j].x - pnt[(i + 1) % drawSZ][j + 1].x, pnt[(i + 1) % drawSZ][j].y - pnt[(i + 1) % drawSZ][j + 1].y, pnt[(i + 1) % drawSZ][j].z - pnt[(i + 1) % drawSZ][j + 1].z);
            c = glm::normalize(glm::cross(a, b));
            cen = glm::vec3(
                (pnt[(i + 1) % drawSZ][j + 1].x + pnt[(i + 1) % drawSZ][j].x + pnt[i][j + 1].x) / 3,
                (pnt[(i + 1) % drawSZ][j + 1].y + pnt[(i + 1) % drawSZ][j].y + pnt[i][j + 1].y) / 3,
                (pnt[(i + 1) % drawSZ][j + 1].z + pnt[(i + 1) % drawSZ][j].z + pnt[i][j + 1].z) / 3
            );        
            vert.push_back(cen.x);
            vert.push_back(cen.y);
            vert.push_back(cen.z);
            vert.push_back(cen.x + c.x * 0.025);
            vert.push_back(cen.y + c.y * 0.025);
            vert.push_back(cen.z + c.z * 0.025);
        }
    }
    glDeleteVertexArrays(1, &vaoID[2]);
    glDeleteBuffers(1, &vboID[2]);

    glGenVertexArrays(1, &vaoID[2]);
    glGenBuffers(1, &vboID[2]);
    glBindVertexArray(vaoID[2]);
    glBindBuffer(GL_ARRAY_BUFFER, vboID[2]);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), vert.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_LINES, 0, vert.size() / 3);
    glBindVertexArray(0);
}