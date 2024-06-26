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
OBJ::OBJ() : pos(0.0f), rot(0.0f), sca(1.0f), pntSZ(50), iterSZ(20), flag(0){
    // read the data
    ifstream inputFile("./creditcard.dat", ios::binary | fstream::in);
    if (!inputFile.is_open())
    {
        cerr << "Unable to open file.\n";
        return;
    }
    char camma;
    inputFile >> dataSZ >> camma >> attSZ;
    cerr << dataSZ << " " << attSZ << "\n";
    data.clear();
    data.resize(dataSZ);
    for (int i = 0; i < dataSZ; ++i)
    {
        data[i].attribute.clear();
        data[i].attribute.resize(attSZ - 1);
        for (int j = 0; j < attSZ - 1; ++j)
        {
            inputFile >> data[i].attribute[j] >> camma;
        }
        inputFile >> data[i].iserror;
    }

    inputFile.close();
    cerr << "Successfully loaded original data\n";
}

void OBJ::genPnt()
{
    accDis = 0;
    lr = 0.97;
    drawSZ = pntSZ;
    flag = 0;
    vary = numeric_limits<double>::infinity();
    cnt = {0, 0};
    tarDis.clear();    tarDis.resize(pntSZ, vector<double>(pntSZ, 0));
    samPos.clear();    samPos.resize(pntSZ);
    clyCenF.X = clyCenF.Y = clyCenT.X = clyCenT.Y = 0;
    priAxisT.fir.X = priAxisT.fir.Y = priAxisT.sec = priAxisF.fir.X = priAxisF.fir.Y = priAxisF.sec = 0;
    // Randomly get the sample points in the data
    srand(time(NULL));
    for (int i = 0; i < pntSZ; ++i)
    {
        int tmp = rand() % (dataSZ - i) + i;
        swap(data[i], data[tmp]);
    }
    // up-triangle
    double mxnDis = 0.0;
    for (int i = 0; i < pntSZ; ++i)
    {
        data[i].iserror ? ++cnt.fir : ++cnt.sec;
        for (int j = i + 1; j < pntSZ; ++j)
        {
            for (int k = 0; k < attSZ - 1; ++k)
            {
                double tmp = data[i].attribute[k] - data[j].attribute[k];
                tarDis[i][j] += tmp * tmp;
            }
            tarDis[i][j] = sqrt(tarDis[i][j]);
            mxnDis = max(mxnDis, tarDis[i][j]);
            if (tarDis[i][j] < delta) tarDis[i][j] = delta;
        }
    }
    for (int i = 0; i < pntSZ; ++i){
        for (int j = i + 1; j < pntSZ; ++j){
            tarDis[i][j] /= mxnDis;
            accDis += tarDis[i][j];
        }
    }
    // Randomly push the sample points in the 2D space
    std::random_device rd;
    std::mt19937 gen(rd());
    uniform_real_distribution<> dist(0.0, 1.0);
    for (int i = 0; i < pntSZ; ++i)
    {
        samPos[i].X = dist(gen);
        samPos[i].Y = dist(gen);
    }
    accE = 0;
    for (int i = 0; i < pntSZ; ++i)
    {
        for (int j = i + 1; j < pntSZ; ++j)
        {
            double tmpX = samPos[i].X - samPos[j].X;
            double tmpY = samPos[i].Y - samPos[j].Y;
            double tmp = sqrt(tmpX * tmpX + tmpY * tmpY);
            if (tmp < delta) tmp = delta;
            accE += powf(tarDis[i][j] - tmp, 2) / tarDis[i][j];
        }
    }
    accE /= accDis;
}

