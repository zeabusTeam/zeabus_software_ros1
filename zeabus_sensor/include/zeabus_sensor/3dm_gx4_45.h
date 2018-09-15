/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: imu_communication.cpp
//
//	Edit	: Aug 15 , 2018
//	Author	: Supasan Komonlit
//
//	Thanks	: Mahisorn Nov 21 , 2014
//			: 3dm_gx4.h
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	<boost/function.hpp>

#include	<zeabus_serial/serial.h>

#include	<bitset>

#include	<zeabus_sensor/imu_communication.h>

namespace zeabus_sensor{

	namespace 3dm_gx4_45{

		class IMU : public zeabus_serial::serial{

			public:
				IMU(){}
				virtual IMU(){}

// Base Command
				void ping();
				void idel();
				void resume();
				void update_gps_time();
				void get_device_information();
				void get_device_descriptor_sets();
				bool run_device_build_in_test();
				void reset();

				void select_baudrate( unsigned int baud);
				void set_imu_data_rate( unsigned int decimation
									,	const std::bitset<IMU_DATA::NUM_IMU_DATA>& sources);
				void select_data_stream( const std::bitset<3>& streams);

				void initialize_filter_with_magneto();

				void set_initial_attitude( float roll , float pitch , float yaw);

				void set_initial_heading( float heading );

				void receive_data_stream();

				void set_imu_data_call_back( const boost::function<void(const IMU_DATA &)>& cb){
					imu_data_call_back = cb;
				}

			protected:
				void send_packet( const MIP& p , int time_out);
				void send_and_receive_packet( const MIP& p);
				
				void process_packet();
				void process_imu_packet();
				void process_respond_packet();
		} 

	} 
}
