#!/usr/bin/python2.7
"""
    File name: imu_reader.py
    Author: skconan
    Date created: 2018/11/29
    Python Version: 2.7
    Description: use mscl lib for python
"""

import tf
import sys
import os
sys.path.append("/usr/share/python2-mscl")
import mscl
import rospy
from sensor_msgs.msg import Imu
import numpy as np
import math

class ImuReader:
    def __init__(self):
        rospy.init_node('ImuReader')
        
        ns = "zeabus_sensor"
        name = "imu"
        
        port = rospy.get_param(ns + "/" + name + "/port", "/dev/ttyACM2")
        baudrate = rospy.get_param(ns + "/" + name + "/baudrate", 115200)
        imu_rate = rospy.get_param(ns + "/" + name + "/imu_rate", 500)
        pub_topic = rospy.get_param(ns + "/" + name + "/pub_topic", "/gx4_45_imu/data")

        print("========= PARAM DATA ===========")
        print(port)
        print(baudrate)
        print(imu_rate)
        print(pub_topic)
        
        self.pub_imu = rospy.Publisher(pub_topic, Imu, queue_size=1)

        # create the connection
        self.connection = mscl.Connection.Serial(port, baudrate)
        sample_rate = imu_rate

        # create the InertialNode
        self.node = mscl.InertialNode(self.connection)

        # ping testing
        success = self.node.ping()

        if success:
            print('Connected')
        else:
            print('Unconnected')

        # put node to idle state
        self.node.setToIdle()

        self.ahrs_imu = mscl.MipChannels()

        self.ahrs_imu.append(mscl.MipChannel(
            mscl.MipTypes.CH_FIELD_SENSOR_STABILIZED_ACCEL_VEC, mscl.SampleRate.Hertz(sample_rate)))
        self.ahrs_imu.append(mscl.MipChannel(
            mscl.MipTypes.CH_FIELD_SENSOR_SCALED_ACCEL_VEC, mscl.SampleRate.Hertz(sample_rate)))
        self.ahrs_imu.append(mscl.MipChannel(
            mscl.MipTypes.CH_FIELD_SENSOR_SCALED_GYRO_VEC, mscl.SampleRate.Hertz(sample_rate)))

        # self.ahrs_imu.append(mscl.MipChannel(
        #     mscl.MipTypes.CH_FIELD_SENSOR_SCALED_MAG_VEC, mscl.SampleRate.Hertz(sample_rate)))
        # self.ahrs_imu.append(mscl.MipChannel(
        #     mscl.MipTypes.CH_FIELD_SENSOR_STABILIZED_MAG_VEC, mscl.SampleRate.Hertz(sample_rate)))

        self.ahrs_imu.append(mscl.MipChannel(
            mscl.MipTypes.CH_FIELD_SENSOR_EULER_ANGLES, mscl.SampleRate.Hertz(sample_rate)))
        self.ahrs_imu.append(mscl.MipChannel(
            mscl.MipTypes.CH_FIELD_SENSOR_ORIENTATION_QUATERNION, mscl.SampleRate.Hertz(sample_rate)))

        self.node.setActiveChannelFields(
            mscl.MipTypes.CLASS_AHRS_IMU, self.ahrs_imu)
        self.node.enableDataStream(mscl.MipTypes.CLASS_AHRS_IMU)

        self.node.resume()

    def streaming(self):
        self.data = {}
        self.linear_accel = [[0], [0], [0]]
        self.angular_accel = [[0], [0], [0]]
        self.rotation = [[0], [0], [0]]
        self.magnitude = [[0], [0], [0]]
        self.imu = Imu()

        while not rospy.is_shutdown():
            packets = self.node.getDataPackets(500)

            for packet in packets:
                packet.descriptorSet()  # the descriptor set of the packet
                # print(packet.descriptorSet())
                # time_stamp = packet.collectedTimestamp()      # the PC time when this packet was received
                self.imu.header.stamp = rospy.Time.now()    
                # print(time_stamp)

                # get all of the points in the packet
                points = packet.data()

                for dataPoint in points:
                    name = dataPoint.channelName()
                    try:
                        self.data[name] = dataPoint.as_float()
                    except:
                        if name == "orientQuaternion":
                            vect = dataPoint.as_Vector()
                            self.data[name] = [vect.as_floatAt(0),vect.as_floatAt(1),vect.as_floatAt(2),vect.as_floatAt(3)]
                        # print(dataPoint.storedAsType())

                # self.linear_accel[0].append(self.data['stabilizedAccelX'])
                # self.linear_accel[1].append(self.data['stabilizedAccelY'])
                # self.linear_accel[2].append(self.data['stabilizedAccelZ'])

                self.imu.linear_acceleration.x = self.data['stabilizedAccelX']
                self.imu.linear_acceleration.y = self.data['stabilizedAccelY']
                self.imu.linear_acceleration.z = self.data['stabilizedAccelZ']

                # self.imu.linear_acceleration.x = self.data['scaledAccelX']
                # # self.imu.linear_acceleration.y = self.data['scaledAccelY']
                # self.imu.linear_acceleration.z = self.data['scaledAccelZ']

                # self.angular_accel[0].append(self.data['scaledGyroX'])
                # self.angular_accel[1].append(self.data['scaledGyroY'])
                # self.angular_accel[2].append(self.data['scaledGyroZ'])

                self.imu.angular_velocity.x = self.data['scaledGyroX']
                self.imu.angular_velocity.y = self.data['scaledGyroY']
                self.imu.angular_velocity.z = self.data['scaledGyroZ']

                # self.magnitude[0].append(self.data['stabilizedMagX'])
                # self.magnitude[1].append(self.data['stabilizedMagY'])
                # self.magnitude[2].append(self.data['stabilizedMagZ'])
                self.imu.orientation.x = self.data['orientQuaternion'][0]
                self.imu.orientation.y = self.data['orientQuaternion'][1]
                self.imu.orientation.z = self.data['orientQuaternion'][2]
                self.imu.orientation.w = self.data['orientQuaternion'][3]

                # self.rotation[0].append(self.data['roll'])
                # self.rotation[1].append(self.data['pitch'])
                # self.rotation[2].append(self.data['yaw'])

                # self.imu.orientation.x = self.data[]

                self.pub_imu.publish(self.imu)

                print("\n================== Linear Accelerometer ===================")
                print("X %+.4f" % (self.imu.linear_acceleration.x))
                print("Y %+.4f" % (self.imu.linear_acceleration.y))
                print("Z %+.4f" % (self.imu.linear_acceleration.z))

                print("\n================== Angular Velocity =======================")
                print("Roll   %+.4f" % (self.imu.angular_velocity.x))
                print("Pitch  %+.4f" % (self.imu.angular_velocity.y))
                print("Yaw    %+.4f" % (self.imu.angular_velocity.z))

                print("\n================== Orientation Quaternion ==================")
                print("X %+.4f" % (self.imu.orientation.x))
                print("Y %+.4f" % (self.imu.orientation.y))
                print("Z %+.4f" % (self.imu.orientation.z))
                print("W %+.4f" % (self.imu.orientation.w))

                print("\n================== Rotation ================================")
                print("Roll  %+.4f" % math.degrees(self.data['roll']))
                print("Picth %+.4f" % math.degrees(self.data['pitch']))
                print("Yaw   %+.4f" % math.degrees(self.data['yaw']))

                euler = tf.transformations.euler_from_quaternion(
                    (self.imu.orientation.x,
                    self.imu.orientation.y,
                    self.imu.orientation.z,
                    self.imu.orientation.w,)
                )
                # print(euler)

                # print("\n================== MAGNITUDE ===================")
                # print("\nX %+.4f" % (self.magnitude[0][-1]))
                # print("Y %+.4f" % (self.magnitude[1][-1]))
                # print("Z %+.4f" % (self.magnitude[2][-1]))
                # rospy.sleep(0.01)
          
        self.node.setToIdle()
        self.connection.disconnect()


if __name__ == "__main__":
    imu = ImuReader()
    imu.streaming()
