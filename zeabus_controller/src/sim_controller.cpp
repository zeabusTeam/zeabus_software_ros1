#include <ros/ros.h> //line 236 print all

//include other file
#include "PID_2018.cpp"
#include "manage_file.cpp"
#include "special_order.cpp"

#include <ros/time.h>
#include <math.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Quaternion.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Bool.h>
#include <std_msgs/String.h>
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/Imu.h>
//#include <zeabus_controller/control_force.h>
#include <zeabus_controller/point_xy.h>
#include <zeabus_controller/orientation.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <tf/transform_datatypes.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Vector3.h>
#include <dynamic_reconfigure/server.h>
#include <zeabus_controller/PIDConstantConfig.h>
#include <queue>
#include <zeabus_controller/drive_x.h>
#include <modbus_ascii_ros/Switch.h>
// include head of service
#include <zeabus_controller/message_service.h>
#include <zeabus_controller/fix_abs_xy.h>
#include <zeabus_controller/fix_abs_x.h>
#include <zeabus_controller/fix_abs_y.h>
#include <zeabus_controller/fix_abs_depth.h>
#include <zeabus_controller/fix_abs_yaw.h>
#include <zeabus_controller/fix_rel_xy.h>
#include <zeabus_controller/ok_position.h>
//this file don't write class type because this file is main run
#define PI 3.14159265
#define earth_gravity 9.780327
#define weight 40.2
#define error_angular 0.1
#define error_angle 0.1
#define bouyaancy -0.0
#define mass weight/earth_gravity
#define error_axis 0.00
#define error_distance 0.0
#define check_correct(a,b,c) abs(a-b)<c?true:false

//double force_output[8] = {0, 0, 0, 0, 0, 0, 0, 0};
double error_z = 0.05;
double error_y = 0.02;
double error_x = 0.02;
double error_roll = 0.04;
double error_pitch = 0.04;
double error_yaw = 0.01 ;

//bound system
bool bound_z = true;
bool bound_x = true;
bool bound_y = true;
bool bound_roll = true;
bool bound_pitch = true;
bool bound_yaw = true;
double bound_force_x = 1.2;
double bound_force_y = 1.1;
double bound_force_z = 0.5;
double bound_force_roll = 0.1;
double bound_force_pitch = 0.1;
double bound_force_yaw = 0.2;

//force about x y z roll pitch yaw
//----------------------------------------
static std::string tune_file = "sim_origin.yaml";
//----------------------------------------
std::string force_message = "";
double warning_depth = -0.9;
double warning_high = -0.4;
bool close_mission = true;
double target_mission = 15;
double start_point[2] = {0, 0};
double cal_distance = 0;

double force_output[6] = { 0 , 0 , 0 , 0 , 0 , 0};
double force_fix[6] = { 0 , 0 , 0 , 0 , 0 , 0 };
void calculate_out();
double distance_xy= 0 ;
double distance_yaw= 0 ;
double diff_yaw = 0 ;

double Kp_position[6] = {0.05, 0.09, 0.5, 0, 0, 0.0238};
double Ki_position[6] = {0.0001, 0.001, 0.01, 0, 0, 0.0001};
double Kd_position[6] = {0.02, 0.02, 0, 0, 0, 0.001};
// value of  acceeration from old code
double Kp_velocity[6] = {0, 0.01, 0.5, 0.2, 0.6, 0.0};
double Ki_velocity[6] = {0, 0, 0.0015, 0, 0, 0.005};
double Kd_velocity[6] = {0, 0, 0.0001, 0, 0, 0.001};
double Kvs_position[6] = {0.06, 0.06, 0.02, 0, 0, 0.01};

geometry_msgs::Twist message_cmd_vel;
nav_msgs::Odometry current_state;
//double quaternion[4] = {0,0,0,0};
//---------------------------------
int step_work = 10; // 1 go depth : 2 tune z : 3 tune yaw : 4 tune pitch : 5 tune roll : 6 tune x : 7 tune y : 8 normal : 9 don't use 
//---------------------------------
// setup tuning z
bool tune_z = true;
int count_z = 1;
double force_z = -0.16;
double current_force_z = 0.0;
double previous_force_z = 0.0;
double previous_velocity_z = 0.0;
double target_depth = 0.0;
double change_force_z = -0.01;
bool already_change = false;

