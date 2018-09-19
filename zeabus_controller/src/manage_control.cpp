#include <iostream>
#include <ros/ros.h>
#include <cstdlib>
#include <std_msgs/Bool.h>
#include <zeabus_controller/control_mode.h>

class manage_control_file{
	public:
		void run_launch(std::string package_name , std::string file_name);
		void kill_node(std::string node_name);
		manage_control_file();
};

void manage_control_file::kill_node(std::string node_name){
	std::string cmd_string = "rosnode kill " + node_name;
	std::cout << "Input comand kill : " << cmd_string << std::endl;
	std::system( cmd_string.c_str() );
}

void manage_control_file::run_launch(std::string package_name, std::string file_name){
	std::string cmd_string = "roslaunch " + package_name + " "+ file_name + " &";
	std::cout << "launch file is : " << cmd_string << std::endl;
	std::system( cmd_string.c_str() );
}

manage_control_file::manage_control_file(){};

//0 is not open 1 is open control
int mode = 0;
int count = 0;
std::string package_name = "zeabus_controller";
std::string package_name_of_dvl = "zeabus_bringup";
std::string package_name_of_sensor = "zeabus_sensor_fusion";
std::string package_name_of_ai = "zeabus_command";
void listen_switch(std_msgs::Bool order);
bool service_mode( zeabus_controller::control_mode::Request &request 
				, zeabus_controller::control_mode::Response &response);
manage_control_file manage_control = manage_control_file();

//this file guaruntee only origin controller


int main(int argc, char **argv){
	ros::init(argc , argv , "manage_controller");
	std::cout << "My node's name is manage_controller" << std::endl;
	ros::NodeHandle nh;
//	ros::Subscriber sub_mode = nh.subscribe("/planner_switch" , 1000, &listen_switch);
	ros::ServiceServer ser_cli_mode = nh.advertiseService("/control_mode" , service_mode);
	ros::spin();
}


bool service_mode( zeabus_controller::control_mode::Request &request , zeabus_controller::control_mode::Response &response){
//	if(request.mode.data == true && mode == 0){
	if(request.mode.data == true){
			mode = 1;
			std::cout << " Change to open " << std::endl;
//			manage_control.run_launch( package_name_of_dvl , "zeabus_dvl_localize.launch" );
//			manage_control.run_launch( package_name_of_sensor , "ex_normal_fusion");
			manage_control.run_launch( package_name , "offset_control.launch");
			response.done.data = true;
	}
//	else if(request.mode.data == false && mode == 1){
	else if(request.mode.data == false){
			mode = 0;
//			manage_control.kill_node("ukf_localization");
			manage_control.kill_node("thrust_mapper");
			manage_control.kill_node("Controller");
			manage_control.kill_node("sensor_fusion");
			std::cout << "Close controller" << std::endl;
			response.done.data = true;
	}
	else response.done.data = false;
	return true;
}

void listen_switch(std_msgs::Bool order){
	if(order.data == true)
		if(mode == 0 && count > 30){
			mode = 1;
			std::cout << " Change to open " << std::endl;
			manage_control.run_launch( package_name_of_ai , "ai.launch" );
			manage_control.run_launch( package_name_of_dvl , "zeabus_dvl_localize.launch" );
			manage_control.run_launch( package_name , "origin_control.launch");
			count = 0;
		}
		else if( mode == 0 ){
			count+=1;
			std::cout << "count for open is " << count << std::endl; 
			ros::spinOnce();
		}
		else count = 0;
	else
		if(mode == 1 && count > 30){
			mode = 0;
			manage_control.kill_node("ukf_localization");
			manage_control.kill_node("Controller");
			manage_control.kill_node("origin_thruster_2018");
			manage_control.kill_node("zeabus_command");

			std::cout << "Close controller" << std::endl;
		}
		else if( mode == 1){
			count+=1;
			std::cout << "count for close " << count << std::endl;
		}
		else count = 0 ;
}
