#ifndef SHADER_CLASS_H
#define SHDAER_CLASS_H
#pragma once

#include <glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>


std::string get_file_contents(const char* filename);


class Shader{
    public:
        GLuint ID;
        Shader(const char* vertexFile, const char* fragmentFile);
        void Activate();
        void Delete();    
};
#endif