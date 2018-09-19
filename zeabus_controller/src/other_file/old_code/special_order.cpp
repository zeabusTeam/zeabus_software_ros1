#include <ros/ros.h>
#include <ros/time.h>
#include <hg_ros_pololu/Pwm.h>
#include <iostream>

class special_order{
	private:
		hg_ros_pololu::Pwm message_pwm;
		ros::NodeHandle nh;
		ros::Publisher tell_pwm;	
	
	public:
		void special_pwm(std::string message);
		special_order();
};

void special_order::special_pwm(std::string message){
	if(message == "max_x"){
		this->message_pwm.pwm[0] = 1460;
		this->message_pwm.pwm[1] = 1460;
		this->message_pwm.pwm[2] = 1537;
		this->message_pwm.pwm[3] = 1537;
		this->message_pwm.pwm[4] = 1100;
		this->message_pwm.pwm[5] = 1100;
		this->message_pwm.pwm[6] = 1900;
		this->message_pwm.pwm[7] = 1900;
	}
	else if(message == "zero_force"){
		this->message_pwm.pwm[0] = 1500;
		this->message_pwm.pwm[1] = 1500;
		this->message_pwm.pwm[2] = 1500;
		this->message_pwm.pwm[3] = 1500;
		this->message_pwm.pwm[4] = 1500;
		this->message_pwm.pwm[5] = 1500;
		this->message_pwm.pwm[6] = 1500;
		this->message_pwm.pwm[7] = 1500;
	}
	tell_pwm.publish(message_pwm);
}; 

special_order::special_order(){
	tell_pwm = nh.advertise<hg_ros_pololu::Pwm>("/pwm" , 1000);	
	message_pwm.pwm.resize(8);
}
