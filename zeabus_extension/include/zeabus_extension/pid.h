/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: pid.
//
//	Last Update		: Oct 16 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	:	for convert type to type
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef IOSTREAM
	#include	<iostream>
	#define		IOSTREAM
#endif

#ifndef VECTOR
	#include	<vector>
	#define		VECTOR
#endif

#ifndef _zeabus_extension_pid_h__
#define _zeabus_extension_pid_h__

namespace zeabus_extension{
namespace zeabus_control{

	class pid_control{
		public:
			pid_control( double p_constant = 0 , double i_constant = 0 
						,double d_constant = 0);
			template<typename value_type> void set_type_calculate( bool limit_size 
											, value_type value = 0);
			template<typename number_type > void set_constant( 
						number_type p_constant ,number_type i_constant ,number_type d_constant);

		protected:
			double p_constant;
			double i_constant;	
			double d_constant;
			double sum_error;
			bool assign_type;
			int bound_size;
			double bound_value;
			bool limit_size;
			std::vector<double> pid_result;
			std::vector<double> list_error;
		
	};

	class discrete_pid : pid_control{
		public:
			discrete_pid( double p_constant = 0 , double i_constant = 0 , double d_constant = 0);
			void calculate_result( double error , double& result);
			void reset_value();
			void bound_value_integral();
			void bound_size_integral();

		private:
			double integral_value;

	};		


}
}

#endif
