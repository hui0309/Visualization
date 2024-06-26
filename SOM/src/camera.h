#define fir first
#define sec second
class CAMERA {
public:
    CAMERA(const glm::vec3& _pos, const glm::vec3& _up) : pos(_pos), worldUp(_up), zoom(45.0f), zNear(0.1f), zFar(1000.0f) {
        yaw = -90.0f; pitch = 0.0f;
        update();
    }
    // get canera space
    glm::mat4 getView() const {
        return glm::lookAt(pos, pos + front, up);
    }
    // get projection space
    glm::mat4 getProjection(const glm::vec2& screenSZ, const float& offset) const {
        return glm::ortho(  -offset * 2.0f, offset * 1.5f,
                            -offset * 2.0f * (screenSZ.x / screenSZ.y) , offset * 1.5f * (screenSZ.x / screenSZ.y) ,
                            zNear, zFar );
    }
    void update(){
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        right = glm::normalize(glm::cross(front, worldUp)); 
        up    = glm::normalize(glm::cross(right, front));
    }
    glm::vec3 pos, front, up, right, worldUp;
    float yaw, pitch, zoom, zNear, zFar;
private:
};
