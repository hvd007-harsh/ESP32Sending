/*
  WriteMultipleFields
  
  Description: Writes values to fields 1,2,3,4 and status in a single ThingSpeak update every 20 seconds.
  
  Hardware: ESP8266 based boards
  
  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!
  
  Note:
  - Requires ESP8266WiFi library and ESP8622 board add-on. See https://github.com/esp8266/Arduino for details.
  - Select the target hardware from the Tools->Board menu
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  
  For licensing information, see the accompanying license file.
  
  Copyright 2020, The MathWorks, Inc.
*/

#include <ESP8266WiFi.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include <Wire.h>
#include <MPU6050.h>


#define SECRET_SSID "taliban"
#define SECRET_PASS "vhom2328"
#define SECRET_CH_ID 1812653
#define SECRET_WRITE_APIKEY "CRHDLJ616PZE6PSM"
char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
MPU6050 mpu;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values

String myStatus = "Initiated";

void setup() {
  Serial.begin(115200);  // Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak


  while(!mpu.begin(MPU6050_SCALE_250DPS ,MPU6050_RANGE_2G))
  {
    Serial.println("Could not Find a valid MPU6050 sensor ,check wiring");
    delay(500);
  }
  mpu.setAccelOffsetX(0);
  mpu.setAccelOffsetY(0);
  mpu.setAccelOffsetZ(0);

  checkSetting();
}

void checkSetting()
{
   Serial.println();

     Serial.println("* Sleep Mode:       ");
     Serial.println(mpu.getSleepEnabled()? "Enabled":"Disable");
     Serial.print(" * Clock Source:          ");

   switch(mpu.getClockSource())
  {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }
      Serial.print(" * Accelerometer:         ");
  switch(mpu.getRange())
  {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }  

  Serial.print(" * Accelerometer offsets: ");
  Serial.print(mpu.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(mpu.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getAccelOffsetZ());
  
  Serial.println();
}
















void loop() {

// Getting reading and normal acceleration for The sensor 
 Vector rawAccel = mpu.readRawAccel();
 Vector normAccel = mpu.readNormalizeAccel();


  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // set the fields with the values
  ThingSpeak.setField(1, rawAccel.XAxis);
  ThingSpeak.setField(2, rawAccel.YAxis);
  ThingSpeak.setField(3, rawAccel.ZAxis);
  ThingSpeak.setField(4, normAccel.XAxis);
  ThingSpeak.setField(5, normAccel.YAxis);
  ThingSpeak.setField(6, normAccel.ZAxis);

  // figure out the status message
    myStatus = String("fields are updated");
  
  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }

  
  // change the values
 
  
  delay(1); // Wait 0.001 seconds to update the channel again
}
