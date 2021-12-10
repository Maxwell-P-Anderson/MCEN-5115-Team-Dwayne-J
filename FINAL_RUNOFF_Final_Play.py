#!/usr/bin/env python3
 
###############################################################################
# Program description: Receives coordinates as comma-separated integers from 
# Arduino, calulates destination coordinates and required motor speeds, then 
# sends motor speeds to arduino.
###############################################################################
 
import serial # Module needed for serial communication
import sys
import signal
import RPi.GPIO as GPIO
import smbus2 as smbus#,smbus2
import time   #Module needed for time keeping.25
import numpy as np #Module needed for storing vectors
import math as m

# Slave Address of motor Arduino
I2C_SLAVE_ADDRESS = 11 #0x0b ou 11

# MPU6050 Address
#mpu = mpu6050(0x68)

# Set the port name and the baud rate set on the Arduino.
# Timeout parameter makes sure that program doesn't get stuck if data isn't
# being received. After 1 second, the function will return with whatever data
# it has. The readline() function will only wait 1 second for a complete line 
# of input.
ser = serial.Serial('/dev/ttyACM0', 38400, timeout=1)


G_BUTTON_GPIO = 25
B_BUTTON_GPIO = 24
Y_BUTTON_GPIO = 16
R_BUTTON_GPIO = 12

# Intialize switches:
G_switch = 1 # Green (default)
B_switch = 0 # Blue mode

Y_switch = 0 # Play switch: 0 = OFF / standby
R_switch = 1 # Stop switch: change to 1 to stop and reset

# Functions for handling buttons:
def signal_handler(sig, frame):
    GPIO.cleanup()
    sys.exit(0)
    
def G_button_pressed_callback(channel):
    print("Green Button pressed!")
    global G_switch
    global B_switch
    G_switch = 1
    B_switch = 0
    
def B_button_pressed_callback(channel):
    print("Blue Button pressed!")
    global G_switch
    global B_switch
    G_switch = 0
    B_switch = 1
    
def Y_button_pressed_callback(channel):
    print("Yellow Button pressed!")
    global Y_switch
    global R_switch
    Y_switch = 1
    R_switch = 0
    
def R_button_pressed_callback(channel):
    print("Red Button pressed!")
    global Y_switch
    global R_switch
    Y_switch = 0
    R_switch = 1
    global i
    i = 1
    #correct_angle == True
# Initialize coordinates as integers:

# GREEN: Our Robot
GX = 260
GY = 180

# BLUE: Other Robot
BX = 260
BY = 180

# YELLOW: Ball (starts at center feild)
YX = 260
YY = 180

# DESTINATION coordinates
DX = 260
DY = 180

# ROBOT coordinates
RX = 260
RY = 180

# for calculating ball vector:
YX_old = YX
YY_old = YY

RX_old = 0
RY_old = 0
RX_new = 0
RY_new = 0

# Intialize the motorspeed integers we'll send to Arduino:
void_var = 100
motor_UL = 150
sign_UL = 0 # 0 is positive, 1 is negative
motor_UR = 150
sign_UR = 0
motor_LL = 150
sign_LL = 0
motor_LR = 150
sign_LR = 0

iDrive = 1

increase_speeds = 1  # only for testing

# Intialize the hit_ball integer we'll send to Arduino
hit_ball = 0 # 0 = don't hit, 1 = hit!

# Intialize in_goal_box variable
in_goal_box = 0 # 0 = out goal box, 1 = in goal box
leave_box = 0  # 0 = don't leave box, 1 = leave box

# Intialize time keeping variables
start_time = time.time()
end_time = time.time()

time1 = time.time()
time2 = time.time()

ball_time_old = time.time()
ball_time_new = time.time()


# This function converts a string to an array of bytes.
def ConvertStringsToBytes(src):
  converted = []
  for b in src:
    converted.append(ord(b))
  return converted

# Get rid of garbage/incomplete data
ser.flush()

# Create the I2C bus
I2Cbus = smbus.SMBus(1)

correct_angle = False
angle_new = 0
angle_requested = 0

