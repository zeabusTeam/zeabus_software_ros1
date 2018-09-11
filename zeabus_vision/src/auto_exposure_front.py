#!/usr/bin/python2.7
import rospy
from auto_exposure import AutoExposure

def adjust():
    EVmin = 0.1
    EVdefault = 70
    sub_topic = rospy.get_param("/auto_exposure_front/topic_front", None)
    client_name = rospy.get_param("/auto_exposure_front/client_front", None)
    print sub_topic
    print client_name
    if sub_topic is not None:
        AEC = AutoExposure(sub_topic=sub_topic, client_name=client_name,
                           default_exposure_value=EVdefault, min_exposure_value=EVmin)
        AEC.adjust_exposure_value()


if __name__ == "__main__":
    rospy.init_node("Auto_Exposure_Front", anonymous=False)
    adjust()