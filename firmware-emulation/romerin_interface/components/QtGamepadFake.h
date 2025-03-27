#ifndef QTGAMEPADFAKE_H
#define QTGAMEPADFAKE_H

struct QGamepad{
    bool isConnected(){return false;}
    float axisLeftX(){return 0;}
    float axisRightX(){return 0;}
    float axisLeftY(){return 0;}
    float axisRightY(){return 0;}
};



#endif // QTGAMEPADFAKE_H