motor_UL = 150
sign_UL = 0 # 0 is positive, 1 is negative
motor_UR = 150
sign_UR = 0
motor_LL = 150
sign_LL = 0
motor_LR = 150
sign_LR = 0


iter_one_x_coor = 0
iter_one_y_coor = 0
GY_previous = 0
GX_previous = 0
BY_previous = 0
BX_previous = 0
previous_iter_vector = 0
angle_new2 = 0
#angle_new3 = 0

i = 1
angle_requested_time = 0
direction = "nan"
if __name__ == '__main__':
    
    # Handles button pressing:
    GPIO.setmode(GPIO.BCM)
    
    # GREEN:
    GPIO.setup(G_BUTTON_GPIO, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.add_event_detect(G_BUTTON_GPIO, GPIO.FALLING, 
            callback = G_button_pressed_callback, bouncetime=80)
    # BLUE:
    GPIO.setup(B_BUTTON_GPIO, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.add_event_detect(B_BUTTON_GPIO, GPIO.FALLING, 
            callback = B_button_pressed_callback, bouncetime=80)
    # YELLOW:
    GPIO.setup(Y_BUTTON_GPIO, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.add_event_detect(Y_BUTTON_GPIO, GPIO.FALLING, 
            callback = Y_button_pressed_callback, bouncetime=80)
    # RED:
    GPIO.setup(R_BUTTON_GPIO, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.add_event_detect(R_BUTTON_GPIO, GPIO.FALLING, 
            callback = R_button_pressed_callback, bouncetime=80)
    
    signal.signal(signal.SIGINT, signal_handler)
    
    ############################
    ##  INFINITE WHILE LOOP:  ##
    ############################
    
    while (1):
        
        if (R_switch == 1): # play switch is OFF
            # wait for switch to be ON:
            print("Waiting to play...")
            
            with smbus.SMBus(1) as I2Cbus:
        
                cmd = str(void_var) + ',' + "000" + ',' + str(sign_UL) + ',' + "000"  + ',' + str(sign_UR) +  ',' + "0000" + ',' + "002" + ',' + direction + ',' + "0"    
                slaveAddress = I2C_SLAVE_ADDRESS
                                
                BytesToSend = ConvertStringsToBytes(cmd)
                #print("Sent: " + str(cmd))
                #print(BytesToSend )
                I2Cbus.write_i2c_block_data(slaveAddress, 0x00, BytesToSend)
                time.sleep(.5)
            
        elif (Y_switch == 1): # play switch is on
            print("Play ball!")
            while (correct_angle == False):
                if (R_switch == 1): # play switch is OFF
                    break
    
                #######################################
                ## READ IN COORDINATES FROM ARDUINO: ##
                #######################################
                
                # Read everything until the new line character
                # Convert the data from a byte into a string of type 'utf-8'
                line = ser.readline().decode('utf-8')
                
                # Take out the commas. Parse the string into a list.
                parsed = line.split(',')
                
                # rstrip() function removes trailing characters like
                # '\n' and '/r'. Also removes white space.
                parsed = [x.rstrip() for x in parsed]
                 
                # We know we need to receive 6 integers. This code helps with any loss
                # of data that might happen as data is transferred between Arduino
                # and the Raspberry Pi.
                if(len(parsed) > 5):
                    print(parsed)
                   
                    # We add the '0' character to the end of each item in the 
                    # parsed list. This makes sure that there are no empty
                    # strings in the list. Adding 0 makes sure that we have
                    # at least 6 string values we can convert into integers.
                    # Dividing by 10 removes the trailing 0 but it makes the integer a float.
                    # We then have to convert the float to an integer.
                    GX = int(int(parsed[0]+'0')/10)
                    GY = int(int(parsed[1]+'0')/10)
                    BX = int(int(parsed[2]+'0')/10)
                    BY = int(int(parsed[3]+'0')/10)
                    YX = int(int(parsed[4]+'0')/10)
                    YY = int(int(parsed[5]+'0')/10)
                    ball_time_old = ball_time_new
                    ball_time_new = time.time()
                    
                if G_switch == 1:
                    motor_UR == 100
                elif B_switch == 1:
                    motor_UR == 200
                
                if R_switch == 1:
                    motor_UR == 300
                    
                if (G_switch == 1):
                    RX = GX;
                    RY = GY;
                else:
                    RX = BX;
                    RY = BY;
                print("RX: " + str(RX))
                print("RY: " + str(RY))
                #print("BX: " + str(BX))
                #print("BY: " + str(BY))
                #print("YX: " + str(YX))
                #print("YY: " + str(YY))
                
            
                if i == 2:
                    print("In Iteration 2")
                    if (G_switch == 1):
                        iter_one_x_coor = GX;
                        iter_one_y_coor = GY;
                    else:
                        iter_one_x_coor = BX;
                        iter_one_y_coor = BY;

                    print("iter_one_x_coor:")
                    print(iter_one_x_coor)
                    print("iter_one_y_coor:")
                    print(iter_one_y_coor)
                    iDrive = 0
                    
                if i == 4:
                    print("In Iteration 4")

                    if (G_switch == 1):
                        iter_two_x_coor = GX;
                        iter_two_y_coor = GY;
                    else:
                        iter_two_x_coor = BX;
                        iter_two_y_coor = BY;
                    if iter_two_y_coor < iter_one_y_coor:
                        direction = "898"
                        print("CCW")
                    else:
                        direction = "989"
                        print("CW")
                    print("iter_two_x_coor" , iter_two_x_coor)
                    print("iter_two_y_coor" , iter_two_y_coor)
                    #over here
                    vector_1 = [(iter_two_x_coor - iter_one_x_coor), (iter_two_y_coor - iter_one_y_coor)]
                    if iter_two_x_coor <= 250:
                        vector_2 = [-1, 0]
                    if iter_two_x_coor > 250:
                        vector_2 = [1, 0]
                    
                    if m.isnan(np.linalg.norm(vector_1)):
                        i = 1;
                        break;
                    else:
                        unit_vector_1 = vector_1/np.linalg.norm(vector_1)
                        
                    if m.isnan(np.linalg.norm(vector_2)):
                        i = 1;
                        break;
                    else:
                        unit_vector_2 = vector_2/np.linalg.norm(vector_2)
            
                        
                    dot_product = np.dot(unit_vector_1, unit_vector_2)
                    angle_requested = abs(np.arccos(dot_product))
                    angle_requested_time = (angle_requested /6.28319)*2110
                    
                if (i>3): #done orienting!
                    
                    if (DX-10 < RX < DX+10 & DY-10 < RX < DY+10):
                        hit_ball = 1;
                    else:
                        hit_ball = 0;
                        
                    if (iter_two_x_coor < 260):
                        DX = YX - 15
                        if (YY > 180):
                            DY = YY + 7
                        else:
                            DY = YY - 7
                    else:
                        DX = YX + 15
                        if (YY > 180):
                            DY = YY + 7
                        else:
                            DY = YY - 7
                            
                    #BOUNDARIES
                    if DX < 30:
                        DX = 35
                    if DX > 490:
                        DX = 485
                    if DY < 10:
                        DY = 15
                    if DY > 350:
                        DY = 345
                        
                    print("DX: ")
                    print(DX)
                    print("DY: ")
                    print(DY)
                    
                    
                    vecAng = m.atan2(DY,DX)    
                    vecMag = m.sqrt(DY**2 + DX**2)
                    
                    turn  = 0
                    
                    if (-m.sin(vecAng - 1/4 * 3.14) * vecMag + turn) >= 0:
                        motor_UL = int(-m.sin(vecAng - 1/4 * 3.14) * vecMag)
                        sign_UL = 0 # 0 is positive, 1 is negative
                    else:
                        motor_UL = abs ( (-m.sin(vecAng - 1/4 * 3.14) * vecMag + turn))
                        sign_UL = 1 # 0 is positive, 1 is negative
                        
                    
                    if (-m.sin(vecAng + 1/4 * 3.14) * vecMag + turn) >= 0:
                        motor_UR = (-m.sin(vecAng + 1/4 * 3.14) * vecMag + turn)
                        sign_UR = 0 # 0 is positive, 1 is negative

                    else:
                        motor_UR = abs ((-m.sin(vecAng + 1/4 * 3.14) * vecMag + turn))
                        sign_UR = 1 # 0 is positive, 1 is negative
                        
                                   
                    if (m.sin(vecAng - 1/4 * 3.14) * vecMag + turn) >= 0:
                        motor_LL = (m.sin(vecAng - 1/4 * 3.14) * vecMag + turn)
                        sign_LL = 0 # 0 is positive, 1 is negative

                    else:
                        motor_LL = abs ((m.sin(vecAng - 1/4 * 3.14) * vecMag + turn))
                        sign_LL = 1 # 0 is positive, 1 is negative
                        
                    
                    if (m.sin(vecAng + 1/4 * 3.14) * vecMag + turn) >= 0:
                        motor_LR = (m.sin(vecAng + 1/4 * 3.14) * vecMag + turn)
                        sign_LR = 0 # 0 is positive, 1 is negative
                    else:
                        motor_LR = abs ((m.sin(vecAng + 1/4 * 3.14) * vecMag + turn))
                        sign_LR = 1 # 0 is positive, 1 is negative
                        
                        
                    z = max(motor_UL,motor_UR,motor_LL,motor_LR)
                    
                    motor_UL = int(motor_UL / z * 255)
                    
                    motor_UR = int(motor_UR / z * 255)
                    
                    motor_LL = int(motor_LL / z * 255)
                    
                    motor_LR = int(motor_LR / z * 255)
                    
                    
                    
                if m.isnan(angle_requested_time):
                    angle_requested_time = 0
                    i = 3
                    break;
                else:
                    angle_requested_time = int(int(angle_requested_time * 10)/10.0)

                print("angle_requested_time:" + str(angle_requested_time))
                print("angle requested: " + str(angle_requested*360/6.283185))
        
        
                ###############################
                ## SEND COMMANDS TO ARDUINO: ##
                ###############################
                angleTime = str(angle_requested_time)
                with smbus.SMBus(1) as I2Cbus:
                
                    print("i is: " + str(i))
                    if i == 2:
                        cmd = str(void_var) + ',' + str(motor_UL) + ',' + str(sign_UL) + ',' + str(motor_UR) + ',' + str(sign_UR) + ',' + '000' + angleTime + ',' + '000' + ',' + direction + ',' + "1"
                    elif 0 <= angle_requested_time <= 9:
                        cmd = str(void_var) + ',' + str(motor_UL) + ',' + str(sign_UL) + ',' + str(motor_UR) + ',' + str(sign_UR) + ',' + "000" + angleTime + ',' + "000" + ',' + direction + ',' + "0"
                        print("Speeds sent")
                    elif 9 < angle_requested_time <= 99:
                        cmd = str(void_var) + ',' + str(motor_UL) + ',' + str(sign_UL) + ',' + str(motor_UR) + ',' + str(sign_UR) + ',' + "00" + angleTime + ',' + "001" + ',' + direction + ',' + "0"
                        print("Time sent")
                        angle_requested_time = 0
                    elif 99 < angle_requested_time <= 999:
                        cmd = str(void_var) + ',' + str(motor_UL) + ',' + str(sign_UL) + ',' + str(motor_UR) + ',' + str(sign_UR) +  ',' + '0' + angleTime + ',' + "001"  + ',' + direction + ',' + "0"

                        print("Time sent")
                        angle_requested_time = 0
                    elif angle_requested_time > 999:
                        cmd = str(void_var) + ',' + str(motor_UL) + ',' + str(sign_UL) + ',' + str(motor_UR) + ',' + str(sign_UR) +  ',' + angleTime +  ',' + "001" + ',' + direction + ',' + "0"
                        print("Time sent")
                        angle_requested_time = 0
                        
                    slaveAddress = I2C_SLAVE_ADDRESS
                        
                    BytesToSend = ConvertStringsToBytes(cmd)
                    #print("Sent: " + str(cmd))
                    #print(BytesToSend )
                    I2Cbus.write_i2c_block_data(slaveAddress, 0x00, BytesToSend)
                    if i == 1:
                        time.sleep(.5)
                    else:
                        time.sleep(.5)
                i = i + 1

                ###########################################
                ## FINISHED SENDING COMMANDS TO ARDUINO! ##
                ###########################################
                
