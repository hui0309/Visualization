#include "./tex.h"

#define X first
#define Y second
#define fir first
#define sec second
#define INT x
#define DEC y
#define MN x
#define MXN y

// constuct TEX
TEX::TEX(const string& _dataName, const glm::vec2& _texSZ, const float& _h, const int& _noiseMethod, const int& _filMethod, unsigned char* color){   
    // cnt the |V|
    speedCnt = new float[256];
    // start to load the vector data
    loadGrid(_dataName);
    // start to create image space
    creFlow(_texSZ, _h);
    // create the noise
    creNoise(_noiseMethod);
    // load the stream on the noise texture
    loadFlow(_filMethod);
    // create 1D texture
    creTRAN(color);
}

// desstruct TEX
TEX::~TEX(){
    // release space
    delete[] noise;
    delete[] speedCnt;
    flow.clear();
}

// read the vector data
void TEX::loadGrid(const string& _dataName) {
    dataName = _dataName;
    flowRange = {100.0f, 0.0f};

    // read the data
    ifstream inputFile("./Vector/" + dataName, ios::binary);
    if (!inputFile.is_open()) {
        cerr << "Unable to open file.\n";
        return;
    }
    inputFile >> gridSZ.x >> gridSZ.y;
    gridData.clear(); gridData.resize(gridSZ.x, vector<glm::vec2>(gridSZ.y));
    for (int i = 0; i < gridSZ.y; ++i) {
        for (int j = 0; j < gridSZ.x; ++j) {
            inputFile >> gridData[j][i].y >> gridData[j][i].x;
        }
    }

    inputFile.close();
    cerr << "Successfully loaded grid data\n";
    cerr << "grid data size" << gridSZ.x << " X " << gridSZ.y << "\n";
}

// to interpolate pnt's vector
glm::vec2 TEX::interpolate(const glm::vec2 & tmpX, const glm::vec2& tmpY) const{
    vector<vector<glm::vec2>> p(2, vector<glm::vec2>(2));
    glm::vec2 d, res;
    p[0][0] = gridData[(int)tmpX.x][(int)tmpY.x];
    p[0][1] = gridData[(int)tmpX.x][(int)tmpY.x + 1];
    p[1][0] = gridData[(int)tmpX.x + 1][(int)tmpY.x];
    p[1][1] = gridData[(int)tmpX.x + 1][(int)tmpY.x + 1];
    d = {tmpX.y, tmpY.y};
    return  p[0][0] * (1 - d.x) * (1 - d.y) +
            p[0][1] * (1 - d.x) * d.y +
            p[1][0] * d.x * (1 - d.y) +
            p[1][1] * d.x * d.y;
}

