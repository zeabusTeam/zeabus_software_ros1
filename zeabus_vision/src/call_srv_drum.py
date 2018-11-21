#!/usr/bin/python2.7
import rospy
from zeabus_vision.msg import vision_drum
from zeabus_vision.srv import vision_srv_drum
from std_msgs.msg import String

if __name__ == "__main__":
    rospy.init_node('call_service')
    service_name = 'vision/drum'
    print('wait service')
    rospy.wait_for_service(service_name)
    print('service start')
    call = rospy.ServiceProxy(service_name, vision_srv_drum)
    while not rospy.is_shutdown():
        try:
            # res = call(String('drum'), String('pick'))
            # print ('pick',res)
            res = call(String('drum'),String('drop'))
            # print ('release',res)
            # res = call(String('drum'), String('pick'))
            #print ('test',res)
        except:
            print('Error')
        rospy.sleep(0.1)
