/**

 15 Hz to 1kHz sine wave on 2R-R resistor ladder DAC.
 
 
 */
#include <util/delay_basic.h>


#define POT_MAX 12 // 1kHz

//#define PIN_SWITCH_A 16 // A2 (PC2 - PCINT10)
//#define PIN_SWITCH_B 17 // A3 (PC3 - PCINT11)
//#define PIN_SWITCH_C 18 // A4 (PC4 - PCINT12)

int sine[255];
int pot = 512;

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

     
  //set up continuous sampling of analog pin 0
  
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
     
} 

void loop() 
{ 
  //wave_sine();
  wave_triangle();
     
  // Read pot
  pot = ADCL;  // read low byte first  
  pot |= ADCH << 8;  // then high

  
  if (pot < POT_MAX) {
    // Limit the max frequency as it gets tricky to control
    pot = POT_MAX; 
  }

}

void wave_sine()
{
  for (int i = 0; i < 255; i++) { 
    PORTD = sine[i]; 
    _delay_loop_2(pot);
          
  }
}

void wave_triangle()
{
  for (int i = 0; i < 255; i = i+2) { 
    PORTD = i; 
    _delay_loop_2(pot);      
  }
  for (int i = 255; i > 0; i = i-2) { 
    PORTD = i; 
    _delay_loop_2(pot);      
  }

}

