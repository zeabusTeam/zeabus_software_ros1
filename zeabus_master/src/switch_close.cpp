/*
	File name			:	switch_close.cpp
	Author				:	Supasan Komonlit
	Date created		:	2019 , MAR 07
	Date last modified	:	2019 , MAR 07
	Purpose				:	This received message switch and open code

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintainance

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
#define _PLAY_QUALIFICATION_
#define _PLAY_ALL_MISSION_

int main( int argv , char** argc ){

	ros::init( argv , argc , "open_switch" );
	
	ros::NodeHandle nh("");
	ros::NodeHandle ph("~");

//===============> PARAMETER PART
	std::string switch_topic = "planner_switch";
	int frequency = 60;
	int limit_count_switch = 60;

//===============> SETUP VARIABLE & ROS SYSTEM
	ros::Rate rate( frequency );

	std_msgs::Bool switch_data;
	int received = 0;

	zeabus_library::subscriber::SubBool listen_switch( &switch_data );
	listen_switch.register_ttl( &received , 1 );

	ros::Subscriber sub_switch = nh.subscribe( switch_topic  , 1 
			, &zeabus_library::subscriber::SubBool::callback_ttl 
			, &listen_switch );

	ros::ServiceClient connect_control = 
			nh.serviceClient< zeabus_library::TwoBool >( "/control/active" );
	zeabus_library::TwoBool srv_connect_control;

#ifdef _PLAY_QUALIFICATION_
	ros::ServiceClient connect_mission_qualification =
			nh.serviceClient< zeabus_library::TwoBool >( "/mission/qualification" );
	zeabus_library::TwoBool srv_connect_mission_qualification;
#endif

#ifdef _PLAY_ALL_MISSION_
	ros::ServiceClient connect_mission_all = 
			nh.serviceClient< zeabus_library::TwoBool >( "/mission/all");
	zeabus_library::TwoBool srv_connect_mission_all;
#endif

//===============> LOOP ROS SYSTEM
	int count_switch = limit_count_switch;
	bool status_back_control = 0; // 0 / false is close and 1 / true is open
#ifdef _PLAY_QUALIFICATION_
	bool status_mission_qualification = 0;
#endif
#ifdef _PLAY_ALL_MISSION_
	bool status_mission_all = 0;
#endif

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

		//===============> Connection to Back Control
		if( status_back_control ){ // Back control are now open
			if( count_switch == 0 ) status_back_control = 0;
		}
		else{ // back control are now close
			if( count_switch == limit_count_switch ){
				status_back_control = 1;
				srv_connect_control.request.data = true;
				connect_control.call( srv_connect_control );
			}
		}
#ifdef _PLAY_ALL_MISSION_
		//===============> Connection to mission all
		if( status_mission_all ){ // Mission all now are running
			if( count_switch == 0 ){
				status_mission_all = 0;
				srv_connect_mission_all.request.data = false;
				connect_control.call( srv_connect_mission_all );
			}
		}
		else{ // Mission all noew are close
			if( count_switch == limit_count_switch ) status_mission_all = 1;
		}
#endif
#ifdef _PLAY_QUALIFICATION_ 
		//===============> Connection to mission qualification
		if( status_mission_qualification ){ // missio qualification are running
			if( count_switch == 0 ){
				status_mission_qualification = 0;
				srv_connect_mission_qualification.request.data = false;
				connect_control.call( srv_connect_mission_qualification );
			}
		}
		else{ // Mission all now are close
			if( count_switch == limit_count_switch ) status_mission_qualification = 1;	
		}
#endif

		#ifdef _SHOW_COUNT_SWITCH_
			count_print++;
			if( count_print == 5 ){
				printf("count_switch_close   : %5d\n" , count_switch );	
				printf("status_control      : %5d\n" , status_back_control );
			#ifdef _PLAY_QUALIFICATION_
				printf("status_qulification : %5d\n" , status_mission_qualification );
			#endif
			#ifdef _PLAY_ALL_MISSION_
				printf("status_mission_all  : %5d\n" , status_mission_all );
			#endif
				printf("\n\n");	
				count_print = 0;
			}
		#endif
	}

}
