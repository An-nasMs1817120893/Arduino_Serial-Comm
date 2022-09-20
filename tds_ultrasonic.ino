#include <SoftwareSerial.h>         //ultrasonic sensor
#include <OneWire.h>                //temp library
#include <DallasTemperature.h>      //temp library
#include <EEPROM.h>
#include "GravityTDS.h"
#include<ArduinoJson.h>

float distance=0;
int percentage=0;
unsigned char data[4] = {};
SoftwareSerial mySerial(11,10);         //RX, TX ultrasonic sensor
SoftwareSerial nodemcu(0, 1);
unsigned long previousTime=0;
String message;

//SoftwareSerial nodemcu(0, 1); //Rx and Tx of the nodemcu 

//ultrasonic sensor
enum statesRX
{
    ST_RX_HEADER=0,
    ST_RX_DATA,
    ST_RX_PROCESSMSG    
};

// ph sensor
int ph_pin = A0;
int measure;
float voltagePH;
float Ph;

//tds sensor
#define TdsSensorPin A1
float tdsValue = 0;
float temperature_tds = 25;
GravityTDS gravityTds;


//temperature sensor
//#define ONE_WIRE_BUS A4              //temp pin
const int oneWireBus = A4; // GPIO where the DS18B20 is connected to
OneWire oneWire(oneWireBus);    // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);    // Pass our oneWire reference to Dallas Temperature sensor
float temperature = 0;

void setup()
{
    Serial.begin(115200);
    while (!Serial) continue;
    pinMode(7, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(4, OUTPUT);
    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
    digitalWrite(5, LOW);
    digitalWrite(4, LOW);
    sensors.begin();   
    gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  //initialization
    //nodemcu.begin(9600); 
    mySerial.begin(9600);    
}




void loop(){

  control_relay(); //must run setiap masa 
  if(millis()-previousTime>=500)
  {  
  

    StaticJsonDocument<1000> doc;
    
    
  tds_gravity();
  
  readutSensor();
  readphSensor();
  readtempSensor();


  doc["Water Level Percentage"] = percentage;
  //Serial.println(percentage);
  doc["tds Value"] = tdsValue,0;
  //Serial.println(tdsValue,0);
  doc["Ph Value"] = Ph;
  //Serial.println(Ph);
  doc["Temperature Value"] = temperature;
  //Serial.println(temperature);
  serializeJson(doc,Serial); //print the json object named "doc" to the serial monitor inside the serial buffer 
  Serial.println("");  
  //water_level_logic();

  //Serial.flush();
//Serial.print("Distance in mm : ");
//Serial.println(distance);    
/*Serial.print("Water Level Percentage : ");
Serial.println(percentage);
Serial.print("TDS Value : ");
Serial.println(tdsValue,0);
Serial.print("Ph Level : ");
Serial.println(Ph);
Serial.print("Temperature in C : ");
Serial.println(temperature);    
Serial.println("\n");
*/



//  message = "{\"SID\":";
//  message += "403";
//  message += ",\"TMP\":";
//  message += temperature;
//  message += ",\"pH\":";
//  message += Ph;
//  message += ",\"TDS\":";
//  message += tdsValue,0;
  //message += ",\"DO\":";
  //message += doValue;
//  message += ",\"Percentage\":";
//  message += percentage;
//  message += "}";
    previousTime=millis();
  }
//  Serial.println(String(message));
}



void readutSensor()
{
    static uint8_t
        stateRX = ST_RX_HEADER,
        rxIdx;
        
    if( mySerial.available() > 0 )
    {
        do
        {
            uint8_t ch = mySerial.read(); 
            switch( stateRX )
            {
                case    ST_RX_HEADER:
                    if( ch == 0xff )
                    {
                        rxIdx = 0;
                        data[rxIdx++] = ch;
                        stateRX = ST_RX_DATA;
                    }
                    
                break; 

                case    ST_RX_DATA:
                    data[rxIdx++] = ch;
                    if( rxIdx == 4 )
                        stateRX = ST_RX_PROCESSMSG;
                break;

                case    ST_RX_PROCESSMSG:
                    uint8_t sum = 0;
                    for( uint8_t i=0; i<3; i++ )
                        sum = sum + data[i];

                    if( sum == data[3] )
                    {
                        distance = ((uint16_t)data[1] << 8) + data[2];
                    }
                    else 
                        Serial.print(""); 

                    stateRX = ST_RX_HEADER;
                    
                break;
                           


                
            }

        percentage = map(distance,300,60,0,100);

            if(percentage<0)
          {
            percentage=0;
          }
          else if(percentage>100)
          {
            percentage=100;
          }

          
            
        }while( mySerial.available() > 0 );
        
    }

}

void readtempSensor()
{
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);
}

