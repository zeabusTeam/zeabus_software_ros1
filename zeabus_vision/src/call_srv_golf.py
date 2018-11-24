#!/usr/bin/python2.7
import rospy
from zeabus_vision.msg import vision_drum
from zeabus_vision.srv import vision_srv_drum
from std_msgs.msg import String

if __name__ == "__main__":
    rospy.init_node('call_service')
    service_name = 'vision_drum'
    print('wait service')
    rospy.wait_for_service(service_name)
    print('service start')
    call = rospy.ServiceProxy(service_name, vision_srv_drum)
    i = 0
    while not rospy.is_shutdown():
        try:
            res = call(String('drum'), String('pick'))
            i += 1
            print('Calling {} times'.format(i))
        except:
            print('Error')
            i = 0
        rospy.sleep(0.05)
