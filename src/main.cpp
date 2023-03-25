/*
The code reads the data from a DHT11 using an ESP32 and send the reading to a TJA1050 CAN tranceiver 
A second TJA1050 reads the data in canbus and and the data is read an printed using the serial monitor of the second ESP32
This code works as a CAN Sender and as a CAN Receiver depending on the call in the loop function.
Change the pins setting depending on your GPIO for both Can-Tranceiver and DHT11

*/

//==================================================================================//
#include<Arduino.h>
#include <CAN.h>
#include "DHTesp.h" // Include the DHT librairy for the ESPx

DHTesp dht;             // create a DHT object
//======================================================================================//
// Please select any GPIO for both RX and TX
#define TX_GPIO_NUM   4  // Connects to CTX
#define RX_GPIO_NUM   5  // Connects to CRX
#define DHT_PIN       13

//==================================================================================//
// This Can sender function that send a packect containing the temp, humidity, and heat indexes
void canSender() {
  // send packet: id is 11 bits, packet can contain up to 8 bytes of data
  Serial.print ("Sending packet ... ");
  float temperatureC = dht.getTemperature();                         // To store the values of tempreature
  float temperatureF = dht.toFahrenheit(temperatureC);              // Temperature in F
  float humidity = dht.getHumidity();                              // To store the values of Humidity
  float HIC = dht.computeHeatIndex(temperatureC, humidity, false); // To sennd the value of the Heat Index in Celsus
  float HIF = dht.computeHeatIndex(dht.toFahrenheit(temperatureC), humidity, true);

  CAN.beginPacket(0x12);  //sets the ID and clears the transmit buffer
  // CAN.beginExtendedPacket(0xabcdef);
  CAN.write (temperatureC); //write data to buffer. data is not sent until endPacket() is called.
  CAN.write (temperatureF);
  CAN.write (humidity);
  CAN.write (HIC);
  CAN.write (HIF);
  CAN.write (humidity);
  CAN.write (HIC);
  CAN.write (HIF);
  CAN.endPacket();

  //RTR packet with a requested data length
  CAN.beginPacket(0x12, 3, true);
  CAN.endPacket();

  Serial.println ("done");

  delay (1000);
}

//==================================================================================//
// The receiver funtion
void canReceiver() {
  // try to parse packet
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    // received a packet
    Serial.print ("Received ");

    if (CAN.packetExtended()) {
      Serial.print ("extended ");
    }

    if (CAN.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print ("RTR ");
    }

    Serial.print ("packet with id 0x");
    Serial.print (CAN.packetId(), HEX);     // the packed ID or address in Hex

    if (CAN.packetRtr()) {
      Serial.print (" and requested length ");
      Serial.println (CAN.packetDlc());   // The data lenght 
    } else {
      Serial.print (" and length ");
      Serial.println (packetSize);      // Packect size 

      // only print packet data for non-RTR packets
      while (CAN.available()) {
        Serial.print ((double) CAN.read());  // This function can be edited to read a given bit of a given byte
      }
      Serial.println();
    }

    Serial.println();
  }
}

//===========================================================================================================

void setup() {
  // put your setup code here, to run once:
  Serial.begin (115200);
  dht.setup(DHT_PIN, DHTesp::DHT11);   // If you are using the DHT11, you just need to change the value 22 to 11
  delay(dht.getMinimumSamplingPeriod());
  Serial.println(dht.getStatusString());
  while (!Serial);
  delay (1000);
  Serial.println ("CAN Receiver/Receiver");

  // Set the pins
  CAN.setPins (RX_GPIO_NUM, TX_GPIO_NUM);

  // start the CAN bus at 500 kbps
  if (!CAN.begin (500E3)) {
    Serial.println ("Starting CAN failed!");
    while (1);
  }
  else {
    Serial.println ("CAN Initialized");
  }
}

//==================================================================================//

void loop() {
  // To change this code to a receiver just uncomment the canReceiver()
 // canSender();
  canReceiver();
}
