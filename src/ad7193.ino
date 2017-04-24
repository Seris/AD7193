#include <Arduino.h>
#include <SPI.h>
#include <ad7193.h>
#include <stdint.h>

/**
 * Init the AD7193 and the SPI hardware
 * @param cs_pin Chip Select PIN
 * @param speed  Clock speed (in Hz)w
 */
void AD7193::begin(int cs_pin, uint32_t speed){
    config = defaultConfig();
    config.cs_pin = cs_pin;
    config.cread = false;
    config.spi = SPISettings(speed, MSBFIRST, SPI_MODE3);

    pinMode(cs_pin, OUTPUT);
    digitalWrite(cs_pin, HIGH);
    SPI.begin();

    reset();
    syncConfig();
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
 * Perform a interior full scale calibration (cf. AD7193 documentation) %TODO
 * @return True if success
 */
bool AD7193::fullScaleCalibration(){
    bool cal_success = false;
    ad7193_mode_t mode = config.md_mode;

    beginTransaction();
    config.md_mode = AD7193_MOD_INTFULLCAL;
    setRegister(AD7193_REG_MODE,
                AD7193_REGSIZE_MODE,
                generateModeRegister(&config));
    if(waitRDYLow()) cal_success = true;
    endTransaction();

    config.md_mode = mode;
    setConfig(&config);

    return cal_success;
}

/**
 * Calibrate a channel
 * @param  channel The channel that will be calibrate
 * @return         True if calibration was successfull
 */
bool AD7193::calibrateChannel(ad7193_chan_t channel){
    bool cal_success = false;
    int channels = config.cr_channels;
    ad7193_mode_t mode = config.md_mode;

    setChannelInConfig(&config, channel);
    setConfig(&config);

    beginTransaction();
    config.md_mode = AD7193_MOD_INTZEROCAL;
    setRegister(AD7193_REG_MODE,
                AD7193_REGSIZE_MODE,
                generateModeRegister(&config));
    if(waitRDYLow()) cal_success = true;
    endTransaction();

    config.md_mode = mode;
    config.cr_channels = channels;
    setConfig(&config);

    return cal_success;
}

/**
 * Wait for data to come on the SPI bus
 * @param  break_time Timeout
 * @return            true if data is available, false otherwise
 */
bool AD7193::waitForData(uint32_t break_time){
    bool status;
    beginTransaction();
    status = waitRDYLow(break_time);
    endTransaction();
    return status;
}

/**
 * Wait for the RDY pin to go low
 * @param  break_time Timeout
 * @return           True if data is available, false otherwise
 */
bool AD7193::waitRDYLow(uint32_t break_time){
    bool is_low = false;
    uint32_t start = millis();

    while((millis() - start < break_time) && is_low == false){
        is_low = (digitalRead(AD7193_PIN_MISO) == LOW);
    }

    return is_low;
}

/**
 * Get converted data from the ADC
 * %TODO continuous reading is currently not supported
 */
uint32_t AD7193::getData(ad7193_chan_t* from_channel){
    uint32_t data;
    uint32_t status;

    if(waitForData()){
        beginTransaction();

        if(!config.md_dat_sta && from_channel != NULL){
            status = getRegister(AD7193_REG_STATUS, AD7193_REGSIZE_STATUS);
        }

        if(!config.cread){
            writeComRegister(AD7193_COM_READ, AD7193_REG_DATA);
        }

        data = readIncomingRegister(AD7193_REGSIZE_DATA);
        if(config.md_dat_sta){
            status = readIncomingRegister(AD7193_REGSIZE_STATUS);
        }

        if(from_channel != NULL){
            *from_channel = (ad7193_chan_t) (status & 0xf);
        }

        endTransaction();
    } else {
        data = AD7193_NODATA;
    }


    return data;
}

/**
 * Get converted data from the ADC in mV
 */
double AD7193::getDataAsMilliVolts(ad7193_chan_t* from_channel){
    int32_t data;
    double max_value = 0;
    switch(config.cr_gain){

        case 0: max_value = 2500.0; break;
        case 3: max_value = 312.5; break;
        case 4: max_value = 156.2; break;
        case 5: max_value = 78.125; break;
        case 6: max_value = 39.6; break;
        case 7: max_value = 19.53; break;
    }

    data = (int32_t) getData(from_channel);
    if(data != (int32_t) AD7193_NODATA){
        if(!config.cr_unipolar){
            return max_value * ((double) data * 2 / 0xffffff - 1);
        } else {
            return ((double) data) / 0xffffff * max_value;
        }
    } else {
        return AD7193_NODATA;
    }
}

/**
 * Sync config with the one stored in the ADC
 */
void AD7193::syncConfig(){
    uint32_t confreg;
    uint32_t modereg;

    beginTransaction();
    confreg = getRegister(AD7193_REG_CONFIG, AD7193_REGSIZE_CONFIG);
    modereg = getRegister(AD7193_REG_MODE, AD7193_REGSIZE_MODE);
    endTransaction();

    updateConfigWithConfReg(&config, confreg);
    updateConfigWithModeReg(&config, modereg);
}

/**
 * Get config to the AD7193
 */
ad7193_config_t AD7193::getConfig(){
    return config;
}

/**
 * Push config to the AD7193
 */
void AD7193::setConfig(ad7193_config_t* newconfig){
    beginTransaction();
    pushConfig(newconfig);
    endTransaction();
}

/**
 * Push config to the AD7193
 */
void AD7193::pushConfig(ad7193_config_t* newconfig){
    uint32_t confreg = generateConfigRegister(newconfig);
    uint32_t modereg = generateModeRegister(newconfig);

    //writeComRegister(AD7193_COM_READ, AD7193_REG_DATA);
    //readIncomingRegister(AD7193_REGSIZE_DATA);
    setRegister(AD7193_REG_CONFIG, AD7193_REGSIZE_CONFIG, confreg);
    setRegister(AD7193_REG_MODE, AD7193_REGSIZE_MODE, modereg);

    config = *newconfig;
}

/**
 * Reset the AD7193
 */
void AD7193::reset(){
    beginTransaction();
    for(int i = 0; i < 7; i++){
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
    writeRegister(AD7193_REGSIZE_COM, (uint32_t) data);
}

/**
 * Read an incoming register on the SPI bus
 * @param size Size of the register in bytes
 */
uint32_t AD7193::readIncomingRegister(ad7193_regsize_t size){
    uint8_t buffer[size];
    memset(buffer, 0, size);
    SPI.transfer(buffer, size);
    return bufferToRegister(size, buffer);
}

/**
 * Write a register in the SPI bus
 * @param size Size of the register
 * @param dat  Data to send
 */
void AD7193::writeRegister(ad7193_regsize_t size, uint32_t dat){
    for(int i = size - 1; i >= 0; i--){
        SPI.transfer((dat >> (i * 8)) & 0xff);
    }
}

/**
 * Set the content of a register
 * @param reg  Targetted register
 * @param size Size of the register
 * @param dat  New register content
 */
void AD7193::setRegister(ad7193_reg_t reg, ad7193_regsize_t size, uint32_t dat){
    writeComRegister(AD7193_COM_WRITE, reg);
    writeRegister(size, dat);
}

/**
 * Get the content of a register
 * @param reg     Targetted register
 * @param regSize Size of the register
 * @return Register content
 */
uint32_t AD7193::getRegister(ad7193_reg_t reg, ad7193_regsize_t size){
    writeComRegister(AD7193_COM_READ, reg);
    return readIncomingRegister(size);
}

/**
 * Return default configuration using default value for registers
 */
ad7193_config_t AD7193::defaultConfig(){
    ad7193_config_t default_config;

    // Configuration Register Power-on/Reset status
    default_config.cr_chop = false;
    default_config.cr_refsel = false;
    default_config.cr_pseudo = false;
    setChannelInConfig(&default_config, AD7193_CHAN0);
    default_config.cr_burn = false;
    default_config.cr_refdet = false;
    default_config.cr_buf = true;
    default_config.cr_unipolar = false;
    default_config.cr_gain = 7;

    // Mode register
    default_config.md_mode = AD7193_MOD_CONTCONV;
    default_config.md_dat_sta = false;
    default_config.md_clk = AD7193_CLK_INT;
    default_config.md_averaging = 0;
    default_config.md_sinc3 = false;
    default_config.md_enpar = false;
    default_config.md_clk_div = false;
    default_config.md_single = false;
    default_config.md_rej60 = false;
    default_config.md_filter_output = 96;

    return default_config;
}

/**
 * Update configuration with the given Configuration Register value
 */
void AD7193::updateConfigWithConfReg(ad7193_config_t* config, uint32_t reg){
    config->cr_chop = (reg >> 23) & 1;
    config->cr_refsel = (reg >> 20) & 1;
    config->cr_pseudo = (reg >> 18) & 1;
    config->cr_channels = (reg >> 8) & 0x3ff;
    config->cr_burn = (reg >> 7) & 1;
    config->cr_refdet = (reg >> 6) & 1;
    config->cr_buf = (reg >> 4) & 1;
    config->cr_unipolar = (reg >> 3) & 1;
    config->cr_gain = reg & 7;
}

/**
 * Update configuration with the given Mode Register value
 */
void AD7193::updateConfigWithModeReg(ad7193_config_t* config, uint32_t reg){
    config->md_mode = (ad7193_mode_t) ((reg >> 21) & 7);
    config->md_dat_sta = (reg >> 20) & 1;
    config->md_clk = (ad7193_clk_t) ((reg >> 18) & 3);
    config->md_averaging = (reg << 16) & 3;
    config->md_sinc3 = (reg >> 15) & 1;
    config->md_enpar = (reg >> 13) & 1;
    config->md_clk_div = (reg >> 12) & 1;
    config->md_single = (reg >> 11) & 1;
    config->md_rej60 = (reg >> 10) & 1;
    config->md_filter_output = (reg) & 0x3ff;
}

/**
 * Generate a Configuration Register value from config
 */
uint32_t AD7193::generateConfigRegister(ad7193_config_t* config){
    uint32_t reg = 0;

    reg |= (uint32_t) config->cr_chop << 23;
    reg |= (uint32_t) config->cr_refsel << 20;
    reg |= (uint32_t) config->cr_pseudo << 18;
    reg |= (uint32_t) config->cr_channels << 8;
    reg |= (uint32_t) config->cr_burn << 7;
    reg |= (uint32_t) config->cr_refdet << 6;
    reg |= (uint32_t) config->cr_buf << 4;
    reg |= (uint32_t) config->cr_unipolar << 3;
    reg |= (uint32_t) config->cr_gain;

    return reg;
}

/**
 * Generate a Mode Register value from config
 */
uint32_t AD7193::generateModeRegister(ad7193_config_t* config){
    uint32_t reg = 0;

    reg |= (uint32_t) config->md_mode << 21;
    reg |= (uint32_t) config->md_dat_sta << 20;
    reg |= (uint32_t) config->md_clk << 18;
    reg |= (uint32_t) config->md_averaging << 16;
    reg |= (uint32_t) config->md_sinc3 << 15;
    reg |= (uint32_t) config->md_enpar << 13;
    reg |= (uint32_t) config->md_clk_div << 12;
    reg |= (uint32_t) config->md_single << 11;
    reg |= (uint32_t) config->md_rej60 << 10;
    reg |= (uint32_t) config->md_filter_output;

    return reg;
}

/**
 * Convert a buffer received through SPI to a register
 */
uint32_t AD7193::bufferToRegister(ad7193_regsize_t size, uint8_t* buffer){
    uint32_t data = 0;
    for(int i = 0; i < size; i++){
        data <<= 8;
        data |= buffer[i];
    }
    return data;
}

/**
 * Set a unique channel in config
 */
void AD7193::setChannelInConfig(ad7193_config_t* c, ad7193_chan_t chan){
    c->cr_channels = 1 << chan;
}


/**
 * Add a channel to config
 */
void AD7193::addChannelToConfig(ad7193_config_t* c, ad7193_chan_t chan){
    c->cr_channels |= 1 << chan;
}

/**
 * Remove a channel from config
 */
void AD7193::removeChannelFromConfig(ad7193_config_t *c, ad7193_chan_t chan){
    c->cr_channels &= ~(1 << chan);
}
