#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"

typedef struct {
  uint8_t x1;
  uint8_t y1;
  uint8_t x2;
  uint8_t y2;
  // buttons?
} dataStruct;
dataStruct sData;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(8,12); // CE, CSN

const uint64_t pipe = 0xF0F0F0F0E1LL;

void setup() {
  radio.setPALevel(RF24_PA_HIGH);
  radio.setChannel(100);  // 2,4GhZ -> 2,525 GhZ (0 ... 124)
  radio.openWritingPipe(pipe);
}

void loop() {
  static uint8_t count = 0;
  count++;
  if(count > 100)
    count = 0;

  radio.write(&count, sizeof(count));
  delay(200);
}
