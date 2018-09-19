#include	<iostream>
#include 	<ros/ros.h>
#include 	<cstdlib>
#include 	<std_msgs/Bool.h>
#include	<nav_msgs/Odometry.h>

#include	<zeabus_controller/manage_control.h>"

void listen_current_state( const nav_msgs::Odometry message);
bool active_close = false;
int count_open_close = 0;
int count_close_close = 0;

/*
int main(int argc, char **argv){
	ros::init(argc , argv , "manage_controller");
	ros::NodeHandle nh;
	ros::Subscriber sub_state = nh.subscribe("/auv/state" , 1000, &listen_current_state);
	ros::Rate sleep(0.25);
	std::cout << "Waiting for open 4 second\n";
	sleep.sleep();
	manage_control.run_launch( "zeabus_controller" , "offset_control.launch");
	std::cout << "finish open\n";
	ros::spin();
}*/

void listen_current_state( const nav_msgs::Odometry message){
	double depth = message.pose.pose.position.z;
	if( active_close && ( depth > -1 ) ){
		count_close_close++;
		if(count_close_close > 10){
            std::cout << "depth is  " << depth << '\n';
			manage_control.kill_node("Controller");
			manage_control.kill_node("thrust_mapper");
			manage_control.kill_node("sensor_fusion");
			std::cout << "I close that\n";	
			active_close = false;
			count_open_close = 0;
			count_close_close = 0;
		}
	}
	else if( depth < -1.5 ){
        count_close_close = 0;
		count_open_close++;
		if( count_open_close > 5){
			active_close = true;
			std::cout << "Now active close\n";
			count_open_close = 0;
			count_close_close = 0;
		}
	}
    else{
        count_close_close = 0;
    }
}
