#ifndef CAMERA_H
#define CAMERA_H
#define fir first
#define sec second
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CAMERA {
public:
	CAMERA(const glm::vec3& _pos, const glm::vec3& _up): 
        pos(_pos), worldUp(_up), zoom(45.0f), zNear(0.1f), zFar(1000.0f), pitch(0.0f), yaw(0.0f){
        rotMode = 0;
        currentTime = glfwGetTime();
    };
    glm::mat3 getView()const{
        glm::vec3 _front, _right, _up;
        _front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        _front.y = sin(glm::radians(pitch));
        _front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        _front = glm::normalize(_front);
        // also re-calculate the Right and Up vector
        // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        _right = glm::normalize(glm::cross(_front, worldUp)); 
        _up    = glm::normalize(glm::cross(_right, _front));
        return glm::lookAt(pos, pos + _front, _up);
    }
    glm::mat3 getProjection(const unsigned int& SCR_WIDTH, const unsigned int& SCR_HEIGHT, const float& offect)const{
        // glm::ortho( left, right, down, up, near, far);
        return glm::ortho( -250.0f * offect, 250.0f * offect, -250.0f * (SCR_WIDTH / SCR_HEIGHT) * offect, 250.0f * (SCR_WIDTH / SCR_HEIGHT) * offect, zNear, zFar);
        //return glm::ortho(-10.0f, 300.0f, -10.0f * (float)SCR_HEIGHT / (float)SCR_WIDTH, 300.0f * (float)SCR_HEIGHT / (float)SCR_WIDTH, 5.0f, 1000.0f);
        //return glm::perspective(glm::radians(zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, zNear, zFar);
    }
    glm::vec3 getPosition()const{
        return pos;
    }
    glm::vec3 getSeeDir()const{
        glm::vec3 _front;
        _front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        _front.y = sin(glm::radians(pitch));
        _front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        _front = glm::normalize(_front);
        return _front;
    }
    pair<float, float> getRot()const{
        return {yaw, pitch};
    }
    int getMode()const{
        return rotMode;
    }
	void update(const pair<float, float>& cmrRot, const int& _rotMode){
        rotMode = _rotMode;
	    float oldTime = currentTime;
        currentTime = glfwGetTime();
	    float dr = (float)(currentTime - oldTime) * 30.0f;
        if(rotMode == 0){
            yaw = cmrRot.fir;
            pitch = cmrRot.sec;
        }
        else if(rotMode == 1){
            yaw = yaw + dr;
            while(yaw >= 360.0f) yaw -= 360.0f;
        }
        else{
            pitch = pitch + dr;
            while(pitch >= 89.999) pitch = -89.999 + (pitch - 89.999);
        }
    }
private:
    glm::vec3 pos, worldUp;
    float zoom, zNear, zFar, yaw, pitch;
    float currentTime;
    int rotMode; 
};

#endif