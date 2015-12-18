#include "Arduino.h"
#include "BitArray.h"

#define __BITARRAY_DEBUG__() callback(buffer,rows,columns)
#define __HALT__()  {while(1) {delay(3000); Serial.println(F("HALT.....")); }}

BitArray::BitArray(uint16_t _rows,uint16_t _columns, 
             void (*_callback)(uint8_t *p,uint16_t _rows,uint16_t _columns )) {
    callback=_callback;
    rows=_rows;
    columns=(_columns+7) >> 3;
}

void BitArray::init() {
    if( (rows+1)*columns > (int)sizeof(buffer)) {
        __HALT__();
    }
}

void BitArray::clear() {
    init();
    uint8_t *p=buffer;
    for(uint8_t i=(columns*rows);i>0;i--) *p++=0;
    __BITARRAY_DEBUG__();
}

void BitArray::inverse() {
    uint8_t *p=buffer;
    for(uint8_t i=(columns*rows);i>0;i--,p++) *p =~(*p);
    __BITARRAY_DEBUG__();
}

byte BitArray::bitReverse( byte x ) {
  //          01010101  |         10101010
  x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
  //          00110011  |         11001100
  x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
  //          00001111  |         11110000
  x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
  return x;
}

uint8_t BitArray::setDot(uint16_t row, uint16_t column, boolean b) {
    uint8_t mask= ((uint8_t)0x80) >> ( column & 0x07);
    column >>=3;
    if(row>=rows || column >=columns) return 0;
    if(b) buffer[row*columns+column] |= mask;
    else buffer[row*columns+column]  &= (~mask);
    __BITARRAY_DEBUG__();
    return mask;
}


void BitArray::load16x16(const byte *p,uint8_t rowOffset,uint8_t columnOffset) {
    uint8_t byteOffset=columnOffset >> 3;
    columnOffset &= 0x07;
    uint8_t *pos,b;
    uint8_t keepMask = ~(0xff >> columnOffset);
    pos = buffer+(rowOffset)*columns + byteOffset;

    if(columnOffset) {
        for(uint8_t c=0; c< 16; c++, pos += (columns-2)) {
            //if(rowOffset+c>=rows)break;

            b=pgm_read_byte(p);
            if( (byteOffset) < columns) {
                *pos = (*pos & keepMask) | (b >> columnOffset);
            }
            pos++; p++;
            if( (byteOffset +1) < columns) {
                *pos = (b << (8-columnOffset)); 
                b=pgm_read_byte(p);
                *pos |= ( b >> columnOffset);
            }
            pos++; p++;
            if( (byteOffset) +2 < columns) {
                *pos   = (b << (8-columnOffset)) | (*pos & (~keepMask));
            }

        }
    } else {
        for(uint8_t c=0; c< 16; c++, pos += (columns-2)) {
            // if(rowOffset+c>=rows)break;
            
            if( (byteOffset < columns)) 
                *pos = pgm_read_byte(p);
            pos++; p++;
            if( (byteOffset+1 < columns)) 
                *pos = pgm_read_byte(p);
            pos++; p++;
        }
    }
    __BITARRAY_DEBUG__();
}

void BitArray::swapV() {
    for(int i=0;i<rows/2;i++) {
        for(uint8_t c=0;c<columns;c++) {
            uint8_t t=buffer[i*columns+c];
            buffer[i*columns+c]=buffer[(rows+i-1)*columns+c];
            buffer[(rows+i-1)*columns+c]=t;
        }
    }
    __BITARRAY_DEBUG__();
}
void BitArray::swapH() {
    uint8_t c;
    for(uint8_t r=0;r<rows;r++) {
        for(c=0;c<columns/2;c++) {
            uint8_t t=bitReverse(buffer[r*columns+c]);
            buffer[r*columns+c]=bitReverse(buffer[(r+1)*columns-c-1]);
            buffer[(r+1)*columns-c-1]=t;
        }
        if(columns & 0x01) buffer[r*columns+c]=bitReverse(buffer[r*columns+c]);
    }
    __BITARRAY_DEBUG__();
}


void BitArray::setRow(uint8_t row, ...) {
    if(row>=rows) return;
    va_list argv;
    va_start(argv, row);
    for(uint8_t c=0;c<columns ;c++) {
        buffer[row*columns+c]=va_arg(argv, unsigned int);
    }
    va_end(argv);
    __BITARRAY_DEBUG__();
}

void BitArray::setColumn(uint8_t column, ...) {
    uint8_t bit= 0x80 >> (column & 0x07);
    uint8_t *pos=buffer +(column>>3);
    if(column>=columns) return;

    va_list argv;  va_start(argv, column);
    for(uint8_t r=rows/8; r>0 ;r--) {
        uint8_t value=va_arg(argv, unsigned int);
        for(uint8_t mask=0x80, k=0;k<8;k++,mask>>=1,pos+=columns) {
            if(value & mask) *pos |= bit; 
            else *pos &= (~bit);
        }
    }
    va_end(argv);
    __BITARRAY_DEBUG__();
}

