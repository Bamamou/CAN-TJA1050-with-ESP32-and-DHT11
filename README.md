# CAN-TJA1050-with-ESP32-and-DHT11

+ The code reads the data from a DHT11 using an ESP32 and send the reading to a TJA1050 CAN tranceiver 
+ A second TJA1050 reads the data in canbus and and the data is read an printed using the serial monitor of the second ESP32
+ This code works as a CAN Sender and as a CAN Receiver depending on the call in the loop function.
+ Change the pins setting depending on your GPIO for both Can-Tranceiver and DHT11
