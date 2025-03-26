#pragma once
//Crea las variables de posicion cartesianas

class Posicion{

   private:
   
    double x;
    double y;
    double z;
   
   public:
    Posicion() : x(0), y(0), z(0) {}
    Posicion setPosicion(double X, double Y, double Z){
        this->x = X;
        this->y = Y;
        this->z = Z;

        return *this;
    }
    bool comprobarPosicion(Posicion pos2){
        if(x == pos2.x && y == pos2.y && z == pos2.z){
            return true;
        }
        else return false;
    }
};