#include "nu32dip.h" // constants, functions for startup and UART
#include <stdio.h>

float pos_to_percent(float pos){
    
    return 0;
}

int main(void) {
    NU32DIP_Startup();
    NU32DIP_YELLOW = 1;
    NU32DIP_GREEN = 1;
    _CP0_SET_COUNT(0);
    
    //OC1 and timer 3 frequency setup
    T3CONbits.TCS = 0;          //internal PBCLK
    PR3 = 29999;                //50Hz
    T3CONbits.TCKPS = 0b101;    //prescaler = 32
    RPA0Rbits.RPA0R = 0b0101;   //configure RPA0 to be OC1 pin (PIN2)
    OC1CONbits.OC32 = 0;        //set OC1 to use 16 bit timer
    OC1CONbits.OCTSEL = 1;      //use timer 3
    OC1CONbits.OCM = 0b110;     //set OC1 to PWM mode (fault pin disabled)
    OC1R = 0;
    OC1RS = 0;
    TMR3 = 0;                   // set the timer count to zero
    T3CONbits.ON = 1;           // turn Timer3 on and start counting
    OC1CONbits.ON = 1;          // enable OC1

    //A1 direction control
    TRISAbits.TRISA1 = 0;
    LATAbits.LATA1 = 1;
    
    
    //every 4 seconds sweep between 0 and 135
    //need to send it commands every 20ms, 50 times per second
    //make an array with positions at every time step
    //array length = 200
    double pwm[100];
    for(int j = 0; j < 100; j++){
        double jj = (double)j/100; 
      pwm[j] = (double)(((45 + (90*jj))*((double)5/180) + 7.5)/100) * PR3;
    }
//    //track time with 24MHz core timer
    int elapsed = 0;
//    
//    //core timer ticks between updates
   int T = (int)(0.02 * 24000000);
//    
    while (1) {
        _CP0_SET_COUNT(0);
        elapsed = 0;
        int i = 0;
        
        //first 2 seconds, sweep forwards 45 -> 135
        while(i < 100){
            //every 50ms
            elapsed = _CP0_GET_COUNT();
            
            if(elapsed > T){
                elapsed = 0;
                _CP0_SET_COUNT(0);
                if(i >= 100){
                    i = 0;
                }
                OC1RS = pwm[i];
                i += 1;
                //change OC1RS and reset elapsed
                char msg[50];
                sprintf(msg, "%f\r\n", pwm[i]);
                NU32DIP_WriteUART1(msg);
            }
        }
        int ii = 99;
        //next 2 seconds, sweep backwards 135 -> 45
        while(ii >= 0){
            elapsed = _CP0_GET_COUNT();
            if(elapsed > T){
                elapsed = 0;
                _CP0_SET_COUNT(0);
                if(ii < 0){
                    ii = 0;
                }
                OC1RS = pwm[ii];
                ii = ii - 1;
//                char msg[50];
//                sprintf(msg, "%f\r\n", pwm[ii]);
//                NU32DIP_WriteUART1(msg);
                //change OC1RS and reset elapsed
            }
        }
        
}
}