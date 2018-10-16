#!/usr/bin/python2.7
# import cv2
import rospy
from zeabus_vision.msg import vision_gate
from zeabus_vision.srv import vision_srv_gate
from std_msgs.msg import String

if __name__ == "__main__":
    rospy.init_node('call_service')
    service_name = 'vision_gate'
    print('wait service')
    rospy.wait_for_service(service_name)
    print('service start')
    call = rospy.ServiceProxy(service_name, vision_srv_gate)
    while not rospy.is_shutdown():
        try:
            res = call(String('gate'), String(''))
            # res = call(String('flare'),String('far'))
        except:
            print('Error')
        rospy.sleep(0.1)
        
