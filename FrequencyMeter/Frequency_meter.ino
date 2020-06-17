#define f_clk 16e6
#define Tgate_ms 1000

volatile bool Req;
volatile unsigned short NOVR, MLow, MHi, ResH, ResL;
volatile long NPCount, ResN;

/**
 * input service routine to count overflows
 * using global volatile var NOVR
 * @param None
 * @return None
 */
ISR(TIMER1_OVF_vect){
  NOVR ++ ;
}

/**
 * input service routine to 
 * save captured value and count ovfs
 * @param None
 * @return None
 */
ISR(TIMER1_CAPT_vect){
  MLow = ICR1;
  MHi = NOVR;
  NPCount ++;

  if(Req){
    ResH = MHi;
    ResL = MLow;
    ResN = NPCount;
    Req = false;
  }
}


/**
 * provides a reading, only accepted
 * if far from the overflow 
 * @param None
 * @return reading
 */
unsigned long request_value(){
  while(1){
    Req = true;
    while(Req); // ISR allows to exit
    if(ResL > 100) break; 
  }

  return (unsigned long) ResL + ((unsigned long) ResH << 16);
}

void setup() {
  Serial.begin(57600);
  Serial.println("Ready to measure");
  pinMode(8, INPUT);

  TCCR1A = 0b00000000; // no action on pins
  TCCR1B = 0B00000001; // normal mode, PSC 1:1

  TIFR1 |= 32 + 1; // clears IF capture and IF ovf flags
  TIMSK1 |= 32 + 1; // enables IE capture and IE ovf flags
}


void loop() {
  unsigned long m1, m2, m, N1, N2, N;

  m1 = request_value();
  N1 = ResN;
  
  delay(Tgate_ms);

  m2 = request_value;
  N2 = ResN;

  m = m2 - m1;
  N = N2 - N1;

  double freq = ((double) f_clk / (double) m) * ((double)N);

  Serial.println(freq, 5);
  delay(500);

}
