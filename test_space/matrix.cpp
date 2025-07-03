#include <iostream>
#include <math.h>

struct Vector3D{
    float x, y, z;
    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}

    void Traslacion(const float p[3]) {
        this->x = this->x + p[0];
        this->y = this->y + p[1];
        this->z = this->z + p[2];
    }
};

struct Matriz_Transformacion{
    float m[4][4];
    Matriz_Transformacion(){
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 4; j++)
                m[i][j] = (i == j) ? 1 : 0; // Matriz identidad
    }
    Matriz_Transformacion(float m[3][3]): Matriz_Transformacion() {
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
                this->m[i][j] = m[i][j];
    }
    Matriz_Transformacion(float p[3]) : Matriz_Transformacion() {
        for(int i = 0; i < 3; i++)
            this->m[i][3] = p[i]; // Translation vector
    }
    Matriz_Transformacion(float m[3][3], float p[3]){
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 4; j++){
                if(j==3) this->m[i][j] = p[i];
                else this->m[i][j] = m[i][j];
            }
        for(int j = 0; j < 4; j++)
            this->m[3][j] = (j == 3) ? 1 : 0; // Last row for homogeneous coordinates
    }

    Matriz_Transformacion operator*(const Matriz_Transformacion& other) const {
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
    
    Matriz_Transformacion Inversa() const{       
        Matriz_Transformacion inv;  
        for(int i = 0; i < 3; i++) 
            for(int j = 0; j < 3; j++) 
                inv.m[i][j] = m[j][i]; // Transpose for rotation
    
        for(int i = 0; i < 3; i++) {
            inv.m[i][3] = -m[0][i] * m[0][3] - m[1][i] * m[1][3] - m[2][i] * m[2][3];
        }
        return inv;
    }
    Matriz_Transformacion Inversa2() const {
        Matriz_Transformacion inv;

        // Transponer rotación
        for(int i = 0; i < 3; i++)       // filas
            for(int j = 0; j < 3; j++)   // columnas
                inv.m[i][j] = m[j][i];

        // Calcular traslación inversa:
        // inv.m[i][3] = - sum_k (inv.m[i][k] * m[k][3])
        for(int i = 0; i < 3; i++) {
            inv.m[i][3] = 0;
            for(int k = 0; k < 3; k++) {
                inv.m[i][3] -= inv.m[i][k] * m[k][3];
            }
        }

        // Última fila homogénea
        for(int j = 0; j < 4; j++)
            inv.m[3][j] = (j == 3) ? 1 : 0;

        return inv;
    }

};

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

/* Da las coordenadas de un punto v respecto al sistema final en el sistema origen */
Vector3D Transformacion(const Vector3D& v, const Matriz_Transformacion &matriz) {
    Vector3D result;
    result.x = v.x * matriz.m[0][0] + v.y * matriz.m[0][1] + v.z * matriz.m[0][2] + matriz.m[0][3];
    result.y = v.x * matriz.m[1][0] + v.y * matriz.m[1][1] + v.z * matriz.m[1][2] + matriz.m[1][3];
    result.z = v.x * matriz.m[2][0] + v.y * matriz.m[2][1] + v.z * matriz.m[2][2] + matriz.m[2][3];
    return result;
}

void rotZ(float angulo, float m[3][3]){
    float rad = angulo * M_PI / 180.0f; // Convert degrees to radians
    m[0][0] = cos(rad); m[0][1] = -sin(rad);  m[0][2] = 0;
    m[1][0] = sin(rad); m[1][1] = cos(rad); m[1][2] = 0;
    m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
}

int main(){

    float m[3][3];
    float p[3] = {80, 82.5, 0};
    rotZ(45, m); // Rotate 90 degrees around Z-axis
    Matriz_Transformacion matriz_Centro_Pata(m, p);
    Vector3D centro, Origen;
    Vector3D pata, TCP{400,0, -100};

    pata = Transformacion(centro, matriz_Centro_Pata);

    std::cout << "Posicion del centro : " << centro << std::endl;
    std::cout << "Coordenada global pata: " << pata << std::endl;
    std::cout << "Coordenada global TCP: " << Transformacion(TCP, matriz_Centro_Pata) << std::endl;
    //std::cout << "Matriz de Transformacion:\n" << matriz_Centro_Pata;
    
    // /* op 1 */
    // float p2[] ={400,0,-100};
    // Vector3D pf = Transformacion(Origen, matriz_Centro_Pata * *new Matriz_Transformacion(p2));
    
    
    //// Movimiento del centro
    // float movimiento[3] {100,0 ,0};
    // Matriz_Transformacion mov(movimiento);
    // centro.Traslacion(p);
    // pf.x -= movimiento[0];
    // std::cout << "Punto final: " << Transformacion(pf, matriz_Centro_Pata.Inversa()) << std::endl;

    /* op 2 */

    //obtener pos global
    Vector3D pf = Transformacion(TCP, matriz_Centro_Pata);
    //Movimiento del centro
    float movimiento[3] {100,0 ,0};
    Matriz_Transformacion mov(movimiento);
    Matriz_Transformacion matriz_final_1 = mov * matriz_Centro_Pata;
    Matriz_Transformacion matriz_final_2 = matriz_Centro_Pata * mov;
    Matriz_Transformacion matriz_final_3 = mov.Inversa() * matriz_Centro_Pata.Inversa();

    std::cout << "Posicion global TCP: " << pf << std::endl;
    std::cout << "Punto final 1: " << Transformacion(pf, matriz_final_1.Inversa()) << std::endl;
    std::cout << "Matriz 1: \n" << matriz_final_1 << std::endl;
    std::cout << "Matriz 1 inversa: \n" << matriz_final_1.Inversa2() << std::endl;


    std::cout << "Punto final 2: " << Transformacion(pf, matriz_final_2.Inversa()) << std::endl;
    std::cout << "Matriz 2: \n" << matriz_final_2 << std::endl;
    std::cout << "Matriz 2 inversa: \n" << matriz_final_2.Inversa2() << std::endl;

    std::cout << "Punto final 3: " << Transformacion(pf, mov.Inversa()) << std::endl;
    std::cout << "Matriz 3: \n" << matriz_final_3 << std::endl;
    std::cout << "Matriz 3 inversa: \n" << matriz_final_3.Inversa2() << std::endl;
    
    // //Transformacion de la pata
    // centro = Transformacion(Origen, mov);
    // pata = Transformacion(Origen, mov * matriz_Centro_Pata);
    // TCP = Transformacion(TCP, (mov * matriz_Centro_Pata).Inversa());


    // std::cout << "Posicion del centro : " << centro << std::endl;
    // std::cout << "Coordenada global pata: " << pata << std::endl;
    // std::cout << "Coordenada TCP respecto a pata: " << TCP << std::endl;
    // std::cout << "Coordenada global TCP: " << Transformacion(TCP, (mov * matriz_Centro_Pata)) << std::endl;
    return 0;
}