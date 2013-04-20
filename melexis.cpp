/*
	MELEXIS.cpp - MELEXIS library

	For interfacing with the MLX90363 triaxis hall effect sensor
	
	by Travis Howse <tjhowse@gmail.com>
	2012.   License, GPL v2 or later

*/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "MELEXIS.h"
#import <Arduino.h> 
#include <SPI.h>
/******************************************************************************
 * Definitions
 ******************************************************************************/
 

#define MELIXIS_GET1 0x13
#define MELIXIS_GET2 0x14
#define MELIXIS_GET3 0x15
#define MELIXIS_MemoryRead 0x01
#define MELIXIS_EEPROMWrite 0x03
#define MELIXIS_EEChallengeAns 0x05
#define MELIXIS_EEReadChallenge 0x0F
#define MELIXIS_NOP 0x10
#define MELIXIS_DiagnosticDetails 0x16
#define MELIXIS_OscCounterStart 0x18
#define MELIXIS_OscCounterStop 0x1A
#define MELIXIS_Reboot 0x2F
#define MELIXIS_Standby 0x31

#define MELIXIS_Get3Ready 0x2D
#define MELIXIS_MemoryReadAnswer 0x02
#define MELIXIS_EEPROMWriteChallenge 0x04
#define MELIXIS_EEReadAnswer 0x28
#define MELIXIS_EEPROMWriteStatus 0x0E
#define MELIXIS_Challenge 0x11
#define MELIXIS_DiagnosticsAnswer 0x17
#define MELIXIS_OscCounterStart 0x09
#define MELIXIS_OscCounterStopAck 0x1B
#define MELIXIS_StandbyAck 0x32
#define MELIXIS_Errorframe 0x3D
#define MELIXIS_NTT 0x3Ea
#define MELIXIS_ReadyMessage 0x2C
 
const char cba_256_TAB [] = {0x00, 0x2f, 0x5e, 0x71, 0xbc, 0x93, 0xe2, 0xcd,
					0x57, 0x78, 0x09, 0x26, 0xeb, 0xc4, 0xb5, 0x9a,
					0xae, 0x81, 0xf0, 0xdf, 0x12, 0x3d, 0x4c, 0x63,
					0xf9, 0xd6, 0xa7, 0x88, 0x45, 0x6a, 0x1b, 0x34,
					0x73, 0x5c, 0x2d, 0x02, 0xcf, 0xe0, 0x91, 0xbe,
					0x24, 0x0b, 0x7a, 0x55, 0x98, 0xb7, 0xc6, 0xe9,
					0xdd, 0xf2, 0x83, 0xac, 0x61, 0x4e, 0x3f, 0x10,
					0x8a, 0xa5, 0xd4, 0xfb, 0x36, 0x19, 0x68, 0x47,
					0xe6, 0xc9, 0xb8, 0x97, 0x5a, 0x75, 0x04, 0x2b,
					0xb1, 0x9e, 0xef, 0xc0, 0x0d, 0x22, 0x53, 0x7c,
					0x48, 0x67, 0x16, 0x39, 0xf4, 0xdb, 0xaa, 0x85,
					0x1f, 0x30, 0x41, 0x6e, 0xa3, 0x8c, 0xfd, 0xd2,
					0x95, 0xba, 0xcb, 0xe4, 0x29, 0x06, 0x77, 0x58,
					0xc2, 0xed, 0x9c, 0xb3, 0x7e, 0x51, 0x20, 0x0f,
					0x3b, 0x14, 0x65, 0x4a, 0x87, 0xa8, 0xd9, 0xf6,
					0x6c, 0x43, 0x32, 0x1d, 0xd0, 0xff, 0x8e, 0xa1,
					0xe3, 0xcc, 0xbd, 0x92, 0x5f, 0x70, 0x01, 0x2e,
					0xb4, 0x9b, 0xea, 0xc5, 0x08, 0x27, 0x56, 0x79,
					0x4d, 0x62, 0x13, 0x3c, 0xf1, 0xde, 0xaf, 0x80,
					0x1a, 0x35, 0x44, 0x6b, 0xa6, 0x89, 0xf8, 0xd7,
					0x90, 0xbf, 0xce, 0xe1, 0x2c, 0x03, 0x72, 0x5d,
					0xc7, 0xe8, 0x99, 0xb6, 0x7b, 0x54, 0x25, 0x0a,
					0x3e, 0x11, 0x60, 0x4f, 0x82, 0xad, 0xdc, 0xf3,
					0x69, 0x46, 0x37, 0x18, 0xd5, 0xfa, 0x8b, 0xa4,
					0x05, 0x2a, 0x5b, 0x74, 0xb9, 0x96, 0xe7, 0xc8,
					0x52, 0x7d, 0x0c, 0x23, 0xee, 0xc1, 0xb0, 0x9f,
					0xab, 0x84, 0xf5, 0xda, 0x17, 0x38, 0x49, 0x66,
					0xfc, 0xd3, 0xa2, 0x8d, 0x40, 0x6f, 0x1e, 0x31,
					0x76, 0x59, 0x28, 0x07, 0xca, 0xe5, 0x94, 0xbb,
					0x21, 0x0e, 0x7f, 0x50, 0x9d, 0xb2, 0xc3, 0xec,
					0xd8, 0xf7, 0x86, 0xa9, 0x64, 0x4b, 0x3a, 0x15,
					0x8f, 0xa0, 0xd1, 0xfe, 0x33, 0x1c, 0x6d, 0x42};
					
