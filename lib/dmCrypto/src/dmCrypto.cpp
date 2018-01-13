#include "dmCrypto.h"

dmCrypto::dmCrypto() {
}

dmCrypto::~dmCrypto() {
}

void dmCrypto::init(I2C *loci2c, Serial *lppc) {
    i2c.initialize(loci2c, lppc);
    ppc = lppc;
}

// TODO: to be changed!
void dmCrypto::calcCRC(uint8_t length, uint8_t *data, uint8_t *crc) {
    uint8_t counter;
    uint16_t  crc_register = 0;
    uint16_t  polynom = 0x8005;
    uint8_t shift_register;
    uint8_t data_bit, crc_bit;

    crc_register = (((uint16_t) crc[0]) & 0x00FF) | (((uint16_t) crc[1]) << 8);

    for (counter = 0; counter < length; counter++) {
        for (shift_register = 0x01; shift_register > 0x00; shift_register <<= 1) {
            data_bit = (data[counter] & shift_register) ? 1 : 0;
            crc_bit = crc_register >> 15;
            crc_register <<= 1;

            if ((data_bit ^ crc_bit) != 0)
                crc_register ^= polynom;
        }
    }

    crc[0] = (uint8_t) (crc_register & 0x00FF);
    crc[1] = (uint8_t) (crc_register >> 8);
}

void dmCrypto::scanI2Cbus() {
    uint8_t devAddress, i;
    uint8_t totalDevices;

    totalDevices = 0;
    i=1;
    for( devAddress = 1; devAddress < 127; devAddress++ ) {
        if ( i2c.write( devAddress, NULL, 0, NULL ) == 0 ) {
            buf[i++] = devAddress;
            totalDevices++;
        }
    }

    for( i = i ; i < 127; i++ )
        buf[i] = 0x00;

    buf[0] = totalDevices;
}

uint8_t dmCrypto::wakeDevice( uint8_t address ) {
    uint8_t i, retCode;
    uint8_t wakeBuf[4] = { 0, 0, 0, 0 };
    // ****************************** NOTE ********************************
    // Arduino Wire lib is running i2c at normal speed 100kHz
    // at 100kHz 1 bit takes 10us, so if we write empty data to adres 0x00
    // it will take min. 80us (since R/!W=0)
    //                        Address 7bit
    //    START   b0   b1   b2   b3   b4   b5   b6  R/!W  ACK    STOP
    //           10us 10us 10us 10us 10us 10us 10us 10us 10us
    // SDA  -\____________________________________________XXX____/-
    // SCL  ---\__/-\__/-\__/-\__/-\__/-\__/-\__/-\__/-\__/-\__/----   <<-- Master is driving SCL
    // ********************************************************************
    // put SDA "low" for the duration of Twlo (min. 60us)
    retCode = DMCRYPTO_OK;

    // TODO: handle already awaken device !!!
    if( i2c.write( address, NULL, 0, NULL) == DMI2C_OK ) {
        // the device is already awaken
        // let's but it to idle mode to preserve data
        wakeBuf[0] = 0x02; // idle command
        i2c.write(address, wakeBuf, 1, &i);
        // now we are ready to wake it up, so the watchdog will be reseted
    }
    // let's wake it up - the speed of the i2c bus must be 100kHz to make it work
    i2c.write(0, NULL, 0, NULL); // ignore the return code
    // wait Twhi (min. 2.5 ms) to make all CryptoAuth chips up and running
    i2c.delayms(3);
    // now we have to read data from the device to flush FIFO buffer
    if ( i2c.read( address, wakeBuf, 4, &i ) ) {
        retCode = DMCRYPTO_ERR_WAKE;
    }
    else {
        // now check the responce
        if ( wakeBuf[0] != 0x04 ||
             wakeBuf[1] != 0x11 ||
             wakeBuf[2] != 0x33 ||
             wakeBuf[3] != 0x43 ) {
                retCode = DMCRYPTO_ERR_WAKE;
        }
    }

    return retCode;
}