void BitArray::rotateUp(boolean fillIn,...) {
    uint8_t *p,t;
    va_list argv;  va_start(argv, fillIn);
    for(uint8_t c=0;c<columns;c++) {
        p=buffer+c;
        t=*p;
        for(uint8_t r=1;r<rows;r++, p+=columns) {
            *p=*(p+columns);
        }
        if(fillIn) *p=va_arg(argv, unsigned int);
        else *p=t;
    }
    va_end(argv);
    __BITARRAY_DEBUG__();
}

void BitArray::rotateDown(boolean fillIn,...) {
    uint8_t *p,t;
    va_list argv;  va_start(argv, fillIn);
    for(uint8_t c=0;c<columns;c++) {
        p=buffer+((rows-1)*columns+c);
        t=*p;
        for(uint8_t r=1;r<rows;r++, p-=columns) {
            *(p)=*(p-columns);
        }
        if(fillIn) *p=va_arg(argv, unsigned int);
        else *p=t;
    }
    va_end(argv);
    __BITARRAY_DEBUG__();
}

void BitArray::rotateLeft(boolean fillIn, ...) {
    va_list argv; va_start(argv, fillIn);
    uint8_t *p=buffer;
    uint8_t mask=0,arg=0,b;
    for(int i=0;i<rows;i++) {
        if( (i & 0x07) ==0) {
            mask=0x80; arg=va_arg(argv, unsigned int);
        } else {
            mask>>=1;
        }

        if(fillIn) b=arg & mask;
        else b= (*p) & 0x80;

        for( int c=0;c<columns;c++, p++) {
            *p <<=1;
            if(*(p+1) & 0x80) *p |= 0x01;
        }
        if(b) *(p-1) |= 0x01;
        else *(p-1) &=0xfe;
    }
    va_end(argv);
    __BITARRAY_DEBUG__();

}

void BitArray::rotateRight(boolean fillIn, ...) {
    va_list argv; va_start(argv, fillIn);
    uint8_t *p=buffer-1;
    uint8_t mask=0,arg=0,b;
    for(int i=0;i<rows;i++, p+=columns) {
        if( (i & 0x07) ==0) {
            mask=0x80; arg=va_arg(argv, unsigned int);
        } else {
            mask>>=1;
        }

	p+=columns;
        if(fillIn) b=arg & mask;
        else b= (*p) & 0x01;

        for( int c=0; c<columns;c++, p--) {
            *p >>= 1;
            if(*(p-1) & 0x01) *p |= 0x80;
        }
        if(b) *(p+1) |= 0x80;
        else *(p+1) &=0x7f;
    }
    va_end(argv);
    __BITARRAY_DEBUG__();

}

//////////////////////////////////////////////
void BitArray::shiftUp16x16In(const byte *p,uint32_t delayTime) {
    for(int i=0;i<16;i++) {
        uint8_t v1=pgm_read_byte(p++);
        uint8_t v2=pgm_read_byte(p++);
        rotateUp(true,v1,v2,0);
        delay(delayTime);
    }
}

void BitArray::shiftDown16x16In(const byte *p,uint32_t delayTime) {
   p+=32;
   for(int i=0;i<16;i++) {
       uint8_t v1=pgm_read_byte(--p);
       uint8_t v2=pgm_read_byte(--p);
       rotateDown(true,v2,v1,0);
       delay(delayTime);
   }
}

void BitArray::shiftRight16x16In(const byte *bitMap,uint32_t delayTime) {
    uint8_t mask=0,loop,count;
    uint8_t v1=0,v2=0;
    for(bitMap+=2, loop=0;loop<16;loop++, bitMap-=32) {
	if((loop & 0x07)==0) {
            mask=0x01;
            bitMap--;
        }  else {
            mask <<= 1;
        }

        for(count=0;count<8;count++, bitMap+=2) {
            v1 <<=1;
            if(pgm_read_byte(bitMap) & mask) v1 |= 0x01;
        }
        for(count=0;count<8;count++, bitMap+=2) {
            v2 <<=1;
            if(pgm_read_byte(bitMap) & mask) v2 |= 0x01;
        }
        rotateRight(true,v1,v2,0);
        delay(delayTime);
    }
}


void BitArray::shiftLeft16x16In(const byte *bitMap,uint32_t delayTime) {
    uint8_t mask=0,loop,count;
    uint8_t v1=0,v2=0;
    bitMap--;
    for(loop=0;loop<16;loop++, bitMap-=32) {
	if((loop & 0x07)==0) {
            mask=0x80;
            bitMap++;
        }  else {
            mask >>= 1;
        }

        for(count=0;count<8;count++, bitMap+=2) {
            v1 <<=1;
            if(pgm_read_byte(bitMap) & mask) v1 |= 0x01;
        }
        for(count=0;count<8;count++, bitMap+=2) {
            v2 <<=1;
            if(pgm_read_byte(bitMap) & mask) v2 |= 0x01;
        }
        rotateLeft(true,v1,v2,0);
        delay(delayTime);
    }
}

