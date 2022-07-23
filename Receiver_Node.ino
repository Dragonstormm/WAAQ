#include <SimpleDHT.h>
#include <SoftwareSerial.h>
// LoRa libraries
#include<SPI.h>
#include<LoRa.h>
#define TxD 10
#define RxD 9
SoftwareSerial bluetoothSerial(TxD, RxD);
void setup() {
  // Setting up bluetooth
  bluetoothSerial.begin(9600);
  Serial.begin(9600);
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
  // put your main code here, to run repeatedly:
  int packetSize = LoRa.parsePacket();
  if (packetSize) 
  {
    // received a paket
    Serial.println("");
    Serial.println("...................................");
    Serial.println("Received packet: ");
    String dta="";
    // read packet
    while (LoRa.available()) {
      char incoming = (char)LoRa.read();
      if (incoming == 'c')
      {
        Serial.print("Error");
      }
      else
      {
        Serial.print(incoming);
      }
      dta=incoming;
    }
    String tempp="";
    tempp+=dta[0];
    tempp+=dta[1];
    bluetoothSerial.print("Temperature : ");  
    bluetoothSerial.println(tempp);

    String humid="";
    humid=dta[2];
    humid+=dta[3];
    bluetoothSerial.print("Humidity : ");
    bluetoothSerial.print(humid); bluetoothSerial.println("%");

    String air="";
    air+=dta[4];
    air+=dta[5];
    air+=dta[6];
    bluetoothSerial.print("Air QUality : ");
    bluetoothSerial.print(airQuality,DEC);
    bluetoothSerial.println(" PPM");

    String rain="";
    rain=dta[7];
    bluetoothSerial.print("Raining? : ");
    bluetoothSerial.println(raining);
    bluetoothSerial.println("-------------------------------------------------------------");
    delay(2000);
  }
}
