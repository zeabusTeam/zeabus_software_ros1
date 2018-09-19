// please set nu tabstop=4
// this include standard operation  of c++

#include	<stdio.h>	// include standard of c so c++ can use library of c
#include	<iostream> 	// include standard function of c++
#include	<ctime>		// include libary about time

#include	<order_log> // for collect data to log
#include	"command.cpp" // for save log

class pid_delta{
	private:
		double constant_p; // constant multi of porpotional
		double constant_i; // constant multi of Integrate and sum
		double constant_d; // constant mulit of different
		double limit_sum_error; //  limit max value from I
		double waiting_time; // waiting time and decide to add or not
		time_t collect_time; // have data about time to collect time		
		time_t current_time; // to know about now time
		double diff_time; // for calculate double of differrent time
		order_log log;

	public:
		pid_delta();
		void set_log( std::string name_file);
		void set_constant( double p , double i , double d 
							, double waiting_time , double ok_error);
		void reset();
		double sum_force; // is only I
		double pid_force;
		double calculate;
		double ok_error;
		double calculate( double error);
};


pid_delta::pid_delta(){}

void pid_delta::set_log( std::string name_file){
	log.set_log( name_file , "zeabus_controller" , "src/delta_control");
}

void pid_delta::reset(){
	time(&collect_time);
	sum_force = 0;
}

void pid_delta::set_constant( double p , double i , double d 
							, double waiting_time , double ok_error){
	this->constant_p = p;
	this->constant_i = i;
	this->constant_d = d;
	this->waiting_time = waiting_time;
	this->ok_error = ok_error;
}
