/*
	File name			:	pid.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 24
	Date last modified	:	2018 , ??? ??
	Purpose				:	For receive message from localize in control system
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Product

	Namespace			:	zeabus_library/control
*/

#include	<stdio.h>

#include	<iostream>

#include	<math.h>

#include	<zeabus_library/general.h>

#ifndef _ZEABUS_LIBRARY_CONTROL_PID_H__
#define _ZEABUS_LIBRARY_CONTROL_PID_H__

namespace zeabus_library{

namespace control{

	class PID{

		public:
			PID( double p = 0 , double i = 0 , double d = 0 );
			~PID();
		
			void set_constant( double p , double i , double d );
			void offset_i( double offset );
			void limit_i( double limit );
			void set_max_limit( double limit );
			void set_min_limit( double limit );
			void set_limit( double min , double max );
			
			
			double calculate( double error );
			void calculate( double error , double& result );

			void set_period( double time );
			void reset_value();

		protected:
			bool first_time;
			double p;
			double i;
			double d;
			double period;
			// value condition of i
			double offset;
			double i_limit;
			double max_limit;
			double min_limit;
			double sum;	
			// value of pid
			double previous_error;
		
	};

}

}

#endif
