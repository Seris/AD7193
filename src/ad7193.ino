#include <Arduino.h>
#include <SPI.h>
#include <ad7193.h>
#include <stdint.h>


void AD7193::begin(int cs_pin, uint32_t speed, bool cread){
    pinMode(cs_pin, OUTPUT);
    digitalWrite(cs_pin, HIGH);
    SPI.begin();
    this->reset();
    this->cs_pin = cs_pin;
    this->cread = cread;
    this->config.spi = SPISettings(speed, MSBFIRST, SPI_MODE3);
}


uint32_t AD7193::getID(){
    uint32_t id;
    this->beginTransaction();
    id = this->getRegister(AD7193_REG_ID, AD7193_REGSIZE_ID);
    this->endTransaction();
    return id;
}

uint32_t AD7193::getData(){
    uint32_t data = 0;
    this->beginTransaction();
    data = this->getRegister(AD7193_REG_DATA, AD7193_REGSIZE_DATA);
    this->endTransaction();
    return data;
}

void AD7193::reset(){
    this->beginTransaction();
    for(int i = 0; i < 6; i++){
        SPI.transfer(0xff);
    }
    this->endTransaction();
}


void AD7193::beginTransaction(){
    SPI.beginTransaction(this->config.spi);
    digitalWrite(cs_pin, LOW);
}

void AD7193::endTransaction(){
    delay(1);
    digitalWrite(cs_pin, HIGH);
    SPI.endTransaction();
}


void AD7193::writeComRegister(ad7193_comreg_op_t op_mode, ad7193_reg_t reg){
    uint8_t data = 0;
    data |= op_mode;
    data |= AD7193_COM_ADDR(reg);
    data |= this->cread ? AD7193_COM_CREAD : 0;
    SPI.transfer(data);
}

void AD7193::readIncomingData(ad7193_regsize_t size, char* buf){
    memset(buf, 0, size);
    SPI.transfer(buf, size);
}

void AD7193::setRegister(ad7193_reg_t reg, ad7193_regsize_t size, char* buf){
    this->writeComRegister(AD7193_COM_WRITE, reg);
    SPI.transfer(buf, size);
}

uint32_t AD7193::getRegister(ad7193_reg_t reg, ad7193_regsize_t size){
    uint32_t data = 0;
    char buffer[size];
    this->writeComRegister(AD7193_COM_READ, reg);
    this->readIncomingData(size, buffer);

    for(int i = 0; i < size; i++){
        data <<= 8;
        data |= (uint8_t) buffer[i];
    }

    return data;
}
