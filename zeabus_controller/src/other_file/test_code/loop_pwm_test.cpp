#include <ros/ros.h>
#include <ros/time.h>
#include <hg_ros_pololu/Pwm.h>
#include <nav_msgs/Odometry.h>
#include <tf/transform_datatypes.h>
#include <tf/transform_listener.h>
#include <iostream>

hg_ros_pololu::Pwm message_pwm;

double current_state[6] = {0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0};

void data_pwm_to_message(double* data);

void listen_current_state(const nav_msgs::Odometry message);

int main(int argc , char **argv){
	ros::init(argc, argv, "test_by_pwm");
	ros::NodeHandle nh;
	ros::Subscriber sub_state = nh.subscribe("/auv/state" , 1000 , &listen_current_state);

	ros::Publisher tell_cmd_vel = nh.advertise<hg_ros_pololu::Pwm>("/pwm" , 1000);
	
	int all_round, amont_set;
	double diff_time;

	message_pwm.pwm.resize(8);

	std::cout << "Enter All Round : ";
	std::cin >> all_round;
	std::cout << "Enter number of set : ";
	std::cin >> amont_set;
	std::cout << "Enter diff time between : ";
	std::cin >> diff_time;
	std::cout << "Please enter for all set" << std::endl << "0 1 2 3 4 5 6 7  : warning please enter betwwen 1100 - 1900" << std::endl; 
	double data[amont_set][8];
	for(int count = 0; count < amont_set ; count++){
		std::cout << "Enter set " << count << " : ";
		for(int i = 0 ; i < 8 ; i++) std::cin >> data[count][i] ;
	}
	std::cout << "Check your pwm" << std::endl;
	for(int count = 0; count < amont_set ; count++){
		std::cout << "set " << count << " : ";
		for(int i = 0 ; i < 8 ; i++) std::cout << data[count][i] << "\t" ;
		std::cout << std::endl;
	}
	int OK;
	std::cout << "Ok with that data? 0 : false and 1 : true ---> ";
	std::cin >> OK;
	int mode = 0; // mode is 0 to < amont_set
	int count = 0; // count is 0 to < all_round
	ros::Time previous = ros::Time::now();
	ros::Time current;
	ros::Rate rate(50);
	double diff;
	std::cout << "start at mode 0 and count 0" << std::endl;
	while(nh.ok() && OK == 1){
		if(count == all_round){
			std::cout << "I will end now" << std::endl;
			break;
		}
		else{
			current = ros::Time::now();
			diff = current.toSec() - previous.toSec();
			if( diff > diff_time){
				std::cout << "=========next mode============" << std::endl;
				mode = (mode+1)%amont_set;
				std::cout << "data is ";
				for(int i = 0 ; i < 8 ; i++) std::cout << data[mode][i] << "\t";
				std::cout << std::endl; 
				if(mode == 0){
					std::cout << "=========next round============" << std::endl;
					count++;
				}
				previous = ros::Time::now();
			}
				std::cout << "round : " << count << "  set : " << mode << " time : " << current.toSec() << "  previous time : " << previous.toSec() << " diff is " << diff << std::endl;
//				std::cout << "Line 72" << std::endl;
				message_pwm.header.stamp = current;
//				std::cout << "Line 74 : " <<data[mode] << std::endl;
//				data_pwm_to_message(data[mode]);	
				for(int i = 0; i < 8 ; i++) message_pwm.pwm[i] = data[mode][i];
//				std::cout << "Line 77";
				tell_cmd_vel.publish(message_pwm);
		}
		rate.sleep();
	}	
}

void listen_current_state(nav_msgs::Odometry message){
// don't use
//	tf::Quaternionl quaternion(message.pose.pose.orientation.x , message.pose.pose.orientation.y , message.pose.pose.orientation.z , message.pose.pose.orientation.w);
//	tfScalar roll. pitch, yaw;
	current_state[0] = message.pose.pose.position.x;
	current_state[1] = message.pose.pose.position.y;
	current_state[2] = message.pose.pose.position.z;
}

void data_pwm_to_message(double* data){
	std::cout << "Line 93" <<std::endl;
	for(int i = 0 ; i < 8 ; i++) message_pwm.pwm[i] = *(data+i);
}
