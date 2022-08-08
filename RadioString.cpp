/*
RadioString.cpp - Sending long data Strings via RHReliableDatagram
Author: Dr. Andrew M. Bierer andrew.bierer@usda.gov
(License here) 2022 Andrew Bierer
Version history:
2022-06-16 V. 1.0
*/

#include <RadioString.h>

///////////////////
//Constructors
RadioString::RadioString(RHGenericDriver& driver, uint8_t thisAddress) : RHReliableDatagram(driver, thisAddress){

}

///////////////////
// Public Methods
///////////////////

///////////////////
// Send a long String
void RadioString::Send_String(String data_string, uint8_t sending_address, uint8_t destination){
  //define needed variables
  RH_RF95 *RHRF95;
  uint32_t total_string_length;
  bool single_packet = false;
  uint8_t total_packets;
  uint8_t whole_packets;
  uint8_t last_packet_length;
  uint32_t count = 0;
  char PACKET_BUFFER [RH_RF95_MAX_MESSAGE_LEN];   //Sending packets one-by-one as they are constructed removes the need for an array of arrays to hold values. Simply use one.
  String packet_num;
  //check input string for beginning and terminating characters
  if (data_string.substring(0, 1) != "[" || data_string.substring(1, 4) != "~~~") {
    Serial.println(F("Invalid String specification, begin data string with '[~~~' "));
    return;
  }

  if (data_string.substring((data_string.length() - 1), data_string.length()) != "]") {
    Serial.println(F("Invalid String specification, terminate data string with ']' "));
    return;
  }

  //determine total number of packets required to send the string
  total_string_length = data_string.length();

  Serial.print(F("Data Length:  ")); //Total String length desired to be sent
  Serial.println(total_string_length);

  if (total_string_length <= RH_RF95_MAX_MESSAGE_LEN) {
    Serial.println(F("Single packet can be sent as-is"));
    total_packets = 1;
    single_packet = true;
  }

  Serial.print(F("Required Packet Number: "));

  if (single_packet) {
    Serial.println(total_packets);
  } else {
    last_packet_length = total_string_length % RH_RF95_MAX_MESSAGE_LEN; //Modulo to find remainder of division
    whole_packets = (total_string_length / RH_RF95_MAX_MESSAGE_LEN); //Integer math leaves truncated (whole number no rounding) result
    total_packets = whole_packets + 1; //add 1 for the last packet whose length is determined above
    Serial.println(total_packets);
  }

  //Replace ~~~ in First Packet with number of expected packets
  packet_num = String(total_packets);
  if (total_packets >= 100) {
    data_string.replace("~~~", packet_num);
  } else if (total_packets >= 10 && total_packets < 100) {
    data_string.replace("~~~", "0" + packet_num);
  } else { // total packets < 10
    data_string.replace("~~~", "00" + packet_num);
  }
  packet_num = "";

  ////////////

  for (int i = 0; i < total_packets; i++) {                                 //for each packet
    for (int q = 0; q < RH_RF95_MAX_MESSAGE_LEN; q++) {                              //init q to track place in buffer
      PACKET_BUFFER[q] = data_string[((i * RH_RF95_MAX_MESSAGE_LEN) + q)];           //transfer string contents to PACKET_BUFFER
    }
    //Preview data to be sent
    Serial.print(F("Data packet  "));
    Serial.print(i + 1);
    Serial.print(F(": "));
    for (int z = 0; z < RH_RF95_MAX_MESSAGE_LEN; z++) {
      Serial.print((char)PACKET_BUFFER[z]);
    }
    Serial.println();

    Serial.print(F("Destination:  "));
    Serial.println(destination);

    ///Send Packets
    if (sendtoWait(PACKET_BUFFER, RH_RF95_MAX_MESSAGE_LEN, destination)) {
      Serial.print(F("SendtoWait packet "));
      Serial.print(i + 1);
      Serial.println(F(" successful."));
    } else {
      Serial.print(F("SendtoWait packet "));
      Serial.print(i + 1);
      Serial.println(F(" failed."));
    }

    //Print Transmission strength information
    Serial.print(F("RSSI: ")); //Recieved Signal Strength Inidicator (RSSI) measures amount of power present in radio signal. A greater negative value indicates a weaker signal.
    Serial.println(_driver.lastRssi(), DEC);

    Serial.print(F("SNR:  "));  //Signal to noise ratio (SNR), a useful indicator of impending link failure.
    Serial.println(RHRF95->lastSNR(), DEC);

  }
  delay(50);
}

