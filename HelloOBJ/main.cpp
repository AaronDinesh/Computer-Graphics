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
    TRACKING,
    GO_TO_ORIGINAL_PLACES,
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
    float xLimMin = 0.0f;
    float xLimMax = 0.0f;
    float offsetZ = 0.0f;
    float zLimMin = 0.0f;
    float zLimMax = 0.0f;
    float heading = 0.0f;
    float accumulatedTime = 0.0f;
    glm::vec2 toCamera = glm::vec2(1.0f);
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
    zombieGroup1.xLimMin = -11.0f;
    zombieGroup1.xLimMax = 83.0f;
    zombieGroup1.zLimMin = -4.0f;
    zombieGroup1.zLimMax = 2.8f;
    struct zombieGroup zombieGroup2;
    zombieGroup2.xLimMin = -72.0f;
    zombieGroup2.xLimMax = 84.0f;
    zombieGroup2.zLimMin = -2.5f;
    zombieGroup2.zLimMax = 4.0f;
    
    Shader baseSceneShader("Shaders/default.vert","Shaders/default.frag");
    Shader armyHelicopterBodyShader("Shaders/armyHelicopterBody.vert","Shaders/armyHelicopterBody.frag");
    Shader armyHelicopterRotorShader("Shaders/armyHelicopterRotor.vert","Shaders/armyHelicopterRotor.frag");
    Shader zombie1Shader("Shaders/zombie.vert", "Shaders/zombie.frag");
    Shader zombie2Shader("Shaders/zombie.vert", "Shaders/zombie.frag");
    Shader zombie3Shader("Shaders/zombie.vert", "Shaders/zombie.frag");
    
    Shader zombie4Shader("Shaders/zombie.vert", "Shaders/zombie.frag");
    Shader zombie5Shader("Shaders/zombie.vert", "Shaders/zombie.frag");
    Shader zombie6Shader("Shaders/zombie.vert", "Shaders/zombie.frag");

    Shader humanShader("Shaders/Human.vert", "Shaders/Human.frag");



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

    zombie4Shader.Activate();
    glUniform4f(glGetUniformLocation(zombie4Shader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(zombie4Shader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);
    
    zombie5Shader.Activate();
    glUniform4f(glGetUniformLocation(zombie5Shader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(zombie5Shader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);
    
    zombie6Shader.Activate();
    glUniform4f(glGetUniformLocation(zombie6Shader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(zombie6Shader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);

    humanShader.Activate();
    glUniform4f(glGetUniformLocation(humanShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(humanShader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);



    zombieGroup1.objectShaders[0] = &zombie1Shader;
    zombieGroup1.objectShaders[1] = &zombie2Shader;
    zombieGroup1.objectShaders[2] = &zombie3Shader;


    zombieGroup2.objectShaders[0] = &zombie4Shader;
    zombieGroup2.objectShaders[1] = &zombie5Shader;
    zombieGroup2.objectShaders[2] = &zombie6Shader;


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
    
    //Second Group of Zombies
    initTransform = glm::translate(glm::mat4(1.0f), glm::vec3(29.5f, -21.5f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Model zombie4("Objects/zombie/scene.gltf", scale, initTransform);
    
    initTransform = glm::translate(glm::mat4(1.0f), glm::vec3(31.5f, -21.5f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Model zombie5("Objects/zombie/scene.gltf", scale, initTransform);
    
    initTransform = glm::translate(glm::mat4(1.0f), glm::vec3(30.5f, -21.5f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Model zombie6("Objects/zombie/scene.gltf", scale, initTransform);    
    
    initTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -19.3f, 2.5f)) * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scale = glm::vec3(2.0f, 2.5f, 2.0f);
    Model human("Objects/HumanBlender/scene.gltf", scale, initTransform, true);
    std::cout << "Center XYZ " << human.centerX << " " << human.centerY << " " << human.centerZ << std::endl;
    
    zombieGroup1.objects[0] = &zombie1;
    zombieGroup1.objects[1] = &zombie2;
    zombieGroup1.objects[2] = &zombie3;

    zombieGroup2.objects[0] = &zombie4;
    zombieGroup2.objects[1] = &zombie5;
    zombieGroup2.objects[2] = &zombie6;


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

        for(int i = 0; i < zombieGroup2.objectCount; i++){
            zombieGroup2.objectShaders[i]->Activate();
            glUniform3f(glGetUniformLocation(zombieGroup2.objectShaders[i]->ID, "skyColor"), fogColor.x, fogColor.y, fogColor.z);
        }
        
        humanShader.Activate();
        glUniform3f(glGetUniformLocation(humanShader.ID, "skyColor"), fogColor.x, fogColor.y, fogColor.z);



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
                    zombieGroup1.randXFloat = randFloat(5)/20.0f;
                    zombieGroup1.randZFloat = randFloat(5)/50.0f;
                    zombieGroup1.groupState = WALKING;
                    break;
                case WALKING:
                    if(zombieGroup1.offsetX < zombieGroup1.xLimMin ||  zombieGroup1.offsetX > zombieGroup1.xLimMax){
                        zombieGroup1.randXFloat = -zombieGroup1.randXFloat;
                    }
                    
                    if(zombieGroup1.offsetZ < zombieGroup1.zLimMin ||  zombieGroup1.offsetZ > zombieGroup1.zLimMax){
                        zombieGroup1.randZFloat = -zombieGroup1.randZFloat;
                    }

                    //Set the heading to be 0 normally except when the zombies are moving backwards
                    //then set the heading to 180
                    zombieGroup1.heading = 0 + (float) 180*(zombieGroup1.randXFloat < 0);
                    zombieGroup1.offsetX += zombieGroup1.randXFloat;
                    zombieGroup1.offsetZ += zombieGroup1.randZFloat;
                    
                    zombieGroup1.accumulatedTime += crntTime - prevTime;
                    if(zombieGroup1.accumulatedTime >= 10.0f){
                        zombieGroup1.randXFloat = randFloat(5)/20.0f;
                        zombieGroup1.randZFloat = randFloat(5)/50.0f;
                        zombieGroup1.accumulatedTime = 0.0f;
                    }
                    break;
                case TRACKING:
                    zombieGroup1.toCamera = glm::normalize(glm::vec2(camera.Position.x, camera.Position.z) - glm::vec2(zombieGroup1.offsetX, zombieGroup1.offsetZ));
                    zombieGroup1.randXFloat = zombieGroup1.toCamera.x/8.0f;
                    zombieGroup1.randZFloat = zombieGroup1.toCamera.y/8.0f;
                    zombieGroup1.offsetX += zombieGroup1.randXFloat;
                    zombieGroup1.offsetZ += zombieGroup1.randZFloat;

                    if(zombieGroup1.toCamera.length() < 0.01f){
                        zombieGroup1.randXFloat = 0.0f;
                        zombieGroup1.randZFloat = 0.0f;    
                    }
                    break;
                case GO_TO_ORIGINAL_PLACES:
                    zombieGroup1.offsetX += zombieGroup1.randXFloat;
                    zombieGroup1.offsetZ += zombieGroup1.randZFloat;

                    if(zombieGroup1.offsetX*zombieGroup1.offsetX + zombieGroup1.offsetZ*zombieGroup1.offsetZ < 0.5f*0.5f){
                        zombieGroup1.groupState = WALKING;
                    }
                    break;
                default:
                    break;
            }

            switch(zombieGroup2.groupState){
                case START:
                    zombieGroup2.randXFloat = randFloat(5)/20.0f;
                    zombieGroup2.randZFloat = randFloat(5)/50.0f;
                    zombieGroup2.groupState = WALKING;
                    
                    // if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
                    //     zombieGroup2.offsetX += 1;
                    // }

                    // if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
                    //     zombieGroup2.offsetX -= 1;
                    // }

                    // if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
                    //     zombieGroup2.offsetZ += 1;
                    // }

                    // if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
                    //     zombieGroup2.offsetZ -= 1;
                    // }

                    // zombieGroup2.heading = 0 + (float) 180*(zombieGroup2.offsetX < 0);
                    // std::cout << "Offset XZ: " << zombieGroup2.offsetX << "," << zombieGroup2.offsetZ <<std::endl;
                    break;
                case WALKING:
                    if(zombieGroup2.offsetX < zombieGroup2.xLimMin ||  zombieGroup2.offsetX > zombieGroup2.xLimMax){
                        zombieGroup2.randXFloat = -zombieGroup2.randXFloat;
                    }
                    
                    if(zombieGroup2.offsetZ < zombieGroup2.zLimMin ||  zombieGroup2.offsetZ > zombieGroup2.zLimMax){
                        zombieGroup2.randZFloat = -zombieGroup2.randZFloat;
                    }
                    
                    
                    //Set the heading to be 0 normally except when the zombies are moving backwards
                    //then set the heading to 180
                    zombieGroup2.heading = 0 + (float) 180*(zombieGroup2.randXFloat < 0);
                    zombieGroup2.offsetX += zombieGroup2.randXFloat;
                    zombieGroup2.offsetZ += zombieGroup2.randZFloat;
                    
                    zombieGroup2.accumulatedTime += crntTime - prevTime;
                    if(zombieGroup2.accumulatedTime >= 10.0f){
                        zombieGroup2.randXFloat = randFloat(5)/20.0f;
                        zombieGroup2.randZFloat = randFloat(5)/50.0f;
                        zombieGroup2.accumulatedTime = 0.0f;
                    }
                    break;
                case TRACKING:
                    zombieGroup2.toCamera = glm::normalize(glm::vec2(camera.Position.x, camera.Position.z) - glm::vec2(zombieGroup2.offsetX, zombieGroup2.offsetZ));
                    zombieGroup2.randXFloat = zombieGroup2.toCamera.x/8.0f;
                    zombieGroup2.randZFloat = zombieGroup2.toCamera.y/8.0f;
                    zombieGroup2.offsetX += zombieGroup2.randXFloat;
                    zombieGroup2.offsetZ += zombieGroup2.randZFloat;

                    if(zombieGroup2.toCamera.length() < 0.01f){
                        zombieGroup2.randXFloat = 0.0f;
                        zombieGroup2.randZFloat = 0.0f;    
                    }
                    break;
                case GO_TO_ORIGINAL_PLACES:
                    zombieGroup2.offsetX += zombieGroup2.randXFloat;
                    zombieGroup2.offsetZ += zombieGroup2.randZFloat;

                    if(zombieGroup2.offsetX*zombieGroup2.offsetX + zombieGroup2.offsetZ*zombieGroup2.offsetZ < 0.5f*0.5f){
                        zombieGroup2.groupState = WALKING;
                    }
                    break;
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

            if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
                camera.cameraState = Camera::HELICOPTER;
            }

            if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
                camera.cameraState = Camera::FREE;
            }

            if(glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS){
                camera.cameraState = Camera::HUMAN;
            }


            // if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS){
            //     zombieGroup1.groupState = TRACKING;
            // }
            
            // if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
            //     zombieGroup1.groupState = GO_TO_ORIGINAL_PLACES;
            //     zombieGroup1.randXFloat = -zombieGroup1.offsetX / 500.0f;
            //     zombieGroup1.randZFloat = -zombieGroup1.offsetZ / 500.0f;
            // }
        }

        camera.Inputs(window);
        glm::vec4 homoCenter = glm::vec4(1.0f);
        switch(camera.cameraState){
            case Camera::HELICOPTER:
                homoCenter = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, -7.0f)) * glm::vec4(armyHelicopterBody.centerX, armyHelicopterBody.centerY, armyHelicopterBody.centerZ, 1.0f);
                camera.Position = glm::vec3(homoCenter.x, homoCenter.y, homoCenter.z);
                break;
            case Camera::HUMAN:
                homoCenter = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, 0.0f)) * glm::vec4(human.centerX, human.centerY, human.centerZ, 1.0f);
                camera.Position = glm::vec3(homoCenter.x, homoCenter.y, homoCenter.z);
                break;
            default:
                break;
        }
        if(camera.cameraState == Camera::HELICOPTER){
            glm::vec4 homoCenter = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, -7.0f)) * glm::vec4(armyHelicopterBody.centerX, armyHelicopterBody.centerY, armyHelicopterBody.centerZ, 1.0f);
            camera.Position = glm::vec3(homoCenter.x, homoCenter.y, homoCenter.z);
        }

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
            zombieGroup1.objects[i]->myTotalTransformation = glm::translate(glm::mat4(1.0f), glm::vec3(zombieGroup1.offsetX+(randFloat(5)/128.0f), 0.0f, zombieGroup1.offsetZ+(randFloat(5)/128.0f))) * glm::rotate(glm::mat4(1.0f), glm::radians(zombieGroup1.heading), glm::vec3(0.0f, 1.0f, 0.0f));
            glUniform4f(glGetUniformLocation(zombieGroup1.objectShaders[i]->ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
            glUniform3f(glGetUniformLocation(zombieGroup1.objectShaders[i]->ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);
            zombieGroup1.objects[i]->Draw((*zombieGroup1.objectShaders[i]), camera, resVec);
        }

        for(int i = 0; i < zombieGroup2.objectCount; i++){
            zombieGroup2.objectShaders[i]->Activate();
            zombieGroup2.objects[i]->myTotalTransformation = glm::translate(glm::mat4(1.0f), glm::vec3(zombieGroup2.offsetX+(randFloat(5)/128.0f), 0.0f, zombieGroup2.offsetZ+(randFloat(5)/128.0f))) * glm::rotate(glm::mat4(1.0f), glm::radians(zombieGroup2.heading), glm::vec3(0.0f, 1.0f, 0.0f));
            glUniform4f(glGetUniformLocation(zombieGroup2.objectShaders[i]->ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
            glUniform3f(glGetUniformLocation(zombieGroup2.objectShaders[i]->ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);
            zombieGroup2.objects[i]->Draw((*zombieGroup2.objectShaders[i]), camera, resVec);
        }

        humanShader.Activate();
        glUniform4f(glGetUniformLocation(humanShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
        glUniform3f(glGetUniformLocation(humanShader.ID, "lightPosition"), lightPos.x, lightPos.y, lightPos.z);
        human.Draw(humanShader, camera, resVec);

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

    for(int i = 0; i < zombieGroup2.objectCount; i++){
        zombieGroup2.objectShaders[i]->Delete();
    }

    humanShader.Delete();

    glfwTerminate();
    return 0;
}

GLFWwindow* glINIT(int WIDTH, int HEIGHT){
    GLFWwindow* window = NULL;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Final Scene", NULL, NULL);

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