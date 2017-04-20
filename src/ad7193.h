#ifndef __AD7193_H__
#define __AD7193_H__

#include <SPI.h>
#include <stdint.h>
#include "ad7193_types.h"

class AD7193 {
private:
    ad7193_config_t config;

    /**
     * Write an operation in the Communcation Register
     * @param op_mode Read/Write
     * @param reg     Register targetted for the operation
     */
    void writeComRegister(ad7193_comreg_op_t op_mode, ad7193_reg_t reg);

    /**
     * Read an incoming register on the SPI bus
     * @param size Size of the register in bytes
     */
    uint32_t readIncomingRegister(ad7193_regsize_t size);

    /**
     * Return default configuration using default value for registers
     */
    static ad7193_config_t defaultConfig(void);

    /**
     * Generate Configuration Register value from config
     */
    static uint32_t generateConfigRegister(ad7193_config_t* config);

    /**
     * Generate a Mode Register value from config
     */
    uint32_t AD7193::generateModeRegister(ad7193_config_t* config);

    /**
     * Update configuration with the given Configuration Register value
     */
    static void updateConfigWithConfReg(ad7193_config_t* config, uint32_t reg);

    /**
     * Update configuration with the given Configuration Mode value
     */
    static void updateConfigWithModeReg(ad7193_config_t* config, uint32_t reg);

    /**
     * Convert a buffer received through SPI to a register
     */
    static uint32_t bufferToRegister(ad7193_regsize_t size, uint8_t* buffer);

public:
    /**
     * Init the AD7193 and the SPI hardware
     * @param cs_pin Chip Select PIN
     * @param speed  Clock speed (in Hz)
     * @param cread  Continuous Read %TODO
     */
    void begin(int cs_pin, uint32_t speed = 4000000, bool cread = false);

    /**
     * Start and end a transaction (SPI and CS PIN management)
     */
    void beginTransaction(void);
    void endTransaction(void);

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
    void setRegister(ad7193_reg_t reg, ad7193_regsize_t size, uint32_t dat);

    /**
     * Get converted data from the ADC
     * %TODO continuous reading is currently not supported
     */
    uint32_t getData(uint8_t* from_channel);

    /**
     * Return the ID of the AD7193
     */
    uint32_t getID(void);

    /**
     * Synchronize stored configuration with the one stored in the ADC
     */
    void syncConfig(void);

    /**
     * Get the ADC7193 current stored configuration
     * @param sync Synchronize current configuration with the one from the ADC
     */
    ad7193_config_t getConfig(void);

    /**
     * Push config to the AD7193
     */
    void setConfig(ad7193_config_t* config);

    /**
     * Add/Remove channel to config
     */
    static void addChannelToConfig(ad7193_config_t* c, ad7193_chan_t chan);
    static void removeChannelFromConfig(ad7193_config_t* c, ad7193_chan_t chan);

    /**
     * Reset the AD7193
     */
    void reset(void);

};

#endif // __AD7193_H__