//void RadioString::Receive_String(String RECEIVED_STRING, uint16_t PacketTimeout, uint16_t ROUTINE_TIMEOUT, uint8_t RECEIVE_ID){
//&String is string by reference, refers to original string | String is string by value, creates local copy
void RadioString::Receive_String(String &MESSAGE_STRING, uint16_t PacketTimeout, uint16_t ROUTINE_TIMEOUT, uint8_t RECEIVE_ID){
  //define needed variables
  RH_RF95 *RHRF95;
  RHReliableDatagram *RHRD;
  //  uint8_t RECEIVE_ID;                                               //Holder to place identity of received transmission

  while(available()){
    LEN_BUFF = sizeof(RECEIVE_BUFFER);
    uint8_t RECEIVE_ID;
    char PACKET_NUM_BUF[4];

    if(recvfromAckTimeout(RECEIVE_BUFFER, &LEN_BUFF, PacketTimeout, &RECEIVE_ID)){
      Serial.print(F("Recieved request from: "));
      Serial.print(RECEIVE_ID);
      Serial.print(F(": "));
      Serial.println((char*)RECEIVE_BUFFER);
      //Can add in checks for timeouts here if necessary.

      //Check Packet conditions: (i) the transmission start character '['; (ii) the number of packets expected immediately following the start character, 1 to 256 'xxx'; (iii) the transmission end character ']'
      if (RECEIVE_BUFFER[0] == '[') {
        Serial.println(F("Start Character Received."));
        for (int i = 0; i < 3; i++) {
          PACKET_NUM_BUF[i] = RECEIVE_BUFFER[i+1];
        }
        PACKET_COUNT ++;
        PACKET_NUM = atoi(PACKET_NUM_BUF);
        Serial.print(F("Expecting "));
        Serial.print(PACKET_NUM);
        Serial.println(F(" Packets."));
        RECEIVED_STRING += (char*)RECEIVE_BUFFER;
        Serial.print(F("Packet  "));
        Serial.print(PACKET_COUNT);
        Serial.print(F(" of "));
        Serial.println(PACKET_NUM);
        //PACKET_COUNT ++;
        if(PACKET_NUM == 1){ //To do if only 1 packet is received.
          Serial.println(F("This should be the complete String."));
          //if the buffer contains the end character ']'
          RECEIVED_STRING += (char*)RECEIVE_BUFFER;

          //Print and set the entire RECEIVED_STRING equal to the String input in the function (declared global in sketch)
          Serial.print(F("Reconstructed String: "));
          Serial.println(RECEIVED_STRING);
          MESSAGE_STRING = RECEIVED_STRING;
          //Print Transmission strength information
          Serial.print(F("RSSI: ")); //Recieved Signal Strength Inidicator (RSSI) measures amount of power present in radio signal. A greater negative value indicates a weaker signal.
          Serial.println(_driver.lastRssi(), DEC);

          Serial.print(F("SNR:  "));  //Signal to noise ratio (SNR), a useful indicator of impending link failure.
          Serial.println(RHRF95->lastSNR(), DEC);

          delay(10);
          RECEIVED_STRING = "";
          PACKET_NUM = 0;
          PACKET_COUNT = 0;
        }
        //  Serial.print(F("Current String:  "));
        //  Serial.println(RECEIVED_STRING);
        //Can we block transmissions from other RECEIVE_ID's until all expected packets are received?
      } else if(PACKET_COUNT < PACKET_NUM && PACKET_COUNT != 0){
                PACKET_COUNT ++;
        Serial.print(F("Packet  "));
        Serial.print(PACKET_COUNT);
        Serial.print(F(" of "));
        Serial.println(PACKET_NUM);
        RECEIVED_STRING += (char*)RECEIVE_BUFFER;

        //  Serial.print(F("Current String:  "));
        //  Serial.println(RECEIVED_STRING);
      } else if (PACKET_COUNT == PACKET_NUM && PACKET_COUNT != 0) {
        Serial.print(F("Packet  "));
        Serial.print(PACKET_COUNT);
        Serial.print(F(" of "));
        Serial.println(PACKET_NUM);
        Serial.println(F("This should be the complete String."));
        //if the buffer contains the end character ']'
        RECEIVED_STRING += (char*)RECEIVE_BUFFER;

        //Print and set the entire RECEIVED_STRING equal to the String input in the function (declared global in sketch)
        Serial.print(F("Reconstructed String: "));
        Serial.println(RECEIVED_STRING);
        //How to do this?

        MESSAGE_STRING = RECEIVED_STRING;

        // strcpy(MESSAGE_STRING, RECEIVED_STRING);
        // for(int i=0; i<= 100; i++){
        //   MESSAGE_STRING[i] = RECEIVED_STRING[i];
        // }

        //Print Transmission strength information
        Serial.print(F("RSSI: ")); //Recieved Signal Strength Inidicator (RSSI) measures amount of power present in radio signal. A greater negative value indicates a weaker signal.
        Serial.println(_driver.lastRssi(), DEC);

        Serial.print(F("SNR:  "));  //Signal to noise ratio (SNR), a useful indicator of impending link failure.
        Serial.println(RHRF95->lastSNR(), DEC);

        delay(10);
        RECEIVED_STRING = "";
        PACKET_NUM = 0;
        PACKET_COUNT = 0;

      } else if (PACKET_COUNT > PACKET_NUM || PACKET_COUNT == 0){
        Serial.println(F("Encountered error, clearing buffer."));
        RECEIVED_STRING = "";
        PACKET_COUNT = 0;
        PACKET_NUM = 0;
        break;
      } else {
        Serial.println(F("Transmission received without the proper format."));
        Serial.println(F("Is the intended transmission formatted correctly?"));
        Serial.println(F("Is it a middle or end packet in a larger transmission?"));
      }
    } else {
      Serial.println(F("recvfromAck returned nothing."));
    }
  }
  delay(ROUTINE_TIMEOUT);
}
//---------- Init -----------------------
bool RadioString::init(float Frequency, uint16_t AckTimeout, int8_t TxPower, uint8_t Retries, uint8_t ThisAddress){
  if(RHDatagram::init()){
    RH_RF95 *myDriver;
    setTimeout(AckTimeout);
    setRetries(Retries);
    setThisAddress(ThisAddress);
    myDriver = (RH_RF95 *)&_driver;
    myDriver->setFrequency(Frequency);
    myDriver->setTxPower(TxPower);
    myDriver->lastSNR();
    return true;
  }
  return false;
}
