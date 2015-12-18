
#ifndef __BIT_ARRAY_H__
#define __BIT_ARRAY_H__

#define _SHIFTIN_DELAY_ 100
class BitArray {
private:
    uint8_t buffer[80];
    void (*callback)(uint8_t *p, uint16_t _rows,uint16_t _columns)=0;
    byte bitReverse(byte value);

public:
    uint8_t rows=24,columns=3;
    BitArray(uint16_t _rows,uint16_t _columns,void (*_callback)(uint8_t *p, uint16_t _rows,uint16_t _columns)=0);
    void init();
    void clear();
    void swapH();
    void swapV();
    void inverse();
    uint8_t setDot(uint16_t x, uint16_t y, boolean b);
    void load16x16(const byte *p,uint8_t rowOffset=0,uint8_t columnByteOffset=0);
    void setRow(uint8_t row,...);
    void setColumn(uint8_t column,...);

    void rotateUp(boolean fillIn=false,...);
    void rotateUp_X(boolean fillIn=false,...);
    void rotateDown(boolean fillIn=false,...);
    void rotateDown_X(boolean fillIn=false,...);
    void rotateLeft(boolean fillIn=false,...);
    void rotateRight(boolean fillIn=false,...);

    void shiftUp16x16In(const byte *p,uint32_t delayTime=_SHIFTIN_DELAY_);
    void shiftDown16x16In(const byte *p,uint32_t delayTime=_SHIFTIN_DELAY_);
    void shiftRight16x16In(const byte *p, uint32_t delayTime=_SHIFTIN_DELAY_);
    void shiftLeft16x16In(const byte *p, uint32_t delayTime=_SHIFTIN_DELAY_);

};
#endif
