
/*
 *  Simple implementation of One wire DS18B20 and DST22 Sensor reading logged on Thingspeaks
 *  With DS18B20 temperature reading
 *
 *  Board detail : https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/pinouts
 *  http://iot-playground.com/2-uncategorised/41-esp8266-ds18b20-temperature-sensor-arduino-ide
 *
 *  Timer
 *  http://www.switchdoc.com/2015/10/iot-esp8266-timer-tutorial-arduino-ide/
 *  
 *  si ereur avec __ieee754_sqrt
 *  https://github.com/esp8266/Arduino/issues/612
 *  https://files.gitter.im/esp8266/Arduino/Abqa/libm.a.tbz replace the one in
 *  C:\Users\axel\AppData\Local\Arduino15\packages\esp8266\tools\xtensa-lx106-elf-gcc\1.20.0-26-gb404fb9\xtensa-lx106-elf\lib
 */

//Include credential files
#include "wifi_log.h"
#include "thingspeak_log.h"

#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

//Serial Config
#define BAUDRATE 115200  // Serial link speed

//Wifi config
const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;
//Function declarations
void wifiConnect(void);
void ClientAction (void);

//Thingspeak config
String myWriteAPIKey = TS_WRITE_KEY;
//Function declarations
void thingSpeakWrite (String, float, float, float, float, float, float, float, float);
                      
//Sensors config
#define ONE_WIRE_BUS 14  // One wire pin for DS18B20
#define DHTPIN 12        // what pin we're connected to
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire); 
DHT dht(DHTPIN, DHT22,15);
//Function declarations
void sensorRead(void);


float temp1;
float temp2;
float tempx;
float hum;

#define BLUE_LED  2 // GPIO2 also used by wifi chip
#define RED_LED   0 // GPIO0
WiFiClient client;


/* void setup(void) 
 *  Setup software run only once
 *  
*/
void setup() {
  
  //Start Serial
  Serial.begin(BAUDRATE);
  Serial.println("");
  Serial.println("--------------------------");
  Serial.println("   ESP8266 Temp Test");
  Serial.println("--------------------------");

  //Init leds
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(RED_LED, LOW);

  //Init wifi
  wifiConnect();

  //Init DHT temperature and Himidity reading
  dht.begin();
 
}

/* void loop(void) 
 *  Main program automatically loaded
 *  
*/
void loop() 
{
  //Toggle LED
  digitalWrite(RED_LED, !digitalRead(RED_LED));
    
  sensorRead();
  
  thingSpeakWrite (   myWriteAPIKey,
                      temp1, temp2, 0xFFFFFFFF, 0xFFFFFFFF,
                      0xFFFFFFFF, 0xFFFFFFFF, tempx, hum);

  Serial.print("Temperature DS18B20 : ");
  Serial.print(temp1);
  Serial.print(" ");
  Serial.println(temp2);

  Serial.print("Temperature DHT22 : ");
  Serial.println(tempx);
  Serial.print("Humidity: ");
  Serial.println(hum);
                      
  //delay(1000*60*15);
  delay(30*1000); //For test
}


/* void wifiConnect(void)
 *  Function 
 *  
 *  Input  : None
 *  Output : None
*/
void wifiConnect(void){
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  Serial.println("");
  Serial.println("WiFi connected");
}

 
void sensorRead(void) {

  //DS18B20 Reading
  DS18B20.requestTemperatures(); 
  temp1 = DS18B20.getTempCByIndex(0);
  temp1 = round(temp1*10)/10;
  temp2 = DS18B20.getTempCByIndex(1);
  temp2 = round(temp2*10)/10;

  //DTH22 Reading
  hum = dht.readHumidity();
  tempx = dht.readTemperature();
 
}

void thingSpeakWrite (String APIKey,
                      float field1, float field2, float field3, float field4,
                      float field5, float field6, float field7, float field8)
{
  
  const char* thingspeakServer = "api.thingspeak.com";

  if (client.connect(thingspeakServer,80)) {  //   "184.106.153.149" or api.thingspeak.com
    String postStr = APIKey;
    if (!isnan(field1))
    {
      postStr +="&field1=";
      postStr += String(field1);
    }
    if (!isnan(field2))
    {
      postStr +="&field2=";
      postStr += String(field2);
    }
    if (!isnan(field3))
    {
      postStr +="&field3=";
      postStr += String(field3);
    }
    if (!isnan(field4))
    {
      postStr +="&field4=";
      postStr += String(field4);
    }
    if (!isnan(field5))
    {
      postStr +="&field5=";
      postStr += String(field5);
    }
    if (!isnan(field6))
    {
      postStr +="&field6=";
      postStr += String(field6);
    }
    if (!isnan(field7))
    {
      postStr +="&field7=";
      postStr += String(field7);
    }
    if (!isnan(field8))
    {
      postStr +="&field8=";
      postStr += String(field8);
    }
    postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+myWriteAPIKey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
 
  }
  client.stop();
}




