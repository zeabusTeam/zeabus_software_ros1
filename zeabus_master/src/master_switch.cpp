/*
	File name			:	master_switch.cpp
	Author				:	Supasan Komonlit
	Date created		:	2019 , FEB 19
	Date last modified	:	2019 , ??? ??
	Purpose				:	This received message switch

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Production

	Namespace			:	-
*/
//===============>

#include	<stdio.h>
#include	<iostream>

#include	<ros/ros.h>

#include	<zeabus_library/TwoBool.h>

#include	<zeabus_library/subscriber/SubBool.h>

#include	<std_msgs/Bool.h>

#define _SHOW_COUNT_SWITCH_

int main( int argv , char** argc ){

	ros::init( argv , argc , "master_switch");

	ros::NodeHandle nh("");
	ros::NodeHandle ph("~");

//===============> PARAMETER PART
	std::string switch_topic = "planner_switch";
	int frequency = 60;
	int limit_count_switch = 50;


//===============> SETUP VARIABLE & ROS SYSTEM
	ros::Rate rate( frequency );

	std_msgs::Bool switch_data;
	int received = 0;

	zeabus_library::subscriber::SubBool listen_switch( &switch_data );
	listen_switch.register_ttl( &received , 1 );

	ros::Subscriber sub_switch = nh.subscribe( switch_topic , 1 
			, &zeabus_library::subscriber::SubBool::callback_ttl
			, &listen_switch);

	ros::ServiceClient connect_control = 
			nh.serviceClient< zeabus_library::TwoBool >("/control/active");

	zeabus_library::TwoBool srv_connet_control;

//================> LOOP ROS SYSTEM	
	int count_switch = 0;
	bool status_back_control = 0;
	
	#ifdef _SHOW_COUNT_SWITCH_
		int count_print = 0;
	#endif

	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		if( received ){
			if( switch_data.data ){ 
				if( count_switch < limit_count_switch ) count_switch++;
			}
			else{ 
				if( count_switch > 0 ) count_switch--;
			}
		}

		//===============> Control Back control
		if( status_back_control ){ // back control are now open
			if( count_switch == 0 ){
				srv_connet_control.request.data = false;
				connect_control.call( srv_connet_control );
				status_back_control = false; //  change to control are now close
			}
		} 
		else{ // back control are now close
			if( count_switch == limit_count_switch ){
				srv_connet_control.request.data = true;
				connect_control.call( srv_connet_control );
				status_back_control = true; // change to control are now open
			}
		}
		#ifdef _SHOW_COUNT_SWITCH_
			count_print++;
			if( count_print == 5 ){
				printf("count_switch : %5d\n" , count_switch );	
				count_print = 0;
			}
		#endif
	}
}
