/*
	File name			:	MIP_COMMUNICATION.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 27
	Date last modified	:	2018 , DEC 05
	Purpose				:	This is header of file collecting number of command for communication

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintain & Using

	Namespace			:	zeabus_sensor
*/


#ifndef _ZEABUS_LIBRARY_ZEABUS_SENSOR_MIP_COMMUNICATION_H__
#define _ZEABUS_LIBRARY_ZEABUS_SENSOR_MIP_COMMUNICATION_H__

namespace zeabus_sensor{

namespace MIP_COMMUNICATION{

	namespace COMMAND{
		
		namespace BASE{
			static const uint8_t DESCRIPTOR = 0x01;
			static const uint8_t PING = 0x01;
			static const uint8_t IDLE = 0x02;
			static const uint8_t GET_INFORMATION = 0x03;
			static const uint8_t GET_DESCRIPTOR = 0x04;

			static const uint8_t RESUME = 0x06;
			static const uint8_t EXTENDED_DEVICE_DESCRIPTOR = 0x07;
			static const uint8_t GPS_TIME_UPDATE = 0x72;
			static const uint8_t DEVICE_RESET = 0x7E;	
		}

		// use sensor instead 3dm because can't set number in front
		namespace SENSOR{
			static const uint8_t DESCRIPTOR = 0x0C;
			static const uint8_t POLL_IMU_DATA = 0x01;
			static const uint8_t POLL_GNSS_DATA = 0x02;
			static const uint8_t POLL_ESTIMATION_FILTER_DATA = 0x03;
			static const uint8_t GET_IMU_DATA_BASE_RATE = 0x06;
			static const uint8_t GET_GNSS_DATA_BASE_RATE = 0x07;
			static const uint8_t IMU_MESSAGE_FORMAT = 0x08;
			static const uint8_t GNSS_MESSAGE_FORMAT = 0x09;
			static const uint8_t ESTIMATION_FILTER_MESSAGE_FORMAT = 0x0A;
			static const uint8_t CONTINUOUS_DATA_STREAM = 0x11;
		}
	}

	namespace DATA{

		namespace IMU_DATA_SET{
			static const uint8_t DESCRIPTOR = 0x80;
			static const uint8_t SCALED_ACCELEROMETER_VECTOR = 0x04;
			static const uint8_t SCALED_GYRO_VECTOR = 0x05;
			static const uint8_t SCALED_MAGNETOMETER_VECTOR = 0x06;
			static const uint8_t SCALED_AMBIENT_PRESSURE = 0x17;
			static const uint8_t DELTA_THETA_VECTOR = 0x7;
			static const uint8_t DELTA_VELOCITY_VECTOR = 0x08;
			static const uint8_t CF_ORIENTATION_MATRXI = 0x09;
			static const uint8_t CF_QUATERNION = 0x0A;
			static const uint8_t CF_EULER_ANGLES = 0x0C;
			static const uint8_t CF_STABILIZED_MAG_VECTOR_NORTH = 0x10;
			static const uint8_t CF_STABILIZED_ACCEL_VECTOR_UP = 0x11;
			static const uint8_t GPS_CORRELATION_TIMESTAMP = 0x12;
		}
	}
}
}

#endif
