#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shaderClass.h"

class Camera{
    public:
        glm::vec3 Position;
        glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

        int screenWidth;
        int screenHeight;
        bool firstClick;

        const float L_SHIFT_SPEED = 0.1f;
        const float NORM_SPEED = 0.01f;
        float speed = NORM_SPEED;

        float sensitivity = 1.0f;


        Camera(int width, int height, glm::vec3 position);

        void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader &shader, const char* uniform);
        void Inputs(GLFWwindow* window);
};


#endif