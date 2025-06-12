#ifndef POINT_3D_H
#define POINT_3D_H

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

#endif // POINT_3D_H
