/**
 * redirects line to H 
 * using DDR reg with approx
 * 1 us delay 
 */
#define DQ_H() DDRD &= ~4


/**
 * redirects line to L
 * using DDR reg with approx
 * 1 us delay
 */ 
#define DQ_L() DDRD |= 4

// commands
#define START_COM     0xCC
#define START_MEASURE 0x44
#define REQ_MEASURE   0xBE


/**
 * @brief manually resets bus
 */
void busReset(){ 
  DQ_L();
  delay(1); // 1 ms delay
  DQ_H();
  delay(1);  
}


/**
 * @param _byte to write
 */
void sendByte(unsigned char _byte){
  cli(); // disable interrupt
  
  for ( int i = 0; i < 8; i++){
      if (_byte & 0x01) {  // current bit 1
      
        DQ_L();
        DQ_H();
        delayMicroseconds(100);   
      }else{ // current bit 0
         DQ_L();
         delayMicroseconds(90);
         DQ_H();
         delayMicroseconds(10);
      }
      _byte >>= 1; // shift
  }
  
  sei(); // enable interrupt
}

/**
 * @return read value
 */
unsigned char readByte(){  
  
  unsigned char read_byte = 0;
    
  cli();  // disable interrupt     
  for (int i = 0; i < 8; i++){
    read_byte >>= 1; // shift
    
     // to make sure 1us passed
     DQ_L();
     DQ_H();
     delayMicroseconds(8);
    
    // reading    
    if(PIND & 4) read_byte |= 128;
 
    delayMicroseconds(92);
  } 
  sei(); // enable interrupt
  
  return read_byte;
} 


/**
 * @return temperature 
 */
float readTemperature(){
  unsigned char lsb, msb;
  
  // bus reset, communication, measure
  busReset();
  sendByte(START_COM);
  sendByte(START_MEASURE);

  // wait for 1 s
  delay(1000);
  
  // bus reset, communication, request measure
  busReset();
  sendByte(START_COM);
  sendByte(REQ_MEASURE);
  
  // reading two bytes
  lsb = readByte();
  msb = readByte();
  
  // bytes to temperature convertion
  return ((float)((unsigned short)(lsb + msb*256)))/16.0; 
}


void setup() {
  Serial.begin(57600);
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);
}

void loop() {
   Serial.println(readTemperature());
}
