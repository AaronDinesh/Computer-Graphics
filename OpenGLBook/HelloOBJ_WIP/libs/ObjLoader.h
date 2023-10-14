#ifndef OBJLOADER_CLASS_H
#define OBJLOADER_CLASS_H

#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <glad.h>
#include <string.h>
#include <cerrno>


class objLoader{

    public:
        static const int BUFF_SIZE = 64;
        char buff[BUFF_SIZE];
        const char* filePath;
        int vector_idx = 0;
        int vertex_idx = 0;
        int numVertexes = 0;
        int numIndices = 0;

        objLoader(const char* path);

        void loadVectorData(glm::vec3** vert_data);
        void loadIndexData(GLuint** index_data);
        void loadDefaultColorData(glm::vec3** vert_data, glm::vec3 color);

    private:
        int countInstances(const char* c);
};
#endif