// x y z roll pitch yaw
double output[6] = {0, 0, 0, 0, 0, 0};
double current_velocity[6] = {0, 0, 0, 0, 0, 0}, target_velocity[6] = {0, 0, 0, 0, 0, 0};
double current_position[6] = {0, 0, 0, 0, 0, 0} , target_position[6] = {0, 0, target_depth, 0, 0, 0};
bool want_fix[6] = {false , false , false , false , false , false};
bool already_position[6] = {false, false, false, true, true, false};
double error_position[6] = {0, 0, 0, 0, 0, 0} , error_velocity[6] = {0, 0, 0, 0, 0, 0}, normal_error[6] = {0, 0, 0, 0, 0, 0};
//double current_pose[7] = {0, 0, 0, 0, 0, 0, 0}, target_pose[7] = {0, 0, 0, 0, 0, 0, 0};

//zeabus_controller::control_force output_force_message;
bool start_run = true;
bool first_time_PID = true;
bool change_PID = false;
bool reset_position = false;

//about time
ros::Time previous_time;
ros::Time current_time;
double diff_time;


double ok_time = 1;

// setup tuning yaw
// bool unknow_torque_yaw = true;
// double previous_omega = 0.0;
// constant is torque/alpha
/*
bool tune_yaw = false;
bool tune_roll = false;
bool tune_pitch = false;
double constant_yaw = 0.0;
double constant_roll = 0.0;
double constant_pitch = 0.0;
*/
// setup function of ros
void listen_current_state(const nav_msgs::Odometry message);
void listen_target_velocity(const geometry_msgs::Twist message);
void listen_target_position(const geometry_msgs::Point message);
void listen_mode(const std_msgs::Int16 message);
void listen_absolute_xy(const zeabus_controller::point_xy message);
void listen_target_depth(const std_msgs::Float64 message);
void listen_absolute_yaw(const std_msgs::Float64 message);
void listen_absolute_orientation(const zeabus_controller::orientation message);
void listen_real_yaw(const std_msgs::Float64 message);
void listen_mode_control(const std_msgs::Int16 message);
void listen_imu_data(const sensor_msgs::Imu message);

// setup Dynamic Reconfig

void config_constant_PID(zeabus_controller::PIDConstantConfig &config, uint32_t level);

// set up function of service
bool service_target_distance( zeabus_controller::fix_rel_xy::Request &request , zeabus_controller::fix_rel_xy::Response &response);
bool service_target_xy( zeabus_controller::fix_abs_xy::Request &request , zeabus_controller::fix_abs_xy::Response &response);
bool service_target_x( zeabus_controller::fix_abs_x::Request &request , zeabus_controller::fix_abs_x::Response &response);
bool service_target_y( zeabus_controller::fix_abs_y::Request &request , zeabus_controller::fix_abs_y::Response &response);
bool service_target_depth( zeabus_controller::fix_abs_depth::Request &request , zeabus_controller::fix_abs_depth::Response &response);
bool service_target_yaw( zeabus_controller::fix_abs_yaw::Request &request , zeabus_controller::fix_abs_yaw::Response &response);
bool service_target_function( zeabus_controller::message_service::Request &request , zeabus_controller::message_service::Response &response); 
bool service_ok_position( zeabus_controller::ok_position::Request &request , zeabus_controller::ok_position::Response &response);

//std_msgs::Bool check_ok_position();
//std_msgs::Bool check_ok_yaw();

//declare normal function
double get_roll_radian(double quaternion[4]);
double get_yaw_radian(double quaternion[4]);
double get_pitch_radian(double quaternion[4]);
double check_radian_tan(double result);
void message_to_quaternion(const geometry_msgs::Quaternion message , double* quaternion); 
double find_min_angular(double current, double target);
geometry_msgs::Twist twist_to_message(double twist[6]);
void set_all_PID();
void reset_all_I();
void reset_I_position(int num);
void reset_I_velocity(int num);
double abs(double a);
int abs(int a);
int found_sign(double direction);
void shutdown_force();
void shutdown_target_velocity();
bool same_direction(double num_01, double num_02);

