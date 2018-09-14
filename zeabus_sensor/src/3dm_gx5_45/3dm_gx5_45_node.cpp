/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: 3dm_gx5_45_node.cpp
//
//	Edit	: Aug 15 , 2018
//	Author	: Supasan Komonlit
//
//	Thanks	: Mahisorn Nov 21 , 2014
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	<ros/ros.h> // include for use publisher data from sensor

#include	<sensor_msgs/Imu.h> // include type message for using to send data

#include	<zeabus_sensor/3dm_gx5.h> // unknow what purpose of that

#include	"td/transform_datatypes.h" // this will use about quaternion

ros::Publisher pub_imu_data;

sensor_msgs::Imu message_imu;

void publish_IMU_data( const zeabus_sensor::IMU_data& data){
	message_imu.stamp = ros::Time::now(); // get time for use in message

// from data have to receive that is cal from frame of IMU this will change to frame of robot

	tf::Quaternion original_quaternion(	data.orientation_quaternion[1],
										data.orientation_quaternion[2],
										data.orientation_quaternion[3],
										data.orientation_quaternion[0]);

	tf::Matrix3x3 original_matrix( original_quaternion );
	
	double roll, pitch, yaw;

	original_matrix.getRPY( roll , pitch , yaw);

	// this line is important please change from frame IMU to Robot
	// Please Read 3dm_gx5_45_user_manal page 31 for understanding frame of imu

	orientation_quaternion.setRPY( roll , pitch , yaw + zeabus_sensor::PI  );
	
}
