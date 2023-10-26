#include <glfw3.h>
#include <iostream>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include "light.h"

using namespace glm;

Light::Light(GLFWwindow* window,
    glm::vec4 init_La,
    glm::vec4 init_Ld,
    glm::vec4 init_Ls,
    glm::vec3 init_position,
    float init_power) : window(window) {
    La = init_La;
    Ld = init_Ld;
    Ls = init_Ls;
    power = init_power;
    lightPosition_worldspace = init_position;

    // setting near and far plane affects the detail of the shadow
    nearPlane = 0.1;
    farPlane = 100.0;

    //projectionMatrix = ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);
    //orthoProj = true;
}


mat4 Light::lightVP1() {
    viewMatrix = lookAt(lightPosition_worldspace, lightPosition_worldspace + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
    return projectionMatrix * viewMatrix;
}

mat4 Light::lightVP2() {
    viewMatrix = lookAt(lightPosition_worldspace, lightPosition_worldspace + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
    return projectionMatrix * viewMatrix;
}

mat4 Light::lightVP3() {
    viewMatrix = lookAt(lightPosition_worldspace, lightPosition_worldspace + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
    return projectionMatrix * viewMatrix;
}

mat4 Light::lightVP4() {
    viewMatrix = lookAt(lightPosition_worldspace, lightPosition_worldspace + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
    return projectionMatrix * viewMatrix;
}

mat4 Light::lightVP5() {
    viewMatrix = lookAt(lightPosition_worldspace, lightPosition_worldspace + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
    return projectionMatrix * viewMatrix;
}

mat4 Light::lightVP6() {
    viewMatrix = lookAt(lightPosition_worldspace, lightPosition_worldspace + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
    return projectionMatrix * viewMatrix;
}