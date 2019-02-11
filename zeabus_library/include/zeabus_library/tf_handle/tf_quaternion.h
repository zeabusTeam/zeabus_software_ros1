/*
	File name			:	tf_quaternion.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 05
	Date last modified	:	2018 , ??? ??
	Purpose				:	For make function peration tf::Quaternion

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	production

	Namespace			:	zeabus_library::tf_handle
*/

#include	<tf/transform_broadcaster.h>

#include	<geometry_msgs/Vector3.h>

#include	<geometry_msgs/Quaternion.h>

#include	<stdio.h>

#include	<iostream>

#include	<math.h>

#ifndef PI
	#define	PI 3.14159265
#endif

#ifndef _ZEABUS_LIBRARY_TF_HANDLE_TF_QUATERNION_H__
#define _ZEABUS_LIBRARY_TF_HANDLE_TF_QUATERNION_H__

namespace zeabus_library{

namespace tf_handle{

	class TFQuaternion : public tf::Quaternion{

		public:
			TFQuaternion();
			TFQuaternion( const tfScalar& x , const tfScalar&y 
							, const tfScalar& z , const tfScalar& w );
			TFQuaternion( const tf::Vector3 &axis , const tfScalar& angle );
			TFQuaternion( const geometry_msgs::Quaternion& quaternion );
			TFQuaternion( const tfScalar& yaw , const tfScalar& pitch , const tfScalar& roll);

			void get_RPY( double& roll , double& pitch , double& yaw );

			double* rotation( double* problem );

			geometry_msgs::Vector3 rotation( geometry_msgs::Vector3 problem );
						
			void rotation( double& x , double& y , double& z );

			geometry_msgs::Quaternion get_quaternion();

			void operator=( tf::Quaternion q);
			void operator=( geometry_msgs::Quaternion q);

			void print_quaternion();
			void print_radian();
			void print_degree();

		private:
			double temp_double[3];
			geometry_msgs::Vector3 temp_vector3;

	};

	void edit_value( double& problem ); // make value -PI<= x <= PI

}

}

#endif
