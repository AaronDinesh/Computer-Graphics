#include "ObjLoader.h"
#include <iostream>


objLoader::objLoader(const char* path){
    filePath = path;
    numVertexes = countInstances("v");
    numIndices  = countInstances("f"); 
}

void objLoader::loadVectorData(glm::vec3** vert_data){
    vertex_idx = 0;
    std::fstream objFile;
    objFile.open(filePath, std::ios::in);
    if(objFile){
        (*vert_data) = (glm::vec3*) malloc(sizeof(glm::vec3)*(numVertexes));

        objFile.seekg(std::ios::beg);
        memset(buff, 0, BUFF_SIZE);
        
        
        while(!objFile.eof()){
            objFile.getline(buff, BUFF_SIZE, '\n');

            if(vertex_idx == numVertexes){
                break;
            }
            
            if(!*(buff)){
                *buff = 'q';
            }

            char* tok = strtok(buff, " ");

            if(strcmp(tok, "v") == 0){
                tok = strtok(NULL, " ");

                while(tok){
                    //Read in the vertex data
                    ((*vert_data)[vertex_idx])[vector_idx] = std::stof(tok);
                    vector_idx++;
                    tok = strtok(NULL, " ");
                }
                vertex_idx++;
            }

            vector_idx = 0;
            memset(buff, 0, BUFF_SIZE);
        }
        objFile.close();
        return;
    }
    throw(errno);
}

void objLoader::loadIndexData(GLuint** index_data){
    vertex_idx = 0;
    std::fstream objFile;

    objFile.open(filePath, std::ios::in);
    if(objFile){
        (*index_data) = (GLuint*) malloc(sizeof(GLuint)*(numIndices)*3);

        objFile.seekg(std::ios::beg);
        memset(buff, 0, BUFF_SIZE);
        while(!objFile.eof()){
            objFile.getline(buff, BUFF_SIZE, '\n');

            if(vertex_idx == numIndices*3){
                break;
            }

            if(!*(buff)){
                *buff = 'q';
            }

            char* tok = strtok(buff, " ");

            if(strcmp(tok, "f") == 0){
                tok = strtok(NULL, " ");

                while(tok){
                    //Read in the vertex data
                    ((*index_data)[vertex_idx]) = std::stoi(tok) - 1;
                    vertex_idx++;
                    tok = strtok(NULL, " ");
                }
            }
            memset(buff, 0, BUFF_SIZE);
        }
        objFile.close();
        return;
    }
    throw(errno);
}


void objLoader::loadDefaultColorData(glm::vec3** vert_data, glm::vec3 color){
    *vert_data = (glm::vec3*) realloc(*vert_data, sizeof(glm::vec3)*numVertexes*2);
    numVertexes = numVertexes*2;

    for(int i = numVertexes/2; i < numVertexes; i++){
        (*vert_data)[i] = color; 
    }
}

int objLoader::countInstances(const char* c){
    std::fstream objFile;
    objFile.open(filePath, std::ios::in);
    if(objFile){
        int instances = 0;
        memset(buff, 0, BUFF_SIZE);

        while(!objFile.eof()){
            objFile.getline(buff, BUFF_SIZE, '\n');

            if(!(*buff)){
                *buff = 'q';
            }

            char* tok = strtok(buff, " ");

            if(strcmp(tok, c) == 0){
                instances++;
            }
            memset(buff, 0, BUFF_SIZE);
        }

        objFile.close();
        return instances;
    }
    throw(errno);
}