// create the image space
void TEX::creFlow(const glm::vec2& _texSZ, const float& _h) {
    texSZ = _texSZ; h = _h;
    flow.clear(); 
    for(int i = 0; i < 256; ++i) speedCnt[i] = 0.0f;
    minFlowCNT = min(texSZ.x / gridSZ.x, texSZ.y / gridSZ.y) * 5; // streanline threshold
    vector<vector<bool>> vis(texSZ.x, vector<bool>(texSZ.y, false)); // cell is passed or not
    glm::vec2 BLK = {gridSZ.x / texSZ.x, gridSZ.y / texSZ.y}; //transform the V from grid to flow
    int cnt = 0;
    flowRange = {100.0f, 0.0f};
    for (int x = 0; x < texSZ.x; x += max((int)(texSZ.x / gridSZ.x * 0.5), 1)) {
        for (int y = 0; y < texSZ.y; y += max((int)(texSZ.y / gridSZ.y * 0.5), 1)) {
            if (vis[x][y]) continue; // if pass, don't vis again
            vector<pair<glm::vec2, float>> lines[2];
            pair<glm::vec2, glm::vec2> line;
            glm::vec2 tmpX, tmpY, v; // divide integer and decimal part
            float len;
            line.fir = {1.0f * x, 1.0f * y}; // start to seed pnt
            tmpX.DEC = modf(BLK.x * x, &tmpX.INT);
            tmpY.DEC = modf(BLK.y * y, &tmpY.INT);
            if (tmpX.x < 0 || tmpX.x >= gridSZ.x - 1 || tmpY.x < 0 || tmpY.x >= gridSZ.y - 1) continue; // out to grid space
            v = interpolate(tmpX, tmpY); //find the pnt's V
            len = (glm::length(v));
            flowRange.MXN = max(flowRange.MXN, len); flowRange.MN = min(flowRange.MN, len);
            for(int dir = 0; dir < 2; ++dir){
                lines[dir].clear();  lines[0].push_back({{x, y},  len});
                for(int iter = 0; iter < 30; ++iter){
                    // RK2 Method
                    tmpX.DEC = modf(BLK.x * line.fir.x, &tmpX.INT);
                    tmpY.DEC = modf(BLK.y * line.fir.y, &tmpY.INT);
                    if (tmpX.x < 0 || tmpX.x >= gridSZ.x - 1 || tmpY.x < 0 || tmpY.x >= gridSZ.y - 1) break; // out to grid space

                    glm::vec2 k1 = interpolate(tmpX, tmpY); //find the pnt's V
                    k1 = glm::normalize(k1);
                    glm::vec2 p1 = line.fir + k1 * (dir? h : -h);
                    if (p1.x < 0.0f || p1.x >= texSZ.x || p1.y < 0.0f || p1.y >= texSZ.y) break;  // out to image space
                    tmpX.DEC = modf(BLK.x * p1.x, &tmpX.INT);
                    tmpY.DEC = modf(BLK.y * p1.y, &tmpY.INT);
                    if (tmpX.x < 0 || tmpX.x >= gridSZ.x - 1 || tmpY.x < 0 || tmpY.x >= gridSZ.y - 1) break; // out to grid space

                    glm::vec2 k2 = interpolate(tmpX, tmpY); //find the pnt's V
                    k2 = glm::normalize(k2);
                    line.sec = line.fir + 0.5f * (k1 + k2); // RK2
                    if (line.sec.x < 0.0f || line.sec.x >= texSZ.x || line.sec.y < 0.0f || line.sec.y >= texSZ.y) break;  // out to image space
                    glm::vec2 p2 = glm::floor(line.sec);
                    glm::vec2 p0 = glm::floor(line.fir);
                
                    if (vis[p2.x][p2.y]) break;  // if pass, don't vis again
                    // vis[p2.x][p2.y] = true;
                    if(p2 != p0) {
                        tmpX.DEC = modf(BLK.x * p2.x, &tmpX.INT);
                        tmpY.DEC = modf(BLK.y * p2.y, &tmpY.INT);
                        if (tmpX.x < 0 || tmpX.x >= gridSZ.x - 1 || tmpY.x < 0 || tmpY.x >= gridSZ.y - 1) break; // out to grid space
                        v = interpolate(tmpX, tmpY); //find the pnt's V
                        len = glm::length(v);
                        flowRange.MXN = max(flowRange.MXN, len); flowRange.MN = min(flowRange.MN, len);
                        lines[dir].push_back({{p2.x, p2.y}, len});
                    }
                    line.fir = line.sec;
                }
                 for (int i = 0; i < lines[dir].size(); ++i){
                    vis[(int)lines[dir][i].fir.x][(int)lines[dir][i].fir.y] = true;
                }
            }
            if (lines[0].size() + lines[1].size() < minFlowCNT ) { //threshold
                for(int dir = 0; dir < 2; ++dir){
                    for (int i = 0; i < lines[dir].size(); ++i){
                        vis[(int)lines[dir][i].fir.x][(int)lines[dir][i].fir.y] = false;
                    }
                }
            } else {
                vector<pair<glm::vec2, float>> douLines;
                for (int i = lines[1].size() - 1; i > 0; --i){
                    douLines.push_back(lines[1][i]);
                }
                for (int i = 0; i < lines[0].size(); ++i){
                    douLines.push_back(lines[0][i]);
                }
                flow.push_back(douLines);
            }
        }
    }
    for(int i = 0;i < flow.size(); ++i){
        for(int j = 0; j < flow[i].size(); ++j){
            flow[i][j].sec = (flow[i][j].sec - flowRange.MN) / (flowRange.MXN - flowRange.MN) * 254.0 + 1.0f;
            speedCnt[(int)flow[i][j].sec] += 1.0f;
            ++cnt;
        }
    }
    speedCnt[0] = texSZ.x * texSZ.y - cnt;
    for(int i = 0; i < 256; ++i){
        if(speedCnt[i]) speedCnt[i] = log(speedCnt[i]);
    }
    cerr << "success to cre the flow\n";
    cerr << "it generate " << cnt << " pnt\n";
}

