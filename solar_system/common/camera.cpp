#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"

using namespace glm;

Camera::Camera(GLFWwindow* window) : window(window)
{
    position = glm::vec3(0, 0, 35);
    horizontalAngle = 3.14f;
    verticalAngle = 0.0f;
    FoV = 60.0f;
    speed = 7.0f;
    mouseSpeed = 0.001f;
    fovSpeed = 0.5f;
    x = 1;
}

void Camera::update()
{
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Get mouse position
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Reset mouse position for next frame
    glfwSetCursorPos(window, width / 2, height / 2);

    // New horizontal and vertical angles, given windows size and cursor position
    horizontalAngle += mouseSpeed * float(width/2 - xPos);
    verticalAngle += mouseSpeed * float(height / 2 - yPos);

    // Right and up vectors of the camera coordinate system
    // use spherical coordinates
    vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // Right vector
    vec3 right(
        sin(horizontalAngle - 3.14f/2.0f),
        0.0,
        cos(horizontalAngle - 3.14f / 2.0f)
    );

    // Up vector
    vec3 up = cross(right, direction);

    // Update camera position using the direction/right vectors
    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += direction * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= direction * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += right * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= right * deltaTime * speed;
    }
    // Move up
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        position += up * deltaTime * speed;
    }
    // Move down
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        position -= up * deltaTime * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
        x = 0;
        position = vec3(0,30,0);
        verticalAngle = -3.14/2;
    }

    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
        x = 0;
        position = vec3(0, 3, 30);
        verticalAngle = 0;
    }
    /*if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        position = vec3(0, 3, 20);
        x = 0;       
    }*/
    

    // Projection and view matrices
    projectionMatrix = perspective(radians(FoV), 4.0f/3.0f, 0.1f, 200.0f);
    viewMatrix = lookAt(
        position,
        position + direction,
        up
    );
    
    if(x==1)
    viewMatrix *= rotate(mat4(), 3.14f / 9, vec3(1.0f, 0.0f, 0.0f));

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}