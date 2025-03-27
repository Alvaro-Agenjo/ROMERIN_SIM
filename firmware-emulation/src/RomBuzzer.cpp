#include "RomBuzzer.h"

void RomBuzzer::setup()
{
  pinMode(BUZZER, OUTPUT);
  state=BUZZ_STATE::IDLE;
  ledcSetup(PWM_CHANNEL_BUZZER, 5000, 8);
  ledcAttachPin(BUZZER, PWM_CHANNEL_BUZZER);
}
void RomBuzzer::loop()
{
    if(state==BUZZ_STATE::IDLE){
        ledcWrite(PWM_CHANNEL_BUZZER, 0);
        return;
    }
    if((state==BUZZ_STATE::PLAY)&&(millis()-time>timeout)){
      ledcWrite(PWM_CHANNEL_BUZZER, 0);
      state=BUZZ_STATE::IDLE;
      return;
    }
    if(state==BUZZ_STATE::PLAYSEC){
        if(millis()-time>timeout){
            if(++playing_note< melodies[playing_melody].num){
                time=millis();
                timeout=melodies[playing_melody].notes[playing_note].time;
                ledcWriteTone(PWM_CHANNEL_BUZZER, melodies[playing_melody].notes[playing_note].freq);
            }
            else
            {
               if(repeat_melody){
                   playing_note=0;
                   time=millis();
                   timeout=melodies[playing_melody].notes[0].time;
                    ledcWriteTone(PWM_CHANNEL_BUZZER, melodies[playing_melody].notes[0].freq);
               }
               else{
                 ledcWrite(PWM_CHANNEL_BUZZER, 0);
                 state=BUZZ_STATE::IDLE;
               }
            }
            
        } 
    }


}
void RomBuzzer::blocking_beep(int ms, int freq){
    ledcWriteTone(PWM_CHANNEL_BUZZER, freq);
    ledcWrite(PWM_CHANNEL_BUZZER, 125);
     //digitalWrite(BUZZER, HIGH);
     delay(ms);
     ledcWrite(PWM_CHANNEL_BUZZER, 0);
     //digitalWrite(BUZZER, LOW);
     delay(100);
}
void RomBuzzer::beep(int ms)
{
  state=BUZZ_STATE::PLAY;
  time=millis();
  timeout=ms;
  ledcWriteTone(PWM_CHANNEL_BUZZER, 2000);
  ledcWrite(PWM_CHANNEL_BUZZER, 125);

  //digitalWrite(BUZZER, HIGH);
}

void RomBuzzer::play(int num, int vol, bool repeat)
{
  state=BUZZ_STATE::PLAYSEC;
  time=millis();
  playing_melody=num;
  playing_note=0;
  repeat_melody=repeat;
  timeout=melodies[num].notes[0].time;
  ledcWrite(PWM_CHANNEL_BUZZER, vol);
  ledcWriteTone(PWM_CHANNEL_BUZZER, melodies[num].notes[0].freq);
}