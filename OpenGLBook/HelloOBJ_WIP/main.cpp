#include <glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

#include "libs/shaderClass.h"
#include "libs/VBO.h"
#include "libs/VAO.h"
#include "libs/EBO.h"
#include "libs/Texture.h"
#include "libs/Camera.h"
#include "libs/ObjLoader.h"



GLFWwindow* glINIT(int WIDTH, int HEIGHT);

int WIDTH = 800;
int HEIGHT = 800;
#define numVertices(x) (sizeof(x) / sizeof((x)[0])/3)
#define arrayLen(x) (sizeof(x) / sizeof((x)[0]))

// glm::vec3* vertices;
// GLuint* indices;
int sizeVert, sizeIdx;


// GLfloat vertices[] =
// { //   COORDINATES   /      /      COLORS     /     TexCoord //
// 	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
// 	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
// 	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
// 	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
// 	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
// };

glm::vec3 vertices[] = {
    glm::vec3(-0.5,0.0,0.5),
    glm::vec3(-0.5,0.0,-0.5),
    glm::vec3(0.5,0.0,-0.5),
    glm::vec3(0.5,0.0,0.5),
    glm::vec3(0.0,0.8,0.0),
    glm::vec3(1.0),
    glm::vec3(1.0),
    glm::vec3(1.0),
    glm::vec3(1.0),
    glm::vec3(1.0)
};



// // Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};

int main(){
    GLFWwindow* window = glINIT(WIDTH, HEIGHT);

    Shader shaderProgram("Shaders/default.vert","Shaders/default.frag");
    // objLoader pyramid("Objects/pyramid.obj");
    // pyramid.loadVectorData(&vertices);
    // pyramid.loadDefaultColorData(&vertices, glm::vec3(0.5f));
    // pyramid.loadIndexData(&indices);
    // sizeVert = pyramid.numVertexes;
    // sizeIdx = pyramid.numIndices;

    // for(int i = 0; i < sizeIdx*3; i+=3){
    //     std::cout << indices[i] << " " << indices[i+1] << " " << indices[i+2] << std::endl;
    // }

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
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 3*sizeof(float), (void*) 0);
    //This specified the attributes for the color
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 3*sizeof(float), (void*)(5*3*sizeof(float)));
    // //This specified the attributes for the texture
    // VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8*sizeof(float), (void*)(6*sizeof(float)));
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();



    // //Image DIMs
    // Texture popCat("Textures/obamna.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    // popCat.texUnit(shaderProgram, "tex0", 0);

    glEnable(GL_DEPTH_TEST);

    Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

    // for(int i = 0; i < sizeVert; i++){
    //     std::cout << glm::to_string(vertices[i]) << std::endl;
    // }


    while(!glfwWindowShouldClose(window)){
        shaderProgram.Activate();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Inputs(window);
        camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

        //popCat.Bind();
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram.Delete();
    //popCat.Delete();
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