#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>
#include "MFRC522.h"
#include <stdbool.h>

//---- Usability ease Changing -----
#define Red_ON PORTD |= 0b00010000 // LED Red On
#define Red_Off PORTD &= 0b11101111 //LED red Off
#define Green_ON PORTD |= 0b00001000 // Green Red On
#define Green_Off PORTD &= 0b11110111 //Green red Off
#define Lock_ON PORTD |= 0b00000010 // Lock Power On
#define Lock_Off PORTD &= 0b11111101 //Lock Power Off

//==========================< Lock Controls >====================

void open_lock(){
	_delay_ms(100);
	Green_ON;
	Lock_Off;
	_delay_ms(500);
	_delay_ms(1000);
	Green_Off;
	_delay_ms(4500);
	Lock_ON;
	_delay_ms(100);
}

void access_denied(){ // lock is still closed
	_delay_ms(100);
	Red_ON;
	_delay_ms(150);
	_delay_ms(500);
	Red_Off;
	_delay_ms(100);
}


//==========================< EEPROM >=========================================
//copy-pasted from documentation
void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));
	/* Set up address and Data Registers */
	EEAR = uiAddress;
	EEDR = ucData;
	/* Write logical one to EEMPE */
	EECR |= (1<<EEMPE);
	/* Start EEprom write by setting EEPE */
	EECR |= (1<<EEPE);
}

unsigned char EEPROM_read(unsigned int uiAddress)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));
	/* Set up address register */
	EEAR = uiAddress;
	/* Start EEprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from Data Register */
	return EEDR;
}

//==========================< Card processing >==================================

void save_card(uchar *card_num){
	int i = 0;
	int j;
	int ee_check;
	while(1){
		ee_check = EEPROM_read(0x000 + 5 + i);
		if (ee_check == 0xff){
			break;
		}
		i+=5;
	}
	for (j=0; j < 5; j++) // 5 - размер записываемого массива
	{
		EEPROM_write(0x000 + 5 + i + j, card_num[j]);
	}
}

bool read_card(uchar *card_num){
	uchar saved_card[5];
	bool access;
	int ee_check;
	int count;
	int i = 0;
	int j;
	while(1){
		ee_check = EEPROM_read(0x000 + 5 + i);
		if(ee_check != 0xff){
			for (count = 0; count < 5; count++)
			{
				ee_check = EEPROM_read(0x000 + 5 + i);
				saved_card[count] = ee_check;
				i++;
			}
			for (j = 0; j < 5; j++ ) {
				if (saved_card[j] == card_num[j])
				{
					if(j == 4){
						return access = true;
					}
				}
				else {
					break;
				}
			}
		}
		else{
			return access = false;
		}
	}
}

bool master_operations(uchar *card_num, uchar* master_card, bool *empty_memory){
	cli();
	bool master_attached;
	_delay_ms(1500);
	Red_ON;
	Green_ON;
	while(1) {
		if(!(PIND&0b00000100)){   // check if button is pressed (Clearing pass-cards memory)
			Green_Off;
			Red_Off;
			while(1){
				if ( MFRC522_Request( PICC_REQIDL, card_num ) == MI_OK ) {
					if ( MFRC522_Anticoll( card_num ) == MI_OK ) {
						break;
					}
				}
			}
			for (int i = 0; i < 5; i++ ) {
				if (master_card[i]==card_num[i])
				{
					if(i==4)
					{
						master_attached = true;
					}
				}
				else{
					master_attached = false;
					break;
				}
			}
			if (master_attached == true)
			{
				int j = 0;
				int ee_check;
				while(1){
					ee_check = EEPROM_read(0x000 + j);
					if (ee_check != 0xff){
						EEPROM_write(0x000 + j, 0xff);
						j++;
					}
					else
					{
						_delay_ms(500);
						Red_ON;
						Green_ON;
						_delay_ms(500);
						Red_Off;
						Green_Off;
						_delay_ms(500);
						Red_ON;
						Green_ON;
						_delay_ms(500);
						Red_Off;
						Green_Off;
						_delay_ms(500);
						Red_ON;
						Green_ON;
						_delay_ms(500);
						Red_Off;
						Green_Off;
						_delay_ms(500);
						*empty_memory = true;
						return master_attached;
					}
				}
			}
			else if (master_attached == false){
				_delay_ms(500);
				Red_ON;
				_delay_ms(500);
				Red_Off;
				_delay_ms(500);
				Red_ON;
				_delay_ms(500);
				Red_Off;
				_delay_ms(500);
				return master_attached = true;
			}
		}
		else if ( MFRC522_Request( PICC_REQIDL, card_num ) == MI_OK ) {
			if ( MFRC522_Anticoll( card_num ) == MI_OK ) {
				Green_Off;
				Red_Off;
				for (int i = 0; i < 5; i++ ) {
					if (master_card[i]==card_num[i])
					{
						if (i == 4){
							_delay_ms(1500);
							return master_attached = true;
						}
					}
					else {
						//read_card(card_num);
						return master_attached = false;
					}
				}
			}
		}
	}
}

