/**

 15 Hz to 1kHz sine wave on 2R-R resistor ladder DAC.
 
 
 */
#include <util/delay_basic.h>


#define POT_MAX 12 // 1kHz

#define WAVE_SINE 0
#define WAVE_TRIANGLE 1
#define WAVE_SQUARE 2

#define MODE_FREQ 0
#define MODE_DUTY 1

uint8_t wave = WAVE_SINE;
uint8_t mode = MODE_FREQ;;
int sine[255];
int pot = 512;
int freq = pot;
int duty = pot;

void setup() 
{ 
    // Momentary switches
    pinMode(8, INPUT_PULLUP);
    pinMode(9, INPUT_PULLUP);
    pinMode(10, INPUT_PULLUP);
    pinMode(11, INPUT_PULLUP);
    // Toggle switch
    pinMode(12, INPUT_PULLUP);
  
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
  PORTC &= ~(1<<PC5); // Blue LED low (on: common anode)
  wave = WAVE_SINE;
  
  //PORTC &= ~(1<<PC3); // Red LED low (on: common anode)
  //PORTC &= ~(1<<PC4); // Green LED low (on: common anode)
  //PORTC &= ~(1<<PC5); // Blue LED low (on: common anode)

  
} 

void leds_off() {
  PORTC |= 1<<PC5; // LED high (off: common anode)
  PORTC |= 1<<PC4; // LED high (off: common anode)
  PORTC |= 1<<PC3; // LED high (off: common anode)
}

void loop() 
{ 
    // http://www.labbookpages.co.uk/electronics/debounce.html
    static uint8_t switches[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  
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
       
    // Read pot 
    pot = ADCL;  // read low byte first  
    pot |= ADCH << 8;  // then high
  
    
    if (pot < POT_MAX) {
      // Limit the max frequency as it gets tricky to control
      pot = POT_MAX; 
    }
   //pot=500; 
    if (wave == WAVE_SINE) {
      freq = pot; // no duty on the sine wave
    } else if (mode == MODE_DUTY) {
      duty = pot;
    } else {
      freq = pot; 
    }
    
  
  
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
    
    // arduino pin 11
    /*
    switches[3] <<= 1;
    bitWrite(switches[3], 0, bitRead(port_b, PINB3)); 
    */
    switches[3] = bitRead(port_b, PINB3);
    
    // arduino pin 12
    /*
    switches[4] <<= 1;
    bitWrite(switches[4], 0, bitRead(port_b, PINB4)); 
    */
    switches[4] = bitRead(port_b, PINB4);
    
    // Switch wave if needed.
    if (switches[0] == 0) {
        if (wave != WAVE_SINE) {
            leds_off();
            PORTC &= ~(1<<PC5); // Blue LED low (on: common anode)
            wave = WAVE_SINE; 
        }
    } else if (switches[1] == 0) {
        if (wave != WAVE_TRIANGLE) {
            leds_off();
            PORTC &= ~(1<<PC4); // Green LED low (on: common anode)
            wave = WAVE_TRIANGLE;
        }
    } else if (switches[2] == 0) {
        // currently not used
    } else if (switches[3] == 0) {
        if (wave != WAVE_SQUARE) {
            leds_off();
            PORTC &= ~(1<<PC3); // Red LED low (on: common anode)
            wave = WAVE_SQUARE; 
        }
    } else if (switches[4] == 0) {
        if (mode != MODE_FREQ) {
            mode = MODE_FREQ;
        }
    } else if (switches[4] == 1) {
        if (mode != MODE_DUTY) {
            mode = MODE_DUTY;
        }
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
  if (mode == MODE_DUTY) {
    // saw tooth 
    for (int i = 0; i < 255; i = i+2) { 
      PORTD = i; 
      _delay_loop_2(duty);      
    }
    for (int i = 255; i > 0; i = i-2) { 
      PORTD = i; 
      _delay_loop_2(duty / 8);  
    }
  } else {
    // triangle
    for (int i = 0; i < 255; i = i+2) { 
      PORTD = i; 
      _delay_loop_2(freq);      
    }
    for (int i = 255; i > 0; i = i-2) { 
      PORTD = i; 
      _delay_loop_2(freq);  
    }
  }
}

void wave_square()
{
  // 0 -> 65536
  if (mode == MODE_DUTY) {
    PORTD = 255;
    _delay_loop_2(freq * 64); // slow down to be in a similar range to the others
    PORTD = 0;
    _delay_loop_2(duty * 64); 
  } else {
    PORTD = 255;
    _delay_loop_2(freq * 64); // slow down to be in a similar range to the others
    PORTD = 0;
    _delay_loop_2(freq * 64); 
  }
}
