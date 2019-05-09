/*
 * comm.cpp
 *
 *  Created on: Apr 6, 2019
 *      Author: György Kovács
 */


#include "Ch376msc.h"

#ifdef kALLOW_SERIAL
uint8_t Ch376msc::readSerDataUSB(){
	uint32_t oldMillis = millis();
		while (!_comPort->available()){ // wait until data is arrive
			if ((millis()- oldMillis) > TIMEOUT){
				return 0xFF; // Timeout valasz
			}//end if
		}//end while
	return _comPort->read();
}
#endif

void Ch376msc::write(uint8_t data){
#ifdef kALLOW_SERIAL
	if(_interface == UART){
		_comPort->write(data);
	} 
#endif
#ifdef kALLOW_SPI
	if( _interface == SPII ) { // SPI
		delayMicroseconds(3);//datasheet TSC min 1.5uSec
		SPI.transfer(data);
	}//end SPI
#endif
}

#ifdef kALLOW_SPI
uint8_t Ch376msc::spiReadData(){
	delayMicroseconds(3);//datasheet TSC min 1.5uSec
	return SPI.transfer(0x00);
}
#endif

void Ch376msc::print(const char str[]){
	uint8_t stringCounter = 0;
#ifdef kALLOW_UART
	if(_interface == UART){
		_comPort->print(str);
	}
#endif
#ifdef kALLOW_SPI
	if( _interface == SPII ) { // SPI
		while(str[stringCounter]){ ///while not NULL
			write(str[stringCounter]);
			stringCounter++;
		}
	}
#endif
}

#ifdef kALLOW_SPI
void Ch376msc::spiReady(){
	uint32_t msTimeout;
	delayMicroseconds(3);
	msTimeout = millis();
	while(digitalRead(_spiBusy)){
		if(millis()-msTimeout > TIMEOUT){
			break;
		}//end if
	}//end while
}

void Ch376msc::waitSpiInterrupt(){
	uint32_t oldMillis = millis();
	while(digitalRead(_intPin)){
		if ((millis()- oldMillis) > TIMEOUT){
			break;
		}//end if
	}//end while
}

uint8_t Ch376msc::getInterrupt(){
	uint8_t _tmpReturn = 0;
	waitSpiInterrupt();
		spiBeginTransfer();
		sendCommand(CMD_GET_STATUS);
		_tmpReturn = spiReadData();
		spiEndTransfer();
	return _tmpReturn;
}


void Ch376msc::spiBeginTransfer(){
	SPI.beginTransaction(SPISettings(SPICLKRATE, MSBFIRST, SPI_MODE0));
	digitalWrite(_spiChipSelect, LOW);
}

void Ch376msc::spiEndTransfer(){
	digitalWrite(_spiChipSelect, HIGH);
	SPI.endTransaction();
}
#endif