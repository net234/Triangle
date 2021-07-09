/*******************************
  Triangle V1.0  Animation lumioneuse avec WS2812

   (C) net234 Pierre HENRY 03/2021



*/

#include <Arduino.h>
#include "Nano.h"




#define APP_VERSION   "Triangle V1.0"

//enum typeEvent { evNill=0, ev100Hz, ev10Hz, ev1Hz, ev24H, evInChar,evInString,evUser=99 };
enum myEvent  {
  evBP0 = 100,
  evLed0,
  evNewLed,
  evNewSpeed,
};

// Gestionaire d'evenemnt
#include <BetaEvents.h>
#include "WS2812.h"
#include  "ledSet.h"




animLed_t animLed1 ;
animLed_t animLed2 ;
animLed_t animLed3 ;
animLed_t animLed4 ;
animLed_t animLed5 ;
animLed_t animLed6 ;
animLed_t animLed7 ;
animLed_t animLed8 ;
animLed_t animLed9 ;

const uint8_t max_led = 9;
animLed_t* ledSet[max_led] {&animLed1, &animLed2, &animLed3, &animLed4, &animLed5, &animLed6, &animLed7, &animLed8, &animLed9 };
//int clockStep = -1;







uint8_t BP0Multi = 0;
e_rvb baseColor = rvb_orange;



void setup() {

  // Serial
  Serial.begin(115200);
  Serial.println(F(APP_VERSION));

  // init event
  MyEvents.begin();
  MyEvents.pushDelayEvent(1000, evNewLed);

  //D9 = 0V
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);


}


int8_t rotateSpeed = 0;
int8_t potar1 = 0;
bool   rotateClock = true;
uint8_t currentLed = 0;

void loop() {
  MyEvents.getEvent();       // get standart event
  MyEvents.handleEvent();      // handle standart event

  switch (MyEvents.currentEvent.code)
  {

    case ev10Hz: {
        int16_t N = analogRead(A0);
        static int16_t potar1read = 0;
        potar1read = (potar1read  + map(N, 0, 1023, -100, 100)) / 2;
        if (potar1read != potar1) {
          potar1 = potar1read;
          MyEvents.pushEvent(evNewSpeed);
          D_println(potar1read);
        }

      }
      break;

    case ev100Hz:
      // refresh led
      for (int8_t N = 0 ; N < max_led; N++) {
        ledSet[N]->led.write();
      }
      animLed1.led.reset();
      for (int8_t N = 0 ; N < max_led; N++) {
        ledSet[N]->step(MyEvents.currentEvent.ext);
      }
      break;

    case evNewSpeed: {
        bool speedWas0 = (rotateSpeed == 0);
        //D_println(potar1);
        rotateSpeed = 100-abs(potar1);
        rotateClock = potar1 >= 0;
        if (speedWas0) MyEvents.pushDelayEvent(500  * rotateSpeed / 100, evNewLed);
        D_println(rotateSpeed);
      }
      break;


    case  evNewLed:
     
      if (rotateSpeed != 0) MyEvents.pushDelayEvent(500  * rotateSpeed / 100, evNewLed);
      ledSet[currentLed]->set(100, 100, 100, baseColor, 100);
      currentLed = (currentLed + (rotateClock ? 1 : max_led-1) ) % max_led;


      break;


    case evBP0:
      D_println(MyEvents.currentEvent.ext);
      switch (MyEvents.currentEvent.ext) {




        case evxBPUp:
          Serial.println(F("BP0 Up"));

          break;
        case evxBPDown:
          randomSeed(micros());
          BP0Multi++;
          Serial.println(F("BP0 Down"));
          baseColor = (e_rvb)(baseColor + 1);
          if (baseColor == rvb_black) baseColor = rvb_white;
          //animLed1.set(200, 10, 200, baseColor, 100);
          break;

        case evxBPLongDown:
          Serial.println(F("BP0 Long Down"));

          break;

        case evxBPLongUp:
          BP0Multi = 0;
          Serial.println(F("BP0 Long Up"));
          break;

      }
      break; // evBP0

    // eventuelles commande via le port serie
    case evInString:
      //      Serial.print(F("evInString '"));
      //      Serial.print(MyEvent.inputString);
      //      Serial.println("'");


      if (MyKeyboard.inputString.equals(F("RESET"))) {
        Serial.println("resetting");
        delay(500);
        helperReset();
        Serial.println("--?-?--");
      }

      if (MyKeyboard.inputString.equals(F("A"))) {
        D_println(animLed1.enable);
        D_println(animLed2.enable);
        D_println(animLed3.enable);
      }
      break;
  }
}
