/*
  Arduino Slave for Raspberry Pi Master
  i2c_slave_ard.ino
  Connects to Raspberry Pi via I2C
  
  DroneBot Workshop 2019
  https://dronebotworkshop.com
*/

#define I2C_SLAVE_ADDRESS 11

// Include the Wire library for I2C
#include <Wire.h>

#include <SparkFun_TB6612.h>
#include <Servo.h>
//#include <studio.h>
Servo myservo;  // create servo object to control a servo

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
#define AIN1 8
#define AIN2 9

#define BIN1 12
#define BIN2 11

#define AIN3 33
#define AIN4 37

#define BIN3 25
#define BIN4 29

#define PWMA1 7
#define PWMB1 13

#define PWMA2 6
#define PWMB2 4

#define STBY1 10
#define STBY2 5

const int offSet = 1;

Motor motorUL = Motor(BIN3, BIN4, PWMB2, offSet, STBY2);
Motor motorUR = Motor(AIN3, AIN4, PWMA2, offSet, STBY2);
Motor motorLL = Motor(AIN1, AIN2, PWMA1, offSet, STBY1);
Motor motorLR = Motor(BIN1, BIN2, PWMB1, offSet, STBY1);

const int MotorDrive = 175;
const int StopLength = 300;

const int Speed = 200;
const int driveTime = 600;
int delayTime = 0;

// initialize iteger varibles:
int voidVar;
int ulSpeed;
int urSpeed;
int llSpeed;
int lrSpeed;
int hitBall;
int z =0;
int start = 0;
bool alreadyRotated = false;
// LED on pin 13
const int ledPin = 13; 
char recievedData[25] = {};
int r;
char ch;
int go = 0;
int *pointerDelayTime;
int *pStart;
int *dir;
int rotDirect = 000;
//******************************************************************

void setup() {
  // Join I2C bus as slave with address 8
  Serial.begin(9600);
  Wire.begin(I2C_SLAVE_ADDRESS);
  
  // Call receiveEvent when data received                
  Wire.onReceive(receiveEvent);
  
  // Setup pin 13 as output and turn LED off
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  myservo.attach(2);  // attaches the servo on pin 2 to the servo object
  myservo.write(47);  // sets the servo at initial 47 degree position
   
  // speeds are initially zero
  motorUL.drive(0);
  motorUR.drive(0);
  motorLL.drive(0);
  motorLR.drive(0);
}

