#include <glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <dlfcn.h>

#include "libs/Mesh.h"
#include "libs/Model.h"
#include "libs/miniaudio.h"

GLFWwindow* glINIT(int WIDTH, int HEIGHT);
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);


int WIDTH = 1920;
int HEIGHT = 1080;



int main(){
    //Defining audio context
    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    result = ma_decoder_init_file("Sounds/Circus.mp3", NULL, &decoder);

    if(result != MA_SUCCESS){
        std::cout << ma_result_description(result) << std::endl;
        return -1;
    }

    ma_data_source_set_looping(&decoder, MA_TRUE);

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;
    deviceConfig.periodSizeInMilliseconds = 200;

    result = ma_device_init(NULL, &deviceConfig, &device);

    if(result != MA_SUCCESS){
        std::cout << ma_result_description(result) << std::endl;
        return -1;
    }

    result = ma_device_start(&device);
    
    if(result != MA_SUCCESS){
        std::cout << ma_result_description(result) << std::endl;
        return -1;
    }



    GLFWwindow* window = glINIT(WIDTH, HEIGHT);
    glm::vec2 resVec = glm::vec2(WIDTH, HEIGHT);

    Shader roadShader("Shaders/default.vert","Shaders/default.frag");
    Shader tower1Shader("Shaders/default.vert","Shaders/default.frag");
    Shader tower2Shader("Shaders/default.vert","Shaders/default.frag");


    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);

    roadShader.Activate();
    glUniform4f(glGetUniformLocation(roadShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(roadShader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);

    tower1Shader.Activate();
    glUniform4f(glGetUniformLocation(tower1Shader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(tower1Shader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);

    tower2Shader.Activate();
    glUniform4f(glGetUniformLocation(tower2Shader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(tower2Shader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);


    glEnable(GL_DEPTH_TEST);

    Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 10.0f));

    //90 aboutz
    glm::mat4 initTransform = glm::mat4(1.0f);
    initTransform = glm::rotate(initTransform, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    Model road("Objects/FinalScene/scene.gltf", 4.0f, initTransform);
    
    
    initTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    initTransform = glm::translate(initTransform, glm::vec3(-3.2f, 3.0f, 0.0f));
    Model tower1("Objects/lowpolybuilding/scene.gltf", 0.2, initTransform);

    initTransform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    initTransform = glm::translate(initTransform, glm::vec3(-3.2f, -3.0f, 0.0f));
    Model tower2("Objects/lowpolybuilding/scene.gltf", 0.2, initTransform);

    glm::vec3 fogColor = glm::vec3(0.07f, 0.13f, 0.17f);

    while(!glfwWindowShouldClose(window)){
        glClearColor(fogColor.x, 0.13f, 0.17f, 1.0f);

        roadShader.Activate();
        glUniform3f(glGetUniformLocation(roadShader.ID, "skyColor"), fogColor.x, fogColor.y, fogColor.z);

        tower1Shader.Activate();
        glUniform3f(glGetUniformLocation(tower1Shader.ID, "skyColor"), fogColor.x, fogColor.y, fogColor.z);

        tower2Shader.Activate();
        glUniform3f(glGetUniformLocation(tower2Shader.ID, "skyColor"), fogColor.x, fogColor.y, fogColor.z);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);
        
        //Perhaps Preetham Sky model on GL_CLEAR_COLOR ???


        road.Draw(roadShader, camera, resVec);        
        tower1.Draw(tower1Shader, camera, resVec);
        tower2.Draw(tower2Shader, camera, resVec);
        tower2.handleKeyboardInputs(window, tower2Shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
    roadShader.Delete();
    tower1Shader.Delete();
    tower2Shader.Delete();
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

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount){
    ma_decoder* pDecoder = (ma_decoder*) pDevice->pUserData;

    if(!pDecoder){
        return;
    }

    ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}