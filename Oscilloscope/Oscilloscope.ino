/**************************************************************
 *  Arduino DSO - 1 Channel (ready for multi-channel)
 * 
 *  4.1 kSa/s - Tempo reale
 *  16 MSa/s  - Tempo equivalente (random sampling)
 **************************************************************/

#define TAA     (30*128+1)      // sampling time
void setup() {

        Serial.begin(115200*2);   
        delay(1000);              
       
        TIMSK0 = 0;               // Disabilitiamo Arduino Timer 0 Interrupts
        TCCR1A = 0b00000000;      // No output compare
        TCCR1B = 0b01001000;      // No Noise-Canceler, Rising Edge, CTC, No-Clock
        OCR1A = TAA-1;            // Impostiamo la soglia A
        OCR1B = OCR1A-256;        // Impostiamo la soglia B
        
        TIFR1 = 32+4;             // Azzeriamo IF Comparatore + IF Overflow Timer 1
        TIMSK1 = 4;               // Abilitiamo In terrupt soglia B

        ACSR  = 0b00000100;       // Impostiamo il comparatore come sorgente di cattura
        DIDR1 = 0b00000011;       // Disabilitiamo gli ingressi digitali del comparatore
        
        analogReference(INTERNAL);  // Seleziona il riferimento interno
        analogRead(A2);             // lettura dummy per configurare l'ADC

        ADCSRA = 0b10100111;      // AD ON, Attiva trigger Hardware, Prescaler 1:128
        ADCSRB = 0b00000101;      // Seleziona Soglia B come trigger AD

        TCCR1B |= 1;              // Start Timer 1 clock 1:1
  
        analogWrite(11, 128); TCCR2B = 1;    // Square generator on pin 11, f = 31250 kHz
           
}

ISR(TIMER1_COMPB_vect) {}

void loop() {

    unsigned short TTRIG, KICR1, TMASK = 0, Vett[2];
    unsigned char NewTrigger = 0;
    while(1)
    {
            TIFR1 |= 2;                       // Clear IF soglia A          
            if (TIFR1 & 32)
            {
                TIFR1 |= 32;                  // cancello IF di input capture (trigger)
                TTRIG = ICR1;                 // acquisisco l'istante dell'ultimo trigger
                NewTrigger = 1;               // segnala che c’è un nuovo tempo di trigger              
            }
            while(!(TIFR1 & 2))               // Mentre aspetto la soglia A
            {
              if (TIFR1 & 32)                 // se c'è un evento di trigger
                if ((KICR1 = ICR1) > 1280)    // avvenuto prima della soglia A  
                {    
                    TIFR1 |= 32;              // cancello IF di input capture (trigger)
                    TTRIG = KICR1;            // e acquisisco il tempo
                    NewTrigger = 1;           // segnala che c’è un nuovo tempo di trigger
                } 
            }               

            
            TMASK ^= 0x4000;                  //  inverto il penultimo bit (non usato) di tempo
            if (NewTrigger)                   //  se abbiamo un nuovo tempo di trigger
            {                                 //  inverto l’ultimo bit (non usato) del campo
                TMASK ^= 0x8000;              //  tempo di trigger per consentire a PC di 
                NewTrigger = 0;               //  capire che si tratta di un nuovo trigger
            }
            ADCSRA |= 16;                     // Cancello il flag di AD       
            Serial.write((char *) Vett, 4);   // Mentre aspetto invio i dati precedenti
            while(!(ADCSRA & 16));            // Attendo il campione
            Vett[0] = ADC;                    // Preparo il vettore dati per il prossimo ciclo
            Vett[1] = TTRIG ^ TMASK;          // con il timestamp dell'ultimo trigger
            
    }
}
