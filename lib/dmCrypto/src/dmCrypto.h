#ifndef DMCRYPTO_H
#define DMDRYPTO_H
#include "../../dmHAL/src/mbed_dmI2C.h"

#define DMCRYPTO_ATSHA204A    0x00
#define DMCRYPTO_ATECC508A    0x01
#define DMCRYPTO_ATAES132A    0x02

#define DMCRYPTO_OK            0x00
#define DMCRYPTO_ERR_WAKE      0x21
#define DMCRYPTO_ERR_SEND      0x22
#define DMCRYPTO_ERR_RECEIVE   0x23
#define DMCRYPTO_ERR_CRC       0x24
#define DMCRYPTO_ERR_BFULL     0x25
#define DMCRYPTO_ERR_PARSE     0x26 // e.g. when read Config zone Slot 3 with 32 byte read
#define DMCRYPTO_ERR_DEVTYPE   0x27
#define DMCRYPTO_ERR_BADPARAM  0x28
#define DMCRYPTO_ERR_NOSUPPORT 0x29

#define DMCRYPTO_SHA_CHECKMAC_MISCOMPARE    0x01
#define DMCRYPTO_SHA_ERR_PARSE              0x03
#define DMCRYPTO_SHA_ERR_EXEC               0x0F
#define DMCRYPTO_SHA_ERR_CRC                0xFF

#define DMCRYPTO_SHA_MWAITMS_DEVREV     2   // max execution time for DEVREV
#define DMCRYPTO_SHA_MWAITMS_RANDOM     50  // exec time for random number
#define DMCRYPTO_SHA_MWAITMS_READ       4   // exec time for reading data

#define DMCRYPTO_ECC_MWAITMS_DEVREV     1   // max execution time for DEVREV
#define DMCRYPTO_ECC_MWAITMS_RANDOM     23  // exec time for random number
#define DMCRYPTO_ECC_MWAITMS_READ       1   // exec time for reading data
#define DMCRYPTO_ECC_MWAITMS_SHA        9   // exec time for SHA/HMAC

// modes used ad P1 in SHA command with ATECC508A chip
#define DMCRYPTO_ECC_SHACMDMODE_SHASTART  0x00
#define DMCRYPTO_ECC_SHACMDMODE_UPDATE    0x01
#define DMCRYPTO_ECC_SHACMDMODE_SHAEND    0x02
#define DMCRYPTO_ECC_SHACMDMODE_PUBLIC    0x03
#define DMCRYPTO_ECC_SHACMDMODE_HMACSTART 0x04
#define DMCRYPTO_ECC_SHACMDMODE_HMACEND   0x05

#define DMCRYPTO_INTERNAL_BUFMAXLEN     127

struct ATSHA204A_ConfigZone {
    // slot 0x01
    uint32_t    serialNumber_B0_3;
    uint32_t    revisionNumber;
    uint32_t    serialNumber_B4_7;
    uint8_t     serialNumber_B8, reserved1,      i2cEnable, reserved2;
    uint8_t     i2cAddress,      checkMacConfig, OTPmode,   SelectorMode;
    uint16_t    slotConfig0,  slotConfig1;
    uint16_t    slotConfig2,  slotConfig3;
    uint16_t    slotConfig4,  slotConfig5;
    // slot 0x01
    uint16_t    slotConfig6,  slotConfig7;
    uint16_t    slotConfig8,  slotConfig9;
    uint16_t    slotConfig10, slotConfig11;
    uint16_t    slotConfig12, slotConfig13;
    uint16_t    slotConfig14, slotConfig15;
    uint8_t     useFlag0,  updateClout0,  useFlag1,  updateClout1;
    uint8_t     useFlag2,  updateClout2,  useFlag3,  updateClout3;
    uint8_t     useFlag4,  updateClout4,  useFlag5,  updateClout5;
    // slot 0x2
    uint8_t     useFlag6,  updateClout6,  useFlag7,  updateClout7;
    uint8_t     useFlag8,  updateClout8,  useFlag9,  updateClout9;
    uint8_t     useFlag10, updateClout10, useFlag11, updateClout11;
    uint8_t     useFlag12, updateClout12, useFlag13, updateClout13;
    uint8_t     useFlag14, updateClout14, useFlag15, updateClout15;
    uint8_t     userExtra, slector,       lockData,  lockConfig;
};

class dmCrypto {
    public:
        dmCrypto();
        ~dmCrypto();
        // public methods
        dmI2C   i2c;
        Serial  *ppc;

        uint8_t buf[DMCRYPTO_INTERNAL_BUFMAXLEN];
        uint8_t bufDataSize;    // number of valid bytes in buf
        uint8_t bufDataCRC;
        uint8_t packetSize;
        uint8_t packetCRC[2];

        ATSHA204A_ConfigZone    *ATSHA204AcfgZone;

        void init(I2C *loci2c, Serial *lppc);
        void scanI2Cbus();
        uint8_t deviceRevision(uint8_t deviceType, uint8_t address);
        uint8_t generateRandom(uint8_t deviceType, uint8_t address);
        uint8_t read(          uint8_t deviceType, uint8_t address, uint8_t zone, uint8_t slot, uint8_t offset, uint8_t blockRd);
        uint8_t readConfig(    uint8_t deviceType, uint8_t address );
        uint8_t genSHA256(     uint8_t deviceType, uint8_t address, uint8_t *data, uint16_t datalen );

        //uint8_t ATSHA204A_GetCfg(uint8_t address);
        //uint8_t ATSHA204A_GetSlotCfg(uint8_t address, uint8_t slot);
        //void mReadATSHA204Aserial();

        void calcCRC(uint8_t length, uint8_t *data, uint8_t *crc);
    protected:
        uint8_t wakeDevice(uint8_t address);
        uint8_t execCmd(uint8_t address, uint8_t *sendBuf, uint8_t sendBufLen, uint8_t *recBuf, uint8_t recBufLen, uint8_t execWait);
        // ATSHA204A
        uint8_t atomic_ATSHA204A_DevRev( uint8_t *locBuf, uint8_t locBufSize, uint8_t address );
        uint8_t atomic_ATSHA204A_Random( uint8_t *locBuf, uint8_t locBufSize, uint8_t address);
        uint8_t atomic_ATSHA204A_Read(uint8_t *locBuf, uint8_t locBufSize, uint8_t address, uint8_t zone, uint8_t slot, uint8_t offset, uint8_t blockRd);
        // ATECC508A
        uint8_t atomic_ATECC508A_DevRev(uint8_t *locBuf, uint8_t locBufSize, uint8_t address );
        uint8_t atomic_ATECC508A_Random( uint8_t *locBuf, uint8_t locBufSize, uint8_t address);
        uint8_t atomic_ATECC508A_Read(uint8_t *locBuf, uint8_t locBufSize, uint8_t address, uint8_t zone, uint8_t slot, uint8_t offset, uint8_t blockRd);
        uint8_t atomic_ATECC508A_SHA(uint8_t *locBuf, uint8_t locBufSize, uint8_t address, uint8_t mode, uint8_t key, uint16_t msglen);
    private:
        uint8_t delBufByte(uint8_t *buffer, uint8_t index, uint8_t arraylen);
};

#endif
