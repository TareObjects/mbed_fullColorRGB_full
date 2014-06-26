#include "mbed.h"


#define M_PI 3.1415926535897932384626433832795


// declare pwm ports
PwmOut pRed(dp1);      // red
PwmOut pGreen(dp2);    // green
PwmOut pBlue(dp18);    // blue


// adjust blightness
const float kRed   = 1.00;
const float kGreen = 0.15;
const float kBlue  = 0.80;


// declare clock 
Ticker  tokei;
volatile long gMSec;


//// calc gause function
//int gause(int u, int x, float rate) {
//    float fx = (x - u) / 100.0;
//    fx = fx * fx;
//    
//    float g = rate / 2.506628 * exp(-fx);
//    
//    return g;
//}

// increment gMSec(mili seconds in global) 
void tickerProcess() {
    gMSec ++;
}

// adjust negative logic and bligtness
void putRed(float v)   { pRed   = 1.0 - v*kRed;   }
void putGreen(float v) { pGreen = 1.0 - v*kGreen; }
void putBlue(float v)  { pBlue  = 1.0 - v*kBlue;  }


// turn off all pwm ports for led
void turnOff() {
    putRed(0.0);
    putGreen(0.0);
    putBlue(0.0);
}


// turn LEDs by each bit    
void switchRGB(int red, int green, int blue) {
    putRed(red     ? 1.0 : 0.0);
    putGreen(green ? 1.0 : 0.0);
    putBlue(blue   ? 1.0 : 0.0);
}


// show pattern1 : on/off each bit
void pattern1() {
    turnOff();
    
    int color = 0;
    
    for (int sec = 0; sec < 30; sec++) {
        if ((sec % 2) == 0) {
            color = (rand() % 7) + 1;    
        }
        switchRGB((color & 4), (color & 2), (color & 1));
        wait(1);  // wait, not too short, not too long. 
    }
}


// setting for pattern2
const int kMaxRotationForOffset   = 18;
const int kMaxPatternsForRotation =  5;
const int kMaxOffsetForRotation  =   5;

const int kMaxDuration  = 10000;
const int kLeastDuration = 1000;


// cals starting point of each LED
float offsetForRotation() {
    return M_PI / (rand() % kMaxRotationForOffset);
}
// cals rotation multiply (larger number for faster blinking)
int multiplyForRotation() {
    return (rand() % kMaxPatternsForRotation) + kMaxOffsetForRotation;
}
// set duration of one session for each LED
long endAt() {
    return gMSec + (rand() % kMaxDuration) + kLeastDuration;
}
// cals sin curve for each LED
float calcSin(float offset, float multiply, float radian) {
    return sin(offset + multiply*radian) * 0.5 + 0.5;
}


// pattern2 : bling each led in random frequency and random duration
void pattern2() {
     gMSec = 99;
     float dOffsetRed = offsetForRotation();
     volatile long  dRedEndAt = 0;     
     float radianRed  = 0.0;
          
     float dOffsetGreen = offsetForRotation();
     long  dGreenEndAt  = 0;    
     float radianGreen  = 0.0;
      
     float dOffsetBlue = offsetForRotation();
     long  dBlueEndAt  = 0;     
     float radianBlue  = 0.0;
     
     float dMultiplyRed   = 0.0;
     float dMultiplyGreen = 0.0;
     float dMultiplyBlue  = 0.0;
        
     while (gMSec < 30000) {    // 1 min
        
        if (gMSec > dRedEndAt) {
            dMultiplyRed = multiplyForRotation();
            dRedEndAt = endAt();
        }
        if (gMSec > dGreenEndAt) {
            dMultiplyGreen = multiplyForRotation();
            dGreenEndAt  = endAt();
        }
        if (gMSec > dBlueEndAt) {
            dMultiplyBlue = multiplyForRotation();
            dBlueEndAt  = endAt();
        }
        putRed(   calcSin(dOffsetRed,   dMultiplyRed,   radianRed)  );
        putGreen( calcSin(dOffsetGreen, dMultiplyGreen, radianGreen));
        putBlue(  calcSin(dOffsetBlue,  dMultiplyBlue,  radianBlue) );

        radianRed   += 0.03;
        radianGreen += 0.03;
        radianBlue  += 0.03;
        
        wait_ms(10);       // pwm needs delay
    }
}


// main mbed
int main() {
    // init pwm freq is 1khz
    pRed.period(0.001);
    pGreen.period(0.001);
    pBlue.period(0.001);
    
    turnOff();
    
    // init time
    gMSec = 0;
    tokei.attach(&tickerProcess, 0.001);
    
    // call each patterns forever
    while(1) {
        // simple on/off each bit ticker
        pattern1();
        
        // random wave-form ticker
        pattern2();
    }
}