#include <iostream>
#include <math.h>

struct Vector3D{
    double x, y, z;
    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

    Vector3D& operator +=(const Vector3D & rhs){
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        return *this;
    }
    Vector3D& operator -=(const Vector3D& rhs){
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        return *this;
    }
    Vector3D &operator /=(const float& rhs){
        this->x /= rhs;
        this->y /= rhs;
        this->z /= rhs;
        return *this;
    }
    double module(){
        return cbrt(this->x * this->x + this->y * this->y + this->z *this->z);
    }
};


struct Matriz_Transformacion{
    double m[4][4];
    
    
    Matriz_Transformacion(){
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 4; j++)
                m[i][j] = (i == j) ? 1 : 0; // Matriz identidad
    }
    Matriz_Transformacion(double m[3][3]): Matriz_Transformacion() {
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
                this->m[i][j] = m[i][j];
    }
    Matriz_Transformacion(double p[3]): Matriz_Transformacion() {
        for(int i = 0; i < 3; i++)
            this->m[i][3] = p[i]; // Translation vector
    }
    Matriz_Transformacion(Vector3D p): Matriz_Transformacion() {
        this->m[0][3] = p.x;
        this->m[1][3] = p.y;
        this->m[2][3] = p.z;
    }
    Matriz_Transformacion(double m[3][3], double p[3]){
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 4; j++){
                if(j==3) this->m[i][j] = p[i];
                else this->m[i][j] = m[i][j];
            }
        for(int j = 0; j < 4; j++)
            this->m[3][j] = (j == 3) ? 1 : 0; // Last row for homogeneous coordinates
        }


    Matriz_Transformacion operator*(const Matriz_Transformacion& other) const{
        Matriz_Transformacion result;
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                result.m[i][j] = 0;
                for(int k = 0; k < 4; k++) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
        }
    Matriz_Transformacion operator*(const int& num) const{
        Matriz_Transformacion result;
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                result.m[i][j] *= num;
            }
        }
        return result;
    }
    Matriz_Transformacion Inversa() const{
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
};


Vector3D Transformacion(const Vector3D& v, const Matriz_Transformacion &matriz){
    Vector3D result;
    result.x = v.x * matriz.m[0][0] + v.y * matriz.m[0][1] + v.z * matriz.m[0][2] + matriz.m[0][3];
    result.y = v.x * matriz.m[1][0] + v.y * matriz.m[1][1] + v.z * matriz.m[1][2] + matriz.m[1][3];
    result.z = v.x * matriz.m[2][0] + v.y * matriz.m[2][1] + v.z * matriz.m[2][2] + matriz.m[2][3];
    return result;
}

Vector3D operator+(Vector3D lhs, const Vector3D& rhs){
    lhs += rhs;
    return lhs;
}
Vector3D operator-( const double lhs[3], Vector3D rhs ){
    rhs.x = lhs[0]- rhs.x;
    rhs.y = lhs[1]- rhs.y;
    rhs.z = lhs[2]- rhs.z;

    return rhs;
}
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

