#!/usr/bin/python2.7
import sys
sys.path.append('/home/skconan/imu_testing/python2-mscl')
import mscl
import numpy as np
import pdb

"""
Sets up a connection to the 3DM-GX4-25 Microstrain IMU
"""


class IMU:

    def __init__(self, sample_rate):
        # Expected data euler angles and angular rates
        self.data = {'time': mscl.Timestamp(), 'estRoll': 0.0, 'estPitch': 0.0, 'estYaw': 0.0, 'estAngularRateX': 0.0, 'estAngularRateY': 0.0, 'estAngularRateZ': 0.0}

        try:
            COM_PORT = "/dev/ttyACM2"
            self.connection = mscl.Connection.Serial(COM_PORT, 921600)
            self.node = mscl.InertialNode(self.connection)

            chs_filter = mscl.MipChannels()
            chs_filter.append(mscl.MipChannel(mscl.MipTypes.CH_FIELD_ESTFILTER_ESTIMATED_ORIENT_EULER, mscl.SampleRate.Hertz(sample_rate)))
            chs_filter.append(mscl.MipChannel(mscl.MipTypes.CH_FIELD_ESTFILTER_ESTIMATED_ANGULAR_RATE, mscl.SampleRate.Hertz(sample_rate)))

            self.node.setActiveChannelFields(mscl.MipTypes.CLASS_ESTFILTER, chs_filter)
            self.node.enableDataStream(mscl.MipTypes.CLASS_ESTFILTER)
            self.node.resume()
        except mscl.Error, e:
            print "Error:", e


    def read(self):
        self.update()
        euler = np.array([[self.data['estRoll']], [self.data['estPitch']], [self.data['estYaw']]])
        ang_rate = np.array([[self.data['estAngularRateX']], [self.data['estAngularRateY']], [self.data['estAngularRateZ']]])
        return self.data['time'], euler, ang_rate

    def update(self):
        packets = self.node.getDataPackets()
        #packets = self.node.getDataPackets(5, 1)
        for packet in packets:
            points = packet.data()
            self.data['time'] = packet.collectedTimestamp()
            for dataPoint in points:
                self.data[dataPoint.channelName()] = dataPoint.as_float()

    def __del__(self):
        self.node.setToIdle()
        self.connection.disconnect()

if __name__ == '__main__':
    imu = IMU(1)
    while True:
        val = imu.read()    
        print(val)