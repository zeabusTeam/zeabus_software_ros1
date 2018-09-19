// please set nu tabstop=4
// this include standard operation of c++
#include 	<iostream>

// this include for use time of ros
#include 	<ros/ros.h>

//////////////////////////////////////////////////////////////
//		------Supasan Komonlit 2018-------	
// This namespace using find what acceleration
// That have many way to find that
// In first case :
//		I will use about physics 1 to find about acceleration
//		but if want that you must have a time so I think 
//		we will use rate to consider that
// In second case :
//		I find to use tunning find what acceleration
//////////////////////////////////////////////////////////////

namespace find_acceleration{

// ----------------------------------------------------------------------------------------//

// I want to use first case
// I will use equation is acceleration = ( v{target} - v{current} ) / time
// so time is rate to calculate or spin time by c++ language for ros

	class first_case{
		protected:
			double calculate_time;
		public:
			first_case();
			double calculate_velocity(double target_velocity , double current_velocity);
			void set_constant(double calculate_time);
	};

// start init object class
	first_case::first_case(){
		this->calculate_time = 0;
	}

	double first_case::calculate_velocity(double target_velocity , double current_velocity){
		return ( target_velocity - current_velocity ) / time;
	}

// this about set up new time if you want to change rate for calculate acceleration
	void set_constant(double calculate_time){
		this->calculate_time = calculate_time;
	}

// I will not make second case because I don't want to use PID to calculate that
}
