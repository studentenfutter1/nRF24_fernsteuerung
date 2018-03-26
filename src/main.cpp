//  output: values from 0 to 100, where 50
//  is the middle position of the thumbsticks

// TBD: DIP Switch. Set PA_LEVEl via switch
// set channel via switch
// change to smaller datatypes

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

#define SERIALDEBUG

#define X1PIN 33
#define Y1PIN 32
#define BTN1PIN 34
#define X2PIN 26
#define Y2PIN 25
#define BTN2PIN 27

//=======================
//    globals
//=======================

const uint8_t channel = 101;
static uint16_t count = 0;

typedef struct data{
  uint8_t x1Val;
  uint8_t y1Val;  // 12-bit ADC
  uint8_t btn1State;
  uint8_t x2Val;
  uint8_t y2Val;  // 12-bit ADC
  uint8_t btn2State;
} data_t;
  data_t gsData;

uint8_t x1Offset = 0;
uint8_t y1Offset = 0;
uint8_t x2Offset = 0;
uint8_t y2Offset = 0;

const uint8_t upperDeadzone = 55;
const uint8_t lowerDeadzone = 45;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus*/
// dont use HSPI pins
RF24 radio(4,5);

// Pipes 1-5 should share the first 32 bits. Only the least significant byte should be unique
const uint64_t pipe = 0xF0F0F0F0A1LL;


//=======================
//    main
//=======================

void setup() {
  #ifdef SERIALDEBUG
    Serial.begin(9600);
  #endif

  pinMode(X1PIN, INPUT);
  pinMode(Y1PIN, INPUT);
  pinMode(BTN1PIN, INPUT);
  pinMode(X2PIN, INPUT);
  pinMode(Y2PIN, INPUT);
  pinMode(BTN2PIN, INPUT);

  gsData.btn1State = 0;
  gsData.btn2State = 0;

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(channel);  // 2,4GhZ -> 2,525 GhZ (0 ... 124)
  radio.openWritingPipe(pipe);
}

void loop() {
  gsData.x1Val = map(analogRead(X1PIN), 0, 4095, 100, 0) ;
  gsData.y1Val = map(analogRead(Y1PIN), 0, 4095, 0, 100) ;
  gsData.btn1State = digitalRead(BTN1PIN);
  gsData.x2Val = map(analogRead(X2PIN), 0, 4095, 100, 0) ;
  gsData.y2Val = map(analogRead(Y2PIN), 0, 4095, 0, 100) ;
  gsData.btn2State = digitalRead(BTN2PIN);

  if(gsData.x1Val <= upperDeadzone && gsData.x1Val >= lowerDeadzone)
    gsData.x1Val = 50;
  if(gsData.y1Val <= upperDeadzone && gsData.y1Val >= lowerDeadzone)
    gsData.y1Val = 50;
  if(gsData.x2Val <= upperDeadzone && gsData.x2Val >= lowerDeadzone)
    gsData.x2Val = 50;
  if(gsData.y2Val <= upperDeadzone && gsData.y2Val >= lowerDeadzone)
    gsData.y2Val = 50;

#ifdef SERIALDEBUG
  Serial.print("x1: "); Serial.print(gsData.x1Val);
  Serial.print("   y1: "); Serial.print(gsData.y1Val);
  Serial.print("   btn1: "); Serial.print(gsData.btn1State);
  Serial.print("   x2: "); Serial.print(gsData.x2Val);
  Serial.print("   y2: "); Serial.print(gsData.y2Val);
  Serial.print("   btn2: "); Serial.println(gsData.btn2State);
#endif

  radio.write(&gsData, sizeof(gsData));

//  delay(200);
}


// =============================================
//  raspberry pi 3b code
// =============================================

/*
'''
pins:
nRF24L01+	PI (phys. num.)
-----------------
V+			1
GND			6
CSN			24
CE			15
MOSI		19
SCK			23
IRQ			-
MISO		21
'''

import RPi.GPIO as GPIO
from RF24 import *
import time
import spidev
import struct

GPIO.setmode(GPIO.BCM)

pipe = 0xF0F0F0F0A1

radio = RF24(22, 0)	 #????
radio.begin()
radio.setChannel(101)	# 2,4GHz -> 2,525 GHz (0...124), same channel as sender
radio.setPALevel(RF24_PA_MAX)

#radio.printDetails()

radio.openReadingPipe(0, pipe)
radio.startListening()

while(1):
	if radio.available():
		receivedMessage = radio.read(6)	# read 11 bytes
		receivedData = struct.unpack("bbbbbb", receivedMessage)	# unpack received data
		x1_val = receivedData[0]
		y1_val = receivedData[1]
		btn1_State = receivedData[2]
		x2_val = receivedData[3]
		y2_val = receivedData[4]
		btn2_State = receivedData[5]

		print("x1: {0} y1: {1} btn1: {2} 	x2: {3} y2:{4} btn2:{5}".format(x1_val, y1_val, btn1_State, x2_val, y2_val, btn2_State))
*/
