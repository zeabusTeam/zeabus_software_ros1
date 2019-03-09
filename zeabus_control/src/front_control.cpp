/*
	File name			:	front_control.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , FEB 06
	Date last modified	:	2019 , FEB 09
	Purpose				:	

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintainance

	Namespace			:	None
*/
//====================>

#define		_LOOK_TRANSFORM_
#define		_PRINT_DATA_

#include	<stdio.h>
#include	<iostream>

#include	<ros/ros.h>

#include	<zeabus_library/text_color.h>

#include	<zeabus_library/linear_equation.h>

#include	<zeabus_library/subscriber/SubOdometry.h>
#include	<zeabus_library/subscriber/SubTwistStamped.h>

#include	<zeabus_library/tf_handle/tf_quaternion.h>

#include	<zeabus_library/control/all_service.h>

#include	"decision_part.cpp"

#include	"support_part.cpp"

#ifdef	_LOOK_TRANSFORM_
	#include	<tf/transform_broadcaster.h>
#endif

namespace control_ = zeabus_library::control;

int main( int argv , char** argc ){

	const static double zero = 0;
	const static double one = 1;

	ros::init( argv , argc , "front_control");

	ros::NodeHandle ph("~");
	ros::NodeHandle nh("");

//====================> PARAMETER PART
	std::string state_topic;
	std::string twist_topic;
	std::string target_topic;
	std::string target_id;
	std::string my_id;
	std::string linear_id;
	std::string save_id;
	std::string world_id;
	int frequency;
	int constant_ttl;
	int aborted_control;

	ph.param< std::string >("state_topic" , state_topic , "/localize/state" );
	ph.param< std::string >("twist_topic" , twist_topic , "/mission/twist" );
	ph.param< std::string >("target_topic" , target_topic , "/control/twist" );
	ph.param< std::string >("target_id" , target_id , "robot_target");
	ph.param< std::string >("world_id" , world_id , "world");
	ph.param< std::string >("save_id" , save_id , "robot_save");
	ph.param< std::string >("linear_id" , linear_id , "robot_linear");
	ph.param< std::string >("my_id" , my_id , "control_front");
	ph.param< int >("frequency" , frequency , 40 );
	ph.param< int >("constant_ttl" , constant_ttl , 20 );

	aborted_control = copysign( frequency , -1 );

//====================> SETUP VARIABLE
	bool fix_velocity[6] = { 0, 0, 0, 0, 0, 0}; // for manage by service  
	double value_fix_velocity[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // use with fix_velocity
	double temp_double[3];

	zeabus_library::tf_handle::TFQuaternion current_quaternion( zero , zero , zero , one );
	zeabus_library::tf_handle::TFQuaternion target_quaternion( zero , zero , zero , one );
	zeabus_library::tf_handle::TFQuaternion diff_quaternion( zero , zero , zero , one );

	geometry_msgs::Point diff_point;

	nav_msgs::Odometry current_state;
	control_::clear_quaternion( current_state.pose.pose.orientation );
	control_::clear_vector3( current_state.pose.pose.position );
	nav_msgs::Odometry target_state; target_state.header.frame_id = target_id;
	control_::clear_quaternion( target_state.pose.pose.orientation );
	control_::clear_vector3( current_state.pose.pose.position );
	nav_msgs::Odometry save_state; save_state.header.frame_id = save_id;
	control_::clear_quaternion( save_state.pose.pose.orientation );
	control_::clear_vector3( current_state.pose.pose.position );
	nav_msgs::Odometry linear_state; linear_state.header.frame_id = linear_id;
	control_::clear_quaternion( linear_state.pose.pose.orientation );
	control_::clear_vector3( current_state.pose.pose.position );
	zeabus_library::LinearEquation lh; // Available only 2-D plan -- linear_handle
	lh.register_point(&current_state.pose.pose.position.x, &current_state.pose.pose.position.y );

	geometry_msgs::TwistStamped control_twist; // output send to back_control
	geometry_msgs::TwistStamped received_twist; // receive twist from mission/twist
	int received_target_twist[6] = { 0, 0, 0, 0, 0, 0};

//====================> SETUP TF 
#ifdef _LOOK_TRANSFORM_
	static tf::TransformBroadcaster broadcaster;

	zeabus_library::tf_handle::TFQuaternion tf_quaternion; // temp_quaterion
	tf::Transform target_transform;
	tf::Transform linear_transform;
	tf::Transform save_transform;
#endif
		
//====================> SETUP ROS SYSTEM
	ros::Rate rate( frequency );
	ros::Time time;

	zeabus_library::control::ServiceThreeOdometry service_three;
	zeabus_library::control::ServiceOneVector3Stamped service_one;
	zeabus_library::control::ServiceTwoStringVector3Stamped service_two;

	service_one.register_all_state( &current_state , &target_state , &linear_state 
			, &save_state);
	service_one.register_equation( &lh );
	service_one.register_velocity( fix_velocity , value_fix_velocity );
	service_one.register_all_quaternion( &current_quaternion , &target_quaternion
			, &diff_quaternion );

	service_two.register_current( &current_state );
	service_two.register_target( &target_state );
	service_two.register_velocity( fix_velocity , value_fix_velocity );
	service_two.register_all_quaternion( &current_quaternion , &target_quaternion
			, &diff_quaternion );

	service_three.register_all_state( &current_state , &target_state );

	zeabus_library::subscriber::SubOdometry listener_state( &current_state );
	bool start_up = true; int received_state = 0; 
	listener_state.register_ttl( &received_state , 1 );
	ros::Subscriber sub_state = nh.subscribe( state_topic , 1 
			,&zeabus_library::subscriber::SubOdometry::callback_ttl , &listener_state );

	zeabus_library::subscriber::SubTwistStamped listerner_twist( &received_twist );
	listerner_twist.register_ttl( received_target_twist , 15 );
	ros::Subscriber sub_twist = nh.subscribe( twist_topic , 1 
			,&zeabus_library::subscriber::SubTwistStamped::callback_ttl , &listerner_twist );

	ros::Publisher tell_target = nh.advertise< geometry_msgs::TwistStamped>( target_topic , 1);

	ros::ServiceServer ser_relative_xy = nh.advertiseService( "/control/relative_xy"
			, &zeabus_library::control::ServiceOneVector3Stamped::callback_relative_xy 
			, &service_one );

	ros::ServiceServer ser_relative_z = nh.advertiseService( "/control/relative_z" 
			, &zeabus_library::control::ServiceOneVector3Stamped::callback_relative_z 
			, &service_one );

	ros::ServiceServer ser_relative_yaw = nh.advertiseService( "/control/relative_yaw" 
			, &zeabus_library::control::ServiceOneVector3Stamped::callback_relative_yaw 
			, &service_one );

	ros::ServiceServer ser_fix_yaw = nh.advertiseService( "/control/fix_yaw"
			, &zeabus_library::control::ServiceOneVector3Stamped::callback_fix_yaw
			, &service_one );

	ros::ServiceServer ser_fix_z = nh.advertiseService( "/control/fix_z"
			, &zeabus_library::control::ServiceOneVector3Stamped::callback_fix_z
			, &service_one );

	ros::ServiceServer ser_velocity_xy = nh.advertiseService( "/control/velocity_xy"
			, &zeabus_library::control::ServiceOneVector3Stamped::callback_velocity_xy
			, &service_one );

	ros::ServiceServer ser_velocity_z = nh.advertiseService( "/control/velocity_z"
			, &zeabus_library::control::ServiceOneVector3Stamped::callback_velocity_z
			, &service_one );

	ros::ServiceServer ser_velocity_yaw = nh.advertiseService( "/control/velocity_yaw"
			, &zeabus_library::control::ServiceOneVector3Stamped::callback_velocity_yaw
			, &service_one );

	ros::ServiceServer ser_reset_target = nh.advertiseService( "/control/reset_target"
			, &zeabus_library::control::ServiceTwoStringVector3Stamped::callback_reset_target
			, &service_two );

	ros::ServiceServer ser_reset_velocity = nh.advertiseService( "/control/reset_velocity"
			, &zeabus_library::control::ServiceTwoStringVector3Stamped::callback_reset_velocity
			, &service_two );

	ros::ServiceServer ser_check_position = nh.advertiseService( "/control/check_position"
			, &zeabus_library::control::ServiceTwoStringVector3Stamped::callback_check_position
			, &service_two );

	ros::ServiceServer ser_get_target = nh.advertiseService( "/control/get_target"
			, &zeabus_library::control::ServiceThreeOdometry::callback_get_state 
			, &service_three );
	  

//====================> LOOP ROS OPERATOR
	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		if( start_up ){
			if( received_state ){
				current_quaternion = current_state.pose.pose.orientation;
				current_quaternion.get_RPY( temp_double[0] , temp_double[1] , temp_double[2] );
				target_quaternion.setRPY( zero , zero , temp_double[2] );
				target_state.pose.pose.position = current_state.pose.pose.position;	
				target_state.pose.pose.orientation = target_quaternion.get_quaternion();
				save_state = target_state;
				linear_state = target_state;
				start_up = false;
				received_state = 0;				
			}
			else{
				zeabus_library::normal_red("Warning can't receive current_state\n");		
			}
		}
		else{ // equation diff_quaternion * current_quaternion = target_quaternion			
			received_state--;
			current_quaternion = current_state.pose.pose.orientation;
			// not have linear quaternion because linear_quaternion = target_quaternion;
			target_quaternion = target_state.pose.pose.orientation;
			diff_quaternion = target_quaternion * current_quaternion.inverse();
			if( received_state < aborted_control ){
				received_state = 0;
				start_up = true;
				continue;
			}
		//====================> PLAN XY
			if( fix_velocity[0] && fix_velocity[1] ){
				lh.cut_point( current_state.pose.pose.position.x 
						, current_state.pose.pose.position.y 
						, target_state.pose.pose.position.x , target_state.pose.pose.position.y 
						, diff_point.x , diff_point.y );
				// This will check case target is opposite of linear or target velocity
				if( control_::wrong_direction( save_state.pose.pose.position.x , 
						target_state.pose.pose.position.x ,linear_state.pose.pose.position.x )){
					bool wrong_direction_x = control_::wrong_direction( 
							save_state.pose.pose.position.x , current_state.pose.pose.position.x
							, linear_state.pose.pose.position.x ); 
					bool wrong_direction_y = control_::wrong_direction( 
							save_state.pose.pose.position.y , current_state.pose.pose.position.y
							, linear_state.pose.pose.position.y ); 
					if( wrong_direction_x && wrong_direction_y ){
						target_state.pose.pose.position.x = save_state.pose.pose.position.x;
						target_state.pose.pose.position.y = save_state.pose.pose.position.y;
					}
					else if( wrong_direction_x ){
						target_state.pose.pose.position.y = current_state.pose.pose.position.y;
						lh.find_point_x( target_state.pose.pose.position.x 
								, target_state.pose.pose.position.y );
					}
					else{
						target_state.pose.pose.position.x = current_state.pose.pose.position.x;
						lh.find_point_y( target_state.pose.pose.position.x 
								, target_state.pose.pose.position.y );
					}
					diff_point.x = target_state.pose.pose.position.x 
							- current_state.pose.pose.position.x; 
					diff_point.y = target_state.pose.pose.position.y
							- current_state.pose.pose.position.y; 
				}
				control_twist.twist.linear.x = value_fix_velocity[0] +
						control_::velocity_xy( diff_point.x );

				control_twist.twist.linear.y = value_fix_velocity[1] +
						control_::velocity_xy( diff_point.y );

				int bit_answer = 0;
				int bit_diff = 0;
				if( control_twist.twist.linear.x < 0 ) bit_answer = 1;
				if( diff_point.x < 0 ) bit_diff = 1;
				if( ( bit_diff + bit_answer ) == 1){
					control_twist.twist.linear.x = copysign( 0.5 , diff_point.x );
					zeabus_library::bold_red( "Convert type\n");
				}
	
			}
			else if( received_target_twist[0] > 0 || received_target_twist[1] > 0 ){
				received_target_twist[0]--;
				received_target_twist[1]--;
				control_twist.twist.linear = current_quaternion.rotation(
						received_twist.twist.linear);
				target_state.pose.pose.position.x = current_state.pose.pose.position.x;
				target_state.pose.pose.position.y = current_state.pose.pose.position.y;
			}
			else{
				control_twist.twist.linear.x = control_::velocity_xy( 
						target_state.pose.pose.position.x - current_state.pose.pose.position.x );
				control_twist.twist.linear.y = control_::velocity_xy( 
						target_state.pose.pose.position.y - current_state.pose.pose.position.y );
			}		
		//====================> PLAN Z
			if( fix_velocity[2] ){
				control_twist.twist.linear.z = value_fix_velocity[2];
				target_state.pose.pose.position.z = current_state.pose.pose.position.z;
			}
			else if( received_target_twist[2] > 0 ){
				received_target_twist[2]--;
				control_twist.twist.linear.z = received_twist.twist.linear.z;
				target_state.pose.pose.position.z = current_state.pose.pose.position.z;
			}
			else{
				control_twist.twist.linear.z = control_::velocity_z(
						target_state.pose.pose.position.z - current_state.pose.pose.position.z );
			}
		//====================> PLAN ROLL PITCH YAW
			if( fix_velocity[2] ){
				current_quaternion.get_RPY( temp_double[0] , temp_double[1] , temp_double[2] );
				control_twist.twist.angular.z = value_fix_velocity[5];
				target_quaternion.setRPY( zero , zero , temp_double[2] );
				target_state.pose.pose.orientation = target_quaternion.get_quaternion();
			}
			else if( received_target_twist[5] > 0 ){
				received_target_twist[5]--;
				control_twist.twist.angular.z = received_twist.twist.angular.z;
				current_quaternion.get_RPY( temp_double[0] , temp_double[1] , temp_double[2] );
				target_quaternion.setRPY( zero , zero , temp_double[2] );
				target_state.pose.pose.orientation = target_quaternion.get_quaternion();
			} 
			else{
				diff_quaternion.get_RPY( temp_double[0] , temp_double[1] , temp_double[2] );
				control_twist.twist.angular.z = control_::velocity_yaw(temp_double[2]);
			}

			tell_target.publish( control_twist );
		}
#ifdef _PRINT_DATA_
	zeabus_library::clear_screen();
	diff_point.x = target_state.pose.pose.position.x - current_state.pose.pose.position.x;
	diff_point.y = target_state.pose.pose.position.y - current_state.pose.pose.position.y;
	diff_point.z = target_state.pose.pose.position.z - current_state.pose.pose.position.z;
	zeabus_library::normal_yellow("LIST POSITION\n");
	printf("CURRENT_POINT :%10.4lf%10.4lf%10.4lf\n" , current_state.pose.pose.position.x
			, current_state.pose.pose.position.y , current_state.pose.pose.position.z );
	printf("DIFF_POINT    :%10.4lf%10.4lf%10.4lf\n" ,diff_point.x ,diff_point.y ,diff_point.z );
	printf("TARGET_POINT  :%10.4lf%10.4lf%10.4lf\n" , target_state.pose.pose.position.x
			, target_state.pose.pose.position.y , target_state.pose.pose.position.z );
	printf("LINEAR_POINT  :%10.4lf%10.4lf%10.4lf\n" , linear_state.pose.pose.position.x
			, linear_state.pose.pose.position.y , linear_state.pose.pose.position.z );
	printf("SAVE_POINT    :%10.4lf%10.4lf%10.4lf\n\n" , save_state.pose.pose.position.x
			, save_state.pose.pose.position.y , save_state.pose.pose.position.z );
	zeabus_library::normal_blue("LIST QUATERNION\n");
	printf("CURRENT_Q_M   :%10.4lf%10.4lf%10.4lf%10.4lf\n" 
			, current_state.pose.pose.orientation.x , current_state.pose.pose.orientation.y
			, current_state.pose.pose.orientation.z , current_state.pose.pose.orientation.w );
	printf("TARGET_Q_M    :%10.4lf%10.4lf%10.4lf%10.4lf\n" 
			, target_state.pose.pose.orientation.x , target_state.pose.pose.orientation.y
			, target_state.pose.pose.orientation.z , target_state.pose.pose.orientation.w );
	printf("LINEAR_Q_M    :%10.4lf%10.4lf%10.4lf%10.4lf\n" 
			, linear_state.pose.pose.orientation.x , linear_state.pose.pose.orientation.y
			, linear_state.pose.pose.orientation.z , linear_state.pose.pose.orientation.w );
	printf("SAVE_Q_M      :%10.4lf%10.4lf%10.4lf%10.4lf\n\n" 
			, save_state.pose.pose.orientation.x , save_state.pose.pose.orientation.y
			, save_state.pose.pose.orientation.z , save_state.pose.pose.orientation.w );
	zeabus_library::normal_magenta("LIST ROLL PITCH YAW\n");
	printf("CURRENT_RPY   :"); current_quaternion.print_radian(); printf("\n");
	printf("TARGET_RPY    :"); target_quaternion.print_radian(); printf("\n");
	printf("DIFF_RPY      :"); diff_quaternion.print_radian(); printf("\n\n");
	zeabus_library::normal_red("LIST ABOUT VELOCITY\n");
	printf("CURRENT_VEL   :%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf\n"
			, current_state.twist.twist.linear.x , current_state.twist.twist.linear.y
			, current_state.twist.twist.linear.z , current_state.twist.twist.angular.x
			, current_state.twist.twist.angular.y , current_state.twist.twist.angular.z );
	printf("VALUE_RECEIVE :%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf\n"
			, received_twist.twist.linear.x , received_twist.twist.linear.y
			, received_twist.twist.linear.z , received_twist.twist.angular.x
			, received_twist.twist.angular.y , received_twist.twist.angular.z );
	printf("RECEIVED_VEL  :%10d%10d%10d%10d%10d%10d\n" 
			, received_target_twist[0] , received_target_twist[1] , received_target_twist[2] 
			, received_target_twist[3] , received_target_twist[4] , received_target_twist[5] );
	printf("FIX_OR_NOT    :%10d%10d%10d%10d%10d%10d\n" , fix_velocity[0] , fix_velocity[1]
			, fix_velocity[2] , fix_velocity[3] , fix_velocity[4] , fix_velocity[5] );
	printf("FIX_VALUE     :%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf\n" , value_fix_velocity[0]
			, value_fix_velocity[1] , value_fix_velocity[2] , value_fix_velocity[3]
			, value_fix_velocity[4] , value_fix_velocity[5] );
	printf("OUTPUT_VEL    :%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf\n" 
			, control_twist.twist.linear.x , control_twist.twist.linear.y 
			, control_twist.twist.linear.z , control_twist.twist.angular.x
			, control_twist.twist.angular.y , control_twist.twist.angular.z	);
	zeabus_library::bold_yellow("\nPRINT DATA LINEAR EQUATION\n");
	lh.print_equation();
		
#endif
#ifdef _LOOK_TRANSFORM_
	linear_transform.setOrigin( tf::Vector3( linear_state.pose.pose.position.x
			, linear_state.pose.pose.position.y , linear_state.pose.pose.position.z ) );
	linear_transform.setRotation( tf::Quaternion(
			 linear_state.pose.pose.orientation.x , linear_state.pose.pose.orientation.y
			, linear_state.pose.pose.orientation.z , linear_state.pose.pose.orientation.w ) );
	broadcaster.sendTransform( tf::StampedTransform( linear_transform ,  time 
			, world_id , linear_id) );
	
	save_transform.setOrigin( tf::Vector3( save_state.pose.pose.position.x
			, save_state.pose.pose.position.y , save_state.pose.pose.position.z ) );
	save_transform.setRotation( tf::Quaternion(
			 save_state.pose.pose.orientation.x , save_state.pose.pose.orientation.y
			, save_state.pose.pose.orientation.z , save_state.pose.pose.orientation.w ) );
	broadcaster.sendTransform( tf::StampedTransform( save_transform ,  time 
			, world_id , save_id) );

	target_transform.setOrigin( tf::Vector3( target_state.pose.pose.position.x
			, target_state.pose.pose.position.y , target_state.pose.pose.position.z ) );
	target_transform.setRotation( tf::Quaternion(
			 target_state.pose.pose.orientation.x , target_state.pose.pose.orientation.y
			, target_state.pose.pose.orientation.z , target_state.pose.pose.orientation.w ) );
	broadcaster.sendTransform( tf::StampedTransform( target_transform ,  time 
			, world_id , target_id) );
#endif
	}

}
