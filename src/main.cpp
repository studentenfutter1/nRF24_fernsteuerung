/*
  nRF24     ESP32
  -----------------
  V+      3v3
  GND     GND
  CSN     SPI_CS0 (5)
  CE      GPIOx   (4)
  MOSI    23
  SCK     18
  IRQ     -
  MISO    19

*/

#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"


//=======================
//    #defines
//=======================

#define X1PIN 32
#define Y1PIN 33
#define BTN1PIN 34
#define X2PIN 25
#define Y2PIN 26
#define BTN2PIN 27

//=======================
//    globals
//=======================

static uint16_t count = 0;

typedef struct data{
  uint16_t x1Val;
  uint16_t y1Val;  // 12-bit ADC
  uint8_t btn1State;
  uint16_t x2Val;
  uint16_t y2Val;  // 12-bit ADC
  uint8_t btn2State;
} data_t;
  data_t gsData;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus*/
// dont use HSPI pins
RF24 radio(4,5);

// Pipes 1-5 should share the first 32 bits. Only the least significant byte should be unique
const uint64_t pipe = 0xF0F0F0F0A1LL;


//=======================
//    main
//=======================

void setup() {
  //Serial.begin(9600);
  pinMode(X1PIN, INPUT);
  pinMode(Y1PIN, INPUT);
  pinMode(BTN1PIN, INPUT);
  pinMode(X2PIN, INPUT);
  pinMode(Y2PIN, INPUT);
  pinMode(BTN2PIN, INPUT);

  gsData.btn1State = 0;
  gsData.btn2State = 0;

  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(101);  // 2,4GhZ -> 2,525 GhZ (0 ... 124)
  radio.openWritingPipe(pipe);
//  radio.printDetails();
}

void loop() {
/*  if(count < 100)
      count++;
  else
    count = 0;
*/
  gsData.x1Val = analogRead(X1PIN);
  gsData.y1Val = analogRead(Y1PIN);
  gsData.btn1State = digitalRead(BTN1PIN);
  gsData.x2Val = analogRead(X2PIN);
  gsData.y2Val = analogRead(Y2PIN);
  gsData.btn2State = digitalRead(BTN2PIN);

  radio.write(&gsData, sizeof(gsData));

/*  Serial.print("x1: "); Serial.print(gsData.x1Val);
  Serial.print("   y1: "); Serial.print(gsData.y1Val);
  Serial.print("   btn1: "); Serial.print(gsData.btn1State);
  Serial.print("   x2: "); Serial.print(gsData.x2Val);
  Serial.print("   y2: "); Serial.print(gsData.y2Val);
  Serial.print("   btn2: "); Serial.println(gsData.btn2State);*/
  //radio.write(&count, sizeof(count));
  delay(200);
}
