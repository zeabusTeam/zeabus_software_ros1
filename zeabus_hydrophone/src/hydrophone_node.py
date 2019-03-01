#!/usr/bin/env python
from lowpass import h_lp
import scipy.optimize as opt
import hydrophone_serial
import numpy as np
from zeabus_hydrophone.msg import hydro_msg
from bearingEstimator import bearingEstimator
import demodulation
import rospy
import matplotlib.pyplot as plt
from time import gmtime, strftime

if __name__ == '__main__':

    ########################## setting parameter #################################

    Sampling_rate = 191930
    Frequency = 40000
    SoundSpeed = 1500
    FrontThreshold = 0.5
    PowerThreshold = 0
    DelayObserve = 2000
    LNA_Gain = 1

    ##############################################################################

    s = hydrophone_serial.open_serial_port()
    hydrophone_serial.sent_dsp_param(s, Frequency, SoundSpeed, FrontThreshold, PowerThreshold, DelayObserve, LNA_Gain)

    fig, axis = plt.subplots(1, 1)
    plot_out = None

    rospy.init_node('HYDROPHONE_2018')
    pub = rospy.Publisher('hydro', hydro_msg, queue_size = 4)

    while not rospy.is_shutdown():

        sig, freq = hydrophone_serial.get_pulse_data(s)
        sig = sig[:, 800:1500]
        t = np.arange(sig.shape[1])
        if plot_out is None:
            plot_out = axis.plot(t, sig[0,:] , t, sig[1,:], t, sig[2,:], t, sig[3,:])
        else:
            for tt in range(4):
                plot_out[tt].set_data(t, sig[tt,:])

        axis.legend(["CH1", "CH2", "CH3", "CH4"])
        fig.canvas.draw()
        plt.show(False)
        plt.pause(0.01)

        complex_baseband = demodulation.demodulation(sig, Sampling_rate, Frequency)

        br = bearingEstimator(complex_baseband, np.array([[0, 0], [20, 0], [20, 20], [0, 20]]),
                            Frequency / 1000.0, Sampling_rate / 1000.0)

        br.delay_time_estimator(1)
        az, ev, az_init, ev_init = br.bearingEstimatorForT0(1, fine_tune=True)

        print("The estimated arrival angle:")
        print("Azimuth: %f, Elevation: %f" % (az, ev))
        print("#########################################################################################")

        az = np.asscalar(az)
        ev = np.asscalar(ev)

        data = hydro_msg()
        data.azi = az
        data.elv = ev
        data.freq = freq[0]

        print(data)

        ping_time = strftime("%d-%b-%Y-%H-%M-%S", gmtime())
        np.savetxt('../data/sig.txt' + ping_time, sig, fmt='%f')
        np.savetxt('../data/data.txt' + ping_time, data, fmt='%f')

        pub.publish(data)

    



