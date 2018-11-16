#include <Adafruit_TCS34725.h>

#include <SoftwareSerial.h>
#include <Wire.h> //Need to recompile this so that it uses fast I2C instead of normal I2C
#include "Adafruit_TCS34725.h" //This is adafruit TCS sensor library, download from adafruit website


#define ledPin 4 //Pin that reads LED state
#define interruptPin 2 //Pin that reads clock signal from MSP
#define ESP_TX 5 //Pin to connect to esp tx
#define ESP_RX 6 //Pin to connect to esp rxl

#define ID 11 //ID of sensor/LED module (0-11)
#define BAUD 57600  //max for arduino uno software serial
//Integration time of sensor
//Options are:
//  TCS34725_INTEGRATIONTIME_2_4MS 
//  TCS34725_INTEGRATIONTIME_24MS 
//  TCS34725_INTEGRATIONTIME_50MS 
//  TCS34725_INTEGRATIONTIME_101MS 
//  TCS34725_INTEGRATIONTIME_154MS 
//  TCS34725_INTEGRATIONTIME_700MS 
#define SENSOR_INTEGRATION_TIME TCS34725_INTEGRATIONTIME_101MS
//Gain of sensor
//Options are:
//  TCS34725_GAIN_1X
//  TCS34725_GAIN_4X
//  TCS34725_GAIN_16X
//  TCS34725_GAIN_60X
#define SENSOR_GAIN TCS34725_GAIN_60X
#define DEBUG 0 //Used for debugging when arduino is connected to laptop running arduio IDE
//Set DEGUB to 1 and arduino serial monitor to use baud rate BAUD with no line terminators to see sensior values and esp responses
#define digitalPinToInterrupt(p)  ((p) == 2 ? 0 : ((p) == 3 ? 1 : -1))
volatile int doRead = 0;
unsigned long int num_clocks = 0;
Adafruit_TCS34725 tcs;

union Data{
  byte packet_size[14]; 
  struct Info {
    byte id;
    uint32_t timestamp;
    char LED_STATE;
    uint16_t R;
    uint16_t G;
    uint16_t B;
    uint16_t C;
  } 
  info;
} 
data;

//SoftwareSerial esp8266(ESP_TX, ESP_RX); // make RX Arduino line is pin 5, make TX Arduino line is pin 6.
// This means that you need to connect the TX line from the esp to pin 5
// and the RX line from the esp to the Arduino's pin 6


void setup() {
  Serial.begin(BAUD);

  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(ledPin, INPUT_PULLUP);        // sets the digital pin 4 as input
  attachInterrupt(digitalPinToInterrupt(interruptPin), read_sensor, RISING);

  //Configure sensor with integration time and gain
  tcs = Adafruit_TCS34725(SENSOR_INTEGRATION_TIME, SENSOR_GAIN);

  if(tcs.begin()) {
    Serial.println("Found Sensor");
  } 
  else {
    Serial.println("No Sensor Found");
  }
  doRead = 0;

  tcs.setInterrupt(true);

  //esp8266.println("AT+CIPSTART=\"UDP\",\"192.168.1.2\",9100"); //IP address of pi
  delay(1);

}


void loop() {
  if (doRead) {

    //Get sensor data
    //If SDA or SCL is disconnected, values will all be 0b1111111111111111
    //If power or ground disconencts, arduino will hang
    tcs.getRawData(&data.info.R, &data.info.G, &data.info.B, &data.info.C); //Time to read = 2.4 ms + time to send I2C data, about 3.34 ms

    //Get LED state
    data.info.LED_STATE = digitalRead(ledPin); //Read takes negligible amount of time, can't be detected on microsecond resolution

      //Get timestamp
    data.info.timestamp = num_clocks;
    //Set ID
    data.info.id = ID;
    
    
    
    Serial.print("ID: "); 
    Serial.print(data.info.id, DEC); 
    Serial.print(" ");
    Serial.print("Time: "); 
    Serial.print(data.info.timestamp, DEC); 
    Serial.print(" ");
    Serial.print("LED: "); 
    Serial.print(data.info.LED_STATE, DEC); 
    Serial.print(" ");
    Serial.print("R: "); 
    Serial.print(data.info.R, DEC); 
    Serial.print(" ");
    Serial.print("G: "); 
    Serial.print(data.info.G, DEC); 
    Serial.print(" ");
    Serial.print("B: "); 
    Serial.print(data.info.B, DEC); 
    Serial.print(" ");
    Serial.print("C: "); 
    Serial.print(data.info.C, DEC); 
    Serial.print(" ");
    Serial.println("  ");

    //Send data to ESP
    //esp8266.println("AT+CIPSEND=14");
    delayMicroseconds(780); //Time to send data over UART at BAUD and wait for esp response
    //esp8266.write(data.packet_size, 14);
    delayMicroseconds(280); //Time to send data over UART at BAUD


    num_clocks++;
    //Don't perform read op again until next clock signal detected
    doRead = 0;
  }
}


void read_sensor() {
  //Clock signal detected
  //Time to service interrupt about 15 us
  doRead = 1;
}

