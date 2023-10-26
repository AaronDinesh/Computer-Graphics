#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include "Mesh.h"

class Model{
    public:
        Model(const char* object_path);
        void Draw(Shader& shader, Camera& camera);

    private:
        Mesh object;
}


#endif