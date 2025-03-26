//Documento generalizado de todos los Los brazos

#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstring>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <Control/ControlPatas.h>

class Robot{
    private:

    Posicion pos;
    int id;
    
    public:
    Robot(Posicion Pos, int ID): pos(Pos), id(ID){}
};
