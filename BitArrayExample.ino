#include <Arduino.h>
#include "BitArray.h"
#include <avr/pgmspace.h>

const byte  char_FLOWER[] PROGMEM = {
  0x00, 0x00,
  0x06, 0x18,
  0x04, 0x10,
  0x7F, 0x7F,
  0x04, 0x10,
  0x00, 0x00,
  0x0C, 0xC0,
  0x08, 0x80,
  0x10, 0x84,
  0x10, 0xFE,
  0x30, 0x80,
  0x50, 0x80,
  0x10, 0x82,
  0x10, 0x82,
  0x10, 0x83,
  0x10, 0x7E
};
const byte  char_SPRING[] PROGMEM  = {
  0x00, 0x00,
  0x01, 0x80,
  0x01, 0x00,
  0x3F, 0xFE,
  0x02, 0x00,
  0x1F, 0xFC,
  0x04, 0x00,
  0x7F, 0xFF,
  0x08, 0x08,
  0x10, 0x04,
  0x6F, 0xFB,
  0x08, 0x08,
  0x0F, 0xF8,
  0x08, 0x08,
  0x0F, 0xF8,
  0x08, 0x08
};

void callback(byte *bitmapBuffer , uint16_t rows, uint16_t columns) {
  uint8_t mask, bits, value;
  Serial.print(F("---------------------------")); 
  Serial.print(rows);Serial.print('X');Serial.println(columns<<3);
  for(uint16_t r=0;r<rows;r++) {
    for(uint16_t c=0;c<columns;c++) {
      mask=0x80;value =*bitmapBuffer++;
      for(bits=0;bits<8;bits++, mask>>=1) {
       Serial.print( (value & mask)?'*':'.'); 
      }
    }
    Serial.print(":line="); Serial.println(r);
  }
    Serial.println();
    Serial.println();
}

BitArray bm (16,24,callback);

void testBit() {
  for(uint8_t i=0;i<bm.rows;i++) {
    for(uint8_t j=0;j<bm.columns;j++) {
      for(uint8_t k=0;k<8;k++)  bm.setDot(i,j*8+k,1);
    }
  }
}

void test() {
  bm.clear();
  
  bm.load16x16(char_FLOWER,0,0);
  bm.rotateRight();
  bm.rotateRight();
  for(uint8_t c=0;c<16;c++)
  bm.rotateDown(false,0xff,0xff,0xff);
}

void setup() {
  Serial.begin(115200);
  bm.init();
  test();
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
