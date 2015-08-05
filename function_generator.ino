/**

 15 Hz to 1kHz sine wave on 2R-R resistor ladder DAC.
 
 
 */
#include <util/delay_basic.h>


#define POT_MAX 12 // 1kHz

#define WAVE_SINE 0
#define WAVE_TRIANGLE 1
#define WAVE_SQUARE 2
// ... @todo mode switch

uint8_t wave = WAVE_SINE;
int sine[255];
int freq = 512;

void setup() 
{ 
    // Momentary switches
    pinMode(8, INPUT);
    pinMode(9, INPUT);
    pinMode(10, INPUT);
  
    // LEDs
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
  leds_off();
  PORTC &= ~(1<<PC3); // Blue LED low (on: common anode)
  wave = WAVE_SINE;
  
  //PORTC &= ~(1<<PC5); // Red LED low (on: common anode)
  //PORTC &= ~(1<<PC4); // Green LED low (on: common anode)
  //PORTC &= ~(1<<PC3); // Blue LED low (on: common anode)

  
} 

void leds_off() {
  PORTC |= 1<<PC5; // LED high (off: common anode)
  PORTC |= 1<<PC4; // LED high (off: common anode)
  PORTC |= 1<<PC3; // LED high (off: common anode)
}

void loop() 
{ 
    // http://www.labbookpages.co.uk/electronics/debounce.html
    static uint8_t switches[3] = {0xFF, 0xFF, 0xFF};
  
    switch (wave) {
        case WAVE_SINE:
            wave_sine();
            break;
        case WAVE_TRIANGLE:
            wave_triangle();
            break;
        case WAVE_SQUARE:
            wave_square();
            break;
    }
    //wave_sine();
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
  
    // Read the switches in one go
    uint8_t port_b = PINB; 

    // arduino pin 8
    /*
    switches[0] <<= 1; // Shift the variable towards the most significant bit
    // Set the least significant bit to the current switch value
    bitWrite(switches[0], 0, bitRead(port_b, PINB0)); 
    */
    switches[0] = bitRead(port_b, PINB0);
    
    // arduino pin 9
    /*
    switches[1] <<= 1;
    bitWrite(switches[1], 0, bitRead(port_b, PINB1)); 
    */
    switches[1] = bitRead(port_b, PINB1);
    
    // arduino pin 10
    /*
    switches[2] <<= 1;
    bitWrite(switches[2], 0, bitRead(port_b, PINB2)); 
    */
    switches[2] = bitRead(port_b, PINB2);
    
    // Switch wave if needed.
    if (switches[0] == 0) {
        if (wave != WAVE_SINE) {
            leds_off();
            PORTC &= ~(1<<PC3); // Blue LED low (on: common anode)
        }
        wave = WAVE_SINE; 
    } else if (switches[1] == 0) {
        if (wave != WAVE_TRIANGLE) {
            leds_off();
            PORTC &= ~(1<<PC4); // Green LED low (on: common anode)
        }
        wave = WAVE_TRIANGLE; 
    } else if (switches[2] == 0) {
        if (wave != WAVE_SQUARE) {
            leds_off();
            PORTC &= ~(1<<PC5); // Red LED low (on: common anode)
        }
        wave = WAVE_SQUARE; 
    }
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