void readphSensor()
{
  measure = analogRead(ph_pin);
  voltagePH = 5 / 1023.0 * measure;    //classic digital to voltage conversion
  Ph = 7 + ((2.5 - voltagePH) / 0.167);
}

void tds_gravity()
{
    //temperature = readTemperature();  //add your temperature sensor and read it
    gravityTds.setTemperature(temperature_tds);  // set the temperature and execute temperature compensation
    gravityTds.update();  //sample and calculate
    tdsValue = gravityTds.getTdsValue();  // then get the value
    //Serial.print(tdsValue,0);
    //Serial.println("ppm");
}

void control_relay(){

    if (Serial.available()>0) { //read from the serial receive buffer either have char or not
    //read serial as a character
    char ser = Serial.read();
    //NOTE because the serial is read as "char" and not "int", the read value must be compared to character numbers
    //hence the quotes around the numbers in the case statement
    switch (ser) {
      case '!':  //receive 0 as an input from the mqtt broker 
        digitalWrite(7, LOW);
        Serial.print("RELAY 4 IS OFF");
        break;
      case '@':  //receive 1 as an input from the mqtt broker 
        digitalWrite(7, HIGH);
        Serial.println("RELAY 4 IS ON");
        break;
      case '#':
        digitalWrite(6, LOW);
        Serial.println("RELAY 3 IS OFF");
        break;
      case '$':
        digitalWrite(6, HIGH);
        Serial.println("RELAY 3 IS ON");
        break;
      case '%':
        digitalWrite(5, LOW);
        Serial.println("RELAY 2 IS OFF");
        break;
      case '^':
        digitalWrite(5, HIGH);        
        
        Serial.println("RELAY 2 IS ON");
        break;
      case '&':
        digitalWrite(4, LOW);
        Serial.println("RELAY 1 IS OFF");
        break;
      case '*':
        digitalWrite(4, HIGH);
        Serial.println("RELAY 1 IS ON");
        break;
      
    } 
  }

}

void water_level_logic(){

if(percentage<50) //percentage less than 60
{

  digitalWrite(5,HIGH); //pin utk water pump akan on !!! // pump duduk kat pin 5 
  Serial.println("Water Pump ON due to low water level");  
} 

else if (percentage>90) //percentage larger than 95 percent
{
  digitalWrite(5,LOW);
  Serial.println("Water Pump OFF due to sufficient water level"); 
}

else
{
  digitalWrite(5,LOW);
  Serial.println("Water level is in the range of 50 - 90 percent \n It is still sufficient !!");
}
}

void temp_logic()
{
  if(temperature<=20) //temperature less than 20
  {
    digitalWrite(3,HIGH);
    Serial.println("Heater ON");
  }
  else if(temperature>=40) //temperature larger than 20
  {
    digitalWrite(3,LOW);
    Serial.println("Heater OFF");    
  }

  else
  {
    digitalWrite(3,LOW);
    Serial.println("Temperature is in the range of 20 - 40 \n This is optimum temperature !!");
  }

}