// filter the 2D array
void TEX::filter2D(unsigned char* value, const glm::vec2& imageSZ) {
    unsigned char* _value = new unsigned char[(int)imageSZ.x * (int)imageSZ.y];
    std::vector<std::vector<float>> kernel(filSZ, std::vector<float>(filSZ));
    int rad = 5 >> 1;
    float weight = 0.0f;
    float sigma = 1.0f;

    // Generate kernel
    for (int i = -rad; i <= rad; ++i) {
        for (int j = -rad; j <= rad; ++j) {
            kernel[i + rad][j + rad] = exp(-0.5f * (i * i + j * j) / (sigma * sigma));
            weight += kernel[i + rad][j + rad];
        }
    }

    // Normalize kernel
    for (int i = 0; i < filSZ; ++i) {
        for (int j = 0; j < filSZ; ++j) {
            kernel[i][j] /= weight;
        }
    }

    // Apply filter
    for (int i = 0; i < texSZ.x; ++i) {
        for (int j = 0; j < texSZ.y; ++j) {
            float sum = 0.0f;
            float _weight = 0.0f;
            for (int ki = -rad; ki <= rad; ++ki) {
                for (int kj = -rad; kj <= rad; ++kj) {
                    int ni = i + ki;
                    int nj = j + kj;
                    if (ni >= 0 && ni < texSZ.x && nj >= 0 && nj < texSZ.y) {
                        sum += value[ni * (int)texSZ.y + nj] * kernel[ki + rad][kj + rad];
                        _weight += kernel[ki + rad][kj + rad];
                    }
                }
            }
            _value[i * (int)texSZ.y + j] = std::min(255.0f, std::max(0.0f, sum / _weight));
        }
    }
    memcpy(value, _value, (int)imageSZ.x * (int)imageSZ.y * sizeof(unsigned char));
    delete[] _value;
}

// create the noise
void TEX::creNoise(const int& _noiseMethod) { 
    if (noise) delete[] noise; 
    noiseMethod = _noiseMethod;
    filSZ = std::max(5, minFlowCNT / 2 * 2 + 1);
    noise = new unsigned char[(int)texSZ.x * (int)texSZ.y];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disX(0, texSZ.x - 1), disY(0, texSZ.y - 1);
    
    if (noiseMethod == 0) { // white method
        std::uniform_int_distribution<> whiteColor(0, 255);
        // std::normal_distribution<> whiteColor(128, 43); // Mean 128, StdDev 20

        for (int i = 0; i < texSZ.x; ++i) {
            for (int j = 0; j < texSZ.y; ++j) {
                int value = whiteColor(gen);
                noise[i * (int)texSZ.y + j] = std::min(std::max(value, 0), 255);
            }
        }
    } else { // spot method
        std::uniform_int_distribution<> r(0, 10);
        std::normal_distribution<> spotColor(128, 43); // Mean 128, StdDev 20
        std::unordered_set<int> vis;
        while (vis.size() < texSZ.x * texSZ.y) {
            int posX = disX(gen), posY = disY(gen);
            int value = static_cast<int>(spotColor(gen));
            int rad = r(gen);
            for (int i = -rad; i <= rad; ++i) {
                if (posX + i < 0 || posX + i >= texSZ.x) continue;
                for (int j = -rad; j <= rad; ++j) {
                    if (posY + j < 0 || posY + j >= texSZ.y) continue;
                    if (sqrt(i * i + j * j) > rad) continue;
                    int pos = (posX + i) * texSZ.y + (posY + j);
                    vis.insert(pos);
                    noise[pos] = std::min(std::max(value, 0), 255);
                }
            }
        }
    }
    filter2D(noise, texSZ);
    std::cerr << "success to create noise\n";
}

// filter the 1D data array
void TEX::filter1D(float* value, const int& imageSZ) {
    float* _value = new float[imageSZ];
    int rad = filSZ >> 1;
    std::vector<float> kernel(filSZ);
    float sigma = 4.0f;
    float weight = 0.0f;

    // Generate kernel
    for (int i = -rad; i <= rad; ++i) {
        if (filMethod == 0)  // Tent
            kernel[i + rad] = 1.0f;
        else if (filMethod == 1) // Box
            kernel[i + rad] = 1.0f + abs(rad - i) * 5;
        else // Gaussian
            kernel[i + rad] = exp(-0.5f * pow(1.0f * i / sigma, 2));
        weight += kernel[i + rad];
    }

    // Normalize kernel
    for (int i = 0; i < filSZ; ++i) kernel[i] /= weight;

    // Apply filter
    for (int i = 0; i < imageSZ; ++i) {
        float sum = 0.0f;
        float _weight = 0.0f;
        for (int j = -rad; j <= rad; ++j) {
            if (i + j >= 0 && i + j < imageSZ) {
                sum += value[i + j] * kernel[j + rad];
                _weight += kernel[j + rad];
            }
        }
        _value[i] = sum / _weight;
    }
    memcpy(value, _value, imageSZ * sizeof(float));
    delete[] _value;
}

