#ifndef MTHOMOGENEA_H
#define MTHOMOGENEA_H

#include <QDebug>

//--------------------------------------------------------------------------------------------//

struct Vector3D;
struct Matriz_Transformacion{
    double m[4][4];
    Matriz_Transformacion();
    Matriz_Transformacion(double m[3][3]);
    Matriz_Transformacion(double p[3]);
    Matriz_Transformacion(Vector3D p);
    Matriz_Transformacion(double m[3][3], double p[3]);
    Matriz_Transformacion(double m[3][3], int16_t p[3]);

    Matriz_Transformacion Inversa() const;
};

Matriz_Transformacion operator*(const Matriz_Transformacion &lhs, const Matriz_Transformacion& other);

struct Vector3D{
    double x, y, z;
    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}


    Vector3D& operator +=(const Vector3D & rhs);
    Vector3D& operator -=(const Vector3D& rhs);
    Vector3D& operator /=(const float& rhs);
    Vector3D& operator /=(const int& rhs);
    double module();

    Vector3D &operator=(const double pos[3]);
};

Vector3D Transformacion(const Vector3D& v, const Matriz_Transformacion &matriz);
Vector3D operator +( Vector3D lhs, const Vector3D& rhs);
Vector3D operator -( Vector3D lhs, const Vector3D& rhs);
Vector3D operator /(Vector3D lhs, const int& rhs);

bool operator ==(const Vector3D &lhs, const double &rhs);
bool operator ==(const Vector3D &lhs, const Vector3D &rhs);
bool operator !=(const Vector3D &lhs, const double &rhs);
bool operator !=(const Vector3D &lhs, const Vector3D &rhs);


QDebug & operator<<(QDebug &os, const Matriz_Transformacion& mt);
QDebug & operator<<(QDebug & os, const Vector3D& v);

void Calc3x3ROT(float a, float b, float c, double orientation[][3]);
#endif // MTHOMOGENEA_H
