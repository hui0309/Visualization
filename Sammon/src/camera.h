#define fir first
#define sec second
class CAMERA {
public:
    CAMERA(const glm::vec3& _pos, const glm::vec3& _up) : pos(_pos), worldUp(_up), zoom(45.0f), zNear(0.1f), zFar(1000.0f) {
        rotMode = 0;
        currentTime = glfwGetTime();
        yaw = -90.0f; pitch = 0.0f;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
    // get canera space
    glm::mat4 getView() const {
        return glm::lookAt(pos, pos + front, up);
    }
    // get projection space
    glm::mat4 getProjection(const glm::vec2& screenSZ, const float& offset) const {
        return glm::ortho(  -offset * 4.0f, offset * 4.0f,
                            -offset * 2.0f * (screenSZ.x / screenSZ.y) , offset * 4.0f * (screenSZ.x / screenSZ.y) ,
                            zNear, zFar );
    }

private:
    glm::vec3 pos, front, up, right, worldUp;
    float yaw, pitch, zoom, zNear, zFar, currentTime;
    int rotMode;
};
