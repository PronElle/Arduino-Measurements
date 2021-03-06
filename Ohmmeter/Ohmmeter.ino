const char E24[25] = {10, 11, 12, 13, 15, 16, 19, 20, 22, 24, 27, 30, 33, 36, 39,
                      43, 47, 51, 56, 62, 68, 75, 82, 91, 100};
const float R_REF[3] = {470.0, 10e3, 220e3};
const char Mult[3] = {' ', 'k', 'M'};

#define NREF 3  // number of ranges
#define NM   40 // number of measures


/**
 * Displays E24 nominal value 
 * resistor 
 * @param resistor
 * @return None
 */
void display_Rnom(float Rx){
  float e = floor(log10(Rx));
  int p = (int)e % NREF;
  int q = (int)e / NREF; 
  float minE = 10e9;

  float c = Rx/pow(10, e - 1);
  char j = 0 ;
  
  for (int i = 0 ; i < 25; ++i){
    float delta = fabs((c - (float)E24[i])/c);
    if(delta < minE){
      minE = delta;
      j = i;
    }
  }

  if(j == 24){
      j = 0;
      e++;
      p = (int)e % NREF;
      q = (int)e / NREF;    
    }
    Serial.print("R = " + String((int)E24[j]*pow(10, p - 1), 1) + " "
    + String(Mult[q]) + "Ohm");
}




/**
 * averages measurements
 * from a specific pin
 * @param val to store result
 * @param analog pin to read from
 * @return None
 */
void avg_measure(float* vect_val, unsigned int pin){
  for(int i = 0; i < NM ; ++i){
    (*vect_val) += analogRead(pin);
    delay(1);
  }
  (*vect_val) /= 40.0;
}



void setup(){
  Serial.begin(57600);
  Serial.println("---Digital Arduino Ohmmeter---");
}



void loop(){
  
  float N[NREF] = {0}, P[NREF] = {0}, G[NREF] = {0}, E[NREF] = {0};
  int bestP;
  float Rx, dRx, bestE = 1e3;

  for(int i = 0; i < NREF; ++i){
     pinMode(2 + i, OUTPUT);
     digitalWrite(2+i , 1);
     delay(5);

      avg_measure(&G[i], A0);
      avg_measure(&N[i], A1);
      avg_measure(&P[i], A3 + i);

      N[i] -= G[i];
      P[i] -= G[i];

      pinMode(2 + i, INPUT);

      E[i] = P[i] / ((P[i] - N[i]) * N[i]);
      if (E[i] < bestE && E[i] > 0){
        bestE = E[i];
        bestP = i;
      }
  }

    Rx = R_REF[bestP] * N[bestP] / (P[bestP] - N[bestP]);

    bestE = bestE * 2.0 + 1.0/100;
    dRx = Rx * bestE; // absolute error

    if(Rx < 20e6){
      display_Rnom(Rx);
      Serial.print("\t\t\t R = " + String(Rx));
      Serial.print("+-" + String(dRx) + " Ohm" + "(" + 
                  String(bestE*100.0)  + "% )");
    } else
      Serial.print("insert resistor ... ");

      Serial.println("");
      delay(500); 
}
