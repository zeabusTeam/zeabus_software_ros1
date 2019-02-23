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

	TFQuaternion::TFQuaternion( const geometry_msgs::Quaternion& quaternion ) :
			Quaternion( quaternion.x , quaternion.y , quaternion.z , quaternion.w ){}

	TFQuaternion::TFQuaternion( const tf::Vector3 &axis , const tfScalar& angle ) : 
			Quaternion( axis , angle ){}

	TFQuaternion::TFQuaternion( const tfScalar& yaw , const tfScalar& pitch 
			, const tfScalar& roll) : Quaternion(){
		this->setRPY( roll , pitch , yaw );
	}

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
		this->setRPY( temp_double[0] , temp_double[1] , temp_double[2] );
	}

	void TFQuaternion::operator=( geometry_msgs::Quaternion q){
		tf::Matrix3x3( tf::Quaternion( q.x , q.y , q.z , q.w ) ).getRPY( 
				temp_double[0] , temp_double[1] , temp_double[2] );
		this->setRPY( temp_double[0] , temp_double[1] , temp_double[2] );
	}

	geometry_msgs::Vector3 TFQuaternion::rotation( geometry_msgs::Vector3 problem ){
		this->temp_vector3.x = problem.x;
		this->temp_vector3.y = problem.y;
		this->temp_vector3.z = problem.z;
		this->rotation( this->temp_vector3.x , this->temp_vector3.y , this->temp_vector3.z );
		return this->temp_vector3;
	}

	geometry_msgs::Quaternion TFQuaternion::get_quaternion(){
		geometry_msgs::Quaternion temp_quaternion;
		temp_quaternion.w = this->w();
		temp_quaternion.x = this->x();
		temp_quaternion.y = this->y();
		temp_quaternion.z = this->z();
		return temp_quaternion;
	}

	void TFQuaternion::print_quaternion(){
		printf("%10.4lf%10.4lf%10.4lf%10.4lf" , this->x() , this->y() , this->z() , this->w() );
	}
			
	void TFQuaternion::print_radian(){
		this->get_RPY( this->temp_double[0] , this->temp_double[1] , this->temp_double[2] );	
		printf("%10.4lf%10.4lf%10.4lf" , this->temp_double[0] , this->temp_double[1] 
				, this->temp_double[2] );
	}
			
	void TFQuaternion::print_degree(){
		this->get_RPY( this->temp_double[0] , this->temp_double[1] , this->temp_double[2] );	
		printf("%10.4lf%10.4lf%10.4lf" , this->temp_double[0] / PI * 180 
				, this->temp_double[1] / PI * 180 , this->temp_double[2] / PI * 180 );
	}

	void edit_value( double& problem){
		for( ; fabs(problem) > PI ; ){
			if( problem < 0 ) problem += ( 2*PI );
			else problem += (-2*PI);
		}
	}
}

}