//declare for testing
void test_current_state(const geometry_msgs::Point message);
void test_current_orientation(const zeabus_controller::orientation message);

//declare calculate variable
int fast_velocity_z = 0;
int fast_velocity_x = 0;
int fast_velocity_y = 0;
double absolute_error = 0;
double right_yaw = 0;
double left_yaw = 0;
bool shutdown_force_z = false; 
bool shutdown_force_xy = false;

// print datail from ros log
bool print_fatal = true;

//pull code from own file
PID *PID_position, *PID_velocity;
manage_PID_file PID_file(tune_file);

// initial code run 1 time
void init(){
	PID_velocity = (PID*)calloc(6, sizeof(PID));
	PID_position = (PID*)calloc(6, sizeof(PID));
	for(int count = 0 ; count < 6 ; count++){
		PID_position[count].set_PID(Kp_position[count], Ki_position[count], Kd_position[count], Kvs_position[count]);
		PID_velocity[count].set_PID(Kp_velocity[count], Ki_velocity[count], Kd_velocity[count], 0);
		PID_position[count].reset_I();
		PID_velocity[count].reset_I();
	}
}

// main file
int main(int argc, char **argv){
// setup ros system
	ros::init(argc, argv, "Controller");
	ros::NodeHandle nh;
// defend emergency
	ros::Subscriber sub_mode = nh.subscribe("/mode_control", 1000, &listen_mode_control);
// setup test topic
	ros::Subscriber test_state = nh.subscribe("/test/point" , 1000, &test_current_state);
	ros::Subscriber test_orientation = nh.subscribe("/test/orientation", 1000, &test_current_orientation);
// setup listen topic
	ros::Subscriber sub_state = nh.subscribe("/auv/state" , 1000, &listen_current_state);//(topic,number of max input,function's address)
	ros::Subscriber sub_target_velocity = nh.subscribe("/zeabus/cmd_vel", 1000, &listen_target_velocity);
	ros::Subscriber sub_target_position = nh.subscribe("/cmd_fix_position", 1000, &listen_target_position);
	ros::Subscriber sub_controller = nh.subscribe("/zeabus_controller/mode", 1000, &listen_mode);
	ros::Subscriber sub_target_depth = nh.subscribe("/fix/abs/depth", 1000, &listen_target_depth);
	ros::Subscriber sub_absolute_yaw = nh.subscribe("/fix/abs/yaw", 1000, &listen_absolute_yaw);
	ros::Subscriber sub_real_yaw = nh.subscribe("/fix/rel/yaw", 1000, &listen_real_yaw);
	ros::Subscriber sub_absolute_xy = nh.subscribe("fix/abs/xy", 1000, &listen_absolute_xy);
	ros::Subscriber sub_absolute_orientation = nh.subscribe("/fix/abs/orientation", 1000, &listen_absolute_orientation);
// set up test data
//	ros::Subscriber sub_imu_data = nh.subscribe("/imu/data", 1000, &listen_imu_data);
// setup service topic
//	ros::ServiceServer sub_target_x_y = nh.advertiseService("/fix/rel/x", 1000, &listen_target_x);
	ros::ServiceServer ser_cli_target_xy = nh.advertiseService("/fix_abs_xy", service_target_xy);
	ros::ServiceServer ser_cli_target_distance = nh.advertiseService("/fix_rel_xy", service_target_distance);
	ros::ServiceServer ser_cli_target_depth = nh.advertiseService("/fix_abs_depth", service_target_depth);
	ros::ServiceServer ser_cli_target_yaw = nh.advertiseService("/fix_abs_yaw", service_target_yaw);
	ros::ServiceServer ser_cli_target_x = nh.advertiseService("/fix_abs_x", service_target_x);
	ros::ServiceServer ser_cli_target_y = nh.advertiseService("/fix_abs_y", service_target_y);
	ros::ServiceServer ser_cli_target_function = nh.advertiseService("/fix_service" , service_target_function);
    ros::ServiceServer ser_cli_ok_position = nh.advertiseService("/ok_position" , service_ok_position);
// setup tell topic
//	ros::Publisher tell_pub = nh.advertise<zeabus_controller::control_force>("/control/force", 1000)
	ros::Publisher tell_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 1000);
	
