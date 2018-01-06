#ifndef DMI2C_H
#define DMI2C_H
#include <mbed.h>

#define DMI2C_OK                0x00
#define DMI2C_ERR_NACK_ADDRESS  0x01
#define DMI2C_ERR_NACK_DATA     0x02
#define DMI2C_ERR_RD_NOBUF      0x03
#define DMI2C_ERR_RD_OVERFLOW   0x04
#define DMI2C_ERR_OTHER         0x05

class dmI2C {
    public:
        dmI2C();
        ~dmI2C();
        // public methods
        void     initialize(I2C *loci2c);  // method to start i2c interface, should be called once
        void     terminate();   // method to stop i2c interface, should be called once
        uint8_t  write(uint8_t address, uint8_t *buf, uint8_t num, uint8_t *bytesWritten); // write "num" of bytes from "buf" at "address"
        uint8_t  read(uint8_t address, uint8_t *buf, uint8_t num, uint8_t *bytesRead);  // read "num" bytes into "buf" from "address"
        void     delayms(uint16_t ms); // delay "ms" milisecons
    private:
        I2C      *i2c;
};

#endif
