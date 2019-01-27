/*
	File name			:	first_control.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 20
	Date last modified	:	2018 , JAN 26
	Purpose				:	For control system will give target velocity
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.2
	status				:	USE

	Namespace			:	-
*/

#include	<stdio.h>

#include	<ros/ros.h>

#include	<math.h>

#include	<zeabus_library/euler.h>

#include	<zeabus_library/text_color.h>

#include	<zeabus_library/rotation/rotation_handle.h>

#include	<zeabus_library/Odometry.h>

#include	<zeabus_library/convert/Point3.h>

#include	<zeabus_library/control/listen_odometry.h>

#include	<zeabus_library/control/listen_twist.h>

#include	<zeabus_library/linear_equation.h>

#include	<zeabus_library/general.h>

#include	<zeabus_library/Twist.h>

#include	<zeabus_library/euler.h>

#include	"first_control_function.cpp"

#define _PRINT_DATA_
//#define _PRINT_QUATERNION_

#define _DEBUG_ORDER_

namespace euler_ = zeabus_library::euler;

int main( int argv , char** argc ){

	ros::init( argv , argc , "first_control" );

	ros::NodeHandle ph("~"); // Handle for manage param from launch file == parameter handle
	ros::NodeHandle nh(""); // Handle for manage about this file in ros system == node handle

///////////////////////////////////-- PARAMETER PART --//////////////////////////////////////////
	std::string topic_state;
	std::string topic_output;
	std::string topic_twist;
	int frequency;
	int constant_value = 12;

	ph.param< std::string >("topic_state" , topic_state , "/localize/state");
	ph.param< std::string >("topic_twist" , topic_twist , "/control/twist");
	ph.param< std::string >("topic_output" ,topic_output , "/control/target" );
	ph.param< int >("frequency" , frequency , 50 );
///////////////////////////////////////////////////////////////////////////////////////////////

	ros::Rate rate( frequency );

	zeabus_library::Twist message; // for send target velocity to node second_control
	clear_point3( message.linear);
	clear_point3( message.angular);
	zeabus_library::Twist temp_message; 
	int count_velocity[6] = { 0 , 0 , 0 , 0 , 0 , 0 };

	zeabus_library::Point3 current_position; // for collection current position		
	zeabus_library::Point3 diff_position; // for collecting error position
	zeabus_library::Point3 target_position; // for collecting target position
	zeabus_library::Point3 temporary_position; // for collecting linear position for velocity
	zeabus_library::Point3 current_velocity; // for collecting linear velocity form localize
	zeabus_library::Point3 current_gyroscope; // for collecting angular velocity form localize
	zeabus_library::LinearEquation line;
	clear_point3( current_position );
	clear_point3( target_position );
	clear_point3( temporary_position );

	zeabus_library::rotation::RotationHandle rh; // rotation handle
	zeabus_library::Point4 current_quaternion;
	zeabus_library::Point4 target_quaternion;
	double current_euler[3] = { 0 , 0 , 0 };
	double target_euler[3] = { 0 , 0 , 0 };
	double diff_euler[3] = { 0 , 0 , 0 };
	clear_point4( current_quaternion );
	clear_point4( target_quaternion );

	zeabus_library::control::ListenOdometry listen_odometry;
	listen_odometry.register_linear_position( &current_position );
	listen_odometry.register_quaternion( &current_quaternion );
	listen_odometry.register_target_position( &target_position );
	listen_odometry.register_target_quaternion( &target_quaternion );
	listen_odometry.register_linear_velocity( &current_velocity );
	listen_odometry.register_gyroscope( &current_gyroscope );

	zeabus_library::control::ListenTwist listen_twist( count_velocity );
	zeabus_library::Point3 target_velocity;
	zeabus_library::Point3 target_gyroscope;
	listen_twist.register_linear( &target_velocity );
	listen_twist.register_angular( &target_gyroscope );
	listen_twist.set_constant( constant_value );
	clear_point3( target_velocity );
	clear_point3( target_gyroscope );

	int mode_control = 0;	//  mode control is consider by roll and pitch
							//	mode 0 is roll & pitch normal is 0
	
	bool temp_bool = false;
	double temp_distance = 0;

//////////////////////////////////////-- ROS SYSTEM --///////////////////////////////////////////
	ros::Subscriber sub_state = nh.subscribe( topic_state , 1 
								, &zeabus_library::control::ListenOdometry::callback
								, &listen_odometry );

	ros::Subscriber sub_target = nh.subscribe( topic_twist , 1
								, &zeabus_library::control::ListenTwist::callback
								, &listen_twist );

	ros::Publisher tell_target = nh.advertise< zeabus_library::Twist >( topic_output , 1 );

	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		// for find diff euler
		rh.set_start_frame( current_quaternion ); 
		rh.set_target_frame( target_quaternion );
		rh.target_frame.get_RPY( target_euler[0] , target_euler[1] , target_euler[2] );
		rh.update_rotation();
		rh.get_RPY( diff_euler[0] , diff_euler[1] , diff_euler[2] ); // start go to target
		rh.update_rotation();

		if( mode_control == 0 ){
			#ifdef _DEBUG_ORDER_
				printf("BEFORE PLAN XY\n");
			#endif
			if( count_velocity[0] > 0 || count_velocity[1] > 0 ){
				temp_bool = (count_velocity[0] == constant_value) 
							|| (count_velocity[1] == constant_value) ;
				if( temp_bool ){
					temp_message.linear.x = target_velocity.x * cos( target_euler[2] )
									+ target_velocity.y * cos( target_euler[2] + euler_::PI );

					temp_message.linear.y = target_velocity.x * sin( target_euler[2] )
									+ target_velocity.y * sin( target_euler[2] + euler_::PI );

					next_point_xy( target_euler[2] , current_position.x , current_position.y
								, temporary_position.x , temporary_position.y 
								, copysign( target_velocity.x * 10 , target_velocity.x ) 
								, copysign( target_velocity.y * 10 , target_velocity.y ) );	 
					line.set_point( current_position.x , current_position.y 
									, temporary_position.x , temporary_position.y );
					line.update();	
				}
				line.distance_split( current_position.x , current_position.y
									, diff_position.x , diff_position.y 
									, target_position.x , target_position.y );
				message.linear.x = temp_message.linear.x + assign_velocity_xy( diff_position.x);
				message.linear.y = temp_message.linear.y + assign_velocity_xy( diff_position.y);
				count_velocity[0]--;
				count_velocity[1]--;
			}
			else{
				diff_position.x = target_position.x - current_position.x;
				diff_position.y = target_position.y - current_position.y;
				message.linear.x = assign_velocity_xy( diff_position.x );
				message.linear.y = assign_velocity_xy( diff_position.y );
			}
			#ifdef _DEBUG_ORDER_
				printf("BEFORE PLAN Z\n");
			#endif
			if( count_velocity[2] != 0 ){
				message.linear.z = target_velocity.z;
				target_position.z = current_position.z;
				count_velocity[2]--;
			}
			else{
				diff_position.z = target_velocity.z - current_position.z;
			}
			#ifdef _DEBUG_ORDER_
				printf("BEFORE PLAN YAW\n");
			#endif
			if( count_velocity[5] != 0 ){
				message.angular.z = target_gyroscope.z;
				target_quaternion = current_quaternion;
				count_velocity[5]--;
			}
			else{
				message.angular.z = assign_gyroscope_z( diff_euler[2] );
			}
		}
		tell_target.publish( message );
		#ifdef _PRINT_DATA_
			zeabus_library::clear_screen();
			rh.start_frame.get_RPY( current_euler[0] , current_euler[1] , current_euler[2] );
			printf("current_position   : %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf\n\n"
						, current_position.x , current_position.y , current_position.z 
						, current_euler[0] , current_euler[1] , current_euler[2] );
			printf("target_position    : %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf\n\n"
						, target_position.x , target_position.y , target_position.z 
						, target_euler[0] , target_euler[1] , target_euler[2] );
			printf("diff_position      : %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf\n\n"
						, diff_position.x , diff_position.y , diff_position.z 
						, diff_euler[0] , diff_euler[1] , diff_euler[2] );
			printf("output_velocity    : %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf\n\n"
						, message.linear.x , message.linear.y ,message.linear.z 
						, message.angular.x , message.angular.y , message.angular.z );
			printf("current_velocity   : %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf\n\n"
						, current_velocity.x , current_velocity.y ,current_velocity.z 
						, current_gyroscope.x , current_gyroscope.y , current_gyroscope.z );
			printf("target_velocity    : %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf\n\n"
						, target_velocity.x , target_velocity.y ,target_velocity.z 
						, target_gyroscope.x , target_gyroscope.y , target_gyroscope.z );
			printf("count_velocity     : %8.3d  %8.3d  %8.3d  %8.3d  %8.3d  %8.3d\n\n"
						, count_velocity[0] , count_velocity[1] ,count_velocity[2] 
						, count_velocity[3] , count_velocity[4] , count_velocity[5] );
		#ifdef _PRINT_QUATERNION_
			printf("current_quaternion : %8.3lf %8.3lf %8.3lf %8.3lf\n\n"
							, current_quaternion.w , current_quaternion.x 
							, current_quaternion.y ,current_quaternion.z );
			printf("target_quaternion : %8.3lf %8.3lf %8.3lf %8.3lf\n\n"
							, target_quaternion.w , target_quaternion.x 
							, target_quaternion.y , target_quaternion.z );
		#endif
		#endif
	}

}

// This first control will interest in world frame but 