////////////////////////////////////////////////////////////////////////////////
// Method : dmCrypto::execCmd
// desc   : - invoke wakeup method,
//          - sends data to i2c bus
//          - waits for execution
//          - reads data from i2c bus
//          - checks CRC of received data
////////////////////////////////////////////////////////////////////////////////
uint8_t dmCrypto::execCmd(uint8_t address, uint8_t *sendBuf, uint8_t sendBufLen, uint8_t *recBuf, uint8_t recBufLen, uint8_t execWait) {
    uint8_t bytesWritten = 0, bytesRead = 0;
    uint8_t error = 0, retVal = 0;
    uint8_t chkCRC[2] = { 0, 0 };

    retVal = DMCRYPTO_OK;
    // wake up device
    retVal = wakeDevice( address );

    // send data to the device and check return code
    if( retVal == DMCRYPTO_OK ) {
        error = i2c.write( address, sendBuf, sendBufLen, &bytesWritten);
        if( error != DMI2C_OK || bytesWritten != sendBufLen ) {
            retVal = DMCRYPTO_ERR_SEND;
        }
    }

    // wait for the command to be executed
    i2c.delayms( execWait );

    // receive data from the device
    if( retVal == DMCRYPTO_OK ) {
        error = i2c.read( address, recBuf, recBufLen, &bytesRead);
        if( error != DMI2C_OK ) {
            retVal = DMCRYPTO_ERR_RECEIVE;
        }
    }

    // check CRC status on system side
    if ( retVal == DMCRYPTO_OK ) {
        chkCRC[0]=0x00; chkCRC[1]=0x00;
        calcCRC( recBuf[0]-2, recBuf, chkCRC );

        if( recBuf[recBuf[0]-2] != chkCRC[0] ||
            recBuf[recBuf[0]-1] != chkCRC[1] )
             retVal = DMCRYPTO_ERR_CRC;
    }

    return retVal;
}



uint8_t dmCrypto::atomic_ATSHA204A_DevRev(uint8_t *locBuf, uint8_t locBufSize, uint8_t address ) {
    uint8_t       retVal     = DMCRYPTO_OK;
    const uint8_t txSize     = 8;
    const uint8_t rxSize     = 7;

    if( txSize > locBufSize ) {
        retVal = DMCRYPTO_ERR_BFULL;
    }

    // prepare transmit buffer
    if ( retVal == DMCRYPTO_OK ) {
        locBuf[0] = 0x03;   // [command] word address/packet function
        locBuf[1] = 0x07;   // [count] starting from [count] ending with [crc]
        locBuf[2] = 0x30;   // [OPCODE] for Device Revision
        locBuf[3] = 0x00;   // [Param1] must be 0
        locBuf[4] = 0x00;   // [Param2] byte0 - must be 0
        locBuf[5] = 0x00;   // [Param2] byte1 - must be 0
        locBuf[6] = 0x00;   // [CRC] byte0 - to be calculated excluding [command]
        locBuf[7] = 0x00;   // [CRC] byte1 - to be calculated

        calcCRC( 5, &locBuf[1], &locBuf[6] );
    }

    // transmit buffer to get data
    if ( retVal == DMCRYPTO_OK ) {
        // we are sending 8 bytes and expect to receive 7 bytes max.
        retVal = execCmd( address, locBuf, txSize, locBuf, rxSize, DMCRYPTO_SHA_MWAITMS_DEVREV );
    }

    return retVal;
}