// setup dynamic configK of PID
	dynamic_reconfigure::Server<zeabus_controller::PIDConstantConfig> server;
	dynamic_reconfigure::Server<zeabus_controller::PIDConstantConfig>::CallbackType f;

	special_order special_command;

	init();
	std::cout << "mass of auv is " << mass << std::endl;
// 100% copy

	f = boost::bind(&config_constant_PID, _1, _2);
	server.setCallback(f);
	ros::Rate rate(50);
	ros::Rate sleep(10);
	while(nh.ok()){
		if(first_time_PID){
			std::cout << "Before download" << std::endl; 
			PID_file.load_file("Controller");
			std::cout << "Finish download" << std::endl;
			first_time_PID = false;
 			sleep.sleep();
			set_all_PID();
			reset_all_I();
            step_work = 8;
		}
		else if(change_PID){
			std::cout << "before save file" << std::endl;
			PID_file.save_file("Controller");
			std::cout << "finish save file" << std::endl;
			change_PID = false;
			set_all_PID();
			reset_all_I();
		}
		else{}
		if(step_work == 1){
		}
		else if(step_work==8){
			for(int i = 0; i < 6 ; i++){
		 		if (target_velocity[i] != 0){
					if(i == 0 || i == 1){
		 				target_position[0] = current_position[0];
						want_fix[0] = false;
		 				target_position[1] = current_position[1];
						want_fix[1] = false;
						i = 1;
					}
					else{
						target_position[i] = current_position[i];
						want_fix[i] = false;
					}
				}
				else want_fix[i] = true;
			}			
			calculate_out();
		}
		else{
			std::cout << "don't have this step" << std::endl;
			shutdown_force();
		}
		for(int i = 0; i < 6; i++){
			normal_error[i] = target_position[i] - current_position[i];
		}
		if(print_fatal){
			ROS_FATAL("current_position: \t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf", current_position[0], current_position[1], current_position[2], current_position[3], current_position[4], current_position[5]);
			ROS_FATAL("target_position: \t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf", target_position[0], target_position[1], target_position[2], target_position[3], target_position[4], target_position[5]);
			ROS_FATAL("normal_error: \t\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf", normal_error[0], normal_error[1], normal_error[2], normal_error[3], normal_error[4], normal_error[5]);
			ROS_FATAL("want_fix: \t\t%d\t%d\t%d\t%d\t%d\t%d", want_fix[0], want_fix[1], want_fix[2], want_fix[3], want_fix[4], want_fix[5]);
			ROS_FATAL("force_output: \t\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf", force_output[0], force_output[1], force_output[2], force_output[3], force_output[4], force_output[5]);
			ROS_FATAL("current_velocity:\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf", current_velocity[0], current_velocity[1], current_velocity[2], current_velocity[3], current_velocity[4], current_velocity[5]);
			ROS_FATAL("target_velocity:\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf", target_velocity[0], target_velocity[1], target_velocity[2], target_velocity[3], target_velocity[4], target_velocity[5]);
			ROS_INFO("--------------------------------------------------------------------------------------------------------------------------------------------");
		}
		std::cout << "Now force message is " << force_message << std::endl;
		if(force_message == "SAUV"){
			if(current_position[2] < warning_depth) special_command.special_pwm("max_x_up");
			else if(current_position[2] > warning_high) special_command.special_pwm("max_x_down");
//			else if(current_position[2] > warning_high) special_command.special_pwm("go_down");
			else special_command.special_pwm("max_x");
			target_position[0] = current_position[0];
			target_position[1] = current_position[1];
			cal_distance = sqrt( pow(target_position[0] - start_point[0] , 2) + pow( target_position[1] - start_point[1] , 2) );
			if(cal_distance > target_mission && close_mission){
				force_message == "";
				target_position[2] = -0.2;
                step_work = 8;
			} 
		}
		else if(force_message == "SHUTDOWN"){
			special_command.special_pwm("zero_force");
		}
		else{
			tell_pub.publish(message_cmd_vel);
		}
		shutdown_target_velocity();
		rate.sleep();
		ros::spinOnce();
	}
}

