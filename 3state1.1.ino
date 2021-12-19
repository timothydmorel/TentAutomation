//############################3 state machine#####################################
//##this controls the leds on timers, fans on timers, and humidifier based on sensor data and lmits##
//##updates
//#
//#
//#
//#   12/18/2021   ####   
//#   
//#   added print lines for status updates and turned humidifer state machine off and set it on all the time becuase I am using an inkbird and not the sensor yet
//#   problem
//       fan was not turning on
//    solution
//       altereed pt2 = t2 in case 3 of sm3, ad=nd added a new pt counter to reset on 15, because the 2 minute counter kept flipping and keeping pin LOW.
//#      added pt3 and pt3 for longer reset counters
//#
//###################################################################################

#include <dht11.h>

//sensor pinouts

dht11 sensor;

#define H1HIGH 94
#define H1LOW 85

//relay pinouts
#define H1Relay 10
//#define H2Relay 12
#define L1Relay 9
//#define L2Relay 10
#define F1Relay 3
//#define F2Relay 3

int state_s1 = 0;
int state_s2 = 0;
int state_s3 = 0;

long t1 = 0;  //led millis
long pt1 = 0;  //shorter reset
long pt4 = 0; //longer reset

long t0 = 0;  //humi millis
long pt0 = 0;

long t2 = 0; //fan millis
long pt2 = 0;  //shorter reset
long pt3 = 0;  //longer reset

long t4 = 120000;  //placeholder for 2 minutes or 13 minutes, flips
long t15 = 900000; //15 minutes in milli seconds
long t24 = 86400000;
long t12 = 43200000;



void setup() {
  Serial.begin(9600);

  pinMode(H1Relay, OUTPUT);
//  pinMode(H2Relay, OUTPUT);
  pinMode(L1Relay, OUTPUT);
//  pinMode(L2Relay, OUTPUT);
  pinMode(F1Relay, OUTPUT);
//  pinMode(F2Relay, OUTPUT);

  digitalWrite(H1Relay, HIGH);
  digitalWrite(L1Relay, LOW);
  digitalWrite(F1Relay, HIGH);
//  digitalWrite(H2Relay, LOW);
//  digitalWrite(L2Relay, LOW);
//  digitalWrite(F2Relay, LOW);
  
  
  }



void SM_s1() {  //humidity state machine

  switch (state_s1) {
    case 0:  //humidify
      digitalWrite(H1Relay, HIGH);
      Serial.println("Humi: ");
      Serial.println(sensor.humidity);

      if (sensor.humidity > H1HIGH) {
        state_s1 = 2;
      }
      Serial.println("s1: ");
      Serial.println(state_s1);
    break;



    case 1:  //dehumidify
      digitalWrite(H1Relay, LOW);
      Serial.println("s1: ");
      Serial.println(state_s1);
      Serial.println("Humi: ");
      Serial.println(sensor.humidity);

      if (sensor.humidity < H1LOW) {
        state_s1 = 1;
      }
    break;
    
  }
  
}





void SM_s2() {  //led state machine
  switch(state_s2) {
    case 0:  //start-day
      digitalWrite(L1Relay, LOW);   //turn off led
      pt4 = t1;                     //initializes at zero, gets set at the start of each day, so the hour-counter math tracks fresh each morning.
      Serial.println("s2: ");
      Serial.println(state_s2);

      state_s2 = 1;                  // and go to wait
    break;

    case 1:  //waiting                                                        
      t1 = millis();              //start counting
      Serial.println("s2: ");
      Serial.println(state_s2);
      if (t1 - pt4 - t24 > 0) {   //over 24 hours, check before 12 hours to avoid missing it entirely
        state_s2 = 0;
      }
      if (t1 - pt1 - t12 > 0) {  //over 12 hours
        state_s2 = 2;
      }
//      pt1 = t1;
      
    break;
      
    case 2:  //switching        //turn on and go back to waiting 
      digitalWrite(L1Relay, HIGH);   //switch led off
      Serial.println("s2: ");
      Serial.println(state_s2);
      state_s2 = 1;             //go back to waiting case
      
    break;
    
  }
  
}






//can i just use one millis counter, to avoid using an extra variable?  I guess the separate variables grab it at slightly different times, perhaps in such a fashion that the most recently grabbed value is more relevant to the calculation?  Perhaps replace each extra t1 w/ t0, and calc recently anyways.  

//set up a nested switch case in order to transition from two subtraction calculations to a single timer-limit calculation and a nested switch case which takes the value of the pin and performs the flip flop




void SM_s3() {  //fan state machine --runs for 15 minute loops
  switch(state_s3) {
    case 0:  //reset
      digitalWrite(F1Relay, HIGH);   //turn on Fan
      state_s3 = 1;                  // and go to wait
      pt3 = t2;
      Serial.println("s3: ");
      Serial.println(state_s3);
    break;


    case 1:  //wait                                                        
      t2 = millis(); 
      Serial.println("s3: ");
      Serial.println(state_s3);
     
      if (t2 - pt3 - t15 > 0) {   //over 15 mins
        state_s3 = 0;
      }
      if (t2 - pt2 - t4 > 0) {  //over 2 mins
        state_s3 = 2;
      }
      
    break;
      
    case 2:  //switching        //turn off and go back to waiting 
      digitalWrite(F1Relay, LOW);   
      Serial.println("s3: ");
      Serial.println(state_s3);
      pt2 = t2; 
      state_s3 = 1;
      
            
    break;

  }
}






//do I add an overall timer which will activate the reset pin of the board after, say, 30 days?  five days?  one day?
void loop() {
  
//  int chk = sensor.read(H1Pin);
  
//  SM_s1();  //humi
  SM_s2();  //led
  SM_s3();  //fans


}
