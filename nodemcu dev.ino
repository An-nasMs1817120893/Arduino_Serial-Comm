#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

const char* ssid     = "Mi 10T"; // ESP32 and ESP8266 uses 2.4GHZ wifi only
const char* password = "rahayu123"; 
const char* mqttuser     = "annas_dzik"; // ESP32 and ESP8266 uses 2.4GHZ wifi only
const char* mqttpass = "annas123"; 
//mqtt setup start

#define mqtt_server "35.198.219.176"
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long previousTime=0;

String message;
const int buffer_size=256;
char buffer[256];
//declaration of variable
int percentage;
float tdsValue;
float Ph;
float temperature;

#define mqttarduino "arduino" //topic for ultrasonic
//#define mqttrelay "inTopic"
//SoftwareSerial nodemcu(3, 1);

//relay 
char* Topic = "inTopic"; //subscribe to topic to be notified about

void setup() {

  Serial.begin(115200);
  while (!Serial) continue;
  //nodemcu.begin(9600);
  Serial.println("");
  
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
  client.setCallback(callback);  
  delay(5000);
  //unsigned status;


}


void loop() {
  if(millis()-previousTime>=500)
  {
  json_deserialize();
  client.publish(mqttarduino,buffer); //publish topic arduino yg ada dlm buffer
  previousTime=millis();   
  }
  if (!client.connected()){
  reconnect();
  } //must run setiap masa
  client.loop(); //must run setiap masa 
}

//*************************************************************FUNCTION FOR RECONNECT TO THE WIFI NETWORK**************************************************************************************


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
      client.subscribe(Topic);      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//*******************************************************FUNCTION FOR RECEIVING THE JSON OBJECT***********************************************************************************************

void json_deserialize(){

    if (Serial.available()) 
  {

  StaticJsonDocument<1000> doc; //create Json object 
  DeserializationError err = deserializeJson(doc, Serial);
    
  if (err == DeserializationError::Ok)     //if json object is received , run this function
    {
      // Print the values
      // (we must use as<T>() to resolve the ambiguity)
      Serial.print("Water Level Percentage = ");
      Serial.println(doc["Water Level Percentage"].as<int>()); //json object name for water level
      Serial.print("tds value = ");
      Serial.println(doc["tds Value"].as<float>()); //json object name for tds 
      Serial.print("Ph Level = ");
      Serial.println(doc["Ph Value"].as<float>()); //json object name for ph
      Serial.print("Temperature Value = ");
      Serial.println(doc["Temperature Value"].as<float>()); //json object name for temperature
      Serial.println("\n");
      serializeJson(doc,buffer); //masuk json object dlm buffer
    }

    else 
    {
      // Print error to the "debug" serial port
      Serial.print("deserializeJson() returned ");
      Serial.println(err.c_str());
  
      // Flush all bytes in the "link" serial port buffer
      while (Serial.available() > 0)
        Serial.read();
    }    
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  //convert topic to string to make it easier to work with
  String topicStr = topic; 
  
  //Print out some debugging info
  ////Serial.println("Callback update.");
  //Serial.print("Topic: ");
  //Serial.println(topicStr);

  //turn the light off if the payload is '0' and publish to the MQTT server a confirmation message
  if (payload[0] == '0'){ //pin 7 off
    Serial.println("!");
    //respond();
    }
  if(payload[0] == '1'){ //pin 7 on
    Serial.println("@");
    //respond();
    }
  if (payload[0] == '2'){ //pin 6 off
    Serial.println("#");
    //respond();
    }
  if(payload[0] == '3'){ //pin 6 on
    Serial.println("$");
    //respond();
    }
  if (payload[0] == '4'){ //pin 5 off
    Serial.println("%");
    //respond();
    }
  if(payload[0] == '5'){ //pin 5 on
    Serial.println("^");
    //respond();
    }
  if (payload[0] == '6'){ //pin 4 off
    Serial.println("&");
    //respond();
    }
  if(payload[0] == '7'){ //pin 4 on 
    Serial.println("*");
    //respond(); 
    }
    
}


