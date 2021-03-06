#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

DHT dht;

int sensor_pin = D2;
int sensor = A0;

// lowest and highest sensor readings:
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum

int output_value ;
 
/* Set these to your desired credentials. */
const char *ssid = "Redmi";  //ENTER YOUR WIFI SETTINGS
const char *password = "rohitmahesh";
 
//Web/Server address to read/write from 
const char *host = "localhost";   
 
//=======================================================================
//                    Power on setup
//=======================================================================
 
void setup() {
  Serial.begin(115200);
  dht.setup(5);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
 
  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  pinMode(sensor,INPUT);
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}
 
//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
  delay(dht.getMinimumSamplingPeriod());
  int temperature = dht.getTemperature();
  int temptofah = (temperature * 1.8) + 32 ;

  output_value= digitalRead(sensor_pin);
  output_value = map(output_value,550,0,0,100);
  
  HTTPClient http;    //Declare object of class HTTPClient
 
  String ADCData, station, postData;
  int adcvalue= temptofah; //Read Analog value of LDR
  ADCData = String(adcvalue);   //String to interger conversion
  station = "Temperature";
  
  Serial.println((adcvalue ));

 //Temperature Sensor
  //Post Data
  postData = "status=" + ADCData + "&station=" + station ;
  
  http.begin("http://192.168.43.178/sql_arduino/temperature.php");              //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
 
  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload
 
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
 
  http.end();  //Close connection

  delay(1000);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////Soil Moisture Sensor
  String station1="Soil Moisture";
  String output_value_soil_moisture = String(output_value);
  String postData1 = "status=" + output_value_soil_moisture + "&station=" + station1 ;
  http.begin("http://192.168.43.178/sql_arduino/soilmoisture.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
 
  int httpCode2 = http.POST(postData1);   //Send the request
  String payload2 = http.getString();    //Get the response payload
 
  Serial.println(httpCode2);   //Print HTTP return code
  Serial.println(payload2);    //Print request response payload
  http.end();

  delay(1000);  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////Water Drop
  String station2="Water Drop";
  
  int sensorReading = digitalRead(D4);
  // map the sensor range (four options):
  // ex: 'long int map(long int, long int, long int, long int, long int)'
  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);
  String weather;
  // range value:
  switch (range) {
 case 0:    // Sensor getting wet
    Serial.println("Flood");
    weather="Flood";
    break;
 case 1:    // Sensor getting wet
    Serial.println("Rain Warning");
    weather="Rain Warning";
    break;
 case 2:    // Sensor dry - To shut this up delete the " Serial.println("Not Raining"); " below.
    Serial.println("Not Raining");
    weather="Not Raining";
    break;
  }
  delay(1000);  // delay between reads

  String postData3 = "status=" + weather + "&station=" + station2 ;
  http.begin("http://192.168.43.178/sql_arduino/waterdrop.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
 
  int httpCode3 = http.POST(postData3);   //Send the request
  if( http.getString() == "OK"){
    Serial.println("Sent ");
    }    //Get the response payload

  http.end();
  delay(1000);
  
  // water level sensor

  String station9="Water Level Sensor";
  int val = analogRead(sensor);
  String output_value_water_level = String(val);
  String postData9 = "status=" + output_value_water_level + "&station=" + station9 ;
  http.begin("http://192.168.43.178/sql_arduino/waterlevel.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
 
  int httpCode9 = http.POST(postData9);   //Send the request
  String payload9 = http.getString();    //Get the response payload
 
  Serial.println(httpCode9);   //Print HTTP return code
  Serial.println(payload9);    //Print request response payload
  http.end();

  delay(1000);
  
  delay(5000);  //Post Data at every 5 seconds
  
}
