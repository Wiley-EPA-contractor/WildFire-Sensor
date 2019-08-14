 //#include <Adafruit_GPS.h>



// This code is used for the Kolibri onboard Teensy 3.2
// The main purpose is to log and transfer PDR and GPS sensor data

// ***************
// PIN Assignmets:
// ***************

// SD Card (SPI)
// pin 6 - CS
// pin 11 - MOSI (DOUT)
// pin 12 - MISO (DIN)
// pin 13 - SCK (CLK)

//Serial Ports
// PDR
// pin 10(TX1) - RX
// pin 9(RX1) - TX
// GPS
// pin 8(TX2) - RX
// pin 7(RX2) - TX
// USB
// pin 1(TX3) - RX
// pin 0(RX3) - TX

// Monitor battery voltage
// pin A8(22) - voltpin

// PDR on/off
// pin 2

// **********************
// End of pin assignments
// **********************


// Libraries ---------------------------------------------------------------------------------
#include <SD.h>                 // SD card library
#include <Wire.h>               // IC2 library
#include <TimeLib.h>            // Used for rtc
#include <SPI.h>                // Used for rtc
#include <Adafruit_GPS.h>       // GPS library
// A new GPS library: https://github.com/rvnash/ultimate_gps_teensy3


#define PDRSerial Serial1
#define GPSSerial Serial2
#define VDIPSerial Serial3
//#define XbeeSerial Serial3
File myfile;
char filename[] = "";

Adafruit_GPS GPS(&GPSSerial);   // GPS library

int first_time = 0;
// Declare pins ----------------------------------------------------------------------------------------
const int SDPin = 6;        // CS pin for SD card
const int PDRPin = 2;       // PDR on/off
const int VoltPin = A8;     // Monitor package voltage
String inByte = "";

// Define variables -----------------------------------------------------------------------------------

int oldsec;
int newsec;
int Volt = 0.0;                  // Package voltage (Volt)
int Altitude = 0;              // Altitude (m)
String pinByte = "";
String PDRByte = "";
String payload = "";
int noOfChars;
int first_run= 0;
String NMEA1; //Variable for first NMEA sentence
String NMEA2; //Variable for second NMEA sentence
char c; //to read characters coming from the GPS
int GPS_RESET = 0;

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void setup() {
  
  Wire.begin();
  setSyncProvider(getTeensy3Time); // Call Teensy RTC subroutine
  analogReadRes(13); // Set Teensy analog read resolution to 13 bits
  
  // Open serial communications and wait for port to open:  --------------------------------------
  Serial.begin(9600);   // computer serial I/O port
  //XbeeSerial.begin(38400);  // Xbee
  GPSSerial.begin(9600);  // GPS
  PDRSerial.begin(19400);  // PDR
  VDIPSerial.begin(9600);  // usb
  
  //setSyncProvider(getTeensy3Time); // Call Teensy RTC subroutine
  
  pinMode(VoltPin, INPUT);        // Package voltage
  pinMode(PDRPin, OUTPUT);        //PDR on/off
  digitalWrite(PDRPin, HIGH);    //Turn off PDR on pulse
  delay(1000);  
  digitalWrite(PDRPin, LOW);     //PDR on pulse
  delay(4500);
  digitalWrite(PDRPin, HIGH);    //Turn off PDR on pulse
    
  SPI.begin();

  VDIPSerial.print("IPA");        //usb stick
  VDIPSerial.print(13, BYTE);     //usb stick
  
  
   // GPS start ------------------------------------------------------------------------------------
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rat
   
  delay(1000); 
    
 //init SD card ---------------------------------------------------------------------------------
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(SDPin, OUTPUT);
  // see if the card is present and can be initialized:
  if (!SD.begin()) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  //end SD card init

  oldsec = second();
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void loop() {
  if (GPS_RESET > 3600){
    char t;
      if (GPSSerial.available()) 
   { 
     while(GPSSerial.available())
     {
          t= GPSSerial.read(); 
     }
     GPSSerial.println(t);
     GPSSerial.end();    // Ends the serial communication once all data is received
     GPSSerial.begin(9600);
     GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rat
   
  delay(1000);                              //or cache
  }
  GPS_RESET = 0;
  }
  
   // GPS time, coordinate, and altitude
    while(GPSSerial.available() > 0) { //Loop until you have a good NMEA sentence
    c=GPS.read();
    }   
    GPS.parse(GPS.lastNMEA()); //Parse that last good NMEA sentence
    NMEA1=GPS.lastNMEA();  //this should be cleared by this statement. 
    // other idea is that lastNMEA is getting full.
  
  // Wait for command from base station
  int i = 0;
  //Address = 0;
  //Code = 0;
  inByte = "";
 
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    inByte += inChar;
    i = 1;   
  } 
Serial.print(inByte);    
   if (i == 1 && inByte.startsWith("T")) {               //if first chars = T set clock
    inByte = inByte.substring(1);
    Serial.println(inByte); 
    time_t t = processSyncMessage(); //call processSyncMessage subroutine & return "t"
    
     if (t != 0) {
      Teensy3Clock.set(t); // set the RTC
      setTime(t);
      Serial.print("CLOCK SET TO: ");
      Serial.print(month());
      Serial.print("/");
      Serial.print(day());
      Serial.print("/");
      Serial.print(year());
      Serial.print(",");
      Serial.print(hour());
      Serial.print(":");
      Serial.print(minute());
      Serial.print(":");
      Serial.println(second());
      Serial.println();
     }
     //Serial.println(">AST1");  // send ack
     i = 0;
     inByte = "";
    }

  if (i == 1 && inByte.startsWith("D")) {   //Print Directory 
        File root = SD.open("/");
    printFiles(root,0);
  i = 0;
  inByte = "";      
  }

  if (i == 1 && inByte.startsWith("R")) {   //stream file to serial port
  inByte = inByte.substring(1); 
  inByte.toCharArray(filename,13); 
  Serial.println(filename); 
  
  myfile = SD.open(filename);
  while (myfile.available()){
    Serial.write(myfile.read());
  }
  myfile.close();
  
  i = 0;
  inByte = "";   
  }

    delay(50);
  
  if (oldsec - second() != 0) 
  {

      
    // PDR read requset
    if (first_time > 20){
    pdr1500 ();
    //read DPR end


    // Voltage read  
     Volt = analogRead(VoltPin) * 0.024306;       

   // Serial.println(Volt);
    
    // Altitude
    Altitude = GPS.altitude;
                       
    oldsec = second(); // restart timer


    // Datalogging
    logger();
    }
    else{
     delay(1000);
    first_time++;
    }
  }
  GPS_RESET++;
}

