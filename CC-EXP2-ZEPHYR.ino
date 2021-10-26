//Zephyr
//Creation & Computation: Experiment Two
//Joanne John
//References:
  // Intro to Servos: Cnavas Module- Nick Puckett
  // Time and Timing with Electronics: Canvas Module- Nick Puckett
  // Controlling the On-Board RGB LED with Microphone- Fabricio Troya (https://docs.arduino.cc/tutorials/nano-33-ble-sense/microphone_sensor)
  // Arduino TinyML Kit Tutorial #4- Robocraze (https://www.youtube.com/watch?v=7ZncQCU9H6w&ab_channel=Robocraze)
  // The genius of my cohort & their infinite patience when helping me :')


#include <Servo.h>
#include <PDM.h>

Servo servo1;
Servo servo2;

int LDRsensorPin = A6; 
int LDRsensorVal; //reading value of sensor

int pos = 0; //position of servo1
int pos2 = 0; //position of servo2

int triggerServo1 = 0; //makes sure servo 2 has been triggered by sound and is moving
int triggerLDR = 0; //checks if LDR has gotten the specified amount of light

int updateRate = 40; //rate at which loop is updated
int totalUpdates = 0; // total number of loops
long lastUpdate; //time since last loop update

// buffer to read samples into, each sample is 16-bits
short sampleBuffer[256];

// number of samples read
volatile int samplesRead;

void setup() {

  Serial.begin(9600);
  
  servo1.attach(6);
  servo1.write(0);
  servo2.attach(3);

  // configure the data receive callback
  PDM.onReceive(onPDMdata);

  // initialize PDM with:
  if (!PDM.begin(1, 16000)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }
  
}

void loop() {

  //trigger one: servo with lid
  if (samplesRead) {

    //print samples to the serial monitor or plotter
    for (int i = 0; i < samplesRead; i++) {
     
      //check if the sound value is higher than 550 & 2s has passed
      if (sampleBuffer[i]>550 && millis() > 2000){

          triggerServo1 = 1; 
          
     }    
    } 
   }

   //trigger one: servo with lid- part 2
   //move servo1 to 30deg if below conditionals are met
   if(millis() - lastUpdate>=updateRate && triggerServo1 == 1 && pos < 30){
    
      lastUpdate = millis();
      servo1.write(pos);
      pos = pos + 5;
      Serial.println("servo rotating PLSSS");
      Serial.println(pos);
  
    }


            
  // triggger two: servo with fan
  //check if position of servo1 is 30deg- if so rotate servo2 in 180deg turns
  if (pos == 30) {
    
      for(pos2 = 0; pos2 <= 180; pos2 +=1){
      servo2.write(pos2);    
      delay(15); 
      }

      for(pos2 = 180; pos2 >= 0; pos2 -= 1){
      servo2.write(pos2);
      delay(15);
      }
      
  }


  //trigger three: close container + stop fan from rotating
  
    //check values of LDR sensor
            
    LDRsensorVal = analogRead(LDRsensorPin);
    Serial.println(LDRsensorVal);


    // check if servo1 has already moved & LDR value is above 800 & position of servo1 is 30deg
    if (LDRsensorVal >= 800 && triggerServo1 == 1 && pos == 30){

        triggerLDR = 1;
        
    }


    //if servo2 has stopped moving, and servo1 has already moved: stop fan and close container
    if(triggerLDR == 1 && triggerServo1 == 1){

            servo2.write(0);
            
            lastUpdate = millis();
            servo1.write(pos);
            pos = pos - 5;
            Serial.println("servo closing PLSSS");
            Serial.println(pos);
          
     }  

     
}

  



void onPDMdata() {
  // query the number of bytes available
  int bytesAvailable = PDM.available();

  // read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}