void check_for_master(uchar *card_num, uchar *master_card){
	if (EEPROM_read(0x000)!=0xff)
	{
		for (int i=0; i<5; i++)
		{
			master_card[i] = EEPROM_read(0x000 + i);
		}
	}
	else{
		while(1) {
			Red_ON;
			Green_ON;
			_delay_ms(250);
			if ( MFRC522_Request( PICC_REQIDL, card_num ) == MI_OK ) {
				if ( MFRC522_Anticoll( card_num ) == MI_OK ) {
					Red_Off;
					Green_Off;
					int j;
					while(1){
						for (j=0; j < 5; j++) // 5 - размер записываемого массива
						{
							EEPROM_write(0x000 + j, card_num[j]);
							master_card[j] = card_num[j];
						}
						_delay_ms(1000);
						Green_ON;
						_delay_ms(500);
						Green_Off;
						_delay_ms(500);
						Green_ON;
						_delay_ms(500);
						Green_Off;
						_delay_ms(500);
						Green_ON;
						_delay_ms(500);
						Green_Off;
						_delay_ms(500);
						return;
					}
				}
			}
			Red_Off;
			Green_Off;
			_delay_ms(250);
		}
	}
}

void master_menu(uchar *card_num, uchar *master_card){
	cli();
	Lock_Off;
	bool empty_memory = false;
	bool master_attached;
	_delay_ms(100);
	while(1){
		master_attached = master_operations(card_num, master_card, &empty_memory);
		if(empty_memory == true){
			check_for_master(card_num, master_card);
			empty_memory = false;
			break;
		}
		else if (master_attached == true){
			break;
		}
		else if (master_attached == false){
			bool access = read_card(card_num);
			if (access == true){ // card already in saved in memory
				Red_Off;
				Green_Off;
				_delay_ms(500);
				_delay_ms(100);
				Red_ON;
				_delay_ms(1500);
				Red_Off;
				_delay_ms(100);
			}
			else { // card isn't in saved, save in memory
				Red_Off;
				Green_Off;
				_delay_ms(500);
				save_card(card_num);
				_delay_ms(100);
				Green_ON;
				_delay_ms(1500);
				Green_Off;
				_delay_ms(100);
			}
		}
	}
	Lock_ON;
	sei();
}

void card_handle(uchar *card_num, uchar *master_card){
	bool access;
	for (int i = 0; i < 5; i++ ) {
		if (master_card[i]==card_num[i])
		{
			if(i == 4){
				access = true;
			}
		}
		else {
			access = false;
			break;
		}
	}
	if (access == true){
		master_menu(card_num, master_card);
	}
	else{						//if (access == false)
		access = read_card(card_num);
		if(access == true){
			open_lock();
		}
		else if (access == false){
			access_denied();
		}
	}
}

void scan_card(uchar *card_num, uchar *master_card){
	while(1) {
		if ( MFRC522_Request( PICC_REQIDL, card_num ) == MI_OK ) {
			if ( MFRC522_Anticoll( card_num ) == MI_OK ) {
				card_handle(card_num, master_card);
			}
		}
	}
}


//==========================< Main Programm >==================================

ISR(INT0_vect){
	cli();
	open_lock();
	sei();
}

int main()
{
	uchar master_card[5]; // master card array of numbers
	uchar card_num[5]; // scanned card
	DDRD = 0b00011011; // setting directions of Port D (input/output)
	PORTD = 0b00000000; // setting values on outputs and pulling resistors on inputs
	cli(); // interrupts are prohibited globally
	EICRA |= (0 << ISC01) | (0 << ISC00); // enabling INT0 interrupts on falling edge
	EIMSK |= (1 << INT0); // enabling external interrupts INT0
	MFRC522_Init(); // NFC/RFID scanner initializer
	check_for_master(card_num, master_card); // check for master card
	sei(); // interrupts are allowed
	Lock_ON; // setting voltage on the lock
	while(1){
		scan_card(card_num, master_card); // waiting for card
	}
}