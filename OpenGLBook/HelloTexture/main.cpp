#include <glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <stb/stb_image.h>

#include "libs/shaderClass.h"
#include "libs/VBO.h"
#include "libs/VAO.h"
#include "libs/EBO.h"
#include "libs/Texture.h"



GLFWwindow* glINIT(int WIDTH, int HEIGHT);

int WIDTH = 800;
int HEIGHT = 600;
#define numVertices(x) (sizeof(x) / sizeof((x)[0])/3)
#define arrayLen(x) (sizeof(x) / sizeof((x)[0]))

GLfloat vertices[] = { 
    //  COORDINATES   /      /     COLORS  /   
	-0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,     0.0f, 0.0f,
	-0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
	 0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
	 0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,     1.0f, 0.0f,
};

GLuint indices[] = {
    0, 2, 1,
    0, 3, 2
};

int main(){
    GLFWwindow* window = glINIT(WIDTH, HEIGHT);

    Shader shaderProgram("Shaders/default.vert","Shaders/default.frag");
    VAO VAO1;
    VAO1.Bind();

    VBO VBO1(vertices, sizeof(vertices));
    EBO EBO1(indices, sizeof(indices));


    /*
        Our array is like:
            x|y|z|r|g|b|x|y|z|r|g|b|x|y|z|r|g|b|
        -The coordinates is at layout 0 and the colour is in the next layout (1)
        -The stride from one set of coords to the other is 6*float since it has to
            go over xyzrgb to get to the next xyz.
        -The offset for the coords is 0 since it is at the start of the array
        -The stride for the colors is the same 6*float
        -The offset for the colors is 3*float since the first color data is after
            the first xyz
    */
    //This specified the attribute for the coordinates
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8*sizeof(float), (void*) 0);
    //This specified the attributes for the color
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8*sizeof(float), (void*)(3*sizeof(float)));
    //This specified the attributes for the texture
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8*sizeof(float), (void*)(6*sizeof(float)));
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    GLuint scaleUniId = glGetUniformLocation(shaderProgram.ID, "scale");

    //Image DIMs
    Texture popCat("Textures/catto.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
    popCat.texUnit(shaderProgram, "tex0", 0);

    while(!glfwWindowShouldClose(window)){
        shaderProgram.Activate();
        glUniform1f(scaleUniId, 0.5f);
        popCat.Bind();
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, arrayLen(indices), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram.Delete();
    popCat.Delete();
    glfwTerminate();
    return 0;
}

GLFWwindow* glINIT(int WIDTH, int HEIGHT){
    GLFWwindow* window = NULL;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "HelloTriangle", NULL, NULL);

    if(!window){
        std::cout << "ERROR::WINDOW::CREATION::Failed to create window"<< std::endl;
        exit(-1);
    }

    //MAKE THE WINDOW THE CONTEXT BEFORE LOADING OPENGL STUFF
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
        std::cout << "Failed to initalise GLAD" << std::endl;
        exit(-1);
    }
    
    glViewport(0, 0, WIDTH, HEIGHT);

    return window;
}