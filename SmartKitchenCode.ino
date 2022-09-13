#include <SimpleDHT.h>                   // Data ---> D3 VCC ---> 3V3 GND ---> GND
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

int gasSensor = A0; // smoke / gas sensor connected with analog pin A1 of the arduino / mega.
int data = 0;
int buzzer = 5; //D1
bool buzzerValue = LOW;
int relay = 4; //D2
bool relayValue = HIGH;
// WiFi parameters
#define WLAN_SSID       "Redmi Note 10S"
#define WLAN_PASS       "123456789m"
// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "manavbansal"
#define AIO_KEY         "aio_qtPY62cGk9SY5Sjj8cbgSGcPTuHp" 
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish Temperature1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temperature");
Adafruit_MQTT_Publish Humidity1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Humidity");
Adafruit_MQTT_Publish Buzzer = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Buzzer");
Adafruit_MQTT_Publish GasValue = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Gas Level");
Adafruit_MQTT_Publish Relay = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Relay");
Adafruit_MQTT_Subscribe buzzerfeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Buzzer");


/** The smtp port e.g.
 * 25  or esp_mail_smtp_port_25
 * 465 or esp_mail_smtp_port_465
 * 587 or esp_mail_smtp_port_587
 */
#define SMTP_PORT 465

/* The log in credentials */
//#define AUTHOR_EMAIL "testIOT20192023@gmail.com"
//#define AUTHOR_PASSWORD "hfpqxzwopobzanxi"
//#define RECEPIENT_MAIL = "shreya.aashi14@gmail.com"
int pinDHT11 = 0;

SimpleDHT11 dht11(pinDHT11);
byte hum = 0;  //Stores humidity value
byte temp = 0; //Stores temperature value
float gasValue = 0;
void setup() {
  Serial.begin(115200);
  Serial.println(F("Adafruit IO Example"));
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  
  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
  pinMode(buzzer,OUTPUT);
//  pinMode(gasValue, INPUT);
  pinMode(relay,OUTPUT);

  // connect to adafruit io
  connect();
  

}

// connect to adafruit io via MQTT
void connect() {
  Serial.print(F("Connecting to Adafruit IO... "));
  int8_t ret;
  while ((ret = mqtt.connect()) != 0) {
    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }

    if(ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Retrying connection..."));
    delay(10000);
  }
  Serial.println(F("Adafruit IO Connected!"));
}

void loop() {                                             
  // ping adafruit io a few times to make sure we remain connected
  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      connect();
  }

  
  dht11.read(&temp, &hum, NULL);
  Serial.print((int)temp); Serial.print(" *C, "); 
  Serial.print((int)hum); Serial.println(" H");

  gasValue = analogRead(gasSensor);
  Serial.println("gasValue : ");
  Serial.print(gasValue); 
  

  if(gasValue>350){
    Serial.println("Smoke Detected");
    buzzerValue = HIGH;
    relayValue = LOW;
    digitalWrite(buzzer,buzzerValue);
    digitalWrite(relay,relayValue);
  }
  delay(3000);
    
  
   if (! Temperature1.publish(temp)) {                     //Publish to Adafruit
      Serial.println(F("Failed"));
    }  
       if (! Humidity1.publish(hum)) {                     //Publish to Adafruit
      Serial.println(F("Failed"));
    }
    if(! GasValue.publish(gasValue)){
       Serial.println(F("Failed"));
    }

    if(! Buzzer.publish(buzzerValue)){
       Serial.println(F("Failed"));
    }
    if(! Relay.publish(relayValue)){
       Serial.println(F("Failed"));
    }
    else {
      Serial.println(F("Sent!"));
    }
}
