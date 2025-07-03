#include <iostream>

int main(){

    for(int i = 0; i < 6; ++i) {
        std::cout <<"X: " << i%2 << " Y: " << i%3 << std::endl;
    }
    return 0;
}