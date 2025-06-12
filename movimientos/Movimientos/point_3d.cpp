#include "point_3d.h"
#include <math.h>

Point_3D& Point_3D::operator+=(const Point_3D& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
}
Point_3D& Point_3D::operator-=(const Point_3D& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    return *this;
}
Point_3D &Point_3D::operator /=(const float &rhs)
{
    this->x /= rhs;
    this->y /= rhs;
    this->z /= rhs;
    return *this;
}

double Point_3D::module()
{
    return cbrt(this->x * this->x + this->y * this->y + this->z *this->z);
}


Point_3D operator+(Point_3D lhs, const Point_3D& rhs)
{
    lhs += rhs;
    return lhs;
}
Point_3D operator-( const double lhs[3], Point_3D rhs ){
    rhs.x = lhs[0]- rhs.x;
    rhs.y = lhs[1]- rhs.y;
    rhs.z = lhs[2]- rhs.z;

    return rhs;
}
Point_3D operator-(Point_3D lhs, const Point_3D& rhs)
{
    lhs -= rhs;
    return lhs;
}
Point_3D operator/(int valor, Point_3D lhs)
{
    return { lhs.x / valor, lhs.y / valor };
}

bool operator ==(const Point_3D &lhs, const double &rhs){
    return lhs.x==rhs & lhs.y == rhs & lhs.z==rhs;
}
bool operator ==(const Point_3D &lhs, const Point_3D &rhs){
    return lhs.x==rhs.x & lhs.y == rhs.y & lhs.z==rhs.z;
}
bool operator !=(const Point_3D &lhs, const double &rhs){
    return !(lhs==rhs);
}
bool operator !=(const Point_3D &lhs, const Point_3D &rhs){
    return !(lhs==rhs);
}
