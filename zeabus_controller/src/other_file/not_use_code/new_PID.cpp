#include <cstdio>
#include <ctime>
#include <ros/ros.h>

const static double default_ttl = 2.0;

class PID{
	public: 
		double calculate_PID(double error, double velocity);
		void set_PID(double Kp, double Ki, double Kd, double Kvs);
		PID();
		PID(double Kp, double Ki, double Kd, double Kvs);
		void reset_I();
	
	private:
		bool use_ttl;
		double Kp, Ki, Kd, Kvs;
		double sum_error;
		double previous_error;
		double diff_time;
		double diff_error;
		double ttl;//ttl(time to live)
		ros::Time previous_time;
};
	
	double PID::calculate_PID(double error, double velocity){
		ros::Time current_time = ros::Time::now();
		this->diff_time = current_time.toSec() - previous_time.toSec();//time.ToSec()(change time's unit to sec)
		this->sum_error += error * diff_time;//sum_error use in Ki
		this->diff_error = error - previous_error;
		this->use_ttl = true;
		double res = (this->Kp * error)+(this->Ki * sum_error)+((this->Kd * this->diff_error)/this->diff_time)-(this->Kvs * velocity);
                        std::cout << "Kp is " << Kp << "\n";
                        std::cout << "Ki is " << Ki << "\n";
                        std::cout << "Kd is " << Kd << "\n";
                        std::cout << "Kvs is " << Kvs << "\n";
                        
		if(use_ttl){
			if(this->use_ttl > default_ttl)
				reset_I();//I(integrate)
			else;
				this->ttl += this->diff_time;//ttl is time
		this->previous_time = current_time;
		return res;
		}
	}
	void PID::set_PID(double Kp, double Ki, double Kd, double Kvs){
		this->Kp = Kp;
		this->Ki = Ki;
		this->Kd = Kd;
		this->Kvs = Kvs;
	}
	
	PID::PID(){
		this->Kp = 0;
		this->Ki = 0;
		this->Kd = 0;
		this->Kvs = 0;
		this->ttl = 0;
		this->previous_time = ros::Time::now();
	}

	PID::PID(double Kp, double Ki, double Kd, double Kvs){
		this->Kp = Kp;
		this->Ki = Ki;
		this->Kd = Kd;
		this->Kvs = Kvs;
		this->ttl = 0;
		this->previous_time = ros::Time::now();
	}

	void PID::reset_I(){//about error(integrate)
		this->ttl = 0;
		this->sum_error = 0;
		this->previous_error = 0;
	}