uint8_t dmCrypto::atomic_ATECC508A_DevRev(uint8_t *locBuf, uint8_t locBufSize, uint8_t address ) {
    uint8_t       retVal     = DMCRYPTO_OK;
    const uint8_t txSize     = 8;
    const uint8_t rxSize     = 7;

    if( txSize > locBufSize ) {
        retVal = DMCRYPTO_ERR_BFULL;
    }

    // prepare transmit buffer
    if ( retVal == DMCRYPTO_OK ) {
        locBuf[0] = 0x03;   // [command] word address/packet function
        locBuf[1] = 0x07;   // [count] starting from [count] ending with [crc]
        locBuf[2] = 0x30;   // [OPCODE] for Info
        locBuf[3] = 0x00;   // [Param1] must be 0 for Revision
        locBuf[4] = 0x00;   // [Param2] byte0 - must be 0
        locBuf[5] = 0x00;   // [Param2] byte1 - must be 0
        locBuf[6] = 0x00;   // [CRC] byte0 - to be calculated excluding [command]
        locBuf[7] = 0x00;   // [CRC] byte1 - to be calculated

        calcCRC( 5, &locBuf[1], &locBuf[6] );
    }

    // transmit buffer to get data
    if ( retVal == DMCRYPTO_OK ) {
        // we are sending 8 bytes and expect to receive 7 bytes max.
        retVal = execCmd( address, locBuf, txSize, locBuf, rxSize, DMCRYPTO_ECC_MWAITMS_DEVREV );
    }

    return retVal;
}

uint8_t dmCrypto::deviceRevision(uint8_t deviceType, uint8_t address) {
    uint8_t retVal = DMCRYPTO_OK;

    switch( deviceType ) {
        case DMCRYPTO_ATSHA204A:
            retVal = atomic_ATSHA204A_DevRev(buf, DMCRYPTO_INTERNAL_BUFMAXLEN, address );
            break;
        case DMCRYPTO_ATECC508A:
            retVal = atomic_ATECC508A_DevRev(buf, DMCRYPTO_INTERNAL_BUFMAXLEN, address );
            break;
        case DMCRYPTO_ATAES132A:
            break;
        default:
            retVal = DMCRYPTO_ERR_DEVTYPE;
    }

    return retVal;
}


uint8_t dmCrypto::atomic_ATSHA204A_Random(uint8_t *locBuf, uint8_t locBufSize, uint8_t address ) {
    uint8_t retVal           = DMCRYPTO_OK;
    const uint8_t txSize     = 8;
    const uint8_t rxSize     = 35;

    if( rxSize > locBufSize ) {
        retVal = DMCRYPTO_ERR_BFULL;
    }

    // prepare transmit buffer
    if ( retVal == DMCRYPTO_OK ) {
        locBuf[0] = 0x03;   // [command] word address/packet function
        locBuf[1] = 0x07;   // [count] starting from [count] ending with [crc]
        locBuf[2] = 0x1B;   // [OPCODE] for RANDOM
        locBuf[3] = 0x00;   // [Param1] 0b=0 will automaticaly update EEPROM seed, 1-7b must be 0
        locBuf[4] = 0x00;   // [Param2] byte0 - must be 0
        locBuf[5] = 0x00;   // [Param2] byte1 - must be 0
        locBuf[6] = 0x00;   // [CRC] byte0 - to be calculated excluding [command]
        locBuf[7] = 0x00;   // [CRC] byte1 - to be calculated

        calcCRC( 5, &locBuf[1], &locBuf[6] );
    }

    // transmit buf[] to get data
    if ( retVal == DMCRYPTO_OK ) {
        retVal = execCmd( address, locBuf, txSize, locBuf, rxSize, DMCRYPTO_SHA_MWAITMS_DEVREV );
    }

    return retVal;
}

uint8_t dmCrypto::atomic_ATECC508A_Random(uint8_t *locBuf, uint8_t locBufSize, uint8_t address ) {
    uint8_t retVal           = DMCRYPTO_OK;
    const uint8_t txSize     = 8;
    const uint8_t rxSize     = 35;

    if( rxSize > locBufSize ) {
        retVal = DMCRYPTO_ERR_BFULL;
    }

    // prepare transmit buffer
    if ( retVal == DMCRYPTO_OK ) {
        locBuf[0] = 0x03;   // [command] word address/packet function
        locBuf[1] = 0x07;   // [count] starting from [count] ending with [crc]
        locBuf[2] = 0x1B;   // [OPCODE] for RANDOM
        locBuf[3] = 0x00;   // [Param1] 0b=0 will automaticaly update EEPROM seed, 1-7b must be 0
        locBuf[4] = 0x00;   // [Param2] byte0 - must be 0
        locBuf[5] = 0x00;   // [Param2] byte1 - must be 0
        locBuf[6] = 0x00;   // [CRC] byte0 - to be calculated excluding [command]
        locBuf[7] = 0x00;   // [CRC] byte1 - to be calculated

        calcCRC( 5, &locBuf[1], &locBuf[6] );
    }

    // transmit buf[] to get data
    if ( retVal == DMCRYPTO_OK ) {
        retVal = execCmd( address, locBuf, txSize, locBuf, rxSize, DMCRYPTO_ECC_MWAITMS_RANDOM );
    }

    return retVal;
}


