/*
   RadioString.h - A library for sending and receiving radio transmissions as Strings.
   Enables sending up to 64000 octets. String must be contained within start '[' and
   end ']' characters. String will be appended after the start character with the number
   of packets transmission requires "[xxx".
   String to be send is then cut into packets and transmitted using RHReliableDatagram
   Received transmissions ignored unless start character is read, appends String until then end character is read.

   Author: Dr. Andrew M. Bierer andrew.bierer@usda.gov
   (License here) 2022 Andrew Bierer

   Version history:
   2022-06-16 V. 1.0
*/

#ifndef RadioString_h
#define RadioString_h

#include "Arduino.h"
#include <RH_RF95.h>
#include <RHReliableDatagram.h>

#define MAX_PACKET_LEN (RH_RF95_MAX_MESSAGE_LEN - 1) //250
#define MAX_STRING_LEN (MAX_PACKET_LEN * (2^8)) //64000 bytes, one character in string/packet = 1 byte

////////////////////////////////////////////////////////////////
/// \class RadioString RadioString.h <RadioString.h>
/// \brief extension of RHReliableDatagram to send large Strings
class RadioString : public RHReliableDatagram {
  public:
    /// Constructor.
    /// \param[in] driver The RadioHead driver to use to transport messages.
    /// \param[in] thisAddress The address to assign to this node. Defaults to 0
    RadioString(RHGenericDriver& driver, uint8_t thisAddress = 0);

    /// Send a String message up to 64000 octets over multiple packets. Packet retries and Acks present.
    /// \param[in] sending_address The address of the unit sending the message.
    /// \param[in] destination The address to send the message to.
    /// \param[in] data_string The String being sent.
    void Send_String(String data_string, uint8_t sending_address, uint8_t destination);

    /// Receive a String message up to 64000 octets.
    /// Parse received single packets.
    void Receive_String()

    /// Clear String buffer
    void clearBuffer()

  private:
    ///
    
};

#endif
