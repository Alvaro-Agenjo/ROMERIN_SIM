#ifndef MTHOMOGENEA_H
#define MTHOMOGENEA_H

#include <QDebug>
struct Point_3D
{
    double x,y,z;

    Point_3D& operator +=(const Point_3D & rhs);
    Point_3D& operator -=(const Point_3D& rhs);
    Point_3D &operator /=(const float& rhs);
    double module();
};

Point_3D operator +( Point_3D lhs, const Point_3D& rhs);
Point_3D operator -( const double lhs[3], Point_3D rhs );
Point_3D operator -( Point_3D lhs, const Point_3D& rhs);
Point_3D operator /(int valor, Point_3D lhs);

bool operator ==(const Point_3D &lhs, const double &rhs);
bool operator ==(const Point_3D &lhs, const Point_3D &rhs);
bool operator !=(const Point_3D &lhs, const double &rhs);
bool operator !=(const Point_3D &lhs, const Point_3D &rhs);



//--------------------------------------------------------------------------------------------//

struct Matriz_Transformacion{
    double m[4][4];
    Matriz_Transformacion();
    Matriz_Transformacion(double m[3][3]);
    Matriz_Transformacion(double p[3]);
    Matriz_Transformacion(double m[3][3], double p[3]);


    Matriz_Transformacion operator*(const Matriz_Transformacion& other) const;
    Matriz_Transformacion operator*(const int& num) const;
    Matriz_Transformacion Inversa() const;
};

struct Vector3D{
    double x, y, z;
    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

    Vector3D Transformacion(const Vector3D& v, const Matriz_Transformacion &matriz);
};


QDebug & operator<<(QDebug &os, const Matriz_Transformacion& mt);
QDebug & operator<<(QDebug & os, const Vector3D& v);

#endif // MTHOMOGENEA_H