uint8_t dmCrypto::generateRandom(uint8_t deviceType, uint8_t address) {
    uint8_t retVal = DMCRYPTO_OK;

    switch( deviceType ) {
        case DMCRYPTO_ATSHA204A:
            retVal = atomic_ATSHA204A_Random(buf, DMCRYPTO_INTERNAL_BUFMAXLEN, address );
            break;
        case DMCRYPTO_ATECC508A:
            retVal = atomic_ATECC508A_Random(buf, DMCRYPTO_INTERNAL_BUFMAXLEN, address );
            break;
        case DMCRYPTO_ATAES132A:
            break;
        default:
            retVal = DMCRYPTO_ERR_DEVTYPE;
    }

    return retVal;
}


uint8_t dmCrypto::atomic_ATSHA204A_Read(uint8_t *locBuf, uint8_t locBufSize, uint8_t address, uint8_t zone, uint8_t slot, uint8_t offset, uint8_t blockRd) {
    uint8_t retVal           = DMCRYPTO_OK;
    const uint8_t txSize     = 8;
    const uint8_t rxSize     = 7;
    const uint8_t rxSize_blk = 35;

    if( blockRd == true && rxSize_blk > locBufSize ) {
        retVal = DMCRYPTO_ERR_BFULL;
    }

    if( blockRd == false && txSize > locBufSize ) {
        retVal = DMCRYPTO_ERR_BFULL;
    }

    // prepare transmit buffer
    if ( retVal == DMCRYPTO_OK ) {
        locBuf[0] = 0x03;   // [command] word address/packet function
        locBuf[1] = 0x07;   // [count] starting from [count] ending with [crc]
        locBuf[2] = 0x02;   // [OPCODE] for READ
        // Param1
        locBuf[3] = 0x00;
        // b0-1 - zone
        // b2-6 - must be 0
        // b7   - If one, 32 bytes are read; otherwise 4 bytes are read. Must be zero if from OTP.
        locBuf[3] = ( zone & 0x03 ) | ( blockRd << 7 );
        // Param2 - byte0
        locBuf[4] = 0x00;
        // b0-2 - offset
        // b3-6 - slot aka block
        // b7   - must be 0
        locBuf[4] = ( ( slot & 0x0F ) << 3 ) | ( offset & 0x07 );
        // Param2 - byte1
        locBuf[5] = 0x00;   // must be 0
        locBuf[6] = 0x00;   // [CRC] byte0 - to be calculated excluding [command]
        locBuf[7] = 0x00;   // [CRC] byte1 - to be calculated

        calcCRC( 5, &locBuf[1], &locBuf[6] );
    }

    // transmit buf[] to get data
    if ( retVal == DMCRYPTO_OK ) {
        if( blockRd )
            retVal = execCmd( address, locBuf, txSize, locBuf, rxSize_blk, DMCRYPTO_SHA_MWAITMS_READ );
        else
            retVal = execCmd( address, locBuf, txSize, locBuf, rxSize, DMCRYPTO_SHA_MWAITMS_READ );
    }

    return retVal;
}

