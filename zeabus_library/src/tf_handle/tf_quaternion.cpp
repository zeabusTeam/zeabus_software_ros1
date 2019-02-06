/*
	File name			:	tf_quaternion.cpp		
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

#include	<zeabus_library/tf_handle/tf_quaternion.h>

namespace zeabus_library{

namespace tf_handle{

	TFQuaternion::TFQuaternion() : Quaternion(){}

	TFQuaternion::TFQuaternion( const tfScalar& x , const tfScalar&y , const tfScalar& z 
					, const tfScalar& w ): Quaternion( x , y , z , w ){}

	TFQuaternion::TFQuaternion( const tf::Vector3 &axis , const tfScalar& angle ) : 
					Quaternion( axis , angle ){}

	TFQuaternion::TFQuaternion( const tfScalar& yaw , const tfScalar& pitch 
					, const tfScalar& roll) : Quaternion( yaw , pitch , roll ){}

	void TFQuaternion::get_RPY( double& roll , double& pitch , double& yaw ){
		tf::Matrix3x3( *this ).getRPY( roll , pitch , yaw );
	}

	double* TFQuaternion::rotation( double* problem ){
		tf::Quaternion data( problem[0] , problem[1] , problem[2] , 0 );
		data = (*this) * data;
		data *= this->inverse();
		this->temp_double[0] = data.x();
		this->temp_double[1] = data.y();
		this->temp_double[2] = data.z();
		return this->temp_double;
	}
			
	void TFQuaternion::rotation( double& x , double& y , double& z ){
		tf::Quaternion data( x , y , z , 0 );
		data = (*this) * data;
		data *= this->inverse();
		x = data.x();
		y = data.y();
		z = data.z();
	}

	void TFQuaternion::operator=( tf::Quaternion q){
		tf::Matrix3x3( q ).getRPY( temp_double[0] , temp_double[1] , temp_double[2] );
		this->setEulerZYX( temp_double[2] , temp_double[1] , temp_double[0] );
	}


}

}
