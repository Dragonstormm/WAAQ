//The MQ-135 Gas sensor can detect gases like Ammonia, nitrogen oxide, alcohols, aromatic compounds, sulfide and smoke.
// Rain drop sensor take 5 volts and give values from 0-1023 the wetter the surface of the sensor gets the less value it will give.
#include <SimpleDHT.h>
// LoRa libraries
#include<SPI.h>
#include<LoRa.h>


int pinDHT11 = 4;// 2nd digital pin
SimpleDHT11 dht11(pinDHT11);

// Initialising the wifi variables
#include <SoftwareSerial.h>
String AP = "Drago";       // AP NAME
String PASS = "noobster"; // AP PASSWORD
String API = "XLO7R4WFPWDOWZA9";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
SoftwareSerial esp8266(2,3);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Setting up the wifi connection
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");

  // Setting up LoRa
  while (!Serial);  
  Serial.println("LoRa Sender");
  if (!LoRa.begin(433E6)) { // or 915E6, the MHz speed of your module
    Serial.println("Starting LoRa failed!");
  }
  else
  {
    Serial.println("Starting LoRa Successfull!!");
  }
}

void loop() {
  Serial.println("=================================");
  Serial.println("");
  // Air Quality monitoring
  int airQuality=analogRead(A1);
  Serial.print("Air QUality : ");
  Serial.print(airQuality,DEC);
  Serial.println(" PPM");

  // Raindrop Detection
  int RainInput=analogRead(A2);
  Serial.print("Sensor value : ");
  Serial.println(RainInput);
  int raining=0;
  if(RainInput<800)
  {
    Serial.println("It's Raining");
    raining=1;
  }
  else
  {
    Serial.println("Not Raining");
    raining=0;
  }

  // Temprature and humidity sensing
  //The DHT11 measures relative humidity. Relative humidity is the amount of water vapor in air vs. the saturation point of water vapor in air. At the saturation point, water vapor starts to condense and accumulate on surfaces forming dew.
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(1000);
    Serial.println("THERE IS ERROR in DHT11");
  }
  
  Serial.print((int)temperature); Serial.print("°C,  "); 
  Serial.print((int)humidity); Serial.println("% Humidity");
  
  // Sending data to the ThinkSpeak cloud
 
 String getData = "GET /update?api_key="+ API +"&field1="+String((int)temperature)+"&field2="+String((int)humidity)+"&field3="+String(airQuality)+"&field4="+String(raining);
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");



  // Sending data using LoRa
  Serial.println("Sending LoRa Packet");
  int data_lora=airQuality*10+raining;
  int data_lora_2=((int)temperature)*100+((int)humidity);
//  LoRa.beginPacket(); 
//  LoRa.print("Temperature : "); 
//  LoRa.print((int)temperature); LoRa.println("°C ");
//  LoRa.print("Humidity : ");
//  LoRa.print((int)humidity); LoRa.println("%");
//  
//  LoRa.print("Air QUality : ");
//  LoRa.print(airQuality,DEC);
//  LoRa.println(" PPM");
//  LoRa.print("Raining? : ");
//  LoRa.println(raining);
  LoRa.print(String(data_lora_2)+String(data_lora));
//  LoRa.println(data_lora);
  LoRa.endPacket();
  delay(50);
  
 // DHT11 sampling rate is 1HZ.
  delay(10000);

}
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
