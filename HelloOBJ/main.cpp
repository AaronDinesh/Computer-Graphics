#include <glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/constants.hpp>
#include <dlfcn.h>
#include <time.h>

#include "libs/Mesh.h"
#include "libs/Model.h"
#include "libs/miniaudio.h"


enum helicopterState{
    SPOOL_UP,
    LIFT_OFF,
    FLYING,
    FLY_BACK,
    LANDING,
    SPOOL_DOWN,
    SHUT_DOWN,
    TESTING
};

struct helicopterObject{
    enum helicopterState heliState = SHUT_DOWN;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float offsetZ = 0.0f;
    float rotation = 0.0f;
    float MAX_ROTATION = 50.0f;
    float SPOOL_UP_TIME = 3.0f;
    float MAX_HEIGHT = 40.0f;
    float LIFT_OFF_TIME = 4.0f;
    float accumulatedTime = 0.0f;
    float randXMax = 20.0f;
    float randZMax = 20.0f;
    float randXFloat = 0.0f;
    float randZFloat = 0.0f;
    float outerRadiusBound = 45.0f;
    float innerRadiusBound = 5.0f;
    float heading = 0.0f;
    float sceneCenterX = 0.0f;
    float sceneCenterY = 0.0f;
    float sceneCenterZ = 0.0f;

};

enum zombieState{
    START,
    WALKING,
    END
};
struct zombieGroup{
    enum zombieState groupState = START;
    static const int objectCount = 3;
    Model* objects[objectCount];
    Shader* objectShaders[objectCount];
    float interpolationNum = 0.0f;
    float randXFloat = 0.0f;
    float randZFloat = 0.0f;
    float offsetX = 0.0f;
    float xLimMin = -11.0f;
    float xLimMax = 83.0f;
    float offsetZ = 0.0f;
    float zLimMin = -4.0f;
    float zLimMax = 2.8f;
    float heading = 0.0f;
    float accumulatedTime = 0.0f;
};



GLFWwindow* glINIT(int WIDTH, int HEIGHT);
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
void calculate_helicopter_heading(struct helicopterObject* helicopter);

int WIDTH = 1920;
int HEIGHT = 1080;

#define randFloat(a) (((rand()/(float) RAND_MAX)*a) - (a/2.0f))


