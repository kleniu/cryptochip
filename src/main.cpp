#include <mbed.h>
#include "../lib/dmMenu/src/dmMenu.h"
#include "../lib/dmCrypto/src/dmCrypto.h"

I2C        i2c(I2C_SDA, I2C_SCL); // SDA, SCL

DigitalOut myled(LED1);

Serial     pc(SERIAL_TX, SERIAL_RX);

dmMenu   menu;
dmCrypto crypto;

void printResult(uint8_t *buf) {
    uint8_t i, j;
    // print len
    pc.printf("LEN(0x%02X)\n\r", buf[0]);

    // print data
    pc.printf(" Wrd#  B0 B1 B2 B3\n\r");
    pc.printf("====== == == == ==");
    for ( i=1, j=0; i<buf[0]-2; i++ ) {
        if( !((i-1)%4) ) {
            pc.printf("\n\r[0x%02X] ", j++);
        }
        pc.printf("%02X ", buf[i]);
    }
    pc.printf("\n\r------------------\n\r");
    i = buf[0];
    pc.printf("CRC(0x%02X 0x%02X)", buf[i-2], buf[i-1]);
}


void mScanI2Cbus() {
    pc.printf(" *** Scanning i2c bus. *** \n\r");
    unsigned char i, devAddress;
    unsigned char totalDevices;

    crypto.scanI2Cbus();
    pc.printf(" Address                 \n\r");

    pc.printf(" 7b   8b   Description   \n\r");
    pc.printf("==== ==== ===============\n\r");
    totalDevices = crypto.buf[0];
    for( i = 1; i <= totalDevices; i++ ) {
        devAddress = crypto.buf[i];
        pc.printf("0x%02X 0x%02X ", devAddress, devAddress<<1);
        switch (devAddress << 1) {
            case 0xC8:
                pc.printf("Atmel ATSHA204A\n\r");
                break;
            case 0xC0:
                pc.printf("Atmel ATECC508A\n\r");
                break;
            case 0xA0:
                pc.printf("Atmel ATAES132A\n\r");
                break;
            default:
                pc.printf("unknown\n\r");
        }
    }
    pc.printf("-------------------------\n\r");
    if ( totalDevices == 0 )
        pc.printf("No I2C devices found");
    else {
        pc.printf("Total devices found : %d", totalDevices );
    }
}

void mReadATSHA204Arev() {
    unsigned char error;
    pc.printf(" *** Read ATSHA204A device revision *** \n\r");

    //// reads ATSHA204A Device Revision ////
    error = crypto.deviceRevision( DMCRYPTO_ATSHA204A, 0x64 );
    /////////////////////////////////////////

    if ( error != DMCRYPTO_OK ) {
        pc.printf("ERR cannot read device revision number from ATSHA204A; error=%2X\n\r", error);
    }
    else {
        printResult(crypto.buf);
    }
}

void mReadATSHA204Arandom() {
    unsigned char error;
    pc.printf(" *** Read ATSHA204A random number *** \n\r");

    ///// reads ATSHA204A Random number /////
    error = crypto.generateRandom( DMCRYPTO_ATSHA204A, 0x64 );
    /////////////////////////////////////////

    if ( error != DMCRYPTO_OK ) {
        pc.printf("ERR cannot read rundom number from ATSHA204A; error=0x%02X", error);
    }
    else {
        printResult(crypto.buf);
    }
}

void mReadATSHA204Adata() {
    unsigned char error, zone, slot, offset, blockRd;
    pc.printf(" *** Read ATSHA204A data *** \n\r");

    zone    = menu.getNumber( "Enter zone number : " );
    slot    = menu.getNumber( "Enter slot number : " );
    offset  = menu.getNumber( "Enter offset      : " );
    blockRd = menu.getBool(   "32B read (y/N)    : ", 'y');

    /////////// reads ATSHA204A data /////////
    error = crypto.read( DMCRYPTO_ATSHA204A, 0x64, zone, slot, offset, blockRd);
    //////////////////////////////////////////

    if ( error != DMCRYPTO_OK ) {
        pc.printf("ERR cannot read data from ATSHA204A; error=0x%02X",error);
    }
    else {
        printResult(crypto.buf);
    }
}

void mReadATSHA204Aconfig() {
    unsigned char error;
    pc.printf(" *** Read ATSHA204A config *** \n\r");

    /////////// reads ATSHA204A data /////////
    error = crypto.readConfig( DMCRYPTO_ATSHA204A, 0x64 );
    //////////////////////////////////////////

    if ( error != DMCRYPTO_OK ) {
        pc.printf("ERR cannot read data from ATSHA204A; error=0x%02X", error);
    }
    else {
        printResult(crypto.buf);
    }
}


int main() {
  myled=0;
  pc.baud( 115200 );
  i2c.frequency(100000);
  crypto.init(&i2c);

  menu.init(&pc);
  menu.setHeader( "    CryptoAuth Pro tests.    " );
  menu.addItem( '1', "Scan i2c bus.", mScanI2Cbus );
  menu.addItem( '2', "Read ATSHA204A device rev.", mReadATSHA204Arev );
  menu.addItem( '3', "Gen. random with ATSHA204A.", mReadATSHA204Arandom );
  menu.addItem( '4', "Read data from ATSHA204A.", mReadATSHA204Adata );
  menu.addItem( '5', "Read config from ATSHA204A.", mReadATSHA204Aconfig );

  while(1) {
    menu.display();
  }

}