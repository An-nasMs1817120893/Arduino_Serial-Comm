//#include <Adafruit_BME280.h>
#include <DHT.h>
#include <WiFi.h>

const char* ssid     = "SEA-IC"; // ESP32 and ESP8266 uses 2.4GHZ wifi only
const char* password = "seaic2022"; 
const char* mqttuser     = "annasdzik"; // ESP32 and ESP8266 uses 2.4GHZ wifi only
const char* mqttpass = "annas123"; 

//MQTT Setup Start
#include <PubSubClient.h>
#define mqtt_server "35.198.219.176"
WiFiClient espClient;
PubSubClient client(espClient);
//#define mqttTemp1 "growShed/temp1"
//#define mqttHum1 "growShed/hum1"
//#define mqttTemp2 "growShed/temp2"
//#define mqttHum2 "growShed/hum2"
#define mqttTempExt "growShed/tempExt" //topic
#define mqttHumExt "growShed/humExt" //topic
//MQTT Setup End

//Adafruit_BME280 bme1; // I2C
//Adafruit_BME280 bme2; // I2C
#define DHTPIN 33
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

float temp1, temp2,tempExt, hum1, hum2, humExt;

void setup() {
  Serial.begin(9600);
  Serial.println();
  
  // begin Wifi connect
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //end Wifi connect

  client.setServer(mqtt_server, 1883);
  
  dht.begin();
  delay(5000);
  unsigned status;
  /*status = bme1.begin(0x76); 
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme1.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1);
    }
    status = bme2.begin(0x77); 
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme2.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1);
    }

    */
  
}

void getValues() {

  //temp1 = bme1.readTemperature();
  //temp2 = bme2.readTemperature();
  tempExt = dht.readTemperature();
  //hum1 = bme1.readHumidity();
  //hum2 = bme2.readHumidity();
  humExt = dht.readHumidity();
  
  //Serial.print("BME 1 Temperature = ");
  //Serial.print(temp1);
  //Serial.println(" *C");

  //Serial.print("BME 1 Pressure = ");
  //Serial.print(bme1.readPressure() / 100.0F);
  //Serial.println(" hPa");

  //Serial.print("BME 1 Humidity = ");
  //Serial.print(hum1);
  //Serial.println(" %");

  //Serial.print("BME 2 Temperature = ");
  //Serial.print(temp2);
  //Serial.println(" *C");

  //Serial.print("BME 2 Pressure = ");
  //Serial.print(bme2.readPressure() / 100.0F);
  //Serial.println(" hPa");

  //Serial.print("BME 2 Humidity = ");
  //Serial.print(hum2);
  //Serial.println(" %");

  Serial.print("Ext Temp = ");
  Serial.print(tempExt);
  Serial.println(" *C");

  Serial.print("Ext Humidity = ");
  Serial.print(humExt);
  Serial.println(" %");
 
  Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  int counter = 0;
  while (!client.connected()) {
    if (counter==5){
      ESP.restart();
    }
    counter+=1;
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
   
    if (client.connect("growTentController", mqttuser, mqttpass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {
  if (!client.connected()){
    reconnect();
  }
  getValues();

  //client.publish(mqttTemp1, String(temp1).c_str(),true);
  //client.publish(mqttHum1, String(hum1).c_str(),true);
  //client.publish(mqttTemp2, String(temp2).c_str(),true);
  //client.publish(mqttHum2, String(hum2).c_str(),true);
  client.publish(mqttTempExt, String(tempExt).c_str(),true);
  client.publish(mqttHumExt, String(humExt).c_str(),true);


  delay(5000);
}