int main(){
    //srand(time(NULL));
    srand(0);

    //Defining audio context
    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    result = ma_decoder_init_file("Sounds/HorrorSoundFull.mp3", NULL, &decoder);

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

    struct zombieGroup zombieGroup1;
    
    Shader baseSceneShader("Shaders/default.vert","Shaders/default.frag");
    Shader armyHelicopterBodyShader("Shaders/armyHelicopterBody.vert","Shaders/armyHelicopterBody.frag");
    Shader armyHelicopterRotorShader("Shaders/armyHelicopterRotor.vert","Shaders/armyHelicopterRotor.frag");
    Shader zombie1Shader("Shaders/zombie.vert", "Shaders/zombie.frag");
    Shader zombie2Shader("Shaders/zombie.vert", "Shaders/zombie.frag");
    Shader zombie3Shader("Shaders/zombie.vert", "Shaders/zombie.frag");



    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);

    baseSceneShader.Activate();
    glUniform4f(glGetUniformLocation(baseSceneShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(baseSceneShader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);

    armyHelicopterBodyShader.Activate();
    glUniform4f(glGetUniformLocation(armyHelicopterBodyShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(armyHelicopterBodyShader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);

    armyHelicopterRotorShader.Activate();
    glUniform4f(glGetUniformLocation(armyHelicopterRotorShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(armyHelicopterRotorShader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);

    zombie1Shader.Activate();
    glUniform4f(glGetUniformLocation(zombie1Shader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(zombie1Shader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);

    zombie2Shader.Activate();
    glUniform4f(glGetUniformLocation(zombie2Shader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(zombie2Shader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);

    zombie3Shader.Activate();
    glUniform4f(glGetUniformLocation(zombie3Shader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(zombie3Shader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);

    zombieGroup1.objectShaders[0] = &zombie1Shader;
    zombieGroup1.objectShaders[1] = &zombie2Shader;
    zombieGroup1.objectShaders[2] = &zombie3Shader;


    glEnable(GL_DEPTH_TEST);

    Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 10.0f));

    //90 aboutz
    glm::mat4 initTransform = glm::mat4(1.0f);
    initTransform = glm::rotate(initTransform, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 scale = glm::vec3(4.0f, 4.0f, 4.0f);
    Model baseScene("Objects/FinalSceneComplete/scene.gltf", scale, initTransform, true);
    

    scale = glm::vec3(3.0f, 4.5f, 3.0f);
    initTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -20.0f, 10.0f)) * initTransform;
    Model armyHelicopterBody("Objects/ArmyHelicopterBody/scene.gltf", scale, initTransform, true);

    initTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, 7.5f, 0.5f)) * initTransform;
    Model armyHelicopterRotor("Objects/ArmyHelicopterRotor/scene.gltf", scale, initTransform);
    
    initTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-21.5f, -21.5f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    scale = glm::vec3(0.25f, 0.25f, 0.25f);
    Model zombie1("Objects/zombie/scene.gltf", scale, initTransform);
    
    initTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-22.5f, -21.5f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Model zombie2("Objects/zombie/scene.gltf", scale, initTransform);
    
    initTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-20.5f, -21.5f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Model zombie3("Objects/zombie/scene.gltf", scale, initTransform);
    
    zombieGroup1.objects[0] = &zombie1;
    zombieGroup1.objects[1] = &zombie2;
    zombieGroup1.objects[2] = &zombie3; 


    glm::vec3 fogColor = glm::vec3(0.07f, 0.13f, 0.17f);

    struct helicopterObject helicopter;

    double prevTime = glfwGetTime();
    double x = 0.0f;

    float test_angle_rm_later = 0.0f;


    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glClearColor(fogColor.x, fogColor.y, fogColor.z, 1.0f);

        baseSceneShader.Activate();
        glUniform3f(glGetUniformLocation(baseSceneShader.ID, "skyColor"), fogColor.x, fogColor.y, fogColor.z);

        armyHelicopterBodyShader.Activate();
        glUniform3f(glGetUniformLocation(armyHelicopterBodyShader.ID, "skyColor"), fogColor.x, fogColor.y, fogColor.z);

        armyHelicopterRotorShader.Activate();
        glUniform3f(glGetUniformLocation(armyHelicopterRotorShader.ID, "skyColor"), fogColor.x, fogColor.y, fogColor.z);

        for(int i = 0; i < zombieGroup1.objectCount; i++){
            zombieGroup1.objectShaders[i]->Activate();
            glUniform3f(glGetUniformLocation(zombieGroup1.objectShaders[i]->ID, "skyColor"), fogColor.x, fogColor.y, fogColor.z);
        }

        double crntTime = glfwGetTime();

        if(crntTime - prevTime >= 1/3600){
            switch(helicopter.heliState){
                case SPOOL_UP:
                    helicopter.sceneCenterX = baseScene.centerX;
                    helicopter.sceneCenterY = baseScene.centerY;
                    helicopter.sceneCenterZ = baseScene.centerZ;

                    x += 3.0f/(helicopter.SPOOL_UP_TIME*50.0f);
                    helicopter.rotation += ((2.0f/(1.0f+glm::exp(-2.0f*x)))-1.0f)*helicopter.MAX_ROTATION;
                    prevTime = crntTime;
                    std::cout << "X: " << x << std::endl;
                    if(x >= 3){
                        helicopter.heliState = LIFT_OFF;
                        std::cout << "Lift Off!!" << std::endl;
                    }
                    break;
                case LIFT_OFF:
                    helicopter.offsetY += helicopter.MAX_HEIGHT/(helicopter.LIFT_OFF_TIME*250.0f);
                    helicopter.rotation += helicopter.MAX_ROTATION;

                    if(helicopter.offsetY >= helicopter.MAX_HEIGHT){
                        helicopter.heliState = FLYING;
                        helicopter.randXFloat = randFloat(helicopter.randXMax) / 125.0f;
                        helicopter.randZFloat = randFloat(helicopter.randZMax) / 125.0f;

                        calculate_helicopter_heading(&helicopter);
                    }
                    break;
                case FLYING:
                    helicopter.rotation += helicopter.MAX_ROTATION;
                    helicopter.offsetX += helicopter.randXFloat;
                    helicopter.offsetZ += helicopter.randZFloat;


                    if(glm::distance2(glm::vec2(helicopter.offsetX, helicopter.offsetZ), glm::vec2(helicopter.sceneCenterX, helicopter.sceneCenterZ)) >= helicopter.outerRadiusBound*helicopter.outerRadiusBound || glm::distance2(glm::vec2(helicopter.offsetX, helicopter.offsetZ), glm::vec2(helicopter.sceneCenterX, helicopter.sceneCenterZ)) <= helicopter.innerRadiusBound*helicopter.innerRadiusBound){
                        helicopter.randXFloat = -helicopter.randXFloat;
                        helicopter.randZFloat = helicopter.randZFloat;

                        calculate_helicopter_heading(&helicopter);
                    }

                    helicopter.accumulatedTime += crntTime - prevTime;
                    if(helicopter.accumulatedTime >= 10.0f){
                        helicopter.randXFloat = randFloat(helicopter.randXMax) / 64.0f;
                        helicopter.randZFloat = randFloat(helicopter.randZMax) / 64.0f;

                        calculate_helicopter_heading(&helicopter);
                        
                        helicopter.accumulatedTime = 0.0f;
                    }
                    break;
                case FLY_BACK:
                    helicopter.rotation += helicopter.MAX_ROTATION;
                    helicopter.offsetX += helicopter.randXFloat;
                    helicopter.offsetZ += helicopter.randZFloat;


                    if(glm::abs(helicopter.offsetX) <= 1 && glm::abs(helicopter.offsetZ) <= 1){
                        helicopter.heliState = LANDING;
                        helicopter.heading = 0;
                    }
                    break;
                case LANDING:
                    helicopter.offsetY -= helicopter.MAX_HEIGHT/(helicopter.LIFT_OFF_TIME*150.0f);
                    helicopter.rotation += helicopter.MAX_ROTATION;

                    if(helicopter.offsetY <= 0){
                        helicopter.heliState = SPOOL_DOWN;
                    }
                    break;

                case SPOOL_DOWN:
                    x -= 3.0f/(helicopter.SPOOL_UP_TIME*50.0f);
                    helicopter.rotation -= ((2.0f/(1.0f+glm::exp(-2.0f*x)))-1.0f)*helicopter.MAX_ROTATION;
                    prevTime = crntTime;
                    std::cout << "X: " << x << std::endl;
                    if(x <= 0){
                        helicopter.heliState = SHUT_DOWN;
                        std::cout << "SHUT OFF!!" << std::endl;
                    }
                    break;
                case TESTING:
                    calculate_helicopter_heading(&helicopter);
                    std::cout << "Angle: " << glm::degrees(helicopter.heading) << " Offset XZ: " << helicopter.randXFloat << ":" << helicopter.randZFloat <<std::endl;
                    break;
                default:
                    break;
            }

            switch(zombieGroup1.groupState){
                case START:
                    zombieGroup1.randXFloat = randFloat(5)/64.0f;
                    zombieGroup1.randZFloat = randFloat(0.1)/64.0f;
                    zombieGroup1.groupState = WALKING;
                    break;
                case WALKING:
                    if(zombieGroup1.offsetX < zombieGroup1.xLimMin ||  zombieGroup1.offsetX > zombieGroup1.xLimMax){
                        zombieGroup1.randXFloat = -zombieGroup1.randXFloat;
                    }
                    
                    if(zombieGroup1.offsetZ < zombieGroup1.zLimMin ||  zombieGroup1.offsetZ > zombieGroup1.zLimMax){
                        zombieGroup1.randZFloat = -zombieGroup1.randZFloat;
                    }

                    zombieGroup1.offsetX += zombieGroup1.randXFloat;
                    zombieGroup1.offsetZ += zombieGroup1.randZFloat;
                    
                    zombieGroup1.accumulatedTime += crntTime - prevTime;
                    if(zombieGroup1.accumulatedTime >= 10.0f){
                        zombieGroup1.randXFloat = randFloat(5)/64.0f;
                        zombieGroup1.randZFloat = randFloat(5)/64.0f;
                        zombieGroup1.accumulatedTime = 0.0f;
                    }
                default:
                    break;
            }

            prevTime = crntTime;

            if(glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS){
                helicopter.heliState = FLY_BACK;
                helicopter.randXFloat = -helicopter.offsetX / 500.0f;
                helicopter.randZFloat = -helicopter.offsetZ / 500.0f;
                calculate_helicopter_heading(&helicopter);
            }

            if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS){
                helicopter.heliState = SPOOL_UP;
            }
        }

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec3 lightPos = glm::vec3(armyHelicopterBody.centerX, armyHelicopterBody.centerY, armyHelicopterBody.centerZ);

        baseSceneShader.Activate();
        glUniform4f(glGetUniformLocation(baseSceneShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
        glUniform3f(glGetUniformLocation(baseSceneShader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);

        baseScene.Draw(baseSceneShader, camera, resVec);
        armyHelicopterBody.myTotalTransformation = glm::translate(glm::mat4(1.0f), glm::vec3(helicopter.offsetX, -helicopter.offsetY, helicopter.offsetZ)) * glm::translate(glm::mat4(1.0f), glm::vec3(armyHelicopterRotor.centerX, armyHelicopterRotor.centerY, armyHelicopterRotor.centerZ)) * glm::rotate(glm::mat4(1.0f), helicopter.heading, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-armyHelicopterRotor.centerX, -armyHelicopterRotor.centerY, -armyHelicopterRotor.centerZ));
        armyHelicopterBody.Draw(armyHelicopterBodyShader, camera, resVec);
        
        armyHelicopterRotor.myTotalTransformation = glm::translate(glm::mat4(1.0f), glm::vec3(helicopter.offsetX, -helicopter.offsetY, helicopter.offsetZ)) * glm::translate(glm::mat4(1.0f), glm::vec3(armyHelicopterRotor.centerX, armyHelicopterRotor.centerY, armyHelicopterRotor.centerZ)) * glm::rotate(glm::mat4(1.0f), glm::radians(helicopter.rotation), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-armyHelicopterRotor.centerX, -armyHelicopterRotor.centerY, -armyHelicopterRotor.centerZ));
        armyHelicopterRotor.Draw(armyHelicopterRotorShader, camera, resVec);

        for(int i = 0; i < zombieGroup1.objectCount; i++){
            zombieGroup1.objectShaders[i]->Activate();
            zombieGroup1.objects[i]->myTotalTransformation = glm::translate(glm::mat4(1.0f), glm::vec3(zombieGroup1.offsetX+(randFloat(5)/128.0f), 0.0f, zombieGroup1.offsetZ+(randFloat(5)/128.0f)));
            glUniform4f(glGetUniformLocation(zombieGroup1.objectShaders[i]->ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
            glUniform3f(glGetUniformLocation(zombieGroup1.objectShaders[i]->ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);
            zombieGroup1.objects[i]->Draw((*zombieGroup1.objectShaders[i]), camera, resVec);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
    baseSceneShader.Delete();
    armyHelicopterBodyShader.Delete();
    armyHelicopterRotorShader.Delete();

    for(int i = 0; i < zombieGroup1.objectCount; i++){
        zombieGroup1.objectShaders[i]->Delete();
    }



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

void calculate_helicopter_heading(struct helicopterObject* helicopter){
    //Calculate the angle in the first quadrant and ensure the denominator is not 0
    float theta = glm::atan(glm::abs((*helicopter).randZFloat/((*helicopter).randXFloat+1e-8)));

    //Calculate the heading based on the trig quadrant rule.
    if((*helicopter).randXFloat > 0 && (*helicopter).randZFloat > 0){
        (*helicopter).heading = glm::three_over_two_pi<float>() - theta;
    }else if((*helicopter).randXFloat > 0 && (*helicopter).randZFloat < 0){
        (*helicopter).heading = glm::three_over_two_pi<float>() + theta;
    }else if((*helicopter).randXFloat < 0 && (*helicopter).randZFloat > 0){
        (*helicopter).heading = glm::half_pi<float>() + theta;
    }else if((*helicopter).randXFloat < 0 && (*helicopter).randZFloat < 0){
        (*helicopter).heading = glm::half_pi<float>() - theta;
    }else{
        (*helicopter).heading = 0;
    }
}