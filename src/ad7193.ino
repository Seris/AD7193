#include <Arduino.h>
#include <SPI.h>
#include <ad7193.h>
#include <stdint.h>

/**
 * Init the AD7193 and the SPI hardware
 * @param cs_pin Chip Select PIN
 * @param speed  Clock speed (in Hz)
 * @param cread  Continuous Read %TODO
 */
void AD7193::begin(int cs_pin, uint32_t speed, bool cread){
    pinMode(cs_pin, OUTPUT);
    digitalWrite(cs_pin, HIGH);
    SPI.begin();
    reset();
    config.cs_pin = cs_pin;
    config.cread = cread;
    config.spi = SPISettings(speed, MSBFIRST, SPI_MODE3);
}

/**
 * Return the ID of the AD7193
 */
uint32_t AD7193::getID(){
    uint32_t id;
    beginTransaction();
    id = getRegister(AD7193_REG_ID, AD7193_REGSIZE_ID);
    endTransaction();
    return id;
}

/**
 * Get converted data from the ADC
 */
uint32_t AD7193::getData(){
    uint32_t data = 0;
    beginTransaction();
    data = getRegister(AD7193_REG_DATA, AD7193_REGSIZE_DATA);
    endTransaction();
    return data;
}

/**
 * Reset the AD7193
 */
void AD7193::reset(){
    beginTransaction();
    for(int i = 0; i < 6; i++){
        SPI.transfer(0xff);
    }
    endTransaction();
}

/**
 * Start a SPI transaction
 */
void AD7193::beginTransaction(){
    SPI.beginTransaction(config.spi);
    digitalWrite(config.cs_pin, LOW);
}

/**
 * End a SPI transaction
 */
void AD7193::endTransaction(){
    delay(1);
    digitalWrite(config.cs_pin, HIGH);
    SPI.endTransaction();
}

/**
 * Write an operation in the Communcation Register
 * @param op_mode Read/Write
 * @param reg     Register targetted for the operation
 */
void AD7193::writeComRegister(ad7193_comreg_op_t op_mode, ad7193_reg_t reg){
    uint8_t data = 0;
    data |= op_mode;
    data |= AD7193_COM_ADDR(reg);
    data |= config.cread ? AD7193_COM_CREAD : 0;
    SPI.transfer(data);
}

/**
 * Read "size" bytes from the AD7193
 * @param size Number of bytes
 * @param buf  buffer of size bytes
 */
void AD7193::readIncomingData(ad7193_regsize_t size, uint8_t* buf){
    memset(buf, 0, size);
    SPI.transfer(buf, size);
}

/**
 * Set the content of a register
 * @param reg  Targetted register
 * @param size Size of the register
 * @param buf  New register content (will be overwritten)
 */
void AD7193::setRegister(ad7193_reg_t reg, ad7193_regsize_t size, uint8_t* buf){
    writeComRegister(AD7193_COM_WRITE, reg);
    SPI.transfer(buf, size);
}

/**
 * Get the content of a register
 * @param reg     Targetted register
 * @param regSize Size of the register
 * @return Register content
 */
uint32_t AD7193::getRegister(ad7193_reg_t reg, ad7193_regsize_t size){
    uint32_t data = 0;
    uint8_t buffer[size];
    writeComRegister(AD7193_COM_READ, reg);
    readIncomingData(size, buffer);

    for(int i = 0; i < size; i++){
        data <<= 8;
        data |= buffer[i];
    }

    return data;
}