// sommon method to update 2d_space distance
int OBJ::sammon()
{
    double newE = 0;
    vector<pair<double, double>> grid(pntSZ, {0, 0});
    for (int i = 0; i < pntSZ; ++i)
    {
        for (int j = i + 1; j < pntSZ; ++j)
        {
            double tmpX = samPos[i].X - samPos[j].X;
            double tmpY = samPos[i].Y - samPos[j].Y;
            double tmp = sqrt(tmpX * tmpX + tmpY * tmpY);
            if (tmp < delta) tmp = delta;
            newE += powf(tarDis[i][j] - tmp, 2) / tarDis[i][j];
            double factor = lr * (1 - tarDis[i][j] / tmp) / tmp;
            grid[i].X -= factor * tmpX;
            grid[i].Y -= factor * tmpY;
            grid[j].X += factor * tmpX;
            grid[j].Y += factor * tmpY;
        }
    }
    for (int i = 0; i < pntSZ; ++i)
    {
        samPos[i].X += grid[i].X;
        samPos[i].Y += grid[i].Y;
    }
    vector<double> mean(2, 0);
    for(int i = 0; i < pntSZ; ++i){
        mean[0] += samPos[i].X;
        mean[1] += samPos[i].Y;
    }
    mean[0] /= pntSZ; mean[1] /= pntSZ; 
    vector<vector<double>> pntAtt(pntSZ, vector<double>(2));
    for(int i = 0; i < pntSZ; ++i){
        pntAtt[i][0] -= mean[0];
        pntAtt[i][1] -= mean[1];
    }
    vector<vector<double>> mat(2, vector<double>(2, 0.0f));
    for(int j = 0; j < pntSZ; ++j){
        for(int k = 0; k < 2; ++k){
            mat[0][k] = (pntAtt[j][0] * pntAtt[j][k]) / 2;
            mat[1][k] = (pntAtt[j][1] * pntAtt[j][k]) / 2;
        }
    }
    mean.clear(); mean.resize(4);
    for(int i = 0; i < pntSZ; ++i){
        if(data[i].iserror){
            mean[0] += samPos[i].X;
            mean[1] += samPos[i].Y;
        } else{
            mean[2] += samPos[i].X;
            mean[3] += samPos[i].Y;
        }
    }
    clyCenF = {mean[0] / cnt.fir, mean[1] / cnt.fir};
    clyCenT = {mean[2] / cnt.sec, mean[3] / cnt.sec};



    double oldE = accE;
    accE = newE / accDis;
    vary = fabs(accE - oldE);
    return (vary > delta? 1 : 4);
}

//eigen value
void OBJ::update_P(vector<vector<double>>& mat, const pair<pair<int, int>, double>& mxn, const double& ang, const int& n){
    double c = cos(ang), s = sin(ang);   
    vector<double>vec_p, vec_q;
    vec_p.clear(); vec_p.resize(n);
    vec_q.clear(); vec_q.resize(n);
    for(int i = 0; i < n; ++i){
        vec_p[i] = c * mat[i][mxn.fir.X] + s * mat[i][mxn.fir.Y];
        vec_q[i] = -s * mat[i][mxn.fir.X] + c * mat[i][mxn.fir.Y];
    }
    for(int i = 0; i < n; ++i){
        mat[i][mxn.fir.X] = vec_p[i];
        mat[i][mxn.fir.Y] = vec_q[i];
    }
}

//eigen vector
void OBJ::update_A(vector<vector<double>>& mat, const pair<pair<int, int>, double>& mxn, const double& ang, const int& n){
    double c = cos(ang), s = sin(ang);   
    vector<double>vec_p, vec_q;
    vec_p.clear(); vec_p.resize(n);
    vec_q.clear(); vec_q.resize(n);
    vec_p[mxn.fir.X] = c * c * mat[mxn.fir.X][mxn.fir.X] + 2.0 * s * c * mat[mxn.fir.X][mxn.fir.Y] + s * s * mat[mxn.fir.Y][mxn.fir.Y];
    vec_q[mxn.fir.Y] = s * s * mat[mxn.fir.X][mxn.fir.X] - 2.0 * s * c * mat[mxn.fir.X][mxn.fir.Y] + c * c * mat[mxn.fir.Y][mxn.fir.Y]; 
    for(int i = 0; i < n; ++i){
        if(i == mxn.fir.X || i == mxn.fir.Y) continue;
        vec_p[i] = c * mat[i][mxn.fir.X] + s * mat[i][mxn.fir.Y];
        vec_q[i] = -s * mat[i][mxn.fir.X] + c * mat[i][mxn.fir.Y];
    }
    for(int i = 0; i < n; ++i){
        mat[mxn.fir.X][i] = mat[i][mxn.fir.X] = vec_p[i];
        mat[mxn.fir.Y][i] = mat[i][mxn.fir.Y] = vec_q[i];
    }
    mat[mxn.fir.X][mxn.fir.Y] = mat[mxn.fir.Y][mxn.fir.X] = 0;
}

