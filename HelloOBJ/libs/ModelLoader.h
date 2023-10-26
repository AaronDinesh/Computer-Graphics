#ifndef MODELLOADER_CLASS_H
#define MODELLOADER_CLASS_H

#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <glad.h>
#include <string.h>
#include <cerrno>
#include <iostream>
#include <nlohmann/json.hpp>

#include "Mesh.h"


/*
OBJ file format

v x y z     -> This specifies one of the vertexes of the object in xyz coords
vt u v [w]  -> This specifies the texture coordinates for a vertex
vn x y z    -> This specifies the vertex normal
f v1 v2 v3  -> This specifies the index of the vertices used to make this face.
               The index starts at 1
*/



/*
errVal is an indicator of what went wrong during Model Loading
*/

typedef enum errval{SUCCESS, WRONG_FILE_TYPE, 
                    JSON_PARSE_ERROR, BINARY_READ_ERROR} err;


class ModelLoader{
    public:

        ModelLoader(const char* path);
        Mesh loadMesh();
        err errVal; 

    private:
        static const int BUFF_SIZE = 64;
        char buff[BUFF_SIZE];
        const char* filePath = NULL;

        std::vector<glm::vec3> vertexCoords;
        std::vector<GLuint> vertexIndices;
        std::vector<glm::vec3> vertexNormals;
        std::vector<glm::vec3> vertexColors;
        std::vector<glm::vec2> textureCoords;

        std::vector<Vertex> vertices;

        int vertex = 0;

        bool haveIndex = false;
        bool haveNormal = false;
        bool haveColor = false;
        bool haveTexture = false;


        const char* getFileExt(const char* filepath);

        //gltf parser functions
        nlohmann::json gltfFile;
        std::vector<char> binData;
        void parseJSON(const char* filePath);
        void readBinaryData();

        //OBJ file parser fuctions
        void loadVectorData();
        void loadIndexData();
        void loadTextureData();
        void loadNormalData();
        void loadDefaultColorData();
        uint parseOBJData();

};
#endif