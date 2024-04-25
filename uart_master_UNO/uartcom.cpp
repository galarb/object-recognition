#include "HardwareSerial.h"
#include <Arduino.h>
#include "uartcom.h"
#include <SoftwareSerial.h>

int adjusted_x;
int adjusted_y;

SoftwareSerial mySerial(3, 2); // RX, TX
const unsigned int MAX_MESSAGE_LENGTH = 128;


uartcom::uartcom(int pin) {
  _pin = pin;
}

void uartcom::begin(double bdrate) {
  Serial.begin(bdrate); 
  mySerial.begin(bdrate);
     
  delay(30);  
  Serial.println("Setup finished");
}

void uartcom::parseuart() {
  while (mySerial.available() > 0) {
    static char message[MAX_MESSAGE_LENGTH];
    static unsigned int message_pos = 0;
    char inByte = mySerial.read();

    if (inByte != '\n' && message_pos < MAX_MESSAGE_LENGTH - 1) {
      message[message_pos++] = inByte;
    } else {
      message[message_pos] = '\0'; // Null-terminate the string
      //Serial.print("Received message: ");
      //Serial.println(message);

      // Parse the message and assign each argument to global variables
      char *token = strtok(message, ",");
      while (token != NULL) {
        // Check for label
        if (strncmp(token, "l", 1) == 0) {
          String label = strtok(NULL, ",");
          if (label != NULL) {
         //   Serial.print("Detected object label: ");
           // Serial.println(label);
          } else {
           // Serial.println("Label token not found");
          }
        }
        // Check for x coordinate
        else if (strncmp(token, "x", 1) == 0) {
          String x_coord = strtok(NULL, ",");
          if (x_coord != NULL) {
            adjusted_x = atoi(x_coord.c_str()); // Convert String to integer
            //Serial.print("Adjusted x coordinate: ");
            //Serial.println(adjusted_x);
          } else {
         //   Serial.println("X coordinate token not found");
          }
        }
        // Check for y coordinate
        else if (strncmp(token, "y", 1) == 0) {
          String y_coord = strtok(NULL, ",");
          if (y_coord != NULL) {
            adjusted_y = atoi(y_coord.c_str()); // Convert String to integer
           // Serial.print("Adjusted y coordinate: ");
            //Serial.println(adjusted_y);
          } else {
            //Serial.println("Y coordinate token not found");
          }
        }
        token = strtok(NULL, ",");
      }

      // Reset message buffer and position for the next message
      message_pos = 0;
    }
  } 
}





int uartcom::getDeltaX(){
  return (adjusted_x);
}

int uartcom::getDeltaY(){
  return (adjusted_y);
}
