// Example to show the subdirectory support
// also uses only SPI interface

// This has been tested on a "Arduino SS Micro" wired as such:

#define kCH376_MISO   (14)
#define kCH376_SCK    (15)
#define kCH376_MOSI   (16)
#define kCH376_CS     (10)
#define kCH376_BUSY   (9)


// set up our flash drive
#include <Ch376msc.h>
Ch376msc flashDrive( kCH376_CS, kCH376_BUSY ); 

#define kBufSz  (100)
char buf[ kBufSz ];

void setup() {
// note: you can comment out all of the Serial1 junk if your 
// arduino doesn't have a second serial port.
  Serial1.begin( 115200 );  // "target" system to echo serial content with
  
  Serial.begin( 2000000 );  // "host" system with human terminal interface
  while( !Serial );         // necessary for 32u4/Leonardo

  // reset and prime the system
  flashDrive.init();

  // not necessary, but display connection info.
  if( !flashDrive.checkDrive() ) {
    Serial.println( "No connection" );
  } else {
    Serial.println( "Connected." );
  }
   
}

/* to delete a file... (not exampled here.)
        flashDrive.setFileName("TEST1.TXT");  //set the file name
        flashDrive.deleteFile();              //delete file
 */

// read the file from the current directory on the disk
void ReadFile( char * fn )
{
  bool okay = true;
  
  Border( "Read" );
  if( !flashDrive.checkDrive() ) {
    Serial.println( "No connection" );
    return;
  }
  
  flashDrive.setFileName( fn );
  flashDrive.openFile();

  while( okay ) {
    Serial.print( "." );
    buf[0] = '\0';
    okay = flashDrive.readFile( buf, kBufSz-1 );
    
    Serial.println( okay );
    Serial.println( buf );
  }
  flashDrive.closeFile();
}

// write some stuff to the end of the file specified.
void WriteFile( char * fn )
{
  char *str = "Hello!\n\r";
  Border( "Write" );
  if( !flashDrive.checkDrive() ) {
    // could be really aggressive with checkDrive()
    Serial.println( "No connection" );
    return;
  }
  
  flashDrive.setFileName( fn );
  flashDrive.openFile();
  flashDrive.seekEnd(); // append to end

  flashDrive.writeFile( str, strlen( str ) );
  flashDrive.writeFile( str, strlen( str ) );
  flashDrive.writeFile( str, strlen( str ) );
  
  flashDrive.closeFile();
}

// get a directory listing of the current directory.
void Dir( void )
{
  Border( "DIR" );
  if( !flashDrive.checkDrive() ) {
    Serial.println( "No connection" );
    return;
  }
  
  while( flashDrive.listDir() ){ // reading next file
    Serial.print(flashDrive.getFileName()); // get the actual file name
    Serial.print(" : ");
    Serial.print(flashDrive.getFileSize()); // get the actual file size in bytes
    Serial.print(" >>>\t");
    Serial.println(flashDrive.getFileSizeStr()); // get the actual file size in formatted string
  }
}

// utility for pretty output
void Border( char * str )
{
  Serial.println( "" );
  Serial.println( "------------" );
  Serial.println( str );
  Serial.println( "------------" );
}

// loop forever...
void loop() {
  unsigned long t;
  char ch;
 
  Serial.println( "working..." );

  // for the sake of this example, there is a directory
  // in the root of the disk named "ADIR"
  // in that directory, is another directory named "1"
  // in that directory is a file called "SUB1.TXT" 
  //
  //  /ADIR/
  //  /ADIR/1/
  //  /ADIR/1/SUB1.TXT

  // open the root directory
  flashDrive.setFileName( "/" );
  flashDrive.openFile();
    // and print the directory list of "/"
    Dir();

    // now, open a subdirectory relative to there.
    flashDrive.setFileName( "ADIR" );
    flashDrive.openFile();
      // now we should be in "/ADIR/"
      // print out the contents of the current directory
      Dir();
      
      flashDrive.setFileName( "1" );
      flashDrive.openFile();
        // now we should be in "/ADIR/1/"
        // print out the contents of the current directory
        Dir();
        // and dump out the contents of the file "/ADIR/1/SUB1.TXT"
        ReadFile( "SUB1.TXT" );
      flashDrive.closeFile(); /* close .../1/ */
      
    flashDrive.closeFile();   /* close .../ADIR/ */
    
  flashDrive.closeFile();     /* close / */
      
  //WriteFile( "/TEST99.TXT" );
  //ReadFile( "/TEST99.TXT" );

  
  // wait for 30 seconds
  t = millis() + 30000;

  // but while we're waiting, let's echo text to the
  // target on the second serial port.
  while( millis() < t ) {
    while( Serial.available() ) {
      ch = Serial.read();
      Serial1.write( ch );
    }
    while( Serial1.available() ) {
      ch = Serial1.read();
      Serial.write( ch );
    }
    delay( 20 );
  }
}
