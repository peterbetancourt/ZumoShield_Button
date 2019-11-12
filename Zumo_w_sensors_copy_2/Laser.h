#ifndef LDS
#define LDS

class Lasers {

  public:
    //  Constructor
    Lasers(int L_pin, int R_pin);

    //  Methods
    void setAddy();
    void i2cScan();
    int sensF();
    int sensB();
    double sensL();
    double sensR();

    double getDistanceR();

  private:

    int _pinF;
    int _pinB;
    int _pinL;
    int _pinR;

};

#endif