// assign flow on the noise texture
void TEX::loadFlow(const int& _filMethod) {
    filMethod = _filMethod;
    unsigned char* IMG = new unsigned char[(int)texSZ.x * (int)texSZ.y * 4];
    unsigned char* color = new unsigned char[(int)texSZ.x * (int)texSZ.y * 4];

    // Initialize the 2D texture
    for (int i = 0; i < texSZ.x; ++i) {
        for (int j = 0; j < texSZ.y; ++j) {
            IMG[(i * (int)texSZ.y + j) * 4 + 0] = noise[i * (int)texSZ.y + j];
            IMG[(i * (int)texSZ.y + j) * 4 + 1] = noise[i * (int)texSZ.y + j];
            IMG[(i * (int)texSZ.y + j) * 4 + 2] = noise[i * (int)texSZ.y + j];
            IMG[(i * (int)texSZ.y + j) * 4 + 3] = 255;

            color[(i * (int)texSZ.y + j) * 4 + 0] = 0;
            color[(i * (int)texSZ.y + j) * 4 + 1] = 0;
            color[(i * (int)texSZ.y + j) * 4 + 2] = 0;
            color[(i * (int)texSZ.y + j) * 4 + 3] = 0;
        }
    }

    // Add the flow on the texture
    for (const auto& lines : flow) {
        float* _flow = new float[lines.size()];
        float* _color = new float[lines.size()];
        int imageSZ = lines.size();
        for (int i = 0; i < imageSZ; ++i) {
            glm::vec2 p = glm::floor(lines[i].fir);
            _flow[i] = noise[(int)(p.x * texSZ.y + p.y)];
            _color[i] = lines[i].sec;
        }
        filter1D(_flow, imageSZ); // Convolution the streamline
        // filter1D(_color, imageSZ); // Convolution the streamline (if needed)
        for (int i = 0; i < imageSZ; ++i) {
            glm::vec2 p = glm::floor(lines[i].fir);
            IMG[(int)((p.x * texSZ.y + p.y) * 4 + 0)] = _flow[i];
            IMG[(int)((p.x * texSZ.y + p.y) * 4 + 1)] = _flow[i];
            IMG[(int)((p.x * texSZ.y + p.y) * 4 + 2)] = _flow[i];
            color[(int)((p.x * texSZ.y + p.y) * 4 + 3)] = std::min(std::max(lines[i].sec, 1.0f), 255.0f);
        }
        delete[] _flow;
        delete[] _color;
    }

    // Texture 0
    glGenTextures(1, &texID[0]);
    glBindTexture(GL_TEXTURE_2D, texID[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)texSZ.x, (int)texSZ.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, IMG);

    // Texture 2
    glGenTextures(1, &texID[2]);
    glBindTexture(GL_TEXTURE_2D, texID[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)texSZ.x, (int)texSZ.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, color);

    std::cerr << "success to create noise texture\n";
    delete[] IMG;
    delete[] color;
}

// create 1D texture(transform funstion)
void TEX::creTRAN(const unsigned char* _color){
    glGenTextures(1, &texID[1]);
    glBindTexture(GL_TEXTURE_1D, texID[1]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, _color);
}

// if somethind change, should be reload
void TEX::update(const string& _dataName, const glm::vec2& _texSZ, const float& _h, const int& _noiseMethod, const int& _filMethod, unsigned char* color){
    if(dataName != _dataName){
        cerr << 1 << "\n";
        loadGrid(_dataName); creFlow(texSZ, _h); loadFlow(_noiseMethod);
    } else if(texSZ != _texSZ){
        cerr << 2 << "\n";
        creFlow(_texSZ, _h); creNoise(_noiseMethod); loadFlow(_filMethod);
    } else if(h != _h){
        cerr << 3 << "\n";
        creFlow(_texSZ, _h); loadFlow(_filMethod);
    }
     else if(noiseMethod != _noiseMethod){
        cerr << 4 << "\n";
        creNoise(_noiseMethod); loadFlow(_filMethod);
    } else if(filMethod != _filMethod){
        cerr << 5 << "\n";
        loadFlow(_filMethod);
    }
    creTRAN(color);
}

void TEX::use()const{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, texID[1]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texID[2]);
}