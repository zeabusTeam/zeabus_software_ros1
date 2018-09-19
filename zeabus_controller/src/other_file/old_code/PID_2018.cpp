#include <cstdio>
#include <ctime>
#include <ros/ros.h>

const static double default_ttl = 1.0;

class PID{
	protected: //this is private
		bool use_ttl;
		double Kp, Ki, Kd , Kvs;
		ros::Time previous_time;
		double sum_error;
		double previous_error;
		double diff_time;
		double diff_error;
		double ttl;

	public: //this is publlic
		double calculate_PID(double error, double velocity);
		double calculate_PID(double error);
		void set_PID(double Kp, double Ki, double Kd, double Kvs);
		void reset_PID();
		PID();
		PID(double Kp, double Ki, double Kd, double Kvs);
		void reset_I();
};
//This is __init__ object you will see if that init file you don't see syntax about type of return
	PID::PID(){
		this->Kp = 0;
		this->Ki = 0;
		this->Kd = 0;
		this->Kvs = 0;
		this->ttl = 0;
		this->previous_time = ros::Time::now();
		this->use_ttl = false;
	}

	PID::PID(double Kp, double Ki, double Kd, double Kvs){
		this->Kp = Kp;
		this->Ki = Ki;
		this->Kd = Kd;
		this->Kvs = Kvs;
		this->ttl = 0;
		this->previous_time = ros::Time::now();
		this->use_ttl = false;
	}

	double PID::calculate_PID(double error, double velocity){
		ros::Time current_time = ros::Time::now();
		this->diff_time = current_time.toSec() -this->previous_time.toSec();
		this->diff_error = error - previous_error;
		this->sum_error += error*this->diff_time;
// calculate   Kp     Ki     Kd    Kvs
		double result = ( this->Kp * error ) + ( this->Ki * this->sum_error ) + ( this->Kd * this->diff_error / this->diff_time) - (velocity*this->Kvs);
		if(use_ttl){
			if(this->ttl > default_ttl)
				reset_I();
			else
 				this->ttl+= this->diff_time;
		}
//		else{
//			this->reset_I();
//		}
		this->previous_time = current_time;
		return result;
	}

	double PID::calculate_PID(double error){
		ros::Time current_time = ros::Time::now();
		this->diff_time = current_time.toSec() - this->previous_time.toSec();
		this->diff_error = error - this->previous_error;
		this->sum_error += error*this->diff_time;
// calculate   Kp     Ki     Kd
		double result = ( this->Kp * error ) + ( this->Ki * this->sum_error * diff_time ) + ( this->Kd * this->diff_error / this->diff_time);
		if(use_ttl){
			if(this->ttl > default_ttl)
				this->reset_I();
			else
 				this->ttl+= this->diff_time;
		}
//		else{
//			this->reset_I();
//		}
		this->previous_time = current_time;
		return result;
	}

	void PID::set_PID(double Kp, double Ki, double Kd, double Kvs){
		this->Kp = Kp;
		this->Ki = Ki;
		this->Kd = Kd;
		this->Kvs = Kvs;
	}

	void PID::reset_I(){
		this->sum_error=0;
		this->ttl = 0;
		this->previous_error = 0;
	}
