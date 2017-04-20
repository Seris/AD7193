#ifndef __AD7193_H__
#define __AD7193_H__

#include <SPI.h>
#include <stdint.h>
#include "ad7193_types.h"

class AD7193 {
private:
    ad7193_config_t config;

    /**
     * Start and end a transaction (SPI and CS PIN management)
     */
    void beginTransaction();
    void endTransaction();

    /**
     * Write an operation in the Communcation Register
     * @param op_mode Read/Write
     * @param reg     Register targetted for the operation
     */
    void writeComRegister(ad7193_comreg_op_t op_mode, ad7193_reg_t reg);


    /**
     * Read "size" bytes from the AD7193
     * @param size Number of bytes
     * @param buf  buffer of size bytes
     */
    void readIncomingData(ad7193_regsize_t size, uint8_t* buf);

    /**
     * Get the content of a register
     * @param reg     Targetted register
     * @param regSize Size of the register
     * @return Register content
     */
    uint32_t getRegister(ad7193_reg_t reg, ad7193_regsize_t regSize);

    /**
     * Set the content of a register
     * @param reg  Targetted register
     * @param size Size of the register
     * @param buf  New register content (will be overwritten)
     */
    void setRegister(ad7193_reg_t reg, ad7193_regsize_t size, uint8_t* buf);

public:
    /**
     * Init the AD7193 and the SPI hardware
     * @param cs_pin Chip Select PIN
     * @param speed  Clock speed (in Hz)
     * @param cread  Continuous Read %TODO
     */
    void begin(int cs_pin, uint32_t speed = 4000000, bool cread = false);

    /**
     * Get converted data from the ADC
     */
    uint32_t getData();

    /**
     * Return the ID of the AD7193
     */
    uint32_t getID();

    /**
     * Reset the AD7193
     */
    void reset(void);
};

#endif // __AD7193_H__
