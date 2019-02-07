/*
	File name			:	tf_localize_01.cpp
	Author				:	Supasan Komonlit	
	Date created		:	2019 , FEB 06
	Date last modified	:	2019 , ??? ??
	Purpose				:	For localize by DVL , IMU 1 , Pressure 

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Using

	Namespace			:	None
*/
//====================>

#include	<stdio.h>

#include	<ros/ros.h>

#include	<math.h>

#include	<tf/transform_listener.h>

#include	<zeabus_library/tf_handle/tf_quaternion.h>

#include	<zeabus_library/text_color.h>

#include	<zeabus_library/subscriber/SubImu.h>
#include	<zeabus_library/subscriber/SubOdometry.h>
#include	<zeabus_library/subscriber/SubTwistWithCovarianceStamped.h>

#define _LOOK_ROTATION_

#define _PRINT_ALL_QUATERNION_
#define _PRINT_CHECK_DATA_
#define _CLEAR_SCREEN_

int main( int argv , char** argc ){

	ros::init( argv , argc , "localize" );

	ros::NodeHandle ph("~");
	ros::NodeHandle nh("");

//====================> PARAMETER PART
	std::string publish_topic, frame_id , id_robot;
	std::string topic_imu , id_imu;
	std::string topic_dvl , id_dvl;
	std::string topic_pressure;
	int frequency;
	double aborted_value;

	ph.param< std::string >("publish_topic" , publish_topic , "/localize/state");
	ph.param< std::string >("topic_imu" , topic_imu , "/sensor/imu");
	ph.param< std::string >("topic_dvl" , topic_dvl , "/sensor/dvl");
	ph.param< std::string >("topic_pressure" , topic_pressure , "/sensor/pressure/node");
	ph.param< std::string >("id_imu" , id_imu , "imu");
	ph.param< std::string >("id_dvl" , id_dvl , "dvl");
	ph.param< std::string >("id_robot" , id_robot , "robot");
	ph.param< std::string >("frame_id" , frame_id , "world");
	ph.param< int >("frequency" , frequency , 50 );
	ph.param< double >("aborted" , aborted_value , 0.0005);
		
//====================> TRANSFORMATION PART
	bool success = false;

	zeabus_library::tf_handle::TFQuaternion imu_quaternion_robot;
	zeabus_library::tf_handle::TFQuaternion imu_quaternion;
	zeabus_library::tf_handle::TFQuaternion dvl_quaternion;
	
	tf::TransformListener listener; // variable for listen about transform
	tf::StampedTransform transform; // for receive data form listener

	printf( "Waiting receive rotation imu -> world ====> ");
	while( nh.ok() && (!success) ){
		try{
			listener.lookupTransform( frame_id , id_imu , ros::Time(0) , transform );
			zeabus_library::normal_green( " SUCCESS\n");
			success = true;
			imu_quaternion = transform.getRotation();
			break;
		}
		catch( tf::TransformException &ex) {
			zeabus_library::bold_red( ex.what() );
			printf("\n");
			printf("\tWaiting --");
			ros::Duration(1.0).sleep();
			printf(" -- try again ====>");
			continue;
		}
	}
	if( ! success ) return -1;	

	printf( "Waiting receive rotation dvl -> world ====> ");
	success = false;
	while( nh.ok() && (!success)){
		try{
			listener.lookupTransform( frame_id , id_dvl , ros::Time(0) , transform );
			zeabus_library::normal_green( " SUCCESS\n");
			success = true;
			dvl_quaternion = transform.getRotation();
			break;
		}
		catch( tf::TransformException &ex) {
			zeabus_library::bold_red( ex.what() );
			printf("\n");
			printf("\tWaiting --");
			ros::Duration(1.0).sleep();
			printf(" -- try again ====>");
			continue;
		}
	}
	if( ! success ) return -1;	

	printf( "Waiting receive rotation imu -> robot ====> ");
	success = false;
	while( nh.ok() && (!success)){
		try{
			listener.lookupTransform( id_robot , id_dvl , ros::Time(0) , transform );
			zeabus_library::normal_green( " SUCCESS\n");
			success = true;
			imu_quaternion_robot = transform.getRotation();
			break;
		}
		catch( tf::TransformException &ex) {
			zeabus_library::bold_red( ex.what() );
			printf("\n");
			printf("\tWaiting --");
			ros::Duration(1.0).sleep();
			printf(" -- try again ====>");
			continue;
		}
	}
	if( ! success ) return -1;	
#ifdef _LOOK_ROTATION_
	printf("imu_quaternion : [%8.3lf%8.3lf%8.3lf%8.3lf]\n" , imu_quaternion.x() 
			, imu_quaternion.y() , imu_quaternion.z() , imu_quaternion.w() );
	printf("dvl_quaternion : [%8.3lf%8.3lf%8.3lf%8.3lf]\n" , dvl_quaternion.x() 
			, dvl_quaternion.y() , dvl_quaternion.z() , dvl_quaternion.w() );
#endif

//====================> SET UP ROS OPERATING SYSTEM
	nav_msgs::Odometry state;
	sensor_msgs::Imu imu_data; 
	geometry_msgs::TwistWithCovarianceStamped dvl_data; 
	nav_msgs::Odometry pressure_data;
	int received_dvl = 0 ,received_imu = 0 , received_pressure = 0;

	zeabus_library::subscriber::SubImu listener_imu( &imu_data );
	zeabus_library::subscriber::SubTwistWithCovarianceStamped listener_dvl( &dvl_data );
	zeabus_library::subscriber::SubOdometry listener_pressure( &pressure_data );
	
	listener_dvl.register_ttl( &received_dvl , 1 );
	listener_imu.register_ttl( &received_imu , 1 );
	listener_pressure.register_ttl( &received_pressure , 1 );
	state.header.frame_id = frame_id;
	ros::Subscriber sub_dvl = nh.subscribe( topic_dvl , 1 
			, &zeabus_library::subscriber::SubTwistWithCovarianceStamped::callback_ttl
			, &listener_dvl );
	ros::Subscriber sub_imu = nh.subscribe( topic_imu , 1 
			, &zeabus_library::subscriber::SubImu::callback_ttl
			, &listener_imu );
	ros::Subscriber sub_pressure = nh.subscribe( topic_pressure , 1 
			, &zeabus_library::subscriber::SubOdometry::callback_ttl
			, &listener_pressure );

	ros::Publisher pub_state = nh.advertise< nav_msgs::Odometry >( publish_topic , 1 );

	ros::Rate rate( frequency );
	double period_time = 1/frequency;

	// collect quaternion of robot in world frame <current quaternion>
	zeabus_library::tf_handle::TFQuaternion state_quaternion;
	// collect quaternion of imu in world rame
	zeabus_library::tf_handle::TFQuaternion receive_quaternion;

//====================> Loop in ROS Oeprate
	state.pose.pose.position.x = 0;
	state.pose.pose.position.y = 0;
	state.pose.pose.position.z = 0;
	double adding_x = 0;
	double adding_y = 0;
	bool setup_state_quaternion = false;
	#ifdef _PRINT_ALL_QUATERNION_
		double temp_euler[3];
	#endif
	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		//====================> Check call back and get value
		if( received_imu ){
			receive_quaternion = zeabus_library::tf_handle::TFQuaternion( imu_data.orientation.x
					, imu_data.orientation.y , imu_data.orientation.z , imu_data.orientation.w );
			received_imu = 0;
			state_quaternion = imu_quaternion * receive_quaternion;
			setup_state_quaternion = true;
			// set up quaternion of current robot
			state.pose.pose.orientation.x = state_quaternion.x();
			state.pose.pose.orientation.y = state_quaternion.y();
			state.pose.pose.orientation.z = state_quaternion.z();
			state.pose.pose.orientation.w = state_quaternion.w();
			// set up angular twist of current robot
			state.twist.twist.angular = imu_quaternion_robot.rotation(imu_data.angular_velocity);
		}
		if( received_dvl && setup_state_quaternion ){
			state.twist.twist.linear = state_quaternion.rotation( 
					dvl_quaternion.rotation( dvl_data.twist.twist.linear ) );
			state.twist.twist.linear.x /= 1000;
			state.twist.twist.linear.y /= 1000;
			state.twist.twist.linear.z /= 1000;
			received_dvl = 0;
			// get data linear twist in world frame already can localize now
		}
		if( received_pressure ){
			state.pose.pose.position.z = pressure_data.pose.pose.position.z;
			received_pressure = 0;
		}
		//	Now we have quaternion for rotation 2 part
		//		part 1 is rotation about frame to world frame
		//		part 2 is rotation about yaw to axis yaw	
		adding_x = state.twist.twist.linear.x * period_time;
		adding_y = state.twist.twist.linear.y * period_time;
		if( fabs(adding_x) > aborted_value ) state.pose.pose.position.x += adding_x;
		if( fabs(adding_y) > aborted_value ) state.pose.pose.position.y += adding_y;

		state.header.stamp = ros::Time::now();
		pub_state.publish( state );
	
		#ifdef _CLEAR_SCREEN_
			zeabus_library::clear_screen();
		#endif	
		#ifdef _PRINT_ALL_QUATERNION_
			
		#endif	
	}


}
