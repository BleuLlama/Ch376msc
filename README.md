# Project Title

Arduino library for CH376 Mass Storage Contoller, supporting SPI and UART 
communication interfaces.

## Getting Started
Configure jumpers on the module

![Alt text](pic/JumperSelect.png?raw=true "Setting")

NOTES:

In order to "share" the SPI bus, be sure to hook up the INT pin to your 
CH376, and also specify the pin at initialization time.


## Versioning

v1.2.1.001	May 11, 2019 - Changes to split out the SPI and UART to separate 
build settings.  Since having one or the other in there is dormant overhead, 
as you'd probably never use both in the same project.  

Change the #define / #undef in the Ch376msc.h file according to your usage needs.  This saves between 1 and 1.5 kbytes of program space.



v1.2.1     Apr 24, 2019 - In use of SPI, CS pin on the module must to be pulled to VCC otherwise communication can be instable on a higher clock rate
- Fixed timing issue on a higher clock rate (TSC)
                  
v1.2 Apr 20, 2019 -extended with SPI communication

v1.1 Feb 25, 2019 -initial version with UART communication

### Acknowledgments

Thanks for the idea to Scott C ,  https://arduinobasics.blogspot.com/2015/05/ch376s-usb-readwrite-module.html

Additions made May 2019 by Scott Lawrence to decrease memory footprint.
