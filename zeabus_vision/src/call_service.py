#!/usr/bin/env python
import cv2
import rospy
from zeabus_vision.msg import *
from zeabus_vision.srv import *
from std_msgs.msg import String

if __name__ == '__main__':
    rospy.init_node('call_service')
    # serviceName = 'vision_slots'
    # serviceName = 'vision_casino_gate'
    serviceName = 'vision_flare'
    #serviceName = 'vision_cash_in'
    # serviceName = 'vision_buy_a_gold_chip'
    # serviceName = 'vision_path'
    # serviceName = 'vision_navigate'
    # serviceName = 'vision_squid'
    # binsrv = 'vision_bin'
    # serviceName = 'vision_shoot_craps'
    print('wait service')
    rospy.wait_for_service(serviceName)
    print('service start')
    # call = rospy.ServiceProxy(serviceName, vision_srv_buy_a_gold_chip)
    call = rospy.ServiceProxy(serviceName, vision_srv_flare)
    #call = rospy.ServiceProxy(serviceName, vision_srv_cash_in)
    # call = rospy.ServiceProxy(serviceName, vision_srv_shoot_craps)
    # call = rospy.ServiceProxy(serviceName, vision_srv_navigate)
    # call = rospy.ServiceProxy(serviceName, vision_srv_casino_gate)
    #call = rospy.ServiceProxy(serviceName, vision_srv_slots)

    # call = rospy.ServiceProxy(serviceName, vision_srv_path)
    # call = rospy.ServiceProxy(serviceName, vision_srv_default)
    while not rospy.is_shutdown():
        try :
            # res = call(String('Navigate'), String('bot'))
            # res = call(String('buy_a_gold_chip'), String('front'))
            # res = call(String('cash_in'), String('bin'))
            # res = call(String('squid'), String('b'))
            # res = call(String('bouy'), String('a'))
            # res = call(String('path'))
            # res = call(String('casino_gate'))
            # res = call(String('yellow_hole'),String('test'))
            # res = call(String('handle'),String('small'))
            res = call(String('flare'),String('near'))
            # res = call(String('cash_in'),String('bin'))
            # res = call(String('bouy'), String('g'))
            # res = call(String('shoot_craps'))
            # res = res.data
            # print res.x
            # print res.y
            # print res.appear
            print res
        except :
            print "error"
        rospy.sleep(0.1)
