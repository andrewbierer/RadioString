
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <RadioString.h>

//Global variables

String Received_String;


////Set Variables Here
uint8_t radioID = 2;                                               //In this case we are receiving communications as the gateway so the radioID == gatewayID
uint8_t gatewayID = 2;                                             //Set the ID of the receiving radio, 0-255
uint16_t TRANSMIT_TIMEOUT = 2000;                                  //Set the minimum retransmit timeout. If sendtoWait is waiting for an ack longer than this time (in milliseconds), it will retransmit the message. Defaults to 200ms.
uint8_t TRANSMIT_POWER = 20;                                       //Set transmission power, 0-20,  defaults to 14
float RADIO_FREQUENCY = 915.0;                                     //Set frequency of radio module
uint16_t ROUTINE_TIMEOUT = 2000;                                   //Set delay at the end of the routine, too low and some packets will be missed, too high and you may also miss packets
uint8_t RETRY_NUM = 3;                                             //Set number of retry attempts for a single packet
uint8_t RECEIVE_ID;                                                //Holder to place identity of received transmission
uint8_t PACKET_NUM;                                                //To hold number of packets expected
uint8_t PACKET_COUNT;                                              //To count/increment on number of packets expected


//Initialize Radio//
RH_RF95 driver(4, 2);                                             //Set (Slave Select, Interrupt) pins on microcontroller
RadioString manager(driver, radioID);                             //Pass the driver and radioID info to RadioString

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(100);

  Serial.println("Begin..");

}

void loop() {
  // put your main code here, to run repeatedly:
  
  //Check for radio initilization.
  Serial.println("Top of loop.");
  if (!manager.init(RADIO_FREQUENCY, TRANSMIT_TIMEOUT, TRANSMIT_POWER, RETRY_NUM, radioID)) {
    Serial.println("Radio Failed");
  }
  if (manager.init(RADIO_FREQUENCY, TRANSMIT_TIMEOUT, TRANSMIT_POWER, RETRY_NUM, radioID)) {
    Serial.println("Radio initilization successful.");
  }

  //Use RadioString::Receive_String to search for transmissions addressed to this unit.
  manager.Receive_String(Received_String, TRANSMIT_TIMEOUT, ROUTINE_TIMEOUT, RECEIVE_ID);

  //When the Receive_String function has parsed all packets of an intended message,the Received_String contains the message.
  if (Received_String != "") {
    Serial.print("Received_String:  ");
    Serial.println(Received_String);
  }
  //Do what you like with Received_String here.

  //Reset the Received_String for next transmission.
  Received_String = ""; 
  Serial.println(F("End loop."));
}
