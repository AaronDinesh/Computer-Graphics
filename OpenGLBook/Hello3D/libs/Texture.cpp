#include "Texture.h"

Texture::Texture(const char* imagePath, GLenum texType, GLenum slot, GLenum format, GLenum pixelType){
    type = texType;
    int imgWidth, imgHeight, imgColCh;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* bytes = stbi_load(imagePath, &imgWidth, &imgHeight, &imgColCh, 0);

    if(!bytes){
        std::cout << "Couldn't load image: " << stbi_failure_reason() << std::endl;
        exit(-1);
    }

    glGenTextures(1, &ID);
    glActiveTexture(slot);
    glBindTexture(texType, ID);

    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(texType, 0, format, imgWidth, imgHeight, 0, format, pixelType, bytes);
    
    glGenerateMipmap(texType);
    stbi_image_free(bytes);

    glBindTexture(texType, 0);
}

void Texture::texUnit(Shader ShaderProgram, const char* uniform, GLuint unit){
    GLuint tex0UniId = glGetUniformLocation(ShaderProgram.ID, uniform);
    ShaderProgram.Activate();
    glUniform1i(tex0UniId, unit);

}

void Texture::Bind(){
    glBindTexture(type, ID);
}

void Texture::Unbind(){
    glBindTexture(type, 0);
}

void Texture::Delete(){
    glDeleteTextures(1, &ID);
}

