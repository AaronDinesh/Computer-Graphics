#ifndef VBO_CLASS_H
#define VBO_CLASS_H
#include <glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 normal = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(0.0f);
    glm::vec2 texUV = glm::vec2(0.0f);
};


class VBO{
    public:
        GLuint ID;
        VBO(std::vector<Vertex>& vertices);

        void Bind();
        void Unbind();
        void Delete();
};

#endif