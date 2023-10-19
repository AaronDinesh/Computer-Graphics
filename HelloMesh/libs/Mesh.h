#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <string>
#include <vector>
#include <glad.h>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

#include "Camera.h"
#include "Texture.h"


class Mesh{
    public:
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

        VAO objectVAO;


        Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& Texture);
        void Draw(Shader& shader, Camera& camera);


};

#endif