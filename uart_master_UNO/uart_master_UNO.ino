//Receiving serial communication from esp32
//pins 3, 2 reserved for RX, TX

#include "uartcom.h"

uartcom myuart(2);//reserved

void setup() {
  myuart.begin(115200);//sets the UART speed as well as the local
}

void loop() {
  myuart.parseuart();//must be in the loop
  int adjusted_x = myuart.getDeltaX();
  if(adjusted_x != 0){
    Serial.print("dx=");
    Serial.println(adjusted_x);
  }  
  delay(200);
}

