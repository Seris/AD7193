#ifndef __AD7193_TYPES_H__
#define __AD7193_TYPES_H__

typedef enum {
    AD7193_REG_COM = 0,     // Communcation Register (WO, 8-bit)
    AD7193_REG_STATUS = 0,  // Status Register (RO, 8-bit)
    AD7193_REG_DATA = 3,    // Data Register (RO, 24-bit)
    AD7193_REG_ID = 4,      // ID Register (RO, 8-bit)
} ad7193_reg_t;

typedef enum {
    AD7193_REGSIZE_COM = 1,
    AD7193_REGSIZE_STATUS = 1,
    AD7193_REGSIZE_DATA = 3,
    AD7193_REGSIZE_ID = 1,
} ad7193_regsize_t;

typedef enum {
    AD7193_COM_READ = 1 << 6,
    AD7193_COM_WRITE = 0
} ad7193_comreg_op_t;

#define AD7193_COM_WEN      1 << 7          // Enable bit. Must be zero.
#define AD7193_COM_ADDR(x)  (x & 0x7) << 3  // Register address
#define AD7193_COM_CREAD    1 << 2          // Enable continuous read

typedef enum {
    AD7193_UNIPOLAR = 1,
    AD7193_POLAR = 0
} ad7193_pol_t;

typedef enum {
    AD7193_CHAN0 = 1,
    AD7193_CHAN1 = 1 << 1,
    AD7193_CHAN2 = 1 << 2,
    AD7193_CHAN3 = 1 << 3,
    AD7193_CHAN4 = 1 << 4,
    AD7193_CHAN5 = 1 << 5,
    AD7193_CHAN6 = 1 << 6,
    AD7193_CHAN7 = 1 << 7,
    AD7193_CHANTEMP = 1 << 8,
    AD7193_CHANSHORT = 1 << 9,
} ad7193_chan_t;

typedef struct {
    SPISettings spi;
    ad7193_pol_t polarity;
    uint8_t gain;
    ad7193_chan_t channels;
} ad7193_config_t;

#endif // __AD7193_TYPES_H__