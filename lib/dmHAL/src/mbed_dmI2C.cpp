#include "mbed_dmI2C.h"
// this is the implementation for mded i2c

dmI2C::dmI2C() {
}

dmI2C::~dmI2C() {
}

void dmI2C::initialize(I2C *loci2c, Serial *lppc) {
    i2c = loci2c;
    ppc = lppc;
}

void dmI2C::terminate() {
}

uint8_t dmI2C::write(uint8_t address, uint8_t *buf, uint8_t num, uint8_t *bytesWritten) {
    uint8_t retVal = DMI2C_OK;
    uint8_t error;

    *bytesWritten=0;
    if( buf == NULL || num == 0 ) {
        // this is needed to wake up device and i2c scanning
        error = i2c->write( address<<1, NULL, 0, false );
        switch (error) {
            case 0:
                retVal = DMI2C_OK;
                break;
            case 1:
                retVal = DMI2C_ERR_NACK_DATA;
                break;
            default:
                retVal = DMI2C_ERR_OTHER;
        }
    }
    else {
        if( retVal ==  DMI2C_OK ) {
            error = i2c->write( address<<1, (char *)buf, num, false );
            switch (error) {
                case 0:
                    *bytesWritten = num;
                    num = 0;
                    break;
                case 1:
                    retVal = DMI2C_ERR_NACK_DATA;
                    break;
                default:
                    retVal = DMI2C_ERR_OTHER;
            }
        }
    }
    return retVal;
}

uint8_t dmI2C::read(uint8_t address, uint8_t *buf, uint8_t num, uint8_t *bytesRead) {
    uint8_t retVal = DMI2C_OK;
    uint8_t error = 0;

    *bytesRead=0;
    if( buf == NULL || num == 0 ) {
        // just the protection against lame invocation
        retVal = DMI2C_ERR_RD_NOBUF;
    }
    else {
        if ( retVal == DMI2C_OK ) {
            error = i2c->read( address<<1, (char *)buf, num, false );
            switch (error) {
                case 0:
                    *bytesRead = num;
                    break;
                case 1:
                    retVal = DMI2C_ERR_NACK_DATA;
                    break;
                default:
                    retVal = DMI2C_ERR_OTHER;
            }
            //ppc->printf("DEBUG: Address=%2X; num=%d; bytesRead=%d; error=%d\n\r", address, num, *bytesRead, error );
            //for (int i=0; i<num; i++) ppc->printf("buf[%d]=%02X\n\r", i, buf[i]);
        }
    }
    return retVal;
}

void dmI2C::delayms(uint16_t ms) {
    wait_ms(ms);
}
