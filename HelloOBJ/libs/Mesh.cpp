#include "mesh.h"


Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& Texture){
    Mesh::vertices = vertices;
    Mesh::indices = indices;
    Mesh::textures = Texture;


    Mesh::objectVAO.Bind();

    VBO VBO(vertices);
    EBO EBO(indices);


    /*
        Our array is like:
            x|y|z|r|g|b|x|y|z|r|g|b|x|y|z|r|g|b|
        -The coordinates is at layout 0 and the colour is in the next layout (1)
        -The stride from one set of coords to the other is 6*float since it has to
            go over xyzrgb to get to the next xyz.
        -The offset for the coords is 0 since it is at the start of the array
        -The stride for the colors is the same 6*float
        -The offset for the colors is 3*float since the first color data is after
            the first xyz
    */
    //This specified the attribute for the coordinates
    Mesh::objectVAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*) 0);
    //This specified the attributes for the normal
    Mesh::objectVAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3*sizeof(float)));
    //This specified the attributes for the color
    Mesh::objectVAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6*sizeof(float)));
    //This specified the attributes for the texuV
    Mesh::objectVAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9*sizeof(float)));
    Mesh::objectVAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();
}

void Mesh::Draw(Shader& shader, Camera& camera, glm::mat4 matrix, glm::vec3 translation, glm::quat rotation, glm::vec3 scale){
    shader.Activate();
    Mesh::objectVAO.Bind();

    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;

    for(unsigned int i = 0; i < textures.size(); i++){
        std::string num;
        std::string type = textures[i].type;

        if(type == "diffuse"){
            num = std::to_string(numDiffuse++);
        }else if(type == "specular"){
            num = std::to_string(numSpecular++);
        }

        textures[i].texUnit(shader, (type+num).c_str(), i);
        textures[i].Bind();
    }

    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
    camera.Matrix(shader, "camMatrix");

    glm::mat4 translationMatrix = glm::mat4(1.0f);
    glm::mat4 rotationMatrix    = glm::mat4(1.0f);
    glm::mat4 scaleMatix        = glm::mat4(1.0f);

    translationMatrix = glm::translate(translationMatrix, translation);
    rotationMatrix = glm::mat4_cast(rotation);
    scaleMatix = glm::scale(scaleMatix, scale);

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(translationMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rotationMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(scaleMatix));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}