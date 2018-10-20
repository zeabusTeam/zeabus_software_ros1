#!/usr/bin/python2.7
"""
    File name: read_exposure.py
    Author: zeabus
    Date created: 2018/10/16
    Python Version: 2.7
"""



import rospy
import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
from statistic import Statistic
from sensor_msgs.msg import CompressedImage
from dynamic_reconfigure.client import Client 
import time

# bgr = None 
# sub_sampling = 0.4
# stat = Statistic()

# def image_callback(msg):
#     global bgr, sub_sampling
#     arr = np.fromstring(msg.data, np.uint8)
#     bgr = cv.resize(cv.imdecode(arr, 1), (0, 0),
#                     fx=sub_sampling, fy=sub_sampling)




def read_exposure():
    time_avg = []
    while not rospy.is_shutdown():
        start = time.time()
        client_name = "ueye_cam_nodelet_front"
        client = Client(client_name)
        params = {"exposure": 33.0}
        client.update_configuration(params)
        rospy.get_param("/" + str(client_name) + "exposure", None)
        stop = time.time() 
        duration = stop - start 
        print(duration)
        time_avg.append(duration)
    print("Duration between call exposure API: ")
    time_avg = np.array(time_avg)
    print('max:',time_avg.max())
    print('min:',time_avg.min())
    print('mean:',time_avg.mean())

if __name__ == '__main__':
    rospy.init_node('read_exposure', anonymous=False)
    read_exposure()