uint8_t dmCrypto::atomic_ATECC508A_Read(uint8_t *locBuf, uint8_t locBufSize, uint8_t address, uint8_t zone, uint8_t slot, uint8_t offset, uint8_t blockRd) {
    uint8_t retVal           = DMCRYPTO_OK;
    const uint8_t txSize     = 8;
    const uint8_t rxSize     = 7;
    const uint8_t rxSize_blk = 35;

    if( blockRd == true && rxSize_blk > locBufSize ) {
        retVal = DMCRYPTO_ERR_BFULL;
    }

    if( blockRd == false && txSize > locBufSize ) {
        retVal = DMCRYPTO_ERR_BFULL;
    }

    // prepare transmit buffer
    if ( retVal == DMCRYPTO_OK ) {
        locBuf[0] = 0x03;   // [command] word address/packet function
        locBuf[1] = 0x07;   // [count] starting from [count] ending with [crc]
        locBuf[2] = 0x02;   // [OPCODE] for READ
        // Param1
        locBuf[3] = 0x00;
        // b0-1 - zone
        // b2-6 - must be 0
        // b7   - If one, 32 bytes are read; otherwise 4 bytes are read. Must be zero if from OTP.
        locBuf[3] = ( zone & 0x03 ) | ( blockRd << 7 );
        // Param2 - byte0
        locBuf[4] = 0x00;
        // b0-2 - offset
        // b3-6 - slot aka block
        // b7   - must be 0
        locBuf[4] = ( ( slot & 0x0F ) << 3 ) | ( offset & 0x07 );
        // Param2 - byte1
        locBuf[5] = 0x00;   // must be 0
        locBuf[6] = 0x00;   // [CRC] byte0 - to be calculated excluding [command]
        locBuf[7] = 0x00;   // [CRC] byte1 - to be calculated

        calcCRC( 5, &locBuf[1], &locBuf[6] );
    }

    // transmit buf[] to get data
    if ( retVal == DMCRYPTO_OK ) {
        if( blockRd )
            retVal = execCmd( address, locBuf, txSize, locBuf, rxSize_blk, DMCRYPTO_ECC_MWAITMS_READ );
        else
            retVal = execCmd( address, locBuf, txSize, locBuf, rxSize, DMCRYPTO_ECC_MWAITMS_READ );
    }

    return retVal;
}


uint8_t dmCrypto::read(uint8_t deviceType, uint8_t address, uint8_t zone, uint8_t slot, uint8_t offset, uint8_t blockRd) {
    uint8_t retVal = DMCRYPTO_OK;

    switch( deviceType ) {
        case DMCRYPTO_ATSHA204A:
            retVal = atomic_ATSHA204A_Read(buf, DMCRYPTO_INTERNAL_BUFMAXLEN, address, zone, slot, offset, blockRd );
            break;
        case DMCRYPTO_ATECC508A:
            retVal = atomic_ATECC508A_Read(buf, DMCRYPTO_INTERNAL_BUFMAXLEN, address, zone, slot, offset, blockRd );
            break;
        case DMCRYPTO_ATAES132A:
            break;
        default:
            retVal = DMCRYPTO_ERR_DEVTYPE;
    }

    return retVal;
}

