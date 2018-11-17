#!/usr/bin/env python

import pygame
import rospy
import cv2 as cv
import numpy as np
from geometry_msgs.msg import Twist


'''
    Mode x|d , select d
    axis - thumb stick or fov stick


    Left hand buttons - hat value -> x,y ->
        1
    -1  0   1
        -1
    
    Right hand buttons - button 0 - 4
        press - true or 1
        release - false or 0

    Left thumb stick
        axis 0 <- -> = -1 to 1
        axis 1 u d   = -1 to 1

        -1
    -1  0   1
        1
    
    Right thumb stick
        axis 2
        axis 3
        same left thumb stick

'''


def display(joy_output):
    r, c = 500, 900
    c_half = c // 2
    r_half = r // 2

    display = np.zeros((r, c), np.uint8)
    display = cv.cvtColor(display, cv.COLOR_GRAY2BGR)

    font = cv.FONT_HERSHEY_SIMPLEX
    next_row = 40
    h = 25
    # cv.putText(display,'Backward:',(10,next_row + h), font, 1,(255,255,255),2,cv.LINE_AA)
    cv.putText(display, 'Backward:', (10, next_row + h),
               font, 1, (255, 255, 255), 2, cv.LINE_AA)
    cv.putText(display, ':Forward', (700, next_row + h),
               font, 1, (255, 255, 255), 2, cv.LINE_AA)
    
    if joy_output['linear_x'] > 0:
        cv.rectangle(display, (c_half, next_row), (c_half +
                                                   int(joy_output['linear_x'] * 200), next_row + h), (0, 255, 0), -1)
    elif joy_output['linear_x'] < 0:
        cv.rectangle(display, (c_half, next_row), (c_half +
                                                   int(joy_output['linear_x'] * 200), next_row + h), (0, 0, 255), -1)

    next_row += h + 10
    cv.putText(display, 'Left:', (10, next_row + h),
               font, 1, (255, 255, 255), 2, cv.LINE_AA)
    cv.putText(display, ':Right', (700, next_row + h),
               font, 1, (255, 255, 255), 2, cv.LINE_AA)
    if joy_output['linear_y'] < 0:
        cv.rectangle(display, (c_half, next_row), (c_half -
                                                   int(joy_output['linear_y'] * 200), next_row + h), (0, 0, 255), -1)
    elif joy_output['linear_y'] > 0:
        cv.rectangle(display, (c_half, next_row), (c_half -
                                                   int(joy_output['linear_y'] * 200), next_row + h), (0, 255, 0), -1)
    next_row += h + 10
    cv.putText(display, 'Z:', (10, next_row + h),
               font, 1, (255, 255, 255), 2, cv.LINE_AA)
    if joy_output['linear_z'] > 0:
        cv.rectangle(display, (c_half, next_row), (c_half +
                                                   int(joy_output['linear_z'] * 200), next_row + h), (0, 255, 0), -1)
    elif joy_output['linear_z'] < 0:
        cv.rectangle(display, (c_half, next_row), (c_half +
                                                   int(joy_output['linear_z'] * 200), next_row + h), (0, 0, 255), -1)
    next_row += h + 10
    cv.putText(display, 'Yaw:', (10, next_row + h),
               font, 1, (255, 255, 255), 2, cv.LINE_AA)
    if joy_output['angular_z'] > 0:
        cv.rectangle(display, (c_half, next_row), (c_half +
                                                   int(joy_output['angular_z'] * 200), next_row + h), (0, 255, 0), -1)
    elif joy_output['angular_z'] < 0:
        cv.rectangle(display, (c_half, next_row), (c_half +
                                                   int(joy_output['angular_z'] * 200), next_row + h), (0, 0, 255), -1)
    return display

def main():
    pygame.init()
    pygame.joystick.init()
    joy_module_status = bool(pygame.joystick.get_init())

    print('Initialize joy module status: %r' % joy_module_status)
    print('\n================ JOYSTICK Information ================')

    number_of_joy = pygame.joystick.get_count()

    if number_of_joy == 0:
        print('Please connect joy to computer or laptop')
    elif number_of_joy == 1:
        joy_id = 0
        joystick = pygame.joystick.Joystick(0)
    else:
        joy_id = int(input("Please select joy id 0 to " +
                           str(number_of_joy - 1) + ": "))
        joystick = pygame.joystick.Joystick(joy_id)

    joystick.init()
    joy_status = bool(joystick.get_init())
    joy_name = joystick.get_name()
    number_of_axes = joystick.get_numaxes()
    number_of_trackballs = joystick.get_numballs()
    number_of_buttons = joystick.get_numbuttons()

    number_of_hats = joystick.get_numhats()

    print('Initialize joy status: %r' % joy_status)
    print('Number of joy: %d' % number_of_joy)
    print("Joy ID: %d" % joy_id)
    print("Joy name: %s" % joy_name)
    print("Number of axes: %d" % number_of_axes)
    print("Number of trackballs: %d" % number_of_trackballs)
    print("Number of buttons: %d" % number_of_buttons)
    print("Number of hats: %d" % number_of_hats)

    axes = [0] * 4
    buttons = [0] * 12

    ready = False

    pub_cmd_vel = rospy.Publisher("/cmd_vel",Twist, queue_size=1)    

    while not rospy.is_shutdown():
        event = pygame.event.get()

        for i in range(5):
            buttons[i] = joystick.get_button(i)

        ready = buttons[4]
        axes[1] = joystick.get_axis(1)
        axes[2] = joystick.get_axis(2)

        joy_output = {
            'linear_x': axes[1] * -1 * ready,
            'linear_y': axes[2] * -1 * ready,
            'linear_z': buttons[1] * -1 * ready + buttons[3] * ready ,
            'angular_z': buttons[0] * -1 * ready + buttons[2] * ready
        }

        cmd_vel = Twist()
        cmd_vel.linear.x = joy_output['linear_x']
        cmd_vel.linear.y = joy_output['linear_y']
        cmd_vel.linear.z = joy_output['linear_z']
        cmd_vel.angular.x = 0
        cmd_vel.angular.y = 0
        cmd_vel.angular.z = joy_output['angular_z']

        pub_cmd_vel.publish(cmd_vel)
        
        joy_display = display(joy_output)
        cv.imshow('Zeabus Joy Display', joy_display)

        k = cv.waitKey(1) & 0xff
        if k == ord('q'):
            break

        if not ready:
            continue

        print(joy_output)
        ready = False
        rospy.sleep(0.1)

    cv.destroyAllWindows()


if __name__ == "__main__":
    rospy.init_node('JoyStick')
    main()
