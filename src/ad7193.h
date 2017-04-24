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
     * Write a register in the SPI bus
     * @param size Size of the register
     * @param dat  Data to send
     */
    void writeRegister(ad7193_regsize_t size, uint32_t dat);

    /**
     * Return default configuration using default value for registers
     */
    static ad7193_config_t defaultConfig(void);

    /**
     * Push config in the AD7193 (cs_pin is not modified)
     */
    void pushConfig(ad7193_config_t* newconfig);

    /**
     * Generate Configuration Register value from config
     */
    static uint32_t generateConfigRegister(ad7193_config_t* config);

    /**
     * Generate a Mode Register value from config
     */
    uint32_t generateModeRegister(ad7193_config_t* config);

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
     */
    void begin(int cs_pin, uint32_t speed = 4000000);

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
     * Perform a interior full scale calibration (cf. AD7193 documentation)
     * @return True if success
     */
    bool fullScaleCalibration(void);

    /**
     * Calibrate a channel
     * @param  channel The channel that will be calibrate
     * @return         True if calibration was successfull
     */
    bool calibrateChannel(ad7193_chan_t channel);

    /**
     * Wait for the RDY pin to go low
     * @param  break_time Timeout
     * @return           True if data is available, false otherwise
     */
    bool waitRDYLow(uint32_t break_time = 10000);

    /**
     * Wait for data to be available on the AD7193
     * @param  break_time Timeout
     * @return           True if data is available, false otherwise
     */
    bool waitForData(uint32_t break_time = 10000);

    /**
     * Get converted data from the ADC
     * %TODO continuous reading is currently not supported
     */
    uint32_t getData(ad7193_chan_t* from_channel);

    /**
     * [getID description]
     * @return  [description]
     */
    double getDataAsMilliVolts(ad7193_chan_t* from_channel);

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
     * Set/Add/Remove channel in config
     */
    static void addChannelToConfig(ad7193_config_t* c, ad7193_chan_t chan);
    static void removeChannelFromConfig(ad7193_config_t* c, ad7193_chan_t chan);
    static void setChannelInConfig(ad7193_config_t* c, ad7193_chan_t chan);

    /**
     * Reset the AD7193
     */
    void reset(void);

};

#endif // __AD7193_H__