void calculate_out(){
	distance_xy = sqrt( pow(current_position[0] - target_position[0],2) + pow( current_position[1] - target_position[1] ,2) );
	distance_yaw = check_radian_tan(atan2( target_position[1] - current_position[1] , target_position[0] - current_position[0]));
	diff_yaw = find_min_angular(current_position[5] , distance_yaw);	
	std::cout << "distance xy : " << distance_xy << " yaw : " << distance_yaw << "  diff_yaw : " << diff_yaw << std::endl;
	for(int check = 0 ; check < 6 ; check++ ){
		if(want_fix[check]){
			if(check == 0 || check == 1){
				error_position[0] = distance_xy*cos(diff_yaw);
				error_position[1] = distance_xy*sin(diff_yaw);
				std::cout << "calculate error check : " << check << " error : " << error_position[check] << "\n";
				absolute_error = abs(error_position[check]);
				if(absolute_error < 0.3){
					force_output[check] = 0;
					reset_I_position(check);
				}
				else{
					force_output[check] = PID_position[check].calculate_PID(error_position[check] , current_velocity[check]);
				}
			}
			else if(check == 2){
				error_position[check] = target_position[check] - current_position[check];
				std::cout << "calculate error check : " << check << " error : " << error_position[check] << "\n";
				absolute_error = abs(error_position[check]);
				if(absolute_error < 0.3){
					force_output[check] = 0;
					reset_I_position(check);
				}
				else{
					force_output[check] = PID_position[check].calculate_PID(error_position[check] , current_velocity[check]);
				}
			}
			else{
				error_position[check] = find_min_angular(current_position[check] , target_position[check]);
				std::cout << "calculate angular check : " << check << " error : " << error_position[check];
				absolute_error = abs(error_position[check]);
				std::cout << " absolute : " << absolute_error << "\n";
				if(absolute_error < 0.1){
					force_output[check] = 0;
					reset_I_position(check);
				}
				else force_output[check] = PID_position[check].calculate_PID(error_position[check], 0);
			}
		}
		else{
//			error_velocity[check] = target_velocity[check] - current_velocity[check];	
//			force_output[check] = PID_velocity[check].calculate_PID(error_velocity[check], -1*current_velocity[check]);			
			force_output[check] = target_velocity[check];
		}
	}
	if(shutdown_force_z){
		force_output[2] = 0;
		force_output[3] = 0;
		force_output[4] = 0;
	}
	if(shutdown_force_xy){
		force_output[1] = 0;
		force_output[0] = 0;
		force_output[5] = 0;
	}
	message_cmd_vel = twist_to_message(force_output);	
}

void shutdown_target_velocity() { for(int i=0 ; i < 6 ; i++) target_velocity[i] = 0; }

void shutdown_force() { for(int i =0 ; i < 6 ; i++) force_output[i] = 0; }

void set_true_fix() { for(int i =0 ; i < 6 ; i++) want_fix[i] = true; }

// 3 max 2 position 1 velocity

void listen_current_state(const nav_msgs::Odometry message){
	tf::Quaternion quaternion(message.pose.pose.orientation.x, message.pose.pose.orientation.y, message.pose.pose.orientation.z, message.pose.pose.orientation.w);
	tfScalar roll, pitch, yaw;
	tf::Matrix3x3(quaternion).getRPY(roll, pitch, yaw);
	if( start_run || reset_position){
		target_position[0] = message.pose.pose.position.x;
		target_position[1] = message.pose.pose.position.y;
		target_position[2] = message.pose.pose.position.z;
		target_position[3] = 0.0;
		target_position[4] = 0.0;
		target_position[5] = check_radian_tan((double)yaw);
		start_point[0] = message.pose.pose.position.x;
		start_point[1] = message.pose.pose.position.y;
		shutdown_force();
		start_run = false;
		reset_position = false;
	}	
	current_position[0] = message.pose.pose.position.x;
	current_position[1] = message.pose.pose.position.y;
	current_position[2] = message.pose.pose.position.z;
	current_position[3] = check_radian_tan((double)roll);
	current_position[4] = check_radian_tan((double)pitch);
	current_position[5] = check_radian_tan((double)yaw);
	current_velocity[0] = message.twist.twist.linear.x;	
	current_velocity[1] = message.twist.twist.linear.y;	
	current_velocity[2] = message.twist.twist.linear.z;	
	current_velocity[3] = message.twist.twist.angular.x;	
	current_velocity[4] = message.twist.twist.angular.y;	
	current_velocity[5] = message.twist.twist.angular.z;	
}