// Function that executes whenever data is received from master
/////////////////////////////////////////////////////////////////
void receiveEvent(int howMany) {

  // initialize character arrays for reading in and parsing commands
  char VV[4] = {}; // needed because of problems reading in and converting the first command
  char UL[4] = {};
  char ULsign[1] = {};
  char UR[4] = {};
  char URsign[1] = {};
  char LL[4] = {};
  char LLsign[1] = {};
  char LR[4] = {};
  char LRsign[1] = {};
  char HB[2] = {}; // Hit Ball(1 = yes, 0 = no)
  char DT[5] = {}; // Delay Time, time to rotate in order to orient
  char GO[4] = {}; // tells arduino to start running the main loop
  char RD[4] = {}; //rotation direction of orientation, CW or CCW
  char message[99] = {};
  int i = 0;

  while (Wire.available()) {
    r = Wire.read();
    if (ch != -1) {
        ch = char(r);
    }    
    message[i] = ch;
    i++;
  }

  // iterators for parsing:
  int count = 1;
  int v = 0;
  int a = 0;
  int as = 0;
  int b = 0;
  int bs = 0;
  int c = 0;
  int cs = 0;
  int d = 0;
  int ds = 0;
  int e = 0;
  int f = 0;
  int g = 0;
  int h = 0;

  // iterate through command, to parse into seperate commands as character arrays
  for(int k = 0; k < i; k++){
    if(message[k] == ','){
          count = count + 1;
          k = k + 1;
    }
    if(count == 1){
      VV[a] = message[k];
      v = v + 1;
    }
    if(count == 2){
      UL[a] = message[k];
      a = a + 1;
    }
    if(count == 3){
      ULsign[as] = message[k];
      as = as + 1;
    }
    if(count == 4){
      UR[b] = message[k];
      b = b + 1;
    }
    if(count == 5){
      URsign[bs] = message[k];
      bs = bs + 1;
    }
    /*
    //if(count == 6){
      //LL[c] = message[k];
      //c = c + 1;
    //}
    //if(count == 7){
      //LLsign[cs] = message[k];
      //cs = cs + 1;
    //}
    //if(count == 8){
      //LR[d] = message[k];
      //d = d + 1;
    //}
    //if(count == 9){
      //LRsign[ds] = message[k];
      //ds = ds + 1;
    //}
    
    if(count == 6){
      HB[e] = message[k];
      e = e + 1;
    }
    */
    if(count == 6){
      DT[f] = message[k];
      f = f + 1;
    }
    if(count == 7){
      GO[g] = message[k];
      g = g + 1;
    }
    if(count == 8){
      RD[g] = message[k];
      h = h + 1;
    }
  }

  // assigns last index in each array to 0 , will be appended during conversion to integer
  VV[3] = 0;
  UL[3] = 0;
  UR[3] = 0;
  LL[3] = 0;
  LR[3] = 0;
  HB[1] = 0;
  DT[4] = 0;
  GO[3] = 0;
  RD[3] = 0;

  // initialize signs to positive
  int ulSign = 1;
  int urSign = 1;
  int llSign = 1;
  int lrSign = 1;

  // convert sign commands to negative as required:
  if (ULsign[0] == '1')
    ulSign = -1;
  if (URsign[0] == '1')
    urSign = -1;
  if (LLsign[0] == '1')
    llSign = -1;
  if (LRsign[0] == '1')
    lrSign = -1;

    

  // use sscanf to convert character arrays to integers:
  sscanf(VV, "%d", &voidVar);
  
  sscanf(UL, "%d", &ulSpeed);
  ulSpeed = ulSpeed*ulSign;

  sscanf(UR, "%d", &urSpeed);
  urSpeed = urSpeed*urSign;
  /*
  sscanf(LL, "%d", &llSpeed);
  llSpeed = llSpeed*llSign;

  sscanf(LR, "%d", &lrSpeed);
  lrSpeed = lrSpeed*lrSign;
  

  sscanf(HB, "%d", &hitBall);
  */

  sscanf(DT, "%d", &delayTime);
  Serial.print("delayTime in function: ");
  Serial.println(delayTime);
  
  pointerDelayTime = &delayTime;
  sscanf(GO, "%d", &go);
  Serial.println("go is: ");
  Serial.println(go);

  sscanf(RD, "%d", &rotDirect);
  Serial.print("rotDirect: ");
  Serial.println(rotDirect);
  dir = &rotDirect;

  if(go == 1){
    start = 1;
    pStart = &start;
  }
    
/*
  if (delayTime == 0){
    Serial.println("Drive");
    // set motor speeds:
    motorUL.drive(urSpeed);
    motorUR.drive(ulSpeed);
    motorLL.drive(ulSpeed);
    motorLR.drive(urSpeed);
  }
  else{
    Wire.end();
    Serial.println("else rotate");
    motorUL.drive(-150);
    motorUR.drive(150);
    motorLL.drive(-150);
    motorLR.drive(150);
    delay(delayTime);
    delayTime =0;
    Stop();
    }
  
  /*
  if (hitBall == 1){
    myservo.write(130);  // rotates servo to 130 degrees to extend hitter
    delay(550);
    myservo.write(47);  // sets servo back to 47 degree position
  }
  */

  // print values for testing:  
  //Serial.println(ulSpeed);
  //Serial.println(urSpeed);
  //Serial.println(llSpeed);
  //Serial.println(lrSpeed);
  //Serial.println(hitBall);
  //Serial.println(' ');  

  //int sum = ulSpeed + urSpeed + llSpeed + lrSpeed + hitBall;
   //Serial.println(sum); // proves that variables are actually integers
  //Serial.println(' ');
  
}

int firstTime = 1;
void loop() {
  if (*pointerDelayTime == 0 && firstTime == 1){
      motorUL.drive(100);
      motorUR.drive(100);
      motorLL.drive(100);
      motorLR.drive(100);
      delay(800);
      Stop();
        firstTime = 0;
  }
  else if(*pStart == 1){
    Serial.println("got inside start");
    if (*pointerDelayTime == 0 || *pointerDelayTime == 1){
      motorUL.drive(70);
      motorUR.drive(70);
      motorLL.drive(70);
      motorLR.drive(70);
    }
    else if(alreadyRotated == false && *pointerDelayTime != 0 && *pointerDelayTime != 1){
      alreadyRotated = true;
      Serial.print("else rotate ");
      Serial.print("delayTime in else: ");
      Serial.println(*pointerDelayTime);
      if(*dir == 898){
        motorUL.drive(-150);
        motorUR.drive(150);
        motorLL.drive(-150);   
        motorLR.drive(150);
        delay(*pointerDelayTime);
        delayTime = 0;
        Stop();
        
      }else{
        motorUL.drive(-150);
        motorUR.drive(150);
        motorLL.drive(-150);   
        motorLR.drive(150);
        delay(*pointerDelayTime);
        delayTime = 0;
        Stop();
      }
     
      }
    else{
      Stop();
      delay(1000);
    }
  }
}


void Rotate(){
  motorUL.drive(-150);
  motorUR.drive(150);
  motorLL.drive(-150);
  motorLR.drive(150);
}

void Stop() {
  brake(motorUL,motorLL);
  brake(motorUR,motorLR);
}
