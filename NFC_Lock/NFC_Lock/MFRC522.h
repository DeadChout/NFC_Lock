/*
* MFRC522.h - Ported from https://github.com/ghalfacree/Arduino-Sketches/blob/master/RFID2560/RFID2560.pde
*
* Created on: 2013-3-26
* Author: shawn
*/

#ifndef MFRC522_H_
#define MFRC522_H_

#define uchar unsigned char
#define uint unsigned int

//MF522 command bits
#define PCD_IDLE 0x00 //NO action; cancel current commands
#define PCD_AUTHENT 0x0E //verify password key
#define PCD_RECEIVE 0x08 //receive data
#define PCD_TRANSMIT 0x04 //send data
#define PCD_TRANSCEIVE 0x0C //send and receive data
#define PCD_RESETPHASE 0x0F //reset
#define PCD_CALCCRC 0x03 //CRC check and caculation

//Mifare_One card command bits
#define PICC_REQIDL 0x26 //Search the cards that not into sleep mode in the antenna area
#define PICC_REQALL 0x52 //Search all the cards in the antenna area
#define PICC_ANTICOLL 0x93 //prevent conflict
#define PICC_SElECTTAG 0x93 //select card
#define PICC_AUTHENT1A 0x60 //verify A password key
#define PICC_AUTHENT1B 0x61 //verify B password key
#define PICC_READ 0x30 //read
#define PICC_WRITE 0xA0 //write
#define PICC_DECREMENT 0xC0 //deduct value
#define PICC_INCREMENT 0xC1 //charge up value
#define PICC_RESTORE 0xC2 //Restore data into buffer
#define PICC_TRANSFER 0xB0 //Save data into buffer
#define PICC_HALT 0x50 //sleep mode

//THe mistake code that return when communicate with MF522
#define MI_OK 0
#define MI_NOTAGERR 1
#define MI_ERR 2

//void MFRC522_MFRC522(int PinSS, int PinRST);
// virtual ~MFRC522();
void MFRC522_Reset(void);
void MFRC522_Init(void);


uchar MFRC522_Request(uchar reqMode, uchar *TagType);
uchar MFRC522_Anticoll(uchar *serNum);

uchar MFRC522_SelectTag(uchar *serNum);
uchar MFRC522_Auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum);

uchar MFRC522_Read(uchar blockAddr, uchar *recvData);
uchar MFRC522_Write(uchar blockAddr, uchar *writeData);
void MFRC522_Halt(void);

void Write_MFRC522(uchar addr, uchar val);
uchar Read_MFRC522(uchar addr);

uchar MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen,uchar *backData, uint *backLen);
void MFRC522_CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData);

void MFRC522_SetBitMask(uchar reg, uchar mask);
void MFRC522_ClearBitMask(uchar reg, uchar mask);
void MFRC522_AntennaOn(void);
void MFRC522_AntennaOff(void);


//int _pinReset;
//int _pinSlaveSelect;

#define pinSlaveSelect_OUTPUT	DDRC |= (1<<PINC0);
#define pinSlaveSelect_LOW		PORTC &= ~(1<<PINC0);
#define pinSlaveSelect_HIGH		PORTC |= (1<<PINC0);

#define pinReset_OUTPUT	DDRC |= (1<<PINC1);
#define pinReset_LOW		PORTC &= ~(1<<PINC1);
#define pinReset_HIGH		PORTC |= (1<<PINC1);

#endif
