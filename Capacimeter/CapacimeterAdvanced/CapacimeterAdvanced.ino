/*--------- TIMER 1 CONFIG macros -----------*/
/*enables CTC mode, no clock*/
#define CTC_mode() TCCR1B = 0b00001000

/*compare threshold A - Pin Hi*/
#define threshold_A_Hi() TCCR1A = 0b11010000

/*forces transition*/
#define transec()        TCCR1C |= 0b10000000

/*compare threshold A - Pin toggle*/
#define threshold_A_toggle() TCCR1A = 0b01010000

/*Starts timer 1 setting prescaler to 1*/
#define startTIM1()    TCCR1B |= 1
 

/*---------- ADC CONFIG macros -------------*/
/*ON, ADATE=1, IF Clear, IE = 0, Prescaler = 111 (1:128)*/
#define setTrigPsc()   ADCSRA =  0b10110111  

/*select Timer 1 threshold B*/
#define TIM1_threshold_B()  ADCSRB = 0b00000101

/*clears IF ADC reg*/
#define clr_IF_ADC()  ADCSRA |= 16

/*waits for IF ADC reg*/
#define wait_IF_ADC() while(!(ADCSRA & 16))


#define NPOINTS 151 // sampled points
#define DT 120      // time interval
#define thA 21013-1
#define thB 0
#define START thA - 1 


/**
 * @brief sets threshold A
 * @param value
 */
void setThresholdA(uint16_t tA){
  OCR1A = tA;
}


/**
 * @brief sets threshold B
 * @param value
 */
void setThresholdB(uint16_t tB){
  OCR1B = tB;
}


/**
 * @brief sets start time
 * @param value
 */
void setStart(uint16_t start){
  OCR1A = start;
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
 * setups timer 1
 * employing macros
 */
void setupTIM1(){
  CTC_mode();
  threshold_A_Hi();
  transec();
  threshold_A_toggle();
  
  // setting threshdolds and start
  setThresholdA(thA); 
  setThresholdB(thB);  
  setStart(START);
  
  enableInterruptB();
}


/**
 * setups ADC
 * employing macros
 * analogRead is a nice way
 * to let Arduino init registers
 * for you
 */
void setupADC(){
  analogRead(A2);
  
  // setting trigger and prescaler 
  setTrigPsc();

  // choosing compare match B timer 1
  TIM1_threshold_B();
}


/**
 * ISR Timer 1 
 */
ISR(TIMER1_COMPB_vect) {}



void setup() {
  Serial.begin(57600);
  pinMode(9, OUTPUT);

  setupTIM1();
  setupADC();
}




void loop() {
  unsigned short TAB = 0; // Memorizza il tempo TAB desiderato
  
  startTIM1();

  // ignores 200 samples (waiting for C to setup) 
  for(int d=0; d<200; d++) 
  {
    clr_IF_ADC();
    wait_IF_ADC();
  }

  //  samples acquisition loop
  while(1) 
  {
    TAB = 0; // Azzera il tempo TAB desiderato
    signed long NextB; // Variabile di supporto
    
    for(int r = 0; r < NPOINTS; r++) // Ciclo di acquisizione di NPOINTS punti
    {   
        // evaluating real threshold B
        NextB = (signed long) TAB - 255; 

        // correction on real threshold B
        if (NextB < 0) NextB += OCR1A + 1 ; 

        //sets real Threshold B
        setThresholdB(NextB);
      
        clr_IF_ADC();
        wait_IF_ADC();
        clr_IF_ADC();

        Serial.print(ADC); // Invia il campione al computer
        Serial.print(" "); // Invia uno spazio separatore al computer

        // waiting for next samples
        wait_IF_ADC();
        TAB += DT; // Incrementa il tempo TAB desiderato di DT
      }
    Serial.println(""); // Termina la linea di testo con campioni
  }
}
