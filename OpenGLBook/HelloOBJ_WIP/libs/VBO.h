#ifndef VBO_CLASS_H
#define VBO_CLASS_H
#include <glad.h>
#include <glm/glm.hpp>

class VBO{
    public:
        GLuint ID;
        VBO(glm::vec3* vertices, GLsizeiptr size);

        void Bind();
        void Unbind();
        void Delete();
};

#endif