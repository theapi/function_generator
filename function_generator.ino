
#include <util/delay_basic.h>

/*
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
*/

int sine[255];
int pot;
byte pot_h;
byte pot_l;

void setup() 
{ 
     pinMode(0, OUTPUT); 
     pinMode(1, OUTPUT); 
     pinMode(2, OUTPUT); 
     pinMode(3, OUTPUT); 
     pinMode(4, OUTPUT); 
     pinMode(5, OUTPUT); 
     pinMode(6, OUTPUT);
     pinMode(7, OUTPUT); 
    
     float x; 
     float y; 
     for(int i=0;i<255;i++)
     {
          x=(float)i;
          y=sin((x/255)*2*PI);
          sine[i]=int(y*128)+128;
     }
     
     /*
     // Faster analogRead()
     sbi(ADCSRA,ADPS2);
     cbi(ADCSRA,ADPS1);
     cbi(ADCSRA,ADPS0);
     */
     
  //set up continuous sampling of analog pin 0 (you don't need to understand this part, just know how to use it in the loop())
  
  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0); //set reference voltage
  //ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
     
     noInterrupts(); // more stable without interuptions
     

} 

void loop() 
{ 

     for (int i=0;i<255;i++) 
     { 
          PORTD=sine[i]; 
 
          
          //_delay_loop_1(255); // 80.6 Hz
          //_delay_loop_1(127); // 160 Hz
          //_delay_loop_1(63); // 312 Hz
          //_delay_loop_1(31); // 588 Hz
          _delay_loop_1(15); // 1.14 kHz
          //_delay_loop_1(7); // 2.00 kHz
          //_delay_loop_1(3); // 3.33 kHz
          
          //  no analogRead() 1.03kHz (Uno)
          
          //_delay_loop_1(100);
          //pot = ADCH; // get new value from A0 3.57kHz
          //analogRead(A0); // no delay = 229Hz (default analogRead speed = 35Hz)
     }
     
     // // get new value from A0
     pot_l = ADCL;
     pot_h = ADCH;  //6.25kHz
     //pot = 

}
