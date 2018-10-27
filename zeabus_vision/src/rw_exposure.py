#!/usr/bin/python2.7
"""
    File name: rw_exposure.py
    Author: zeabus
    Date created: 2018/10/16
    Python Version: 2.7
"""


import rospy
import numpy as np
from dynamic_reconfigure.client import Client
import time


def rw_exposure():
    time_avg = []
    while not rospy.is_shutdown():
        start = time.time()
        client_name = "ueye_cam_nodelet_front"
        client = Client(client_name)
        params = {"exposure": 33.0}
        try:
            client.update_configuration(params)
        except KeyboardInterrupt:
            print("Duration between call exposure API: ")
            time_avg = np.array(time_avg)
            print('max:', time_avg.max())
            print('min:', time_avg.min())
            print('mean:', time_avg.mean())
            break
        rospy.get_param("/" + str(client_name) + "exposure", None)
        stop = time.time()
        duration = stop - start
        print(duration)
        time_avg.append(duration)


if __name__ == '__main__':
    rospy.init_node('read_exposure', anonymous=False)
    rw_exposure()
