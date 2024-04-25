/*                 ******************
              Class for esp32-Cam communication
    Featuring: UART communication with Arduino UNO
                                      
                         
                By: Gal Arbel, 2024
                **************************************              */
#ifndef CAMUART_H
#define CAMUART_H

class camuart { 

  public:
    camuart(int ppin); 
    void begin(double bdrate);


  private:
    int _ppin;
};
#endif 