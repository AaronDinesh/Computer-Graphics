user_libs=$(ls -d libs/* | grep -i ".cpp" | tr '\n' ' ')


g++ main.cpp ${user_libs} -o main -g -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lGLAD -lstb -lglm
