#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CAMERA {
public:
	CAMERA(const glm::vec3& _pos, const glm::vec3& _view, const glm::vec3& _up):
        pos(_pos), up(_up), view(_view), zoom(45.0f), zNear(0.1f), zFar(1000.0f){};
    glm::mat3 getView()const{
        return glm::lookAt(pos, view, up);
    }
    glm::mat3 getProjection(const unsigned int& SCR_WIDTH, const unsigned int& SCR_HEIGHT)const{
        //return glm::ortho(-10.0f, 300.0f, -10.0f * (float)SCR_HEIGHT / (float)SCR_WIDTH, 300.0f * (float)SCR_HEIGHT / (float)SCR_WIDTH, 5.0f, 1000.0f);
        return glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, -1.0f, 1.0f);
        //return glm::perspective(glm::radians(zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, zNear, zFar);
    }
    glm::vec3 getPosition()const{
        return pos;
    }
    glm::vec3 getSeeDir()const{
        return view - pos;
    }
	// void update_pos(const float&, const float&, const float&, const float&);
	// void update_view(const float&, const float&, const float&, const float&);
	const float u[3][3] = { {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0} };
	// glm::vec3 cen_view = view - pos;
	// glm::vec3 now_view = cen_view;
private:
    glm::vec3 pos, up, view;
    float zoom, zNear, zFar;
};

#endif