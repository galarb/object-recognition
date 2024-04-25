/*                 ******************
              Class for UART communications
      
      Featuring: 
          object recognition arguments parsing:
            Label, x and y camera adjusted positions
                         
                By: Gal Arbel, 2024
                **************************************              */
#ifndef UARTCOM_H
#define UARTCOM_H

class uartcom { 

  public:
    uartcom(int pin); //pin reserved for future application
    void begin(double bdrate); //must be called from setup()
    void parseuart(); //must be in the loop()
    int getDeltaX(); //0 is on the object. right +, left -
    int getDeltaY();


  private:
    int _pin;
    
};
#endif 