std::ostream& operator<<(std::ostream& os, const Matriz_Transformacion& mt) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            os << mt.m[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}
std::ostream& operator<<(std::ostream& os, const Vector3D& v) {
    os << "Vector3D(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

void rotZ(float angulo, double m[3][3]){
    float rad = angulo * M_PI / 180.0f; // Convert degrees to radians
    m[0][0] = cos(rad); m[0][1] = -sin(rad);  m[0][2] = 0;
    m[1][0] = sin(rad); m[1][1] = cos(rad); m[1][2] = 0;
    m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
}

int main(){

    double m[3][3];
    double p[3] = {0.08, 0.0825, 0};
    rotZ(45.8814, m); // Rotate 90 degrees around Z-axis
    Matriz_Transformacion matriz_Centro_Pata_THOR(m, p);

    p[0] = -0.08; p[1] = 0.0825; p[2] = 0.0;
    rotZ(134.1186,m);
    Matriz_Transformacion matriz_Centro_Pata_FRIGG(m,p);

    p[0] = -0.08; p[1] = -0.0825; p[2] = 0.0;
    rotZ(-134.1186,m);
    Matriz_Transformacion matriz_Centro_Pata_ODIN(m,p);

    p[0] = 0.08; p[1] = -0.0825; p[2] = 0.0;
    rotZ(-45.8814,m);
    Matriz_Transformacion matriz_Centro_Pata_LOKI(m,p);
    Vector3D centro, Origen;
    Vector3D THOR, FRIGG, LOKI, ODIN;
    Vector3D TCP{0.402,0.035, 0.037}, THOR_TCP, FRIGG_TCP, ODIN_TCP, LOKI_TCP;

    THOR = Transformacion(centro, matriz_Centro_Pata_THOR);
    FRIGG = Transformacion(centro, matriz_Centro_Pata_FRIGG);
    ODIN = Transformacion(centro, matriz_Centro_Pata_ODIN);
    LOKI = Transformacion(centro, matriz_Centro_Pata_LOKI);

    THOR_TCP = Transformacion(TCP, matriz_Centro_Pata_THOR);
    FRIGG_TCP = Transformacion(TCP, matriz_Centro_Pata_FRIGG);
    ODIN_TCP = Transformacion(TCP, matriz_Centro_Pata_ODIN);
    LOKI_TCP = Transformacion(TCP, matriz_Centro_Pata_LOKI);


    std::cout << "Posicion del centro : " << centro << std::endl;
    std::cout << "\nCoordenada global pata THOR: " << THOR << std::endl;
    std::cout << "Coordenada global TCP THOR: " << THOR_TCP << std::endl;
    std::cout << "\nCoordenada global pata FRIGG: " << FRIGG << std::endl;
    std::cout << "Coordenada global TCP FRIGG: " << FRIGG_TCP << std::endl;
    std::cout << "\nCoordenada global pata ODIN: " << ODIN << std::endl;
    std::cout << "Coordenada global TCP ODIN: " << ODIN_TCP << std::endl;
    std::cout << "\nCoordenada global pata LOKI: " << LOKI << std::endl;
    std::cout << "Coordenada global TCP LOKI: " << LOKI_TCP << std::endl;
    //std::cout << "Matriz de Transformacion:\n" << matriz_Centro_Pata_THOR;
    
    
    //Movimiento del centro
    double movimiento[3] {0.1,0 ,0};
    Matriz_Transformacion mov(movimiento);
    // std::cout << "Matriz de movimiento:\n" << mov << std::endl;
    // std::cout << "Matriz de transformacion pata:\n" << matriz_Centro_Pata_THOR << std::endl;
    // std::cout << "Matriz de transformacion final:\n" << mov * matriz_Centro_Pata_THOR << std::endl;
    //centro.Traslacion(p);

    //Transformacion de la pata
    centro = Transformacion(Origen, mov);
    
    THOR = Transformacion(Origen, mov * matriz_Centro_Pata_THOR);
    FRIGG = Transformacion(Origen, mov * matriz_Centro_Pata_FRIGG);
    ODIN = Transformacion(Origen, mov * matriz_Centro_Pata_ODIN);
    LOKI = Transformacion(Origen, mov * matriz_Centro_Pata_LOKI);

    TCP = Transformacion(TCP, (mov * matriz_Centro_Pata_THOR).Inversa());
    THOR_TCP = Transformacion(THOR_TCP, (mov * matriz_Centro_Pata_THOR).Inversa());
    FRIGG_TCP = Transformacion(FRIGG_TCP, (mov * matriz_Centro_Pata_FRIGG).Inversa());
    ODIN_TCP = Transformacion(ODIN_TCP, (mov * matriz_Centro_Pata_ODIN).Inversa());
    LOKI_TCP = Transformacion(LOKI_TCP, (mov * matriz_Centro_Pata_LOKI).Inversa());

    //std::cout << "Posicion del centro : " << centro << std::endl;
    std::cout << "\nCoordenada global pata THOR: " << THOR << std::endl;
    std::cout << "Coordenada global TCP THOR: " << THOR_TCP << std::endl;
    std::cout << "\nCoordenada global pata FRIGG: " << FRIGG << std::endl;
    std::cout << "Coordenada global TCP FRIGG: " << FRIGG_TCP << std::endl;
    std::cout << "\nCoordenada global pata ODIN: " << ODIN << std::endl;
    std::cout << "Coordenada global TCP ODIN: " << ODIN_TCP << std::endl;
    std::cout << "\nCoordenada global pata LOKI: " << LOKI << std::endl;
    std::cout << "Coordenada global TCP LOKI: " << LOKI_TCP << std::endl;

    //std::cout << "Coordenada global TCP: " << Transformacion(TCP, (mov * matriz_Centro_Pata_THOR)) << std::endl;
    return 0;
}