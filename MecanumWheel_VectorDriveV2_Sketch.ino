#include <SparkFun_TB6612.h>
#include <Servo.h>

Servo servo_test;

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
#define AIN1 9
#define AIN2 8

#define BIN1 11
#define BIN2 12

#define AIN3 33
#define AIN4 37

#define BIN3 29
#define BIN4 25

#define PWMAMD1 7
#define PWMBMD1 13

#define PWMAMD2 6
#define PWMBMD2 4

#define STBYMD1 10
#define STBYMD2 5

const int offSetUL = 1;
const int offSetLL = 1;

const int offSetUR = 1;
const int offSetLR = 1;


Motor motorLL = Motor(AIN1, AIN2, PWMAMD1, offSetUL, STBYMD1);
  
Motor motorLR = Motor(BIN1, BIN2, PWMBMD1, offSetLL, STBYMD1);
  
Motor motorUR = Motor(AIN3, AIN4, PWMAMD2, offSetUR, STBYMD2);
  
Motor motorUL = Motor(BIN3, BIN4, PWMBMD2, offSetLR, STBYMD2);

const int StopLength = 500;

const int MotorSpeed = 150;

const int motorRPS = 5;

double ULspeed = 0;

double LLspeed = 0;

double LRspeed = 0;

double URspeed = 0;

void driveSpeeds(double vecAng, double vecMag, double turn);
void driveAll(int driveTime);

int var = 0;
int x = 0;
int y = 0;


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  Serial.println("Hello World");
  
  Serial.println("Driving Upper Left");
  motorUL.drive(150);
  delay(500);
  Stop();
  delay(StopLength);

  Serial.println("Driving Upper Right");
  motorUR.drive(150);
  delay(500);
  Stop();
  delay(StopLength);

  Serial.println("Driving Lower Left");
  motorLL.drive(150);
  delay(500);
  Stop();
  delay(StopLength);

  Serial.println("Driving Lower Right");
  motorLR.drive(150);
  delay(500);
  Stop();
  delay(StopLength);

  Serial.println("Testing??: ");
  forward(motorUL,motorUR,150);
  forward(motorLL,motorLR,150);
  delay(2000);
  Stop();
  delay(StopLength);

  servo_test.attach(13);
  servo_test.write(20);
  
}

void loop() {

  //Read in the desired vector from th radio

  //Parse command

  Serial.println("Generating Vec:");
  Serial.println(var);

  switch(var) {

    case 0:

      x = 10;
  
      y = 10;

      var = var + 1;
    
      break;

    case 1:

      x = -10;
  
      y = 10;

      var = var + 1;
       
      break;

    case 2:

      x = -10;
  
      y = -10;

      var = var + 1;
      
      break;

    case 3:
    
      x = 10;
  
      y = -10;

      var = 0;
      
      break;

    default:

      x = 10;

      y = 10;
      
      break;

  }

  Serial.print("X Coord: ");
  Serial.println(x);

  Serial.print("Y Coord: ");
  Serial.println(y);

  //Read current Angle of the robot from accelerometer
  Serial.print("Generating Random Robo Angle: ");
  double currAng = random(0,360);

  Serial.println(currAng);

  double vecAng = atan2(y,x);

  double vecMag = sqrt(y*y + x*x); // possible this is just = 255

  Serial.print("Vector Magnitude: ");
  Serial.println(vecMag);

  int driveTime = int(vecMag * motorRPS * 0.0485 * 100);

  Serial.print("Delay Duration: ");
  Serial.println(driveTime);

  //double turn = vecAng - currAng;
  double turn = 0;


  driveSpeeds(vecAng, vecMag, turn);
  driveAll(driveTime);
  
}

void Stop() {

  brake(motorUL,motorLL);
  brake(motorUR,motorLR);

}

void driveSpeeds(double vecAng, double vecMag, double turn) {

  ULspeed = (-sin(vecAng - 1/4 * PI) * vecMag + turn);

  LLspeed = (-sin(vecAng + 1/4 * PI) * vecMag + turn);

  LRspeed = (sin(vecAng - 1/4 * PI) * vecMag + turn);
  
  URspeed = (sin(vecAng + 1/4 * PI) * vecMag + turn);

  int a = abs(ULspeed);
  int b = abs(LLspeed);
  int c = abs(LRspeed);
  int d = abs(URspeed);

  int x = max(a,b);
  int y = max(c,d);

  int z = max(x,y);

  ULspeed = int((ULspeed / z) * 255);
  LLspeed = int((LLspeed / z) * 255);
  LRspeed = int((LRspeed / z) * 255);
  URspeed = int((URspeed / z) * 255);

  Serial.print("Motor Speeds: ");
  Serial.println(ULspeed);
  Serial.println(LLspeed);
  Serial.println(LRspeed);
  Serial.println(URspeed);
  

}

void driveAll(int driveTime) {

  motorUL.drive(ULspeed,driveTime);
  motorLL.drive(LLspeed,driveTime);
  motorLR.drive(LRspeed,driveTime);
  motorUR.drive(URspeed,driveTime);
  delay(driveTime);
  
}
