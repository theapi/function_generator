/**

 15 Hz to 1kHz sine wave on 2R-R resistor ladder DAC.
 
 
 */
#include <util/delay_basic.h>


#define POT_MAX 12 // 1kHz

//#define PIN_SWITCH_A 16 // A2 (PC2 - PCINT10)
//#define PIN_SWITCH_B 17 // A3 (PC3 - PCINT11)
//#define PIN_SWITCH_C 18 // A4 (PC4 - PCINT12)

int sine[255];
int freq = 512;

void setup() 
{ 
  pinMode(A5, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A3, OUTPUT);
  
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

     
  //Set up continuous sampling of analog pin 0
  
  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0); //set reference voltage
  //ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enable auto trigger
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
     
  noInterrupts(); // more stable without interuptions
  
  // LEDs off
  PORTC |= 1<<PC5; // LED high (off: common anode)
  PORTC |= 1<<PC4; // LED high (off: common anode)
  PORTC |= 1<<PC3; // LED high (off: common anode)
  
  //PORTC &= ~(1<<PC5); // Red LED low (on: common anode)
  //PORTC &= ~(1<<PC4); // Green LED low (on: common anode)
  //PORTC &= ~(1<<PC3); // Blue LED low (on: common anode)

  
} 

void loop() 
{ 
  wave_sine();
  //wave_triangle();
  //wave_square();
     
  // Read pot for frequency
  freq = ADCL;  // read low byte first  
  freq |= ADCH << 8;  // then high

  
  if (freq < POT_MAX) {
    // Limit the max frequency as it gets tricky to control
    freq = POT_MAX; 
  }
//freq=12;
}

void wave_sine()
{
  for (int i = 0; i < 255; i++) { 
    PORTD = sine[i]; 
    _delay_loop_2(freq);
          
  }
}

void wave_triangle()
{
  for (int i = 0; i < 255; i = i+2) { 
    PORTD = i; 
    _delay_loop_2(freq);      
  }
  for (int i = 255; i > 0; i = i-2) { 
    PORTD = i; 
    _delay_loop_2(freq);
    //_delay_loop_2(freq/8);  // adjust this for saw tooth    
  }

}

void wave_square()
{
  // 0 -> 65536
  PORTD = 255;
  _delay_loop_2(freq * 64); // slow down to be in a similar range to the others
  PORTD = 0;
  _delay_loop_2(freq * 64); 
  
}
