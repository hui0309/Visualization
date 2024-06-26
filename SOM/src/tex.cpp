#define STB_IMAGE_IMPLEMENTATION
#include "./tex.h"
#include "./std_image.h"

#define X first
#define Y second
#define fir first
#define sec second
#define INT x
#define DEC y
#define MN x
#define MXN y

TEX::TEX():selectData(0) {
    dataName[0] = "./IMG/flower.jpg";
    dataName[1] = "./IMG/skylight.jpg";
    dataName[2] = "./IMG/beach.jpg";

    for (int i = 0; i < 3; ++i) {
        glGenTextures(1, &texID[i]);
        glBindTexture(GL_TEXTURE_2D, texID[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        unsigned char *data = stbi_load(dataName[i], &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
}

// destructor TEX
TEX::~TEX() {
    // release space
    for (int i = 0; i < 3; ++i) {
        delete[] dataName[i];
    }
}

void TEX::use() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID[selectData]);
}