void listen_target_velocity(const geometry_msgs::Twist message){
	target_velocity[0] = message.linear.x;
	target_velocity[1] = message.linear.y;
	target_velocity[2] = message.linear.z;
	target_velocity[3] = message.angular.x;
	target_velocity[4] = message.angular.y;
	target_velocity[5] = message.angular.z;
}

void listen_target_position(const geometry_msgs::Point message){
	target_position[0] = message.x;
	target_position[1] = message.y;
	target_position[2] = message.z;
}

void listen_mode(const std_msgs::Int16 message){}

void listen_target_depth(const std_msgs::Float64 message){
	target_position[2] = message.data;
	ROS_DEBUG("In listen depth");
}

void listen_absolute_orientation(const zeabus_controller::orientation message){
	target_position[3] = message.roll;
	target_position[4] = message.pitch;
	target_position[5] = message.yaw;
}

void listen_absolute_yaw(const std_msgs::Float64 message){
	target_position[5] = message.data;//specific
}

void listen_absolute_xy(const zeabus_controller::point_xy message){
	target_position[0] = message.x;
	target_position[1] = message.y;
}

void listen_real_yaw(const std_msgs::Float64 message){
	target_position[5] = current_position[5] - message.data;
	if(target_position[5] < 0) target_position[5]+=2*PI;
	else if(target_position[5] > 2*PI) target_position[5]-=2*PI;
}

void config_constant_PID(zeabus_controller::PIDConstantConfig &config, uint32_t level){
	ROS_ERROR("!!!--K changed---!!!");
	Kp_position[0] = config.KPPx;
	Kp_position[1] = config.KPPy;
	Kp_position[2] = config.KPPz;
	Kp_position[3] = config.KPProll;
	Kp_position[4] = config.KPPpitch;
	Kp_position[5] = config.KPPyaw;

	Ki_position[0] = config.KIPx;
	Ki_position[1] = config.KIPy;
	Ki_position[2] = config.KIPz;
	Ki_position[3] = config.KIProll;
	Ki_position[4] = config.KIPpitch;
	Ki_position[5] = config.KIPyaw;
	
	Kd_position[0] = config.KDPx;
	Kd_position[1] = config.KDPy;
	Kd_position[2] = config.KDPz;
	Kd_position[3] = config.KDProll;
	Kd_position[4] = config.KDPpitch;
	Kd_position[5] = config.KDPyaw;
	
	Kp_velocity[0] = config.KPVx;
	Kp_velocity[1] = config.KPVy;
	Kp_velocity[2] = config.KPVz;
	Kp_velocity[3] = config.KPVroll;
	Kp_velocity[4] = config.KPVpitch;
	Kp_velocity[5] = config.KPVyaw;

	Ki_velocity[0] = config.KIVx;
	Ki_velocity[1] = config.KIVy;
	Ki_velocity[2] = config.KIVz;
	Ki_velocity[3] = config.KIVroll;
	Ki_velocity[4] = config.KIVpitch;
	Ki_velocity[5] = config.KIVyaw;
	
	Kd_velocity[0] = config.KDVx;
	Kd_velocity[1] = config.KDVy;
	Kd_velocity[2] = config.KDVz;
	Kd_velocity[3] = config.KDVroll;
	Kd_velocity[4] = config.KDVpitch;
	Kd_velocity[5] = config.KDVyaw;

	Kvs_position[0] = config.KVSx;
	Kvs_position[1] = config.KVSy;
	Kvs_position[2] = config.KVSz;
	Kvs_position[3] = config.KVSroll;
	Kvs_position[4] = config.KVSpitch;
	Kvs_position[5] = config.KVSyaw;
	std::cout << "change PID" << std::endl;
	std::cout << "Kp_position of x is " << Kp_position[0] << std::endl;
	set_all_PID();
	if(not first_time_PID){
		change_PID = true;
	}
}

std_msgs::Bool is_at_fix_position(double error){}

