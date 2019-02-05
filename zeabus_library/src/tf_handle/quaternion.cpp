/*
	File name			:	quaternion.cpp		
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

#include	<zeabus_library/tf_handle/quaternion.h>

namespace zeabus_library{

namespace tf_handle{

	TF_Quaternion::TF_Quaternion() : Quaternion(){}

	TF_Quaternion::TF_Quaternion( const tfScalar& x , const tfScalar&y , const tfScalar& z 
					, const tfScalar& w ): Quaternion( x , y , z , w ){}

	TF_Quaternion::TF_Quaternion( const tf::Vector3 &axis , const tfScalar& angle ) : 
					Quaternion( axis , angle ){}

	TF_Quaternion::TF_Quaternion( const tfScalar& yaw , const tfScalar& pitch 
					, const tfScalar& roll) : Quaternion( yaw , pitch , roll ){}

	void TF_Quaternion::get_RPY( double& roll , double& pitch , double& yaw ){
		tf::Matrix3x3( *this ).getRPY( roll , pitch , yaw );
	}

	double* TF_Quaternion::rotation( double* problem ){
		tf::Quaternion data( problem[0] , problem[1] , problem[2] , 0 );
		data = (*this) * data;
		data *= this->inverse();
		this->temp_double[0] = data.x();
		this->temp_double[1] = data.y();
		this->temp_double[2] = data.z();
		return this->temp_double;
	}
			
	void TF_Quaternion::rotation( double& x , double& y , double& z ){
		tf::Quaternion data( x , y , z , 0 );
		data = (*this) * data;
		data *= this->inverse();
		x = data.x();
		y = data.y();
		z = data.z();
	}


}

}