// find max error(?)
pair<pair<int, int>, double> OBJ::max_off_diag_entry(vector<vector<double>>&mat, const int& n){
    pair<pair<int, int>, double>mxn;
    mxn = {{0, 1}, mat[0][1]};
    for(int i = 0; i < n; ++i)
        for(int j = i + 1; j < n; ++j)
            if( fabs(mat[i][j]) > mxn.sec )
                mxn = {{i, j}, fabs(mat[i][j])};
    return mxn;
}

// jacobi method on the eigenspace
pair<int, int> OBJ::jacobi(const vector<vector<double>>& _mat, const int& n, const int& mode) {
    vector<vector<double>> mat = _mat;
    vector<vector<double>> P(n, vector<double>(n, 0));
    for (int i = 0; i < n; ++i) P[i][i] = 1;

    pair<pair<int, int>, double> mxn = max_off_diag_entry(mat, n);
    while (mxn.second > delta) {
        double ang = atan2(2.0 * mat[mxn.first.first][mxn.first.second], mat[mxn.first.first][mxn.first.first] - mat[mxn.first.second][mxn.first.second]) * 0.5;
        update_P(P, mxn, ang, n);
        update_A(mat, mxn, ang, n);
        mxn = max_off_diag_entry(mat, n);
    }
    if(mode == 1) priAxisF = {{sqrt(mat[0][0]), sqrt(mat[1][1])}, atan2(P[1][1], P[0][0])};
    if(mode == 2) priAxisT =  {{sqrt(mat[0][0]), sqrt(mat[1][1])}, atan2(P[1][1], P[0][0])};
    cerr << priAxisF.sec << " ";
    cerr << priAxisT.sec << "\n";
    // Determine principal axes
    pair<int, int> priAxis = {0, 1};
    if (mat[0] < mat[1]) {
        swap(priAxis.fir, priAxis.sec);
    }
    for (int i = 2; i < n; ++i) {
        if (mat[i] >= mat[priAxis.fir]) {
            priAxis.sec = priAxis.fir;
            priAxis.fir = i;
        } else if (mat[i] > mat[priAxis.sec]) {
            priAxis.sec = i;
        }
    }
    return priAxis;
}

//project data on 2D space according to eigenspace
void OBJ::pca(){
    vector<vector<double>> pntAtt(pntSZ, vector<double>(attSZ - 1));
    vector<double> mean(attSZ - 1, 0);
    for(int i = 0; i < pntSZ; ++i){
        for(int j = 0; j < attSZ - 1; ++j){
            mean[j] += data[i].attribute[j];
            pntAtt[i][j] = data[i].attribute[j];
        }
    }
    for(int j = 0; j < attSZ - 1; ++j){
        mean[j] /= pntSZ;
        for(int i = 0; i < pntSZ; ++i){
            pntAtt[i][j] -= mean[j];
        }
    }

    vector<vector<double>> mat(attSZ - 1, vector<double>(attSZ - 1, 0.0f));
    for(int i = 0; i < attSZ - 1; ++i){
        for(int j = 0; j < pntSZ; ++j){
            for(int k = 0; k < attSZ - 1; ++k){
                mat[i][k] += (pntAtt[j][i] * pntAtt[j][k]) / (attSZ - 1);
            }
        }
    }
    pair<int, int> priAxis = jacobi(mat, attSZ - 1, 0);
    // Use principal axes to update samPos
    for (int i = 0; i < pntSZ; ++i) {
        samPos[i].X = data[i].attribute[priAxis.X];
        samPos[i].Y = data[i].attribute[priAxis.Y];
    }
    
    pntAtt.clear(); pntAtt.resize(cnt.fir);
    for(int i = 0, j = 0; i < pntSZ; ++i){
        if(data[i].iserror){
            clyCenF.X += samPos[i].X;
            clyCenF.Y += samPos[i].Y;
            pntAtt[j].clear();
            pntAtt[j].push_back(samPos[i].X);
            pntAtt[j].push_back(samPos[i].Y);
            ++j;
        }
    }
    clyCenF = {clyCenF.X / cnt.fir, clyCenF.Y / cnt.fir};
    for(int i = 0; i < cnt.fir; ++i){
        pntAtt[i][0] -= clyCenF.X;
        pntAtt[i][1] -= clyCenF.Y;
    }
    mat.clear(); mat.resize(2);
    mat[0].resize(2); mat[1].resize(2);
    for(int i = 0; i < 2; ++i){
        for(int j = 0; j < pntSZ; ++j){
            for(int k = 0; k < 2; ++k){
                mat[i][k] += (pntAtt[j][i] * pntAtt[j][k]) / 2.0;
            }
        }
    }
    jacobi(mat, 2, 1);
    pntAtt.clear(); pntAtt.resize(cnt.sec);
    for(int i = 0, j = 0; i < pntSZ; ++i){
        if(!data[i].iserror){
            clyCenT.X += samPos[i].X;
            clyCenT.Y += samPos[i].Y;
            pntAtt[j].clear();
            pntAtt[j].push_back(samPos[i].X);
            pntAtt[j].push_back(samPos[i].Y);
            ++j;
        }
    }
    clyCenT = {clyCenT.X / cnt.sec, clyCenT.Y / cnt.sec};
    for(int i = 0; i < cnt.sec; ++i){
        pntAtt[i][0] -= clyCenT.X;
        pntAtt[i][1] -= clyCenT.Y;
    }
    mat.clear(); mat.resize(2);
    mat[0].resize(2); mat[1].resize(2);
    for(int i = 0; i < 2; ++i){
        for(int j = 0; j < pntSZ; ++j){
            for(int k = 0; k < 2; ++k){
                mat[i][k] += (pntAtt[j][i] * pntAtt[j][k]) / 2.0;
            }
        }
    }
    jacobi(mat, 2, 2);
}

