#include <Arduino.h>
#include "FastLED.h"
#include "config.h"
#include "timeObj.h"

#pragma region INITIALIZATION
int TurnSignalState = LOW;
int StopSignalState = LOW;
int RunSignalState = LOW;

int brightens2[4] = {80,40,20,0};

CRGB TurnSignalStrip[TS_LED_NUMBER];
CRGB StopSignalStrip[SS_LED_NUMBER];
CRGB RunningStrip[RS_LED_NUMBER];

int delayTurnLedAnim = 15; 
bool onStartupRunning = true;
bool fStop = true;
bool StopAnimationCanBeActive = true;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;

timeObj STOPTimer(10.0*1000.0,false);


void timeLoop (unsigned long int startMillis, unsigned long int interval) { // delay substitute function
  while (millis() - startMillis < interval) {}
}
#pragma endregion INITIALIZATION

#pragma region ANIMATIONS

void StartUpAnimation(){
  int max_leds = 0;
  if(TS_LED_NUMBER >= SS_LED_NUMBER) max_leds = TS_LED_NUMBER;
  if(RS_LED_NUMBER >= max_leds) max_leds = RS_LED_NUMBER;

  for(int i=0;i<=max_leds;i++){
    for(int y=0;y<max_leds-i;y++){
      TurnSignalStrip[y] = CHSV( BOOT_HUE, BOOT_SAT, brightens2[0]);
      TurnSignalStrip[y-1] = CHSV( 0, 0, 0);

      StopSignalStrip[y] = CHSV( BOOT_HUE, BOOT_SAT, brightens2[0]);
      StopSignalStrip[y-1] = CHSV( 0, 0, 0);

      RunningStrip[y] = CHSV( BOOT_HUE, BOOT_SAT, brightens2[0]);
      RunningStrip[y-1] = CHSV( 0, 0, 0);

      FastLED[TURN_INDEX].showLeds();
      FastLED[STOP_INDEX].showLeds();
      FastLED[RUN_INDEX].showLeds();
      timeLoop(millis(),SPEED_CONTROL-40);
    }
    timeLoop(millis(),SPEED_CONTROL+20);
  }

  timeLoop(millis(),1500);
  FastLED[TURN_INDEX].showColor(CRGB::Black);
  FastLED[STOP_INDEX].showColor(CRGB::Black);
  FastLED[RUN_INDEX].showColor(CRGB::Black);
  
}
void TurnSignalAnimationON(){
  // follow animation
  for(int dot=0;dot<TS_LED_NUMBER;dot++){
      if(dot<=TS_LED_NUMBER-2){
        TurnSignalStrip[dot] = CHSV( TURN_HUE, TURN_SAT, brightens2[0]);
        TurnSignalStrip[dot+1] = CHSV( TURN_HUE, TURN_SAT, brightens2[1]);
        TurnSignalStrip[dot+2] = CHSV( TURN_HUE, TURN_SAT, brightens2[2]);
      }else{
        TurnSignalStrip[dot] = CHSV( TURN_HUE, TURN_SAT, brightens2[0]);
      }
      FastLED[TURN_INDEX].showLeds();
      currentMillis = previousMillis = millis();
      while(previousMillis + delayTurnLedAnim >= currentMillis){
        FastLED[TURN_INDEX].showLeds();
        currentMillis = millis();
      }
    }
    for(int dot=0;dot<TS_LED_NUMBER;dot++){
      if(dot<=TS_LED_NUMBER-2){
        TurnSignalStrip[dot] = CHSV( TURN_HUE, TURN_SAT, brightens2[3]);
        TurnSignalStrip[dot+1] = CHSV( TURN_HUE, TURN_SAT, brightens2[2]);
        TurnSignalStrip[dot+2] = CHSV( TURN_HUE, TURN_SAT, brightens2[1]);
      }else{
        TurnSignalStrip[dot] = CHSV( TURN_HUE, TURN_SAT, brightens2[3]);
      }
      FastLED[TURN_INDEX].showLeds();
      currentMillis = previousMillis = millis();
      while(previousMillis + delayTurnLedAnim >= currentMillis){
        FastLED[TURN_INDEX].showLeds();
        currentMillis = millis();
      }
    }
}
void TurnSignalAnimationOFF(){
  fill_solid(TurnSignalStrip,TS_LED_NUMBER,CRGB::Black);
  FastLED[TURN_INDEX].showLeds();
}
void StopSignalAnimationON() {
  // fix here
  if(fStop){
    // blink animation here
    STOPTimer.start();
    fStop = false;
    for(int i=0;i<5;i++){
      fill_solid(StopSignalStrip, SS_LED_NUMBER, CHSV(STOP_HUE, STOP_SAT, brightens2[0]));
      timeLoop(millis(),random(100,350));
      fill_solid(StopSignalStrip, SS_LED_NUMBER, CHSV(STOP_HUE, STOP_SAT, brightens2[3]));
      timeLoop(millis(),random(50,200));
    }  
  }else{
    // static animation here
    fill_solid(StopSignalStrip, SS_LED_NUMBER, CHSV(STOP_HUE, STOP_SAT, brightens2[0]));
    FastLED[STOP_INDEX].showLeds();
  }
  
}
void StopSignalAnimationOFF(){
  fill_solid(StopSignalStrip,SS_LED_NUMBER,CRGB::Black);
  FastLED[STOP_INDEX].showLeds();
}
void RunSignalAnimationON(){
  if(onStartupRunning){
      for(int i=0;i<10;i++){
        fill_solid(RunningStrip,RS_LED_NUMBER,CHSV(RUN_HUE,RUN_SAT,i*25));
        currentMillis = previousMillis = millis();
        while(previousMillis + 100 >= currentMillis){
          FastLED[RUN_INDEX].showLeds();
          currentMillis = millis();
        }
      }
      onStartupRunning = false;
    }else{
      fill_solid(RunningStrip,RS_LED_NUMBER,CHSV(RUN_HUE,RUN_SAT,255));
      FastLED[RUN_INDEX].showLeds();
    }
}
void RunSignalAnimationOFF(){
  onStartupRunning = true;
    fill_solid(RunningStrip,RS_LED_NUMBER,CRGB::Black);
    FastLED[RUN_INDEX].showLeds(); 
}