void pdr1500()
{
 PDRSerial.println("O");  // shows that output is beginning
// if(first_run < 10){ // let bad data clear out, run for 10 samples and then start collecting data points.
// delay (1000);
// first_run++;
 
// }
//else{
 delay(100);
 boolean data_is_valid = false;  // checking if valid coming through is correct  or not
 boolean data_received = false;  // checking for first integer variable.
 // 
 int valid_data_length = 0;
 // if data is valid, check to make sure we don't see early cutoff of data.
 int data_pointer = 0;
  while(PDRSerial.available() > 0) 
 {
  data_pointer++;
  char PDRChar = PDRSerial.read();
  int x = (int)PDRChar; //x is the ascii int code of the character PDRChar
  if (data_pointer > 8){
  if (PDRChar == '\r' || x == 13  || x == 10){ // for some reason, not all return characters are being recognized.
    if (!data_is_valid){  
    break;
    }  
    else{
    if (valid_data_length < 14){
      valid_data_length = 0;
      PDRByte ="";
      break;
    }
    else 
    valid_data_length = 0;
    break;
      }
  }
  else if (x == 110 ){ // n/a message is received, print "1" message
    PDRChar = '1';
    PDRByte = "1";
    break;
  }
  else if (x == 32 && data_received) // if we receive a space after data, make it a comma
  {
    PDRChar = ',';
     PDRByte += PDRChar;
     valid_data_length++;
  }
  else if (x == 32 && !data_received){ // if we receive a space before data, do nothing to it
    PDRByte += PDRChar;
      valid_data_length++;

  }
  else if ( x == 46  || (x>=48 && x<= 57)){ //checks for "normal" characters when the Character is loaded in
  //Normal Chars:  46 == period, 48<=x<=57 == 0-9
  //N/a points become ones
  data_is_valid = true;
  data_received = true;
  valid_data_length++;
  
  PDRByte += PDRChar;
  //  Serial.print(PDRChar);
  }
  else {
    //Serial.print("error: ");
    //Serial.println((int)PDRChar);
    //continue;
  }
 }
 }
 //PDRByte = PDRByte.substring(7); // Basically read in OUTPUT and then after 7 is your data that you actually record 
// }
}


