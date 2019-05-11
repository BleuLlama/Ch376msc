/*
 * Ch376msc.h
 *
 *  Created on: Feb 25, 2019
 *      Author: György Kovács
 *		Contributor: Scott Lawrence - yorgle@gmail.com
 *
 *
 *  v1.2.1.S1	May 11, 2019	SL
 *		Changes to minimize footprint: 
 *			added #ifdefs to enable/disable functionality
 *			mainly, to not use SPI and UART at the same time
 *		various text formatting changes
 *
 ******************************************************
 *  v1.2 Apr 24, 2019
 *  	-fixing timing issue on higher SPI clock
 *  	 datasheet 7.3 Time Sequence table (TSC)
 ******************************************************
 *  v1.2 Apr 20, 2019
 *  	-extended with SPI communication
 ******************************************************
 *	v1.1 Feb 25, 2019
 *		-initial version with UART communication
 ******************************************************
 */

#ifndef Ch376msc_H_
#define Ch376msc_H_

#include <Arduino.h>
#include "CommDef.h"
#include <Stream.h>
#include <SPI.h>


// Usually, you'd either want one or the other, and including both is generally
// a waste. Especially for a thing like the CH376 where the major advantage is 
// super-low overhead since there's no need for the FAT libraries, etc.
//
// barebones sketch, on Arduino Leonardo
// Both DEFINEd, using SPI
//		Sketch uses 6634 bytes (23%) of program storage space. Maximum is 28672 bytes.
// Both DEFINEd, using hardware Serial
//		Sketch uses 7388 bytes (25%) of program storage space. Maximum is 28672 bytes.
//
// SPI only
//		Sketch uses 5610 bytes (19%) of program storage space. Maximum is 28672 bytes.
//
// Serial only
//  	Sketch uses 6298 bytes (21%) of program storage space. Maximum is 28672 bytes.
// 
//  it's not much, but 1kbyte is a nice savings when you're pressed for space...

// use only the SERIAL interface
#undef kUSE_SERIAL

// use only the SPI interface
#define kUSE_SPI

// but only one!
#ifdef kUSE_SERIAL
#ifdef kUSE_SPI
	*** ERROR.  Cannot use both!
#endif
#endif

// allow all of the time/date functionality
#define kALLOW_TIMEDATE

// allow misc utilities (timedate string etc.)
#define kALLOW_UTILITIES

#define TIMEOUT 2000 // waiting for data from CH
#define SPICLKRATE 125000 //Clock rate 125kHz
						// max 8000000 (8MHz)on UNO, Mega (16 000 000 / 2 = 8 000 000)

class Ch376msc {
public:
	/////////////Constructors////////////////////////
#ifdef kUSE_SERIAL
	Ch376msc(HardwareSerial &usb, uint32_t speed);//HW serial
	Ch376msc(Stream &sUsb);// SW serial
#endif
#ifdef kUSE_SPI
	Ch376msc(uint8_t spiSelect, uint8_t busy);//SPI with MISO as Interrupt pin
	Ch376msc(uint8_t spiSelect, uint8_t busy, uint8_t intPin);
#endif
	virtual ~Ch376msc();//destructor
	////////////////////////////////////////////////
	void init();

	uint8_t mount();
	uint8_t dirInfoSave();
	uint8_t openFile();
	uint8_t closeFile();
	uint8_t moveCursor(uint32_t position);
	uint8_t deleteFile();
	uint8_t pingDevice();
	uint8_t listDir();
	uint8_t readFile(char* buffer, uint8_t b_num);
	uint8_t writeFile(char* buffer, uint8_t b_num);
	bool checkDrive(); // check is it any interrupt message came from CH
	//bool listDir(char (*nameBuff)[12],uint32_t *sizeBuff,uint8_t listElements); //376_7
	//void reset();

//set/get

	//uint32_t getComSpeed();
	uint32_t getFileSize();
	uint8_t getStatus();
	char* getFileName();

	bool getDeviceStatus(); // usb device mounted, unmounted
	bool getCHpresence();
	void setFileName(const char* filename);

#ifdef kALLOW_TIMEDATE
	uint16_t getYear();
	uint16_t getMonth();
	uint16_t getDay();
	uint16_t getHour();
	uint16_t getMinute();
	uint16_t getSecond();
	void setYear(uint16_t year);
	void setMonth(uint16_t month);
	void setDay(uint16_t day);
	void setHour(uint16_t hour);
	void setMinute(uint16_t minute);
	void setSecond(uint16_t second);
#endif

#ifdef kALLOW_UTILITIES
	char* getFileSizeStr();
#endif

private:
	//uint8_t read();
	void write(uint8_t data);
	void print(const char str[]);

	uint8_t getInterrupt();
	uint8_t fileEnumGo();
	uint8_t byteRdGo();
	uint8_t fileCreate();
	uint8_t byteWrGo();
	uint8_t reqByteRead(uint8_t a);
	uint8_t reqByteWrite(uint8_t a);
	uint8_t writeDataFromBuff(char* buffer);
	uint8_t readDataToBuff(char* buffer);
	uint8_t dirInfoRead();
	uint8_t setMode(uint8_t mode);

	void rdUsbData();
	void sendCommand(uint8_t b_parancs);
	void sendFilename();
	void writeFatData();

#ifdef kUSE_SPI
	void spiReady();
	void spiBeginTransfer();
	void spiEndTransfer();
	void waitSpiInterrupt();
	uint8_t spiReadData();
#endif

#ifdef kUSE_SERIAL
	void setSpeed();
	uint8_t readSerDataUSB();
#endif

#ifdef kALLOW_TIMEDATE
	void constructDate(uint16_t value, uint8_t ymd);
	void constructTime(uint16_t value, uint8_t hms);
#endif

//#define TEST
#ifdef TEST

	void fileReOpen(); // if cluster is full
	void wrDummyByte(uint8_t asd);
#endif
	///////Global Variables///////////////////////////////
	bool _fileWrite = false; // read or write mode, needed for close operation
	bool _mediaStatus;				//false USB levalsztva, true csatlakoztatva
	bool _controllerReady = false; // ha sikeres a kommunikacio

	uint8_t _byteCounter = 0; //vital variable for proper reading,writing
	uint8_t _answer = 0;	//a CH jelenlegi statusza INTERRUPT
	uint8_t _spiChipSelect; // chip select pin SPI
	uint8_t _spiBusy; //   busy pin SPI
	uint8_t _intPin; // interrupt pin
	uint16_t _sectorCounter = 0;// variable for proper reading
	uint32_t _speed; // Serial communication speed
	char _filename[12];

	HardwareSerial* _comPortHW; // Serial interface
	Stream* _comPort;

	fileProcessENUM fileProcesSTM = REQUEST;

	fatFileInfo _fileData;
};//end class

#endif /* Ch376msc_H_ */