#pragma endregion ANIMATIONS

#pragma region ANIMATION CONTROL

void readSignalStates() {
  TurnSignalState = digitalRead(TURNS_INPUT_PIN);
  StopSignalState = digitalRead(STOPS_INPUT_PIN);
  RunSignalState = digitalRead(RUNS_INPUT_PIN);
}
void controlTurnSignalAnimation() {
  if (TurnSignalState == HIGH) {
    TurnSignalAnimationON();
  } else if (TurnSignalState == LOW) {
    TurnSignalAnimationOFF();
  }
}
void controlStopSignalAnimation() {
  if (StopSignalState == HIGH) {
    StopSignalAnimationON();
  } else if (StopSignalState == LOW) {
    StopSignalAnimationOFF();
  }
}
void controlRunSignalAnimation() {
  if (RunSignalState == HIGH) {
    RunSignalAnimationON();
  } else if (RunSignalState == LOW) {
    RunSignalAnimationOFF();
  }
}

#pragma endregion ANIMATION CONTROL


#pragma region MAIN FUNCTIONS
void setup() {
  pinMode(TURNS_INPUT_PIN,INPUT);
  pinMode(STOPS_INPUT_PIN,INPUT);
  pinMode(RUNS_INPUT_PIN,INPUT);

  pinMode(TURNS_OUTPUT_PIN,OUTPUT);
  pinMode(STOPS_OUTPUT_PIN,OUTPUT);
  pinMode(RUNS_OUTPUT_PIN,OUTPUT);

  FastLED.addLeds<LED_TYPE, TURNS_OUTPUT_PIN, COLOR_ORDER>(TurnSignalStrip, TS_LED_NUMBER).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, STOPS_OUTPUT_PIN, COLOR_ORDER>(StopSignalStrip, SS_LED_NUMBER).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, RUNS_OUTPUT_PIN, COLOR_ORDER>(RunningStrip, RS_LED_NUMBER).setCorrection(TypicalLEDStrip);

  fill_solid(TurnSignalStrip,TS_LED_NUMBER,CRGB::Black);
  fill_solid(StopSignalStrip,SS_LED_NUMBER,CRGB::Black);
  fill_solid(RunningStrip,RS_LED_NUMBER,CRGB::Black);

  StartUpAnimation();
}

void loop() {
  readSignalStates();
  controlTurnSignalAnimation();
  controlStopSignalAnimation();
  controlRunSignalAnimation();

  if (STOPTimer.ding()) {
    fStop = true;
  }
}
#pragma endregion MAIN FUNCTIONS