// logging data
void logger()
{
  //Begin Write data to file *************************************
  // open file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
    char filename[] = "00000000.CSV"; //default file name
    getFilename(filename); //daily file name
    Serial.println("Logger called");
    Serial.println(PDRByte);
    //Serial.println(filename);
    payload = ""; payload += month();
    payload += "/";
    payload += day();
    payload += "/";
    payload += year(); 
    payload += ",";   
    payload += hour(); 
    payload += ":"; 
    payload += minute();
    payload += ":";
    payload += second();
    payload += ",";
    payload += GPS.fix;
    payload += ",";
    payload += GPS.hour;
    payload += ":";
    payload += GPS.minute;
    payload += ":";
    payload += GPS.seconds;
    payload += ",";
    payload += GPS.latitude;
    payload += ",";
    payload += GPS.longitude;
    payload += ",";
    payload += GPS.altitude;
    payload += ",";
    payload += PDRByte;
    noOfChars = payload.length() + 2;
    
   // Serial.println(payload);
   if(filename){
    VDIPSerial.print("OPW ");
    VDIPSerial.print(filename);    
    VDIPSerial.print(13, BYTE);
    VDIPSerial.print("WRF ");
    VDIPSerial.print(noOfChars);
    VDIPSerial.print(13, BYTE);
    VDIPSerial.println(payload);
    VDIPSerial.print(13, BYTE);
    VDIPSerial.print("CLF ");
    VDIPSerial.print(filename);
    VDIPSerial.print(13, BYTE);
   }
   else{
    Serial.println("error opening USB file");
    PDRByte = "";
   }
   
   
 //  Serial.println(filename);
 //  if(filename){
 //    VDIPSerial.print("CLF ");
 //   VDIPSerial.print(filename);
 //  VDIPSerial.println();
 //   VDIPSerial.print("OPW ");
 //   VDIPSerial.print(filename);    
 //  VDIPSerial.println();
 //   VDIPSerial.print("WRF ");
 //   VDIPSerial.print(noOfChars);
  // VDIPSerial.println();
  //  VDIPSerial.println(payload);
  //  //VDIPSerial.print('\n');
  //  VDIPSerial.print("CLF ");
 //   VDIPSerial.print(filename);
 //  VDIPSerial.println();
 //  }
  // else{
  //   Serial.println("error opening USB file");
  //  PDRByte = "";
  //  }
   // while (VDIPSerial.available()>0){
    //    Serial.print("Test: ");
    //    Serial.println(VDIPSerial.read());
    //}
   
    
    payload = "";
    
    File dataFile = SD.open(filename, FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {

    dataFile.print(month());
    dataFile.print("/");
    dataFile.print(day());
    dataFile.print("/");
    dataFile.print(year());
    dataFile.print(",");
    dataFile.print(hour());
    dataFile.print(":");
    dataFile.print(minute());
    dataFile.print(":");
    dataFile.print(second());
    dataFile.print(",");
    
    // GPS time, coordinate, and altitude
      dataFile.print(GPS.fix); dataFile.print(",");
      dataFile.print(GPS.hour, DEC); dataFile.print(':');
      dataFile.print(GPS.minute, DEC); dataFile.print(':');
      dataFile.print(GPS.seconds, DEC); dataFile.print(",");
      dataFile.print(GPS.latitude,6); //dataFile.print(GPS.lat);
      dataFile.print(",");
      dataFile.print(GPS.longitude,6); //dataFile.println(GPS.lon);
      dataFile.print(",");
      dataFile.print(GPS.altitude);
      dataFile.print(",");
      
    // PDR data:
    dataFile.println(PDRByte);
    dataFile.close(); //close file  
    PDRByte = "";
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog file");
    PDRByte = "";
  }
  NMEA1 ="";
}

//RTC subroutine
time_t getTeensy3Time() 
{
  return Teensy3Clock.get();
}
/*  code to process time messages from the serial port   */
//#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 

  
     pctime = inByte.toInt();  
 
     return pctime;
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = 0L; // return 0 to indicate that the time is not valid
     }
    return pctime; 
  }
  
    //file date / name generator:
void getFilename(char *filename) {
    filename[0] = '2';
    filename[1] = '0';
    filename[2] = int(year()/10)%10 + '0';
    filename[3] = year()%10 + '0';
    filename[4] = month()/10 + '0';
    filename[5] = month()%10 + '0';
    filename[6] = day()/10 + '0';
    filename[7] = day()%10 + '0';
    filename[8] = '.';
    filename[9] = 'C';
    filename[10] = 'S';
    filename[11] = 'V';
    
}

void printFiles(File dir, int numTabs)
{
  while (true)
  {
    File entry =  dir.openNextFile();
    if (! entry)
    {
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++)
    {
      Serial.print('\t');
    }
    Serial.println(entry.name());
    entry.close();
  }
}
