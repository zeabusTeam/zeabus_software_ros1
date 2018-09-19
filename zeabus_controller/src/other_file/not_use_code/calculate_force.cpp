// please :set nu tabstop=4

#include 	<iostream> // include standard c++

// next line is for want to show data
#define debug

//#include 	<ros/ros.h> // include if you want to use time
//#include 	<math.h> // use math for calculate

//////////////////////////////////////////////////////////////////////////////////////////
//			-------Supasan Komonlit 2018--------------
// this code will use to calculate what force you want in each axis
// that mean I will use physics to calculate force so
// I have to know about mass to calculate that
// equation is SumOfF = ma
//		then SumOfF is F{thruster} + F{gravity} + F{drag or liquid} + F{nature} + F{adding}
//		we have decide about sign of variable by right hand rules
// I accept about my knowledge have few knowledge to use this solution
// but I think that is one way to control about velocity and have 
// constant force will want to control abot roll pitch in the future
/////////////////////////////////////////////////////////////////////////////////////////

// init const values
// mass is normal_mass + add_mass
const static double mass = 32.0;
const static double volumn = 44000.0/1000000.0;
const static double density_liquid = 1000;

// normal from is F(thruster) F(gravity) F(drag of liquid)

namespace find_force{
	// this class will use only about calculate axis
	class first_case_axis{
//use equation F{thruster} = ma{target} - F{gravity} - F{drag of liquid} - F{nature} + F{adding}
//						   = ma         - mg         - k(v{current})^2   - qVg{u}    + F{adding}
		private:
			double force_gravity;
			double force_thruster;

		protected:
			double constant; // constant is k
			double force_constant; //force_constant is F{nature}
			double force_adding;
			bool use_force_constant;
		
		public:
			first_case_axis(double gravity, double force_adding, 
							double constant, bool use_force_constant);
			double calculate_force_axis(double target_acceleration , double current_velocity);
			void set_constant(double constant, double force_adding, bool use_force_constant);	
	};
// set value will constant always for calculate
	first_case_axis::first_case_axis(	double gravity, 
										double force_adding,
										double constant, 
										bool use_force_constant){
		#ifdef debug
		std::cout 	<< "Init first case about find force \n";
		std::cout 	<< "Input value for gravity is " << gravity
					<< " and value for force_adding " << force_adding 
					<< " and use_force_constant " << use_force_constant << "\n";
		#endif
		this->force_gravity = gravity*mass;
		this->force_adding = force_adding;
		this->force_constant = density_liquid * volumn * gravity * (-1);
		this->use_force_constant = use_force_constant;
		this->constant = constant;
	}

// We will use this equation
//       F{thruster} = ma{target} - F{gravity} - F{drag of liquid} - F{nature} + F{adding}
	double first_case_axis::calculate_force_axis(	double target_acceleration, 
													double current_velocity){
//		if(this->use_force_constant){
			#ifdef debug
			std::cout 	<< "use force constant to calculate \n";
			std::cout 	<< "force_gravity is " << force_gravity;
			std::cout 	<< "\nforce_drag is " 
						<< this->constant * current_velocity * current_velocity << "\n";
			std::cout	<< "force_constant is " << this->force_constant;
			std::cout 	<< "\nforce_adding " << this->force_adding;
			#endif
			force_thruster =	mass * target_acceleration // ma 
							-	this->force_gravity // force_gravity
							-	this->constant * current_velocity * current_velocity // force_drag
							-	this->force_constant // F{nature}
							+	this->force_adding; // F{adding}
/*		}
		else
			force_thruster =	mass * target_acceleration // ma 
							-	this->force_gravity // force_gravity
							-	this->constant * current_velocity * current_velocity // force_drag
							+	this->force_adding; // F{adding}*/
	}
// for set any value can change
	void first_case_axis::set_constant(	double constant, 
										double force_adding, 
										bool use_force_constant){
		this->constant = constant;
		this->force_adding = force_adding;
		this->use_force_constant = use_force_constant;
	}

	//this class will use only about calculate angle
	// we will call force in this class torque
	// sum torque = torque_adding or torque = torque_tune
//	class first_case_angle{
			
//	};
}

#ifdef debug
	int main(){
		std::cout 	<< "Welcome to main of calculate force\n";
		std::cout 	<< "Welcome to test calculate force \n";
		std::cout 	<< "I will use find_force::first_case_axiis\n";
		std::cout 	<< "mass is " << mass << "\n";
		std::cout 	<< "volumn is " << volumn << "\n";
		std::cout 	<< "function calculate have use target_axeleration and current_velocity \n";
		find_force::first_case_axis test_z(-9.807 , 0 , 200 , true);
		double target_acceleration , current_velocity;
		int working;
		while(true){
			std::cout	<< "Input your data working target_acceleration current_velocity : ";
			std::cin	>> working >> target_acceleration >> current_velocity;
			if(working == 0) break;
			else std::cout << "\nresult is " << test_z.calculate_force_axis(target_acceleration , current_velocity) << "\n";
	}
}
#endif
