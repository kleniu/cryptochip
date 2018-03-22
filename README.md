# MBED libs for CryptoAuth Xplained Pro
![source ](etc/CAXP.png)

## Development environment setup
1) Install Atom editor

2) Install PlatformIO plugin

3) Cone this repo to your comp

## Board connections
Connect MBED according to depicted schema. (Arduino Nano v3)

![Breadboard](etc/connections.png)

## Usage demo
Just compile and upload into your board.
Open the terminal connection with 115200 baud rate.
```
*****************************
    CryptoAuth Pro tests.    
press 0 or hit ENTER for menu
*****************************
 1 - Scan i2c bus
 2 - ATSHA204A - Read device rev
 3 - ATSHA204A - Generate random
 4 - ATSHA204A - Read data
 5 - ATSHA204A - Read config
 6 - ATECC508A - Read device rev
 7 - ATECC508A - Generate random
 8 - ATECC508A - Read data
 9 - ATECC508A - Read config
 a - ATECC508A - Generate SHA256
> 1 
 *** Scanning i2c bus. *** 
 Address                 
 7b   8b   Description   
==== ==== ===============
0x50 0xA0 Atmel ATAES132A
0x60 0xC0 Atmel ATECC508A
0x64 0xC8 Atmel ATSHA204A
-------------------------
Total devices found : 3
> 2 
 *** Read ATSHA204A device revision *** 
LEN(0x07)
 Wrd#   B#      B0 B1 B2 B3
============    == == == ==
[0x00;00-03]    00 02 00 09 
---------------------------
CRC(0x60 0x2B)
> 3 
 *** Read ATSHA204A random number *** 
LEN(0x23)
 Wrd#   B#      B0 B1 B2 B3
============    == == == ==
[0x00;00-03]    FF FF 00 00 
[0x01;04-07]    FF FF 00 00 
[0x02;08-11]    FF FF 00 00 
[0x03;12-15]    FF FF 00 00 
[0x04;16-19]    FF FF 00 00 
[0x05;20-23]    FF FF 00 00 
[0x06;24-27]    FF FF 00 00 
[0x07;28-31]    FF FF 00 00 
---------------------------
CRC(0x41 0x1A)
> 4 
 *** Read ATSHA204A data *** 
Enter zone number : 0
Enter slot number : 0
Enter offset      : 0
32B read (y/N)    : true
LEN(0x23)
 Wrd#   B#      B0 B1 B2 B3
============    == == == ==
[0x00;00-03]    01 23 E7 A5 
[0x01;04-07]    00 09 04 00 
[0x02;08-11]    F5 1F 24 53 
[0x03;12-15]    EE 0D 01 00 
[0x04;16-19]    C8 00 55 00 
[0x05;20-23]    8F 80 80 A1 
[0x06;24-27]    82 E0 A3 60 
[0x07;28-31]    94 40 A0 85 
---------------------------
CRC(0xAE 0x32)
> 5 
 *** Read ATSHA204A config *** 
LEN(0x5B)
 Wrd#    B#        B0    bits       B1    bits       B2    bits       B3    bits   
==============    ==== ========    ==== ========    ==== ========    ==== ========
[0x00;000-003]    0x01 .......|    0x23 ..|...||    0xE7 |||..|||    0xA5 |.|..|.|    
[0x01;004-007]    0x00 ........    0x09 ....|..|    0x04 .....|..    0x00 ........    
[0x02;008-011]    0xF5 ||||.|.|    0x1F ...|||||    0x24 ..|..|..    0x53 .|.|..||    
[0x03;012-015]    0xEE |||.|||.    0x0D ....||.|    0x01 .......|    0x00 ........    
[0x04;016-019]    0xC8 ||..|...    0x00 ........    0x55 .|.|.|.|    0x00 ........    
[0x05;020-023]    0x8F |...||||    0x80 |.......    0x80 |.......    0xA1 |.|....|    
[0x06;024-027]    0x82 |.....|.    0xE0 |||.....    0xA3 |.|...||    0x60 .||.....    
[0x07;028-031]    0x94 |..|.|..    0x40 .|......    0xA0 |.|.....    0x85 |....|.|    
[0x08;032-035]    0x86 |....||.    0x40 .|......    0x87 |....|||    0x07 .....|||    
[0x09;036-039]    0x0F ....||||    0x00 ........    0x89 |...|..|    0xF2 ||||..|.    
[0x0A;040-043]    0x8A |...|.|.    0x7A .||||.|.    0x0B ....|.||    0x8B |...|.||    
[0x0B;044-047]    0x0C ....||..    0x4C .|..||..    0xDD ||.|||.|    0x4D .|..||.|    
[0x0C;048-051]    0xC2 ||....|.    0x42 .|....|.    0xAF |.|.||||    0x8F |...||||    
[0x0D;052-055]    0xFF ||||||||    0x00 ........    0xFF ||||||||    0x00 ........    
[0x0E;056-059]    0xFF ||||||||    0x00 ........    0xFF ||||||||    0x00 ........    
[0x0F;060-063]    0xFF ||||||||    0x00 ........    0xFF ||||||||    0x00 ........    
[0x10;064-067]    0xFF ||||||||    0x00 ........    0xFF ||||||||    0x00 ........    
[0x11;068-071]    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    
[0x12;072-075]    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    
[0x13;076-079]    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    
[0x14;080-083]    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    
[0x15;084-087]    0x00 ........    0x00 ........    0x55 .|.|.|.|    0x55 .|.|.|.|    
----------------------------------------------------------------------------------
CRC(0x5B 0x8E)
> 6 
 *** Read ATECC508A device revision *** 
LEN(0x07)
 Wrd#   B#      B0 B1 B2 B3
============    == == == ==
[0x00;00-03]    00 00 50 00 
---------------------------
CRC(0x03 0x91)
> 7 
 *** Read ATECC508A random number *** 
LEN(0x23)
 Wrd#   B#      B0 B1 B2 B3
============    == == == ==
[0x00;00-03]    C9 2B 21 59 
[0x01;04-07]    CB 8B 4D AE 
[0x02;08-11]    94 AF 23 B9 
[0x03;12-15]    54 21 84 DD 
[0x04;16-19]    22 C8 F5 9F 
[0x05;20-23]    C2 B1 9D 2A 
[0x06;24-27]    92 D5 42 43 
[0x07;28-31]    89 D0 AA BA 
---------------------------
CRC(0x4F 0x33)
> 8 
 *** Read ATECC508A data *** 
Enter zone number : 0
Enter slot number : 0
Enter offset      : 0
32B read (y/N)    : true
LEN(0x23)
 Wrd#   B#      B0 B1 B2 B3
============    == == == ==
[0x00;00-03]    01 23 96 AE 
[0x01;04-07]    00 00 50 00 
[0x02;08-11]    0B BA CB 9A 
[0x03;12-15]    EE C0 19 00 
[0x04;16-19]    C0 00 AA 00 
[0x05;20-23]    81 A0 81 A0 
[0x06;24-27]    81 A0 81 A0 
[0x07;28-31]    81 A0 81 A0 
---------------------------
CRC(0x02 0x37)
> 9 
 *** Read ATECC508A config *** 
LEN(0x83)
 Wrd#    B#        B0    bits       B1    bits       B2    bits       B3    bits   
==============    ==== ========    ==== ========    ==== ========    ==== ========
[0x00;000-003]    0x01 .......|    0x23 ..|...||    0x96 |..|.||.    0xAE |.|.|||.    
[0x01;004-007]    0x00 ........    0x00 ........    0x50 .|.|....    0x00 ........    
[0x02;008-011]    0x0B ....|.||    0xBA |.|||.|.    0xCB ||..|.||    0x9A |..||.|.    
[0x03;012-015]    0xEE |||.|||.    0xC0 ||......    0x19 ...||..|    0x00 ........    
[0x04;016-019]    0xC0 ||......    0x00 ........    0xAA |.|.|.|.    0x00 ........    
[0x05;020-023]    0x81 |......|    0xA0 |.|.....    0x81 |......|    0xA0 |.|.....    
[0x06;024-027]    0x81 |......|    0xA0 |.|.....    0x81 |......|    0xA0 |.|.....    
[0x07;028-031]    0x81 |......|    0xA0 |.|.....    0x81 |......|    0xA0 |.|.....    
[0x08;032-035]    0x81 |......|    0xA0 |.|.....    0x81 |......|    0xA0 |.|.....    
[0x09;036-039]    0x81 |......|    0xA0 |.|.....    0x81 |......|    0xA0 |.|.....    
[0x0A;040-043]    0x81 |......|    0xA0 |.|.....    0x81 |......|    0xA0 |.|.....    
[0x0B;044-047]    0x81 |......|    0xA0 |.|.....    0x81 |......|    0xA0 |.|.....    
[0x0C;048-051]    0x81 |......|    0xA0 |.|.....    0x81 |......|    0xA0 |.|.....    
[0x0D;052-055]    0xFF ||||||||    0x00 ........    0xFF ||||||||    0x00 ........    
[0x0E;056-059]    0xFF ||||||||    0x00 ........    0xFF ||||||||    0x00 ........    
[0x0F;060-063]    0xFF ||||||||    0x00 ........    0xFF ||||||||    0x00 ........    
[0x10;064-067]    0xFF ||||||||    0x00 ........    0xFF ||||||||    0x00 ........    
[0x11;068-071]    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    
[0x12;072-075]    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    
[0x13;076-079]    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    
[0x14;080-083]    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    0xFF ||||||||    
[0x15;084-087]    0x00 ........    0x00 ........    0x00 ........    0x00 ........    
[0x16;088-091]    0xFE |||||||.    0xFF ||||||||    0x00 ........    0x00 ........    
[0x17;092-095]    0x00 ........    0x00 ........    0x00 ........    0x00 ........    
[0x18;096-099]    0x33 ..||..||    0x00 ........    0x33 ..||..||    0x00 ........    
[0x19;100-103]    0x33 ..||..||    0x00 ........    0x33 ..||..||    0x00 ........    
[0x1A;104-107]    0x33 ..||..||    0x00 ........    0x33 ..||..||    0x00 ........    
[0x1B;108-111]    0x33 ..||..||    0x00 ........    0x33 ..||..||    0x00 ........    
[0x1C;112-115]    0x3C ..||||..    0x00 ........    0x3C ..||||..    0x00 ........    
[0x1D;116-119]    0x3C ..||||..    0x00 ........    0x3C ..||||..    0x00 ........    
[0x1E;120-123]    0x3C ..||||..    0x00 ........    0x3C ..||||..    0x00 ........    
[0x1F;124-127]    0x3C ..||||..    0x00 ........    0x3C ..||||..    0x00 ........    
----------------------------------------------------------------------------------
CRC(0x20 0x7B)
> a 
 *** ATECC508A generate SHA256 *** 
Enter string value : test
LEN(0x23)
9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08
CRC(0xF9 0x12)
>
```

