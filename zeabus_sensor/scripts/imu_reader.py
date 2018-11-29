
#!/usr/bin/python2.7
"""
    File name: imu_reader.py
    Author: skconan
    Date created: 2018/11/29
    Python Version: 2.7
    Description: use mscl lib for python
"""

import sys
import os
PATH = os.path.dirname(os.path.abspath(__file__))
sys.path.append(PATH + '/python2-mscl')
import mscl
import matplotlib.pyplot as plt
import tf
import time


class Imu:
    def __init__(self):
        # create the connection
        connection = mscl.Connection.Serial("/dev/ttyACM0", 115200)
        sample_rate = 500
        # create the InertialNode
        self.node = mscl.InertialNode(connection)

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
        # self.ahrs_imu.append(mscl.MipChannel(
        #     mscl.MipTypes.CH_FIELD_SENSOR_SCALED_ACCEL_VEC, mscl.SampleRate.Hertz(sample_rate)))
        self.ahrs_imu.append(mscl.MipChannel(
            mscl.MipTypes.CH_FIELD_SENSOR_SCALED_GYRO_VEC, mscl.SampleRate.Hertz(sample_rate)))

        # self.ahrs_imu.append(mscl.MipChannel(
        #     mscl.MipTypes.CH_FIELD_SENSOR_SCALED_MAG_VEC, mscl.SampleRate.Hertz(sample_rate)))
        self.ahrs_imu.append(mscl.MipChannel(
            mscl.MipTypes.CH_FIELD_SENSOR_STABILIZED_MAG_VEC, mscl.SampleRate.Hertz(sample_rate)))
        self.ahrs_imu.append(mscl.MipChannel(
            mscl.MipTypes.CH_FIELD_SENSOR_EULER_ANGLES, mscl.SampleRate.Hertz(sample_rate)))

        self.node.setActiveChannelFields(
            mscl.MipTypes.CLASS_AHRS_IMU, self.ahrs_imu)
        self.node.enableDataStream(mscl.MipTypes.CLASS_AHRS_IMU)

        self.node.resume()

    def streaming(self):
        self.data = {}
        self.linear_accel = [[0], [0], [0]]
        self.angular_accel = [[0], [0], [0]]
        self.magnitude = [[0], [0], [0]]

        while True:
            packets = self.node.getDataPackets(500)

            for packet in packets:
                packet.descriptorSet()  # the descriptor set of the packet
                # print(packet.descriptorSet())
                # packet.timestamp()      # the PC time when this packet was received

                # get all of the points in the packet
                points = packet.data()

                for dataPoint in points:
                    try:
                        # print(dataPoint.channelName())
                        self.data[dataPoint.channelName(
                        )] = dataPoint.as_float()
                    except:
                        pass

                self.linear_accel[0].append(self.data['stabilizedAccelX'])
                self.linear_accel[1].append(self.data['stabilizedAccelY'])
                self.linear_accel[2].append(self.data['stabilizedAccelZ'])

                self.angular_accel[0].append(self.data['scaledGyroX'])
                self.angular_accel[1].append(self.data['scaledGyroY'])
                self.angular_accel[2].append(self.data['scaledGyroZ'])

                self.magnitude[0].append(self.data['stabilizedMagX'])
                self.magnitude[1].append(self.data['stabilizedMagY'])
                self.magnitude[2].append(self.data['stabilizedMagZ'])

                print("\n================== ACCEL ===================")

                print("X %+.4f" % (self.linear_accel[0][-1]))
                print("Y %+.4f" % (self.linear_accel[1][-1]))
                print("Z %+.4f" % (self.linear_accel[2][-1]))

                print("\nRoll  %+.4f" % (self.angular_accel[0][-1]))
                print("Pitch  %+.4f" % (self.angular_accel[1][-1]))
                print("Yaw    %+.4f" % (self.angular_accel[2][-1]))

                print("\n================== MAGNITUDE ===================")
                print("\nX %+.4f" % (self.magnitude[0][-1]))
                print("Y %+.4f" % (self.magnitude[1][-1]))
                print("Z %+.4f" % (self.magnitude[2][-1]))


if __name__ == "__main__":
    imu = Imu()
    imu.streaming()
