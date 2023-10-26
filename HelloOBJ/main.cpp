#include <glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "libs/Mesh.h"
#include "libs/ModelLoader.h"



GLFWwindow* glINIT(int WIDTH, int HEIGHT);

int WIDTH = 800;
int HEIGHT = 800;

// Vertex vertices[] =
// { //               COORDINATES           /            COLORS          /           NORMALS         /       TEXTURE COORDINATES    //
// 	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
// 	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
// 	Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
// 	Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
// };

// // Indices for vertices order
// GLuint indices[] =
// {
// 	0, 1, 2,
// 	0, 2, 3
// };

// Vertex lightVertices[] =
// { //     COORDINATES     //
// 	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
// 	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
// 	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
// 	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
// 	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
// 	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
// 	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
// 	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
// };

// GLuint lightIndices[] =
// {
// 	0, 1, 2,
// 	0, 2, 3,
// 	0, 4, 7,
// 	0, 7, 3,
// 	3, 7, 6,
// 	3, 6, 2,
// 	2, 6, 5,
// 	2, 5, 1,
// 	1, 5, 4,
// 	1, 4, 0,
// 	4, 5, 6,
// 	4, 6, 7
// };


int main(){
    GLFWwindow* window = glINIT(WIDTH, HEIGHT);

    Texture textures[]{
        Texture("Textures/planks.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
        Texture("Textures/planksSpecular.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
    };

    Shader shaderProgram("Shaders/default.vert","Shaders/default.frag");

    ModelLoader floorLoader("Objects/floor.obj");
    std::vector<Texture> tex(textures, textures + sizeof(textures)/sizeof(Texture));
    
    Mesh floor = floorLoader.loadMesh();
    floor.textures = tex;


    Shader lightShader("Shaders/light.vert","Shaders/light.frag");
    
    ModelLoader lightLoader("Objects/light.obj");
    Mesh light = lightLoader.loadMesh();
    light.textures = tex;

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);


    glm::vec3 pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 pyramidModel = glm::mat4(1.0f);
    pyramidModel = glm::translate(pyramidModel, pyramidPos);


    lightShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
    glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"),lightColor.x, lightColor.y, lightColor.z, lightColor.w);

    shaderProgram.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"),lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPosition"),lightPos.x, lightPos.y, lightPos.z);

    glEnable(GL_DEPTH_TEST);

    Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

    while(!glfwWindowShouldClose(window)){
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        shaderProgram.Activate();
        floor.Draw(shaderProgram, camera);
        light.Draw(lightShader, camera);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();
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