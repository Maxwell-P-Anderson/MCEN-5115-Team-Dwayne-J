#include <SparkFun_TB6612.h>
#include <Servo.h>

Servo servo_test;

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
#define AIN1 25
#define AIN2 27

#define BIN1 29
#define BIN2 31

#define AIN3 33
#define AIN4 35

#define BIN3 37
#define BIN4 39

#define PWMAMD1 7
#define PWMBMD1 5

#define PWMAMD2 11
#define PWMBMD2 9

#define STBYMD1 6
#define STBYMD2 10

const int offSetUL = 1;
const int offSetLL = 1;

const int offSetUR = 1;
const int offSetLR = 1;


Motor motorUL = Motor(AIN1, AIN2, PWMAMD1, offSetUL, STBYMD1);
  
Motor motorUR = Motor(BIN1, BIN2, PWMBMD1, offSetLL, STBYMD1);
  
Motor motorLL = Motor(AIN3, AIN4, PWMAMD2, offSetUR, STBYMD2);
  
Motor motorLR = Motor(BIN3, BIN4, PWMBMD2, offSetLR, STBYMD2);

const int MotorDrive = 500;
const int StopLength = 500;

const int MotorSpeed = 150;


void setup() {
  // put your setup code here, to run once:

  motorUL.drive(MotorSpeed);
  delay(250);
  Stop();
  delay(StopLength);

  motorUR.drive(MotorSpeed);
  delay(250);
  Stop();
  delay(StopLength);

  motorLL.drive(MotorSpeed);
  delay(250);
  Stop();
  delay(StopLength);

  motorLR.drive(MotorSpeed);
  delay(250);
  Stop();
  delay(StopLength);

  servo_test.attach(13);
  servo_test.write(20);
  
}

void loop() {

  Stop();
  delay(StopLength);

  driveRight(MotorSpeed);
  delay(MotorDrive);
  Stop();
  delay(StopLength);

  driveLeft(MotorSpeed);
  delay(MotorDrive);
  Stop();
  delay(StopLength);

  servo_test.write(170);
  delay(800);

  servo_test.write(20);
  delay(1500);

  driveForward(MotorSpeed);
  delay(MotorDrive);
  Stop();
  delay(StopLength);

  driveBackward(MotorSpeed);
  delay(MotorDrive);
  Stop();
  delay(StopLength);

  driveLFDiag(MotorSpeed);
  delay(MotorDrive);
  Stop();
  delay(StopLength);

  driveLBDiag(MotorSpeed);
  delay(MotorDrive);
  Stop();
  delay(StopLength);

  driveRFDiag(MotorSpeed);
  delay(MotorDrive);
  Stop();
  delay(StopLength);

  driveRBDiag(MotorSpeed);
  delay(MotorDrive);
  Stop();
  delay(StopLength);

  rotateCent(MotorSpeed,1);
  delay(MotorDrive);
  Stop();
  delay(StopLength);

  rotateCent(MotorSpeed,2);
  delay(MotorDrive);
  Stop();
  delay(StopLength);

  
}

void driveRight(int speed) {

  right(motorUL,motorUR, speed);
  left(motorLL,motorLR, speed);
  
}

void driveLeft(int speed) {

  left(motorUL,motorUR, speed);
  right(motorLL,motorLR, speed);
  
}

void driveForward(int speed) {

  forward(motorUL,motorLL, speed);
  forward(motorUR,motorLR, speed);
  
}

void driveBackward(int speed){

  back(motorUL,motorLL, speed);
  back(motorUR,motorLR, speed);
  
}

void driveLFDiag(int speed) {

  forward(motorUR,motorLL, speed);
  
}

void driveRFDiag(int speed) {
  
  forward(motorUL,motorLR, speed);
  
}

void driveLBDiag(int speed) {

  back(motorUR,motorLL, speed);
  
}

void driveRBDiag(int speed) {

  back(motorUL,motorLR, speed);
  
}

void rotateCent(int speed, bool dir) {

  if (dir) {
    forward(motorUL,motorLL, speed);
    back(motorUR,motorLR, speed);

  } else {

    back(motorUL,motorLL, speed);
    forward(motorUR,motorLR, speed);
    
  }
  
}

void rotateSL(int speed, bool Low) {

  if (Low) {
    forward(motorUR,motorLR, speed);

  } else {

    back(motorUR,motorLR, speed);
    
  }

}

void rotateSR(int speed, bool Low) {

  if (Low) {
    forward(motorUL,motorLL, speed);

  } else {

    back(motorUL,motorLL, speed);
    
  }

}

void rotateBow(int speed, bool dir) {

  if (dir) {
    motorLL.drive(speed);
    motorLR.drive(-speed);

  } else {
    motorLL.drive(-speed);
    motorLR.drive(speed);
    
  }
  
}

void rotateStearn(int speed, bool dir) {

  if (dir) {
    motorUL.drive(speed);
    motorUR.drive(-speed);

  } else {
    motorUL.drive(-speed);
    motorUR.drive(speed);
    
  }
  
}

void rotatePort(int speed, bool dir) {

  if (dir) {
    motorUR.drive(speed);
    motorLR.drive(-speed);

  } else {
    motorUR.drive(-speed);
    motorLR.drive(speed);
    
  }
 
}

void rotateStarboard(int speed, bool dir) {

  if (dir) {
    motorUL.drive(speed);
    motorLL.drive(-speed);

  } else {
    motorUL.drive(-speed);
    motorLL.drive(speed);
    
  }

}

void Stop() {

  brake(motorUL,motorLL);
  brake(motorUR,motorLR);

}
