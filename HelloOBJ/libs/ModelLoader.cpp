#include "ModelLoader.h"

//Add GLTF SUPPORT

ModelLoader::ModelLoader(const char* path){
    filePath = path;

    const char* fileExt = getFileExt(path);


    if(fileExt == "obj"){
        parseOBJData();
    }else if(fileExt == "gltf"){
        //TODO: Impliment the parse GLTF FUNCTION
        parseGLTFData();
    }else{
        errVal = WRONG_FILE_TYPE;
    }
}


//GLTF Functions
void ModelLoader::parseJSON(const char* filePath){
    std::ifstream f(filePath);   
    try{
        gltfFile = nlohmann::json::parse(f);
    }catch(const nlohmann::json::parse_error& e){
        std::cout << "message: " << e.what() << '\n'
                  << "exception id: " << e.id << '\n'
                  << "byte position of error: " << e.byte << std::endl;
        errVal = JSON_PARSE_ERROR;
    }
    f.close();
}


void ModelLoader::readBinaryData(){

    try{
        std::string binFilename = gltfFile["buffers"]["0"]["uri"];
        //Opens the file as a binary and seeks immediately to the end
        std::ifstream binaryData(binFilename, std::ios::binary | std::ios::ate);
        //Returns the current position of the cursor, which is at the end, effecively giving us the filesize
        size_t fileSize = binaryData.tellg();
        //Set the cursor to the begining
        binaryData.seekg(0);
    }catch(const int e){
        errVal = BINARY_READ_ERROR;
    }

}



//OBJ Functions
void ModelLoader::loadVectorData(){
    std::fstream objFile;
    int vertCoordPos = 0;
    objFile.open(filePath, std::ios::in);
    if(objFile){
        objFile.seekg(std::ios::beg);
        memset(buff, 0, BUFF_SIZE);
        
        while(!objFile.eof()){
            objFile.getline(buff, BUFF_SIZE, '\n');

            if(!*(buff)){
                *buff = 'q';
            }

            char* tok = strtok(buff, " ");

            if(strcmp(tok, "v") == 0){
                glm::vec3 vertexVec = glm::vec3(1.0f);
                tok = strtok(NULL, " ");

                while(tok){
                    //Read in the vertex data
                    vertexVec[vertCoordPos] = std::atof(tok);
                    vertCoordPos = (vertCoordPos + 1) % 3;
                    tok = strtok(NULL, " ");
                }

                vertexCoords.push_back(vertexVec);
            }
            memset(buff, 0, BUFF_SIZE);
        }
        objFile.close();
        return;
    }
    throw(errno);
}

void ModelLoader::loadIndexData(){
    std::fstream objFile;
    objFile.open(filePath, std::ios::in);
    if(objFile){
        objFile.seekg(std::ios::beg);
        memset(buff, 0, BUFF_SIZE);
        
        while(!objFile.eof()){
            objFile.getline(buff, BUFF_SIZE, '\n');

            if(!*(buff)){
                *buff = 'q';
            }

            char* tok = strtok(buff, " ");

            if(strcmp(tok, "f") == 0){
                tok = strtok(NULL, " ");

                while(tok){
                    //Read in the vertex data
                    vertexIndices.push_back((GLuint) std::atoi(tok) - 1);
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

void ModelLoader::loadTextureData(){
    std::fstream objFile;
    int vertCoordPos = 0;
    objFile.open(filePath, std::ios::in);
    if(objFile){
        objFile.seekg(std::ios::beg);
        memset(buff, 0, BUFF_SIZE);
        
        while(!objFile.eof()){
            objFile.getline(buff, BUFF_SIZE, '\n');

            if(!*(buff)){
                *buff = 'q';
            }

            char* tok = strtok(buff, " ");

            if(strcmp(tok, "vt") == 0){
                glm::vec2 textureVec = glm::vec2(1.0f);
                tok = strtok(NULL, " ");

                while(tok){
                    //Read in the texture data
                    textureVec[vertCoordPos] = std::atof(tok);
                    vertCoordPos = (vertCoordPos + 1) % 2;
                    tok = strtok(NULL, " ");
                }

                textureCoords.push_back(textureVec);
            }
            memset(buff, 0, BUFF_SIZE);
        }
        objFile.close();
        return;
    }
    throw(errno);
}

void ModelLoader::loadNormalData(){
    std::fstream objFile;
    int vertCoordPos = 0;
    objFile.open(filePath, std::ios::in);
    if(objFile){
        objFile.seekg(std::ios::beg);
        memset(buff, 0, BUFF_SIZE);
        
        while(!objFile.eof()){
            objFile.getline(buff, BUFF_SIZE, '\n');

            if(!*(buff)){
                *buff = 'q';
            }

            char* tok = strtok(buff, " ");

            if(strcmp(tok, "vn") == 0){
                glm::vec3 normalVec = glm::vec3(1.0f);
                tok = strtok(NULL, " ");

                while(tok){
                    //Read in the normal data
                    normalVec[vertCoordPos] = std::atof(tok);
                    vertCoordPos = (vertCoordPos + 1) % 3;
                    tok = strtok(NULL, " ");
                }

                vertexNormals.push_back(normalVec);
            }
            memset(buff, 0, BUFF_SIZE);
        }
        objFile.close();
        return;
    }
    throw(errno);
}

void ModelLoader::loadDefaultColorData(){
    for(unsigned int i = 0; i < vertexCoords.size(); i++){
        vertexColors.push_back(glm::vec3(1.0f));
    }
}


uint ModelLoader::parseOBJData(){
        loadVectorData();
        loadIndexData();
        loadTextureData();
        loadNormalData();
}

Mesh ModelLoader::loadMesh(){
    
    if(vertexIndices.size()){
        haveIndex = true;
    }

    if(vertexNormals.size()){
        haveNormal = true;
    }

    if(textureCoords.size()){
        haveTexture = true;
    }

    if(vertexColors.size()){
        haveColor = true;
    }

    for(unsigned int i = 0; i < vertexCoords.size(); i++){
        vertices.push_back(Vertex{vertexCoords[i], haveNormal ? vertexNormals[i] : glm::vec3(0.0f), haveColor ? vertexColors[i] : glm::vec3(0.0f), haveTexture ? textureCoords[i] : glm::vec2(0.0f)});
    }

    std::vector<Texture> tex;
    return Mesh(vertices, vertexIndices, tex);
}

const char* ModelLoader::getFileExt(const char* filepath){
    const char* dot = strrchr(filepath, '.');

    if(dot || dot == filepath){
        return NULL;
    }

    return dot + 1;
}