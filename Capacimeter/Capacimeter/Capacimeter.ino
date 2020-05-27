/*--------- TIMER 1 CONFIG macros ---------*/
/*sets CTC mode*/
#define CTC_mode()        TCCR1B = 0b00001000

/*compare threshold A - Pin Hi*/
#define threshold_A_Hi()  TCCR1A = 0b11000000 

/*compare threshold A - Pin Low*/
#define threshold_A_Low() TCCR1A = 0b10000000

/*forces transition*/
#define transec()         TCCR1C = 0b10000000

/*compare threshold B - Pin toggle*/
#define threshold_B_toggle() TCCR1A |= 0b00010000

/*Starts timer 1 setting prescaler to 1*/
#define startTIM1()    TCCR1B |= 1

/*Stops timer 1 setting prescaler to 0*/
#define stopTIM1()     TCCR1B &= ~ 1  

/*---------- ADC CONFIG macros -------------*/
/*enables trigger hardware and sets Prescaler 128*/
#define setTrigPsc()   ADCSRA =  0b10110111  

/*select Timer 1 threshold B*/
#define TIM1_threshold_B()  ADCSRB = 0b00000101

/*clears IF ADC reg*/
#define clr_IF_ADC()  ADCSRA |= 16

/*waits for IF ADC reg*/
#define wait_IF_ADC() while(!(ADCSRA & 16))


#define Rs    100000.0 // 100 kOhm
#define T     0.0004   // 400 us
#define thA   T*16e6  //threshold A
#define thB   10      // threshold B
#define START thA - 1  
#define SAMPLES 1000



void setupTIM1(){
  CTC_mode();
  threshold_A_Hi();
  transec();
  threshold_A_Low();
  threshold_B_toggle();
  
  // setting threshdolds and start
  setThresholdA(thA); 
  setThresholdB(thB);  
  setStart(START);
  
  enableInterruptB();
}


void setupADC(){
  analogRead(A2);
  
  // setting trigger and prescaler 
  setTrigPsc();

  // choosing compare match B timer 1
  TIM1_threshold_B();
}

/**
 * measures voltage clearing
 * IF ADC, waiting and then reading
 * V from ADC
 */
uint16_t readV(){
  clr_IF_ADC();
  wait_IF_ADC();
  
  uint16_t  V = ADC;
  
  return V;
}

/**
 * sets threshold A 
 * manipulating OCR1A register
 * @param threshold A
 */
void setThresholdA(unsigned int tA){
  OCR1A = tA;
}

/**
 * sets threshold B 
 * manipulating OCR1B register
 * @param threshold B
 */
void setThresholdB(unsigned int tB){
  OCR1B = tB;   
}


/**
 * sets start time  
 * manipulating TCNT1 register
 * @param start
 */
void setStart(unsigned int start){
  TCNT1 = start;
}


/**
 * enables interrupt for
 * threshold B manipulating
 * TIFR1 and TIMSK1 registers
 */
void enableInterruptB(){
  TIFR1 |= 4;
  TIMSK1 |= 4;   
}

/**
 * interrupt service routine
 * TIMER 1 
 */
ISR(TIMER1_COMPB_vect) {}



void setup() {
  // timer 1 setup
  Serial.begin(57600);
  pinMode(9, OUTPUT);

  setupTIM1();
  setupADC();
}

void loop() {
   uint16_t V0, V1;
   
   delay(10);
   startTIM1();
   
   // reading   
   V0 = readV();
   V1 = readV();

   stopTIM1();

   // computing capacity
   float C = T  / (Rs * log((float)V0/(float)V1));
  
   // dislaying C
   Serial.println("C = " + String(C* 1000000000.0) + " nF");

   // stop program
   while(1);
}