## References
### I2C
- [I2C-bus specification and user manual](https://www.nxp.com/docs/en/user-guide/UM10204.pdf) by NXP Semiconductors.
- [7-bit, 8-bit, and 10-bit I2C Slave Addressing](https://www.totalphase.com/support/articles/200349176-7-bit-8-bit-and-10-bit-I2C-Slave-Addressing#reserved) by Total Phase
- [MBEDOS I2C Driver Reference](https://os.mbed.com/docs/latest/reference/i2c.html) by ARM MBED
- [Using the I2C Bus](http://www.robot-electronics.co.uk/i2c-tutorial) by Robot Electronics
### ATSHA204A
- [Using the Atmel CryptoAuthentication ATSHA204A](https://www.insidegadgets.com/2017/04/21/alarm-system-v3-part-3-using-the-atmel-cryptoauthentication-atsha204a/) by insideGadgets
### CRC online calculators
- [CRC Calculator (Javascript)](http://www.sunshine2k.de/coding/javascript/crc/crc_js.html) by Bastian Molkenthin
```
To check ATMEL CRC-16 set folowing parameters:
- CRC width                                 = CRC-16
- CRC parametrization                       = Custom
- CRC detailed parameters, Input reflected  = ON
- CRC detailed parameters, Result reflected = OFF
- CRC detailed parameters, Polynomial       = 0x8005
- CRC detailed parameters,Initial Value     = 0x00
- CRC detailed parameters,Final Xor Value   = 0x00  
```

## Basic terminology
**nonce** - is an arbitrary number that can only be used once. It is often a pseudo-random number.

**crc polynomial** - a polynomial which becomes the divisor in a polynomial long division, which takes the message as the dividend and in which the quotient is discarded and the remainder becomes the result.

## Byte and Bit ordering
| Device    | Array of Bytes     | Byte ordering | Bit ordering (i2c) | Word ordering in Keys                       |
|:---------:|--------------------|---------------|--------------------|---------------------------------------------|
| ATSHA204A |byte index #0 first | LSB first     | MSb first          |                                             |
| ATECC508A |byte index #0 first | LSB first     | MSb first          | most significant word at the lowest address |
| ATAES132A |byte index #0 first | MSB first     | MSb first          |                                             |

## ATSHA204A

### ATSHA204A Jargon
*encrypted reads* - encrypted reads with the key (read key) in specific slot

*source key for the computation performed by the DeriveKey command* - n/a

*Roll-Key operation* - the source key that will be combined with TempKey is the target key specified in the command DeriveKey (SlotConfig[TargetKey].Bit12 must be 0)

*Create Key operation* - the source key is the parent key of the target key, which is found in SlotConfig[TargetKey].WriteKey (SlotConfig[TargetKey].Bit12 must be 1)

*parent key* - n/a

*hardware transport key array* - n/a



### ATSHA204A Slot Configs bytes
```
|Byte 0                         |Byte 1                           |
| b0  b1  b2  b3  b4  b5  b6  b7| b8  b9  b10 b11 b12 b13 b14 b15 |
  <---+--------> <+> <+> <+> <+>  <------+------> <------+------>
      |           |   |   |   |          |               +>> *WriteConfig* Write Configuration Bits
      |           |   |   |   |          +>> *WriteKey* Slot of the key to be used to validate encrypted writes.
      |           |   |   |   +>> *IsSecret* - 0=The slot is not secret and allows clear read, clear write,
      |           |   |   |                      no MAC check, and no Derivekey Command
      |           |   |   |                    1=The slot is secret. Reads and writes if allowed, must be encrypted.
      |           |   |   +>> *EncryptRead*
      |           |   +>> *SingleUse* - 0=No limit on the number of time the key can be used.
      |           |                     1=Limit on the number of time the key can be used based on the UseFlag
      |           |                       (or LastKeyUse)
      |           +>> *CheckOnly*
      +>> *ReadKey* - Slot of the key to be used for encrypted reads.


Command.   
```

## TODO:
calculate SHA256 - DONE!
- cmd 0x47 for ATECC508A
    * input: msg, msglen
    * output: sha256 + store it in TempKey

calculate HMAC
- cmd 0x47 for ATECC508A
    * input: msg, msglen, keyID
    * output: HMAC + store it in TempKey

sign with ECDSA
- cmd 0x41 for ATECC508A
    * input: keyID
    * output: signed digest from TempKey
