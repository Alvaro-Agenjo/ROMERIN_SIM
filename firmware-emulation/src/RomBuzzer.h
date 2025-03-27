#pragma once
#include <Arduino.h>
#include "RomerinDefinitions.h"
struct RomNote{
    int freq;
    int time;
    };
struct RomMelody{
        int num;
        const RomNote *notes;
    };
const RomNote music1[3]={{1000,100},
                        {2000,100},
                        {3000,100}
                        };
const RomNote music2[5]={{3500,100},
                        {3000,100},
                        {2500,100},
                        {2000,100},
                        {1500,100}
                        };

class RomBuzzer
{
    enum BUZZ_STATE {IDLE, PLAY, PLAYSEC } state;

    RomMelody melodies[2]={
        {3,music1},
        {5,music2}
    };
    unsigned long time;
    unsigned long timeout;
    int playing_melody;
    int playing_note;
    bool repeat_melody;
    public:
        void setup();
        void loop();
        void blocking_beep(int ms, int freq=2000);
        void beep(int ms);
        void play(int num, int vol=125, bool repeat=false);
};