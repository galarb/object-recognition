#include "camuart.h"
#include <Arduino.h>

camuart::camuart(int ppin) {
  _ppin = ppin;
}

void camuart::begin(double bdrate) {
  Serial.begin(bdrate);      
  delay(30);
  
}

