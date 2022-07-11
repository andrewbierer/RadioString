
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <RadioString.h>

//Global variables

String Send_Short_String = "Hello World! A short string takes no more than 1 packet.";
String Send_Long_String = "Hello World! Each radio packet can hold 251 octets/String characters. If needed, RadioString will break up the string into the number of packets required and send them one at a time. The maximum number of octets RadioString can handle is 64000. This statement will now repeat to demonstrate multiple packet handling. Hello World! Each radio packet can hold 251 octets/String characters. If needed, RadioString will break up the string into the number of packets required and send them one at a time. The maximum number of octets RadioString can handle is 64000. This statement will now repeat to demonstrate multiple packet handling. Hello World! Each radio packet can hold 251 octets/String characters. If needed, RadioString will break up the string into the number of packets required and send them one at a time. The maximum number of octets RadioString can handle is 64000. This statement will now repeat to demonstrate multiple packet handling. Hello World! Each radio packet can hold 251 octets/String characters. If needed, RadioString will break up the string into the number of packets required and send them one at a time. The maximum number of octets RadioString can handle is 64000. This statement will now repeat to demonstrate multiple packet handling.";


////Set Variables Here
uint8_t radioID = 101;                                             //Set the ID of this radio, 0-255
uint8_t gatewayID = 2;                                             //Set the ID of the receiving radio, 0-255
uint16_t TRANSMIT_TIMEOUT = 2000;                                  //Set the minimum retransmit timeout. If sendtoWait is waiting for an ack longer than this time (in milliseconds), it will retransmit the message. Defaults to 200ms.
uint8_t TRANSMIT_POWER = 20;                                       //Set transmission power, 0-20,  defaults to 14
float RADIO_FREQUENCY = 915.0;                                     //Set frequency of radio module
uint16_t ROUTINE_TIMEOUT = 2000;                                   //Set delay at the end of the routine, too low and some packets will be missed, too high and you may also miss packets
uint8_t RETRY_NUM = 3;

//Initialize Radio//
RH_RF95 driver(4, 2);                                              //Set (Slave Select, Interrupt) pins on microcontroller
RadioString manager(driver, radioID);                              //Pass the driver and radioID info to RadioString

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(100);

}

void loop() {
  // put your main code here, to run repeatedly:

  //Append the desired script with the start "[" and end "]" characters and the buffer "~~~" to be replaced with packet information.
  //E.G. [~~~Your String Goes Here.

  //Append Start character and buffer
  String The_Short_String = "[~~~" + Send_Short_String;
  String The_Long_String = "[~~~" + Send_Long_String;

  //Append end character
  The_Short_String += ']';
  The_Long_String += ']';

 // Serial.println(Send_Short_String);
 // Serial.println(Send_Long_String);

  //Check for radio initilization
  if (!manager.init(RADIO_FREQUENCY, TRANSMIT_TIMEOUT, TRANSMIT_POWER, RETRY_NUM, radioID)) {
    Serial.println("Radio Failed");
  }
  if (manager.init(RADIO_FREQUENCY, TRANSMIT_TIMEOUT, TRANSMIT_POWER, RETRY_NUM, radioID)) {
    Serial.println("Radio initilization successful.");
  }

  //Send desired string (now the modified buffers above) & clear the example string buffers
  manager.Send_String(The_Long_String, radioID, gatewayID);
  delay(5000);
  The_Short_String = "";
  The_Long_String = "";
  Serial.println("End Loop.");
}
