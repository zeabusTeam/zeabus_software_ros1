/*
	File name			:	listen_DVL_geometry.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 09
	Date last modified	:	2018 , ??? ??
	Purpose				:	For receive message from dvl node < velocity in robot frame >
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Product

	Namespace			:	zeabus_library/localize
*/

#include	<stdio.h>

#include	<zeabus_library/matrix.h>

#include	<zeabus_library/error_code.h>

#include	<geometry_msgs/TwistWithCovarianceStamped.h>

#ifndef _ZEABUS_LIBRARY_LOCALIZE_LISTEN_DVL_GEOMETRY_H__
#define _ZEABUS_LIBRARY_LOCALIZE_LISTEN_DVL_GEOMETRY_H__

namespace zeabus_library{

namespace localize{

	class ListenDVLGeometry{

		public:
			ListenDVLGeometry( boost::numeric::ublas::matrix< double>& variable );
		
			size_t register_velocity( boost::numeric::ublas::matrix< double >& variable );

			void callback( const geometry_msgs::TwistWithCovarianceStamped& message );

		private:
			boost::numeric::ublas::matrix< double > velocity;

	};

}

}

#endif
