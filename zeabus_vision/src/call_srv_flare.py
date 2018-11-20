#!/usr/bin/python2.7
import rospy
from zeabus_vision.msg import vision_flare
from zeabus_vision.srv import vision_srv_flare
from std_msgs.msg import String

if __name__ == "__main__":
    rospy.init_node('call_service')
    service_name = 'vision/flare'
    print('wait service')
    rospy.wait_for_service(service_name)
    print('service start')
    call = rospy.ServiceProxy(service_name, vision_srv_flare)
    while not rospy.is_shutdown():
        try:
            res = call(String('flare'), String('near'))
            print ('near',res)
            rospy.sleep(0.1)
            res = call(String('flare'),String('far'))
            print ('far',res)
        except:
            print('Error')
        rospy.sleep(0.1)