uint8_t dmCrypto::readConfig( uint8_t deviceType, uint8_t address ) {
    uint8_t retVal = DMCRYPTO_OK;
    uint8_t config[88+1+2];
    uint8_t confLen=0;
    uint8_t i=0;

    switch( deviceType ) {
        case DMCRYPTO_ATSHA204A:
            confLen=0;
            for( i=0; i<2; i++) {
                if ( retVal == DMCRYPTO_OK ) {
                    retVal = atomic_ATSHA204A_Read(buf, DMCRYPTO_INTERNAL_BUFMAXLEN, address, 0x00, i, 0x00, true );
                    memcpy( config+confLen, buf+1, buf[0]-3 );
                    confLen+=buf[0]-3;
                }
            }
            for( i=0; i<6; i++) {
                if ( retVal == DMCRYPTO_OK ) {
                    retVal = atomic_ATSHA204A_Read(buf, DMCRYPTO_INTERNAL_BUFMAXLEN, address, 0x00, 0x02, i, false );
                    memcpy( config+confLen, buf+1, buf[0]-3 );
                    confLen+=buf[0]-3;
                }
            }
            if( retVal == DMCRYPTO_OK ) {
                buf[0]=confLen + 3; // length + crc1 + crc2
                memcpy( buf+1, config, confLen );
                buf[confLen + 1] = 0x0;
                buf[confLen + 2] = 0x0;
                calcCRC( confLen + 1, buf, buf + confLen + 1 );
            }
            break;
        case DMCRYPTO_ATECC508A:
            confLen=0;
            for( i=0; i<3; i++) {
                if ( retVal == DMCRYPTO_OK ) {
                    retVal = atomic_ATECC508A_Read(buf, DMCRYPTO_INTERNAL_BUFMAXLEN, address, 0x00, i, 0x00, true );
                    memcpy( config+confLen, buf+1, buf[0]-3 );
                    confLen+=buf[0]-3;
                }
            }
            if( retVal == DMCRYPTO_OK ) {
                buf[0]=confLen + 3; // length + crc1 + crc2
                memcpy( buf+1, config, confLen );
                buf[confLen + 1] = 0x0;
                buf[confLen + 2] = 0x0;
                calcCRC( confLen + 1, buf, buf + confLen + 1 );
            }
            break;
        case DMCRYPTO_ATAES132A:
            break;
        default:
            retVal = DMCRYPTO_ERR_DEVTYPE;
    }

    return retVal;
}

uint8_t dmCrypto::atomic_ATECC508A_SHA(uint8_t *locBuf, uint8_t locBufSize, uint8_t address, uint8_t mode, uint8_t key, uint16_t msglen ) {
    uint8_t retVal = DMCRYPTO_OK;
    uint8_t tmpBuf[64];
    uint8_t txSize     = 0x00;
    uint8_t rxSize     = 0x00;


    switch(mode) {
        case DMCRYPTO_ECC_SHACMDMODE_SHASTART:
            if ( msglen == 0 ) {
              //ppc->printf("DEBUG: sha start;\n\r");
              memset(locBuf, 0x00, locBufSize);
              locBuf[0] = 0x03;   // [command] word address/packet function
              locBuf[1] = 0x07; // packet size
              locBuf[2] = 0x47; // [OPCODE] for SHA
              locBuf[3] = DMCRYPTO_ECC_SHACMDMODE_SHASTART; // Param1
              locBuf[4] = 0x00; // Param2[0] must be zero (length)
              locBuf[5] = 0x00; // Param2[1] must be zero (length)
              locBuf[6] = 0x00; // CRC[0]
              locBuf[7] = 0x00; // CRC[1]
              calcCRC( locBuf[1]-2 , &locBuf[1], &locBuf[6] );
              txSize = 8;
              rxSize = 4;
            }
            else {
              retVal = DMCRYPTO_ERR_BADPARAM;
            }
            break;
        case DMCRYPTO_ECC_SHACMDMODE_UPDATE:
            if( locBufSize >= 64 + 7 && msglen == 64 ) {
              //ppc->printf("DEBUG: sha update;\n\r");
              memcpy( tmpBuf, locBuf, 64 );
              memset( locBuf, 0x00, locBufSize);
              locBuf[0] = 0x03;
              locBuf[1] = 0x47; // packet size 64+7 = 71
              locBuf[2] = 0x47; // [OPCODE] for SHA
              locBuf[3] = DMCRYPTO_ECC_SHACMDMODE_UPDATE; // Param1
              locBuf[4] = 0x40; // Param2[0] length
              locBuf[5] = 0x00; // Param2[1] it must be 0 since total length can be max 0xFF
              memcpy( &locBuf[6], tmpBuf, 64);
              locBuf[70] = 0x00; // CRC[0]
              locBuf[71] = 0x00; // CRC[1]
              calcCRC( locBuf[1]-2 , &locBuf[1], &locBuf[70] );
              txSize = 72; // 64+8
              rxSize = 4;
            }
            else {
              retVal = DMCRYPTO_ERR_BADPARAM;
            }
            break;
        case DMCRYPTO_ECC_SHACMDMODE_SHAEND:
            if( locBufSize >= msglen + 7 && msglen < 64 ){
              //ppc->printf("DEBUG: sha end;\n\r");
              memcpy( tmpBuf, locBuf, msglen );
              memset( locBuf, 0x00, locBufSize);
              locBuf[0] = 0x03;
              locBuf[1] = msglen+7; // packet size
              locBuf[2] = 0x47; // [OPCODE] for SHA
              locBuf[3] = DMCRYPTO_ECC_SHACMDMODE_SHAEND; // Param1
              locBuf[4] = msglen; // Param2[0] length
              locBuf[5] = 0x00; // Param2[1] it must be 0 since total length can be max 0xFF
              memcpy( &locBuf[6], tmpBuf, msglen);
              locBuf[6+msglen] = 0x00; // CRC[0]
              locBuf[7+msglen] = 0x00; // CRC[1]
              calcCRC( locBuf[1]-2 , &locBuf[1], &locBuf[6+msglen] );
              txSize = 8 + msglen;
              rxSize = 35; // 1B(length) + 32B(digest) + 2B (CRC)
            }
            else {
              retVal = DMCRYPTO_ERR_BADPARAM;
            }
            break;
        case DMCRYPTO_ECC_SHACMDMODE_HMACSTART:
            break;
        case DMCRYPTO_ECC_SHACMDMODE_HMACEND:
            break;
        case DMCRYPTO_ECC_SHACMDMODE_PUBLIC:
            break;
        default:
            retVal = DMCRYPTO_ERR_BADPARAM;
    }

    if( retVal==DMCRYPTO_OK) {
        retVal = execCmd( address, locBuf, txSize, locBuf, rxSize, DMCRYPTO_ECC_MWAITMS_SHA );
    }

    return retVal;
}

