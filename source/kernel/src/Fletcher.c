#include "kernel/inc/Fletcher.h"

U16_t FletcherGenerate(U8_t *data, U16_t dlen)
{
    if(dlen == 0) {
        return 0;
    }
    uint16_t sum1 = 0;
    uint16_t sum2 = 0;
    int index;
    for( index = 0; index < dlen; ++index ) {
        sum1 = (sum1 + data[index]) % 0xFF;
        sum2 = (sum2 + sum1) % 0xFF;
    }
    uint8_t cb0 = 0, cb1 = 0;
    cb0 = 0xFF - (( sum1 + sum2) % 0xFF);
    cb1 = 0xFF - (( sum1 + cb0 ) % 0xFF);

    return (cb0 << 8) | cb1;

}

bool FletcherVerify(U16_t fletcher, U8_t *data, U16_t dlen)
{
    if(dlen == 0) {
        return false;
    }
    
    U16_t new_fletcher = FletcherGenerate(data, dlen);
    if(fletcher != new_fletcher) {
        return false;
    } else {
        return true;
    }
    //uint8_t cb0 = 0, cb1 = 0;
    //uint16_t sum1 = 0;
    //uint16_t sum2 = 0;
    //int index;
//
    //cb1 = (uint8_t)(fletcher & 0x00FF);
    //cb0 = (uint8_t)((fletcher >> 8) & 0x00FF);
    //for( index = 0; index < dlen; ++index ) {
        //sum1 = (sum1 + data[index]) % 0xFF;
        //sum2 = (sum2 + sum1) % 0xFF;
    //}
//
    //sum1 = (sum1 + cb0) % 0xFF;
    //sum2 = (sum2 + sum1) % 0xFF;
//
    //sum1 = (sum1 + cb1) % 0xFF;
    //sum2 = (sum2 + sum1) % 0xFF;
//
    //if(((sum2 << 8) | sum1) != 0) {
        //return false;
    //} else {
        //return true;
    //}
}