// update the iter 
void OBJ::update()
{
    if (flag == 0 || flag == 2 || flag == 4) return;
    if (flag == 3){
        pca(); flag = 4; 
        return;
    }
    for (int i = 0; i < iterSZ; ++i)
        flag = sammon();
    lr *= 0.97;
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
void OBJ::drawPnt(const float *colorT, const float *colorF)
{
    vector<float> vert;
    for (int i = 0; i < drawSZ; ++i)
    {
        vert.push_back(samPos[i].X);
        vert.push_back(samPos[i].Y);
        if (data[i].iserror)
        {
            vert.push_back(colorT[0]);
            vert.push_back(colorT[1]);
            vert.push_back(colorT[2]);
            vert.push_back(colorT[3]);
        }
        else
        {
            vert.push_back(colorF[0]);
            vert.push_back(colorF[1]);
            vert.push_back(colorF[2]);
            vert.push_back(colorF[3]);
        }
    }
    glDeleteVertexArrays(1, &vaoID[0]);
    glDeleteBuffers(1, &vboID[0]);

    glGenVertexArrays(1, &vaoID[0]);
    glGenBuffers(1, &vboID[0]);
    glBindVertexArray(vaoID[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), vert.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(0 * sizeof(float)));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_POINTS, 0, drawSZ);
    glBindVertexArray(0);
}

// draw the eigen space clylider
void OBJ::drawCly(const float *colorT, const float *colorF)
{
    vector<float> vert;

    vert.push_back(clyCenF.X);
    vert.push_back(clyCenF.Y);
    cerr << clyCenF.X << " " << clyCenF.Y << "\n";
    vert.push_back(colorT[0]);
    vert.push_back(colorT[1]);
    vert.push_back(colorT[2]);
    vert.push_back(colorT[3]);
    vert.push_back(priAxisF.fir.X);
    vert.push_back(priAxisF.fir.Y);
    vert.push_back(priAxisF.sec);
    cerr << priAxisF.fir.X << " " << priAxisF.fir.Y << " " << priAxisF.sec << "\n";


    vert.push_back(clyCenT.X);
    vert.push_back(clyCenT.Y);
    vert.push_back(colorF[0]);
    vert.push_back(colorF[1]);
    vert.push_back(colorF[2]);
    vert.push_back(colorF[3]);
    vert.push_back(priAxisT.fir.X);
    vert.push_back(priAxisT.fir.Y);
    vert.push_back(priAxisT.sec);

    glDeleteVertexArrays(1, &vaoID[1]);
    glDeleteBuffers(1, &vboID[1]);
    glGenVertexArrays(1, &vaoID[1]);
    glGenBuffers(1, &vboID[1]);
    glBindVertexArray(vaoID[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), vert.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(0 * sizeof(float)));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(2 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glDrawArrays(GL_POINTS, 0, 2);
    glBindVertexArray(0);
}

