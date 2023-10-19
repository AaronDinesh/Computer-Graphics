#include "Texture.h"

Texture::Texture(const char* imagePath, const char* texType, GLuint slot, GLenum format, GLenum pixelType){
    type = texType;
    int imgWidth, imgHeight, imgColCh;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* bytes = stbi_load(imagePath, &imgWidth, &imgHeight, &imgColCh, 0);

    if(!bytes){
        std::cout << "Couldn't load image: " << stbi_failure_reason() << std::endl;
        exit(-1);
    }

    glGenTextures(1, &ID);
    glActiveTexture(GL_TEXTURE0 + slot);
    unit = slot;
    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, pixelType, bytes);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(bytes);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texUnit(Shader ShaderProgram, const char* uniform, GLuint unit){
    GLuint tex0UniId = glGetUniformLocation(ShaderProgram.ID, uniform);
    ShaderProgram.Activate();
    glUniform1i(tex0UniId, unit);

}

void Texture::Bind(){
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind(){
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete(){
    glDeleteTextures(1, &ID);
}