const uint8_t slaveSelectPin = 20;
uint8_t i,j;
uint8_t crc;
					
uint8_t outbuffer[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t inbuffer[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/******************************************************************************
 * Constructors
 ******************************************************************************/
MELEXIS::MELEXIS()
{	
	pinMode (slaveSelectPin, OUTPUT);
	digitalWrite(slaveSelectPin,HIGH); 
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setDataMode(SPI_MODE1);
}
/******************************************************************************
 * User API
 ******************************************************************************/

uint16_t MELEXIS::get_x()
{
	return inbuffer[0] | ((inbuffer[1]&0x3F)<<8);
}
uint16_t MELEXIS::get_y()
{
	return inbuffer[2] | (inbuffer[3]&0x3F)<<8;
}

uint16_t MELEXIS::get_z()
{
	return inbuffer[4] | (inbuffer[5]&0x3F)<<8;
}

uint8_t MELEXIS::get_diag()
{
	return (inbuffer[1]&0xC0)>>6;
}

uint8_t MELEXIS::get_roll()
{
	return (inbuffer[6] & 0x3F);
}

uint16_t MELEXIS::get_diag_0()
{
	return inbuffer[0] | inbuffer[1]<<8;
}

uint16_t MELEXIS::get_diag_1()
{
	return inbuffer[2] | inbuffer[3]<<8;
}

uint8_t MELEXIS::poll()
{
	for (i=0;i<8;i++)
		outbuffer[i] = 0x00;

	outbuffer[1] = 0x01;
	outbuffer[2] = 0xFF;
	outbuffer[3] = 0xFF;
	outbuffer[6] = 0xC0 | MELIXIS_GET3;
	
	return do_SPI();
}

uint8_t MELEXIS::diag_poll()
{
	for (i=0;i<8;i++)
		outbuffer[i] = 0;

	outbuffer[2] = 0xFF;
	outbuffer[3] = 0xFF;
	outbuffer[6] = 0xC0 | MELIXIS_DiagnosticDetails;
	
	return do_SPI();
}


uint8_t MELEXIS::do_SPI()
{
	do_checksum(outbuffer);
	digitalWrite(slaveSelectPin,LOW);
	for (i=0; i<8; i++)
		inbuffer[i] = SPI.transfer(outbuffer[i]);
	digitalWrite(slaveSelectPin,HIGH); 
	return do_checksum(inbuffer);
}

bool MELEXIS::do_checksum(uint8_t* message)
{
	// Sets the last byte of the message to the CRC-8 of the first seven bytes.
	// Also checks existing checksum, returns 0 if OK, 1 if fail.
	crc = message[7];
	message[7] = 0xFF; 
	for (j=0; j<7; j++)
		message[7] = cba_256_TAB[ message[j] ^ message[7] ];
	message[7] = ~message[7]; 
	
	return !(message[7]==crc);
	
}
