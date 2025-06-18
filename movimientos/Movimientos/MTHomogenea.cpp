#include "MTHomogenea.h"
#include <math.h>
//--------------------------------------------------------------------------------------------//


Matriz_Transformacion::Matriz_Transformacion(){
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            m[i][j] = (i == j) ? 1 : 0; // Matriz identidad
}
Matriz_Transformacion::Matriz_Transformacion(double m[3][3]): Matriz_Transformacion() {
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            this->m[i][j] = m[i][j];
}
Matriz_Transformacion::Matriz_Transformacion(double p[3]) : Matriz_Transformacion() {
    for(int i = 0; i < 3; i++)
        this->m[i][3] = p[i]; // Translation vector
}
Matriz_Transformacion::Matriz_Transformacion(Vector3D p) : Matriz_Transformacion() {
    this->m[0][3] = p.x;
    this->m[1][3] = p.y;
    this->m[2][3] = p.z;
}
Matriz_Transformacion::Matriz_Transformacion(double m[3][3], double p[3]){
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 4; j++){
            if(j==3) this->m[i][j] = p[i];
            else this->m[i][j] = m[i][j];
        }
    for(int j = 0; j < 4; j++)
        this->m[3][j] = (j == 3) ? 1 : 0; // Last row for homogeneous coordinates
}

Matriz_Transformacion Matriz_Transformacion::Inversa() const{
    Matriz_Transformacion inv;  // M-1 =M^t
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++) {
            inv.m[i][j] = m[j][i]; // Transpose for rotation
        }
    for(int i = 0; i < 3; i++) {
        inv.m[i][3] = -m[0][i] * m[0][3] - m[1][i] * m[1][3] - m[2][i] * m[2][3];
    }
    return inv;
}

// Matriz_Transformacion Matriz_Transformacion::operator*(const float& num) const {
//     Matriz_Transformacion result;
//     for(int i = 0; i < 4; i++) {
//         for(int j = 0; j < 4; j++) {
//             result.m[i][j] *= num;
//         }
//     }
//     return result;
// }


Matriz_Transformacion operator*(const Matriz_Transformacion& lhs, const Matriz_Transformacion& rhs) {
    Matriz_Transformacion result;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            result.m[i][j] = 0;
            for(int k = 0; k < 4; k++) {
                result.m[i][j] += lhs.m[i][k] * rhs.m[k][j];
            }
        }
    }
    return result;
}


QDebug & operator<<(QDebug & os, const Matriz_Transformacion& mt) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            os << mt.m[i][j] << " ";
        }
        os << endl;
    }
    return os;
}
QDebug & operator<<(QDebug & os, const Vector3D& v) {
    os << "Vector3D(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

/* Da las coordenadas de un punto v respecto al sistema final en el sistema origen */
Vector3D Transformacion(const Vector3D& v, const Matriz_Transformacion &matriz) {
    Vector3D result;
    result.x = v.x * matriz.m[0][0] + v.y * matriz.m[0][1] + v.z * matriz.m[0][2] + matriz.m[0][3];
    result.y = v.x * matriz.m[1][0] + v.y * matriz.m[1][1] + v.z * matriz.m[1][2] + matriz.m[1][3];
    result.z = v.x * matriz.m[2][0] + v.y * matriz.m[2][1] + v.z * matriz.m[2][2] + matriz.m[2][3];
    return result;
}

Vector3D& Vector3D::operator+=(const Vector3D& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
}
Vector3D& Vector3D::operator-=(const Vector3D& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    return *this;
}
Vector3D &Vector3D::operator /=(const float &rhs)
{
    this->x /= rhs;
    this->y /= rhs;
    this->z /= rhs;
    return *this;
}
double Vector3D::module()
{
    return cbrt(this->x * this->x + this->y * this->y + this->z *this->z);
}


Vector3D operator+(Vector3D lhs, const Vector3D& rhs)
{
    lhs += rhs;
    return lhs;
}
/*
Vector3D operator-( const double lhs[3], Vector3D rhs ){
    rhs.x = lhs[0]- rhs.x;
    rhs.y = lhs[1]- rhs.y;
    rhs.z = lhs[2]- rhs.z;

    return rhs;
}
*/
Vector3D operator-(Vector3D lhs, const Vector3D& rhs)
{
    lhs -= rhs;
    return lhs;
}


bool operator ==(const Vector3D &lhs, const double &rhs){
    return lhs.x==rhs & lhs.y == rhs & lhs.z==rhs;
}
bool operator ==(const Vector3D &lhs, const Vector3D &rhs){
    return lhs.x==rhs.x & lhs.y == rhs.y & lhs.z==rhs.z;
}
bool operator !=(const Vector3D &lhs, const double &rhs){
    return !(lhs==rhs);
}
bool operator !=(const Vector3D &lhs, const Vector3D &rhs){
    return !(lhs==rhs);
}


