#include <DHT.h>  
#include <ESP8266WiFi.h>                   // Including libraries
#include <ThingSpeak.h>;
#include <LiquidCrystal.h>

String apiKey = "ZVL81P1K38AFBIX2";     // Write API key from ThingSpeak 
const char* WapiKey = "ZVL81P1K38AFBIX2";     //  Write API key from ThingSpeak
const char* RapiKey = "LRKU897ZMY63P5T7";     //  Read API key from ThingSpeak
 
const char *ssid =  "Mr_Gilly";     // ssid and wpa2 key
const char *pass =  "Gilly@Huawei12";
const char* server = "api.thingspeak.com";

const int RS = D3, EN = D4, d4 = D5, d5 = D6, d6 = D7, d7 = D8; //Declaring LCD pins

unsigned long myChannelNumber = 1706060; // Geting mu channel number
const int relay = 16;
 
#define DHTPIN 4          //pin where the dht11 is connected
 
DHT dht(DHTPIN, DHT11);
LiquidCrystal lcd(RS, EN, d4, d5, d6, d7); //Initiating my LCD pins
WiFiClient client;
 
void setup() 
{
       Serial.begin(9600);  //Initiating Serial moniator
       delay(10);
       dht.begin();         // Inititiaing DHT 
       lcd.begin(16, 2);     //Initiating LCD
 
       Serial.println("Connecting to ");
       Serial.println(ssid);            //Displaying is we are connected to WIFI
 
 
       WiFi.begin(ssid, pass);     //Getting connected to WiFi
       ThingSpeak.begin(client);    // Initiating Thing speak
 
      while (WiFi.status() != WL_CONNECTED)  // Checking if we are connected
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");  // If connection was successful 
      pinMode(relay, OUTPUT);
      digitalWrite(relay, LOW);  // Opening Relay
 
}
 
void loop() 
{
  
      float h = dht.readHumidity();     //Getiing Humididty and Temperature from DHT sensor
      float t = dht.readTemperature();
      
              if (isnan(h) || isnan(t))    // Checking if we Humidity and Temperature come well from DHT
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }

                 if(t>=27){        //cheking if Temperature is equal or great than 27
                  
                  digitalWrite(relay, LOW);   // Opening relay that openes Fan
                 }
                else{
                    digitalWrite(relay, HIGH); //Closing relathat close the Fan
                   }

                lcd.setCursor(0, 0);
                lcd.print("Temp :"+String(t));   // Displaying on LCD screen
                lcd.setCursor(0, 1);
                lcd.print("Hum :"+String(h));

               //Sending Data to cloud
                   
                if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";              // connecting to Fieleds
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr += "\r\n\r\n";
                             ThingSpeak.setField(1, t);      // setting Values to fields
                             ThingSpeak.setField(2, h);
                             int x = ThingSpeak.writeFields(myChannelNumber, WapiKey);
                             long Temperature = ThingSpeak.readLongField(myChannelNumber, 1, RapiKey);    //Update in ThingSpeak
                             long Humidity = ThingSpeak.readLongField(myChannelNumber, 2, RapiKey);

                       
                             
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");                      //Checking if connection to cloud(Thingspeak) was successful
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);

 
                             Serial.print("Temperature: ");
                             Serial.print(t);                          //pring on Serial monitor 
                             Serial.print("Humidity: ");
                             Serial.print(h);
                             Serial.println("Send to Thingspeak.");
                        }
          client.stop();                                 //stopnt the connection to the cloud
 
          Serial.println("Waiting...");

                 
 
                        

  delay(1000);
  
}