std_msgs::Bool is_at_fix_orientation(double error){}

double get_roll_radian(double quaternion[4]){
	double result, x, y;
	y = 2*(quaternion[0] * quaternion[1] + quaternion[2]*quaternion[3]);
	x = pow(quaternion[0],2) + pow(quaternion[1],2) - pow(quaternion[2],2) - pow(quaternion[3],2);
	result = atan2(y,x);
	return check_radian_tan(result);
}

double get_yaw_radian(double quaternion[4]){
	double result, x, y;
	y = 2.0*(quaternion[0] * quaternion[3] + quaternion[2]*quaternion[1]);
	x = pow(quaternion[0],2) - pow(quaternion[1],2) - pow(quaternion[2],2) + pow(quaternion[3],2);
	result = atan2(y,x);
	ROS_ERROR("get yaw result is %.5lf",result);
//	std::cout << result << std::endl;
	return check_radian_tan(result);
}

double get_pitch_radian(double quaternion[4]){
	double result = ((-1)*asin( 2 * ( quaternion[1]*quaternion[3] - quaternion[0]*quaternion[2])) * 180 / PI);
	return result;
}

double check_radian_tan(double result){
	double check = result;
	if(check > 0 ) return result;
	else return result + 2*PI;
}

double find_min_angular(double current, double target){
	if(current > target){
		right_yaw = -(current - target);
		left_yaw = (2*PI)+right_yaw; 
	}
	else{
		left_yaw = target-current;
		right_yaw = -((2*PI)-left_yaw);
	}
	double abs_right_yaw = abs(right_yaw);
	double abs_left_yaw = abs(left_yaw);
	if(abs_right_yaw<abs_left_yaw) return right_yaw;
	else return left_yaw;
}

geometry_msgs::Twist twist_to_message(double twist[6]){
	geometry_msgs::Twist answer;
	answer.linear.x = twist[0];
	answer.linear.y = twist[1];
	answer.linear.z = twist[2];
	answer.angular.x = twist[3];
	answer.angular.y = twist[4];
	answer.angular.z = twist[5];
	return answer;
}

void message_to_quaternion(const geometry_msgs::Quaternion message, double* quaternion){
	*quaternion = message.x;
	*(quaternion+1) = message.y;
	*(quaternion+2) = message.z;
	*(quaternion+3) = message.w;
	
}

void set_all_PID(){
	for(int count = 0 ; count < 6 ; count++){
		PID_position[count].set_PID(Kp_position[count], Ki_position[count], Kd_position[count], Kvs_position[count]);
		PID_velocity[count].set_PID(Kp_velocity[count], Ki_velocity[count], Kd_velocity[count], 0);
	}
	reset_all_I();
}

void reset_I_position(int number){ PID_position[number].reset_I(); }

void reset_I_velocity(int number){ PID_velocity[number].reset_I(); }

void reset_all_I(){
	for(int check = 0 ; check < 6 ; check++){
		PID_position[check].reset_I();
		PID_velocity[check].reset_I();
	}
}

int found_sign(double direction){
	if(direction < 0) return -1;
	else return 1;
}

double abs(double a){
	if(a < 0) return a*(-1);
	return a;
}

int abs(int a){
	if(a < 0) return a*(-1);
	return a;
}


void listen_mode_control(const std_msgs::Int16 message){
	step_work = message.data;
}

void test_current_state(const geometry_msgs::Point message){
	current_position[0] = message.x;
	current_position[1] = message.y;
	current_position[2] = message.z;
}

void test_current_orientation(const zeabus_controller::orientation message){
	current_position[3] = message.roll;
	current_position[4] = message.pitch;
	current_position[5] = message.yaw;
}

bool same_direction(double num_01, double num_02){
	if(num_01*num_02 >= 0) return true;
	else return false;
}

bool service_target_distance( zeabus_controller::fix_rel_xy::Request &request , zeabus_controller::fix_rel_xy::Response &response){
	target_position[0] += request.distance_x*cos(target_position[5]);
	target_position[1] += request.distance_x*sin(target_position[5]);
	target_position[0] += request.distance_y*cos(target_position[5]+(PI/2));
	target_position[1] += request.distance_y*sin(target_position[5]+(PI/2));
	response.success = true;
	return true;
}

