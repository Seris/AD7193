#ifndef __AD7193_TYPES_H__
#define __AD7193_TYPES_H__

#include <stdint.h>

#ifdef ARDUINO_AVR_UNO
    #define AD7193_PIN_MISO 12
#else
    #error "Platform not supported ! Add an issue on github and i'll try to do something about it"
#endif

// AD7193 use 24-bit register. Therefore, it can't return this value
#define AD7193_NODATA       0xf0f0f0f0

#define AD7193_COM_WEN      1 << 7          // Enable bit. Must be zero.
#define AD7193_COM_ADDR(x)  (x & 0x7) << 3  // Register address
#define AD7193_COM_CREAD    1 << 2          // Enable continuous read

typedef enum {
    AD7193_REG_COM = 0,     // Communcation Register (WO, 8-bit)
    AD7193_REG_STATUS = 0,  // Status Register (RO, 8-bit)
    AD7193_REG_MODE = 1,    // Mode Register (RW, 24-bit)
    AD7193_REG_CONFIG = 2,  // Configuration Register (RW, 24-bit)
    AD7193_REG_DATA = 3,    // Data Register (RO, 24-bit)
    AD7193_REG_ID = 4,      // ID Register (RO, 8-bit)
    AD7193_REG_OFFSET = 6,  // Offset Register (RW, 24-bit)
    AD7193_REG_FLSCALE = 7, // Full-Scale Register (RW, 24-bit)
} ad7193_reg_t;

typedef enum {
    AD7193_REGSIZE_COM = 1,
    AD7193_REGSIZE_STATUS = 1,
    AD7193_REGSIZE_MODE = 3,
    AD7193_REGSIZE_CONFIG = 3,
    AD7193_REGSIZE_DATA = 3,
    AD7193_REGSIZE_ID = 1,
    AD7193_REGSIZE_OFFSET = 3,
    AD7193_REGSIZE_FLSCALE = 3,
} ad7193_regsize_t;

typedef enum {
    AD7193_COM_READ = 1 << 6,
    AD7193_COM_WRITE = 0
} ad7193_comreg_op_t;

typedef enum {
    AD7193_CHAN0 = 0,
    AD7193_CHAN1 = 1,
    AD7193_CHAN2 = 2,
    AD7193_CHAN3 = 3,
    AD7193_CHAN4 = 4,
    AD7193_CHAN5 = 5,
    AD7193_CHAN6 = 6,
    AD7193_CHAN7 = 7,
    AD7193_CHANTEMP = 8,
    AD7193_CHANSHORT = 9,
} ad7193_chan_t;

typedef enum {
    AD7193_MOD_CONTCONV = 0,
    AD7193_MOD_SINGCONV = 1,
    AD7193_MOD_IDLE = 2,
    AD7193_MOD_PWDOWN = 3,
    AD7193_MOD_INTZEROCAL = 4,
    AD7193_MOD_INTFULLCAL = 5,
    AD7193_MOD_SYSZEROCAL = 6,
    AD7193_MOD_SYSFULLCAL = 7
} ad7193_mode_t;

typedef enum {
    AD7193_CLK_EXTK1K2 = 0,
    AD7193_CLK_EXTK2 = 1,
    AD7193_CLK_INT = 2,
    AD7193_CLK_INTK2 = 3
} ad7193_clk_t;

typedef struct {
    SPISettings spi;
    bool cread;
    int cs_pin;

    // Configuration Register
    bool cr_chop;
    bool cr_refsel;
    bool cr_pseudo;
    int cr_channels;
    bool cr_burn;
    bool cr_refdet;
    bool cr_buf;
    bool cr_unipolar;
    uint8_t cr_gain;

    // Mode register
    ad7193_mode_t md_mode;
    bool md_dat_sta;
    ad7193_clk_t md_clk;
    uint8_t md_averaging;
    bool md_sinc3;
    bool md_enpar;
    bool md_clk_div;
    bool md_single;
    bool md_rej60;
    uint16_t md_filter_output;
} ad7193_config_t;

#endif // __AD7193_TYPES_H__