uint8_t dmCrypto::genSHA256( uint8_t deviceType, uint8_t address, uint8_t *data, uint16_t datalen ) {
    uint8_t retVal = DMCRYPTO_OK;
    uint16_t index;
    bool exitLoop = false;

    switch( deviceType ) {
        case DMCRYPTO_ATSHA204A:
            break;
        case DMCRYPTO_ATECC508A:
            if( datalen > 0 ) {
              retVal = atomic_ATECC508A_SHA(buf, DMCRYPTO_INTERNAL_BUFMAXLEN, address, DMCRYPTO_ECC_SHACMDMODE_SHASTART, 0x00, 0x00 );
              if( retVal == DMCRYPTO_OK ) {
                index=0;
                exitLoop = false;
                while( exitLoop == false ) {
                  if( datalen >= 64 ) {
                    memcpy( buf, &data[index], 64);
                    retVal = atomic_ATECC508A_SHA(buf, DMCRYPTO_INTERNAL_BUFMAXLEN, address, DMCRYPTO_ECC_SHACMDMODE_UPDATE, 0x00, 64 );
                    if( retVal != DMCRYPTO_OK ) {
                      exitLoop = true; // exit while
                    }
                    index+=64; datalen-=64;
                  }
                  else {
                    memcpy( buf, &data[index], datalen );
                    retVal = atomic_ATECC508A_SHA(buf, DMCRYPTO_INTERNAL_BUFMAXLEN, address, DMCRYPTO_ECC_SHACMDMODE_SHAEND, 0x00, datalen );
                    index+=datalen; datalen=0;
                    exitLoop = true;
                  } // else
                } // while
              }
            }
            else {
              retVal = DMCRYPTO_ERR_BADPARAM;
            }
            break;
        case DMCRYPTO_ATAES132A:
            retVal = DMCRYPTO_ERR_NOSUPPORT;
            break;
    }

    return retVal;
}

uint8_t dmCrypto::delBufByte(uint8_t *buffer, uint8_t index, uint8_t arraylen) {
    uint8_t retVal = 0x00;
    uint8_t i;

    if( index >= arraylen )
        retVal = 0x01;
    else {
        for(i = index; i < arraylen - 1; i++)
            buf[i] = buf[i + 1];
        buf[arraylen - 1] = 0x00;
    }

    return retVal;
}
