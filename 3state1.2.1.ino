//############################3 state machine#####################################
//##this controls the leds on timers, fans on timers, and humidifier based on sensor data and lmits##
//##updates
//#
//##############################################
//#########     SAVE AS 1.2.1    #################
//##############################################
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
//
//    1/16/2022 #######
////      problem
//          fans still not working
//          extra vars
//        solution
//          remove humi millis and reduce led and fan millis to one var
//          organize variables and comments
//          remove reset periods from state machines 2 and 3--led and fans-- and restructure to case 1 on, case 2 off (leds 12 hours each, fans 2 min, then 13 min)
//        add room temperature, and tent temperature
//
//      1.2.1:
//          fan circuit not quite working properly and when s3 switched after 2 minutes, it would freeze the pi and the fan was also not starting up quite right.  Wire looks weird need to rewire
//        

//###################################################################################

#include <dht11.h>

//sensor pinouts

dht11 sensor;

#define H1HIGH 92
#define H1LOW 89

//relay pinouts
#define H1Relay 10
#define L1Relay 9
#define F1Relay 2
//#define H2Relay 12
//#define L2Relay 10
//#define F2Relay 3

int state_s1 = 0;
int state_s2 = 0;
int state_s3 = 0;

long t0 = 0;  //millis counter

long lastLoff = 0;  //led
long lastLon = 0; //led

long lastFoff = 0;  //fan
long lastFon = 0;  //fan

long t2m = 120000;  //
long t13m = 780000; //15 minutes in milli seconds
long t12h = 43200000;  //  12 hours in milli seconds



void setup() {
  Serial.begin(9600);

  pinMode(H1Relay, OUTPUT);
  pinMode(L1Relay, OUTPUT);
  pinMode(F1Relay, OUTPUT);
//  pinMode(H2Relay, OUTPUT);
//  pinMode(L2Relay, OUTPUT);
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
      digitalWrite(H1Relay, HIGH);    // h on
      
      Serial.println("s1: ");
      Serial.println(state_s1);
      Serial.println("Humi: ");
      Serial.println(sensor.humidity);
      delay(500);
      if (sensor.humidity > H1HIGH) {  //over limit, turn off
        state_s1 = 1;
      }
    break;



    case 1:  //dehumidify
      digitalWrite(H1Relay, LOW);     //h off
      
      Serial.println("s1: ");
      Serial.println(state_s1);
      Serial.println("Humi: ");
      Serial.println(sensor.humidity);
      delay(500);
      if (sensor.humidity < H1LOW) {   //under limit, turn on
        state_s1 = 0;
      }
    break;
    
  }
  
}





void SM_s2() {  //led state machine
  switch(state_s2) {
    case 0:                             //start-day
      digitalWrite(L1Relay, LOW);       //turn off led
      t0 = millis();                    //get time
      
      Serial.println("s2: ");
      Serial.println(state_s2);
      delay(500);
      if ((t0 - lastLoff) - t12h > 0) {  //over 12 hours
        state_s2 = 1;                  //change state variable before break so machine loads in to other case next time it runs
        lastLon = t0;                  //set last On time counter in order to be able to zero-offset the math
      }                  
    break;

    case 1:       
      digitalWrite(L1Relay, HIGH);   //switch led on                                               
      t0 = millis();                 //get time
      
      Serial.println("s2: ");
      Serial.println(state_s2);
      delay(500);
      if ((t0 - lastLon) - t12h > 0) {  //over 12 hours
        state_s2 = 0;                  //change state variable before break so machine loads in to other case next time it runs
        lastLoff = t0;                 //set last On time counter in order to be able to zero-offset the math
      }                        
    break;
      
    
  }
  
}


void SM_s3() {  //fan state machine --runs for 15 minute loops
  switch(state_s3) {
    case 0:                             //start-day
      digitalWrite(F1Relay, HIGH);      //turn on Fan
      t0 = millis();                    //get time
      
      Serial.println("s3: ");
      Serial.println(state_s3);
      delay(500);
      if ((t0 - lastFon) - t2m > 0) {    //over 2 minutes
        state_s3 = 1;                  //change state variable before break so machine loads in to other case next time it runs
        lastFoff = t0;                  //set last Off time counter (because this if statement is a step of turning it off) in order to be able to zero-offset the math
      }                  
    break;

    case 1:       
      digitalWrite(F1Relay, LOW);    //switch Fan off                                               
      t0 = millis();                 //get time
      
      Serial.println("s3: ");
      Serial.println(state_s3);
      delay(500);

      if ((t0 - lastFoff) - t13m > 0) {   //over 2 minutes
        state_s3 = 0;                  //change state variable before break so machine loads in to other case next time it runs
        lastFon = t0;                  //set last On time counter in order to be able to zero-offset the math
      }                        
    break;
      
    
  }
}



void loop() {
  
//  int chk = sensor.read(6);
//  Serial.println(sensor.temperature*(1.8)+32);
  
//  SM_s1();  /humi
  delay(100);
  SM_s2();  //led
  delay(100);
  SM_s3();  //fans
  delay(100);

}