bool service_target_xy( zeabus_controller::fix_abs_xy::Request &request , zeabus_controller::fix_abs_xy::Response &response){
	target_position[0] = request.x;
	target_position[1] = request.y;
//	target_position[0] = request.data.x;
//	target_position[1] = request.data.y;
	want_fix[0] = true;
	want_fix[1] = true;	
	response.success = true;
	return true;
}

bool service_target_depth( zeabus_controller::fix_abs_depth::Request &request , zeabus_controller::fix_abs_depth::Response &response){
	target_position[2] = request.fix_depth;
	want_fix[2] = true;
	response.success = true;
	return true;
}

bool service_target_function( zeabus_controller::message_service::Request &request , zeabus_controller::message_service::Response &response){
	force_message = request.message.data;
	response.success = true;
	return true;
} 

bool service_target_x( zeabus_controller::fix_abs_x::Request &request , zeabus_controller::fix_abs_x::Response &response){
	target_position[0] = request.fix_x;
	want_fix[0] = true;
	response.success = true;
	return true;
}

bool service_target_y( zeabus_controller::fix_abs_y::Request &request , zeabus_controller::fix_abs_y::Response &response){
	target_position[1] = request.fix_y;
	want_fix[1] = true;
	response.success = true;
	return true;
}

bool service_target_yaw( zeabus_controller::fix_abs_yaw::Request &request , zeabus_controller::fix_abs_yaw::Response &response){
	target_position[5] = check_radian_tan(request.fix_yaw);
	want_fix[5] = true;
	response.success = true;
	return true;
}

void listen_imu_data(const sensor_msgs::Imu message){
	tf::Quaternion quaternion(message.orientation.x, message.orientation.y, message.orientation.z, message.orientation.w);
	tfScalar roll, pitch, yaw;
	tf::Matrix3x3(quaternion).getRPY(roll, pitch, yaw);
	std::cout << roll << "\t" << pitch << "\t" << yaw << std::endl;
}

/*
std_msgs::Bool check_ok_position(){
	std_msgs::Bool data;
	double check_error;
	check_error = abs(normal_error[0]);
	if( normal_error[0] < error_x) already_position[0] = true;
	else already_position[0] = false;
	check_error = abs(normal_error[1]);
	if( normal_error[1] < error_y) already_position[1] = true;
	else already_position[1] = false;
	check_error = abs(normal_error[2]);
	if( normal_error[2] < error_z) already_position[2] = true;
	else already_position[2] = false;
	if(already_position[0] && already_position[1] && already_position[2]) data.data = true;
	else data.data = false;
	return data;
}

std_msgs::Bool check_ok_yaw(){
	std_msgs::Bool data;
	double check_error = abs(normal_error[5]);
	if( normal_error[5] < error_yaw) already_position[5] = true;
	else already_position[5] = false;
	if(already_position[5]) data.data = true;
	else data.data = false;
	return data;
}
*/

bool service_ok_position( zeabus_controller::ok_position::Request &request , zeabus_controller::ok_position::Response &response){
    std::cout << "service check " << request.type.data << "and use adding is " << request.adding << std::endl;
//    ROS_WARN("Service Check is %s and adding is %.4lf", request.type.data , request.adding);
    if(request.type.data == "xy"){
        if( (abs(normal_error[0]) < error_x+request.adding) && (abs(normal_error[1]) < error_y+request.adding)) response.ok = true;
        else response.ok = false;
    }
    else if(request.type.data == "z"){
        if( (abs(normal_error[2]) < error_z+request.adding)) response.ok = true;
        else response.ok = false;
    }
	else if(request.type.data == "xyz"){
		if( (abs(normal_error[0]) < error_x+request.adding) && (abs(normal_error[1]) < error_y+request.adding) && abs(normal_error[2]) < error_z+request.adding) response.ok = true;
		else response.ok = false;
	}
	else if(request.type.data == "yaw"){
		if( (abs(error_position[5])) < error_yaw+request.adding) response.ok = true;
		else response.ok = false;
	}
    else response.ok = false;
//    ROS_WARN("result is %s", response.ok);
    std::cout << "Result is " << response.ok << std::endl;
    return true;
}
