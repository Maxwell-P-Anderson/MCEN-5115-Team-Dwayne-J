#!/usr/bin/env python3
 
###############################################################################
# Program description: Receives coordinates as comma-separated integers from 
# Arduino, calulates destination coordinates and required motor speeds, then 
# sends motor speeds to arduino.
###############################################################################
 
import serial # Module needed for serial communication
import sys
import smbus2 as smbus#,smbus2
import time
import numpy as np
# Slave Address
I2C_SLAVE_ADDRESS = 11 #0x0b ou 11
Vector = []

# This function converts a string to an array of bytes.
def ConvertStringsToBytes(src):
  converted = []
  for b in src:
    converted.append(ord(b))
  return converted
  
  
# Set the port name and the baud rate. This baud rate should match the
# baud rate set on the Arduino.
# Timeout parameter makes sure that program doesn't get stuck if data isn't
# being received. After 1 second, the function will return with whatever data
# it has. The readline() function will only wait 1 second for a complete line 
# of input.
#ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
 
# Intialize the motorspeed integers we'll send to Arduino
void_var = 100
motor_UL = 130
sign_UL = 0 # 0 is positive, 1 is negative
motor_UR = 130
sign_UR = 0
motor_LL = 130
sign_LL = 0
motor_LR = 130
sign_LR = 0

hit_ball = 0
increase_speeds = 1

 
# Get rid of garbage/incomplete data
#ser.flush()
 
while (1):
        
    # Create the I2C bus
    I2Cbus = smbus.SMBus(1)
    with smbus.SMBus(1) as I2Cbus:
            
        cmd = str(void_var) + ',' + str(motor_UL) + ',' + str(sign_UL) + ',' + str(motor_UR) + ',' + str(sign_UR) + ',' + str(motor_LL) + ',' + str(sign_LL) + ',' + str(motor_LR)+ ',' + str(sign_LR)+ ',' + str(hit_ball)
        slaveAddress = I2C_SLAVE_ADDRESS
                
        BytesToSend = ConvertStringsToBytes(cmd)
        #print("Sent: " + str(cmd))
        #print(BytesToSend )
        I2Cbus.write_i2c_block_data(slaveAddress, 0x00, BytesToSend)
        time.sleep(0.5)
        
        if increase_speeds == 1:
            if motor_UL < 230:
                hit_ball = 0; # rests hit_ball to no
                motor_UL = motor_UL + 5
                motor_UR = motor_UR + 5
                motor_LL = motor_LL + 5
                motor_LR = motor_LR + 5
            else:
                increase_speeds = 0
        else:
            if motor_UL > 130:
                motor_UL = motor_UL - 5
                motor_UR = motor_UR - 5
                motor_LL = motor_LL - 5
                motor_LR = motor_LR - 5
            else:
                increase_speeds = 1
                hit_ball = 1
                
        #if motor_UL == 150:
            #hit_ball = 1
        #else:
            #hit_ball = 0
            
    #return 0
