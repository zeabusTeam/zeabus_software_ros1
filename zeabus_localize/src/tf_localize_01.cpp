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
	std::string publish_topic, frame_id , robot_id;
	std::string imu_topic , imu_id;
	std::string dvl_topic , dvl_id;
	std::string pressure_topic;
	int frequency;
	double aborted_value;

	ph.param< std::string >("publish_topic" , publish_topic , "/localize/state");
	ph.param< std::string >("imu_topic" , imu_topic , "/sensor/imu");
	ph.param< std::string >("dvl_topic" , dvl_topic , "/sensor/dvl");
	ph.param< std::string >("pressure_topic" , pressure_topic , "/sensor/pressure");
	ph.param< std::string >("imu_id" , imu_id , "imu");
	ph.param< std::string >("dvl_id" , dvl_id , "dvl");
	ph.param< std::string >("robot_id" , robot_id , "robot");
	ph.param< std::string >("frame_id" , frame_id , "world");
	ph.param< int >("frequency" , frequency , 50 );
	ph.param< double >("aborted" , aborted_value , 0.0005);
		
//====================> TRANSFORMATION PART
	bool success = false;

	zeabus_library::tf_handle::TFQuaternion dvl_rotation_robot;
	zeabus_library::tf_handle::TFQuaternion imu_rotation_robot;
	zeabus_library::tf_handle::TFQuaternion robot_rotation_world;
	
	tf::TransformListener listener; // variable for listen about transform
	tf::StampedTransform transform; // for receive data form listener

	printf( "Waiting receive rotation imu -> robot ====> ");
	while( nh.ok() && (!success) ){
		try{
			listener.lookupTransform( robot_id , imu_id , ros::Time(0) , transform );
			zeabus_library::normal_green( " SUCCESS\n");
			success = true;
			imu_rotation_robot = transform.getRotation();
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

	printf( "Waiting receive rotation dvl -> robot ====> ");
	success = false;
	while( nh.ok() && (!success)){
		try{
			listener.lookupTransform( frame_id , dvl_id , ros::Time(0) , transform );
			zeabus_library::normal_green( " SUCCESS\n");
			success = true;
			dvl_rotation_robot = transform.getRotation();
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

	printf( "Waiting receive rotation robot -> world ====> ");
	success = false;
	while( nh.ok() && (!success)){
		try{
			listener.lookupTransform( frame_id , robot_id , ros::Time(0) , transform );
			zeabus_library::normal_green( " SUCCESS\n");
			success = true;
			robot_rotation_world = transform.getRotation();
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
	printf("IMU -> ROBOT   : "); imu_rotation_robot.print_radian(); printf("\n");
	printf("ROBOT -> WORLD : "); robot_rotation_world.print_radian(); printf("\n");
	printf("DVL -> ROBOT   : "); dvl_rotation_robot.print_radian(); printf("\n");
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
	ros::Subscriber sub_dvl = nh.subscribe( dvl_topic , 1 
			, &zeabus_library::subscriber::SubTwistWithCovarianceStamped::callback_ttl
			, &listener_dvl );
	ros::Subscriber sub_imu = nh.subscribe( imu_topic , 1 
			, &zeabus_library::subscriber::SubImu::callback_ttl
			, &listener_imu );
	ros::Subscriber sub_pressure = nh.subscribe( pressure_topic , 1 
			, &zeabus_library::subscriber::SubOdometry::callback_ttl
			, &listener_pressure );

	ros::Publisher pub_state = nh.advertise< nav_msgs::Odometry >( publish_topic , 1 );

	ros::Rate rate( frequency );
	double period_time = 1.0/frequency;

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
			state.twist.twist.angular = imu_rotation_robot.rotation( imu_data.angular_velocity );
			received_imu = 0;
		}
		if( received_pressure ){
			state.pose.pose.position.z = pressure_data.pose.pose.position.z;
			received_pressure = 0;
		}
		//	Now we have quaternion for rotation 2 part
		//		part 1 is rotation about frame to world frame
		//		part 2 is rotation about yaw to axis yaw	
		listener.lookupTransform( frame_id , robot_id , ros::Time(0) , transform );
		robot_rotation_world = transform.getRotation();
		state.twist.twist.linear = robot_rotation_world.rotation( 
				dvl_rotation_robot.rotation( dvl_data.twist.twist.linear ) );
		state.pose.pose.orientation = robot_rotation_world.get_quaternion();
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
			printf("IMU->ROBOT       :"); imu_rotation_robot.print_radian(); printf("\n"); 
			printf("DVL->ROBOT       :"); dvl_rotation_robot.print_radian(); printf("\n"); 
			printf("ROBOT->WORLD     :"); robot_rotation_world.print_radian(); printf("\n"); 
		#endif
		#ifdef _PRINT_CHECK_DATA_
			printf("PRESENT STATE OF ROBOT\n");
			printf("\nPOSITION       :%10.4lf%10.4lf%10.4lf\n" , state.pose.pose.position.x
					, state.pose.pose.position.y , state.pose.pose.position.z );
			printf("\nADDING DATA    :%10.4lf%10.4lf\n" , adding_x , adding_y );
			printf("\nVELOCITY LINEAR:%10.4lf%10.4lf%10.4lf\n" , state.twist.twist.linear.x 
					, state.twist.twist.linear.y , state.twist.twist.linear.z );
			printf("\nRoll Pitch YAW :"); robot_rotation_world.print_radian(); printf("\n");
			printf("\nVELOCITY RAD   :%10.4lf%10.4lf%10.4lf\n", state.twist.twist.angular.x
					, state.twist.twist.angular.y , state.twist.twist.angular.z );
		#endif	
	}


}
