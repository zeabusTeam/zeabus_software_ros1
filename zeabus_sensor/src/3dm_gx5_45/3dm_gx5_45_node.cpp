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

#include	"tf/transform_datatypes.h" // this will use about quaternion

ros::Publisher pub_imu_data;

sensor_msgs::Imu imu_data;

void publish_IMU_data( const zeabus_sensor::3dm_gx4_45::IMU_DATA& data){
	imu_data.stamp = ros::Time::now(); // get time for use in message

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

	imu_data.orientation.w = original_quaternion.w()	
	imu_data.orientation.x = original_quaternion.x()	
	imu_data.orientation.y = original_quaternion.y()	
	imu_data.orientation.z = original_quaternion.z();

	imu_data.linear_acceleration.x = data.scaled_accelerometer[0];
	imu_data.linear_acceleration.y = data.scaled_accelerometer[2];
	imu_data.linear_acceleration.z = data.scaled_accelerometer[1];
	imu_data.angular_velocity.x = data.scaled_gyro[0];
	imu_data.angular_velocity.y = data.scaled_gyro[2];
	imu_data.angular_velocity.z = data.scaled_gyro[1];

	pub_imu_data.publish( imu_data );
}

int main( int argc , char **argv){

	ros::init( argc , argv , "node_imu");
	ros::NodeHandle nh("~"); // component for use roslib

	zeabus_sensor::3dm_gx4_45::IMU imu;

// declare variable for can get pass launch
	std::string publish_topic;
	std::string device;
	int baudrate;
	int imu_rate;

	nh.param<std::string>( "topic" , publish_topic , "imu/data");
	nh.param<std::string>( "device" , device , "/dev/ttyACM0");
	nh.param<int>("baudrate" , baudrate , 460800 );
	mh.param<int>("imu_rate" , imu_rate , 500);

	if( !imu.open_port( device , baudrate)){
		ROS_ERROR_STREAM("Cannot open : " << device);
		return -1;
	}
	else{
		ROS_INFO("Sucess open : " << device << " for IMU");
	}

	imu.ping();
	imu.idle();
	imu.select_baudrate( 460800);

	if( imu_rate > 500) imu_rate = 500;
	else if( imu_rate <= 0 ) imu_rate = 100;

	static const int decimation = (500/imu_rate);

	imu.set_imu_data_rate(	decimation
						,	zeabus_sensor::3dm_gx4_45::imu_data::SCALED_ACCELEROMETER 
						|	zeabus_sensor::3dm_gx4_45::imu_data::SCALED_GYRO
						|	zeabus_sensor::3dm_gx4_45::imu_data::CF_QUATERNION 
						|	0
						);

	imu.select_data_stream(	zeabus_sensor::3dm_gx4_45::data_stream::IMU_DATA 
						|	0
						);

	imu.resume();

	imu.initialize_filter_with_magneto();

	pub_imu_data = nh.advertise< sensor_msgs:IMU >(publish_topic , 1);

	imu.set_imu_data_call_back( publish_IMU_data );

	while( ros::ok() ){
		imu.receive_data_stream();
	}

	

}
