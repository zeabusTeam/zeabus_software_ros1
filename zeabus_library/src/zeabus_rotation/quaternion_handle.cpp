/*
	File name			:	quaternion_handle.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of code for use metrix trasform by quaternion

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/zeabus_rotation/quaternion_handle.h>

#define _ROTATION_EULER_

#ifdef _ROTATION_EULER_
//	#define _DEBUG_ROTATION_EULER_
#endif

#define _ROTATION_QUATERNION_

#ifdef _ROTATION_QUATERNION_
//	#define _DEBUG_ROTATION_QUATERNION_
#endif

#define _DEBUG_ZEABUS_LIBRARY_QUATERNION_HANDLE_

namespace zeabus_library{

namespace zeabus_rotation{

	QuaternionHandle::QuaternionHandle() : Quaternion() {}

////////////////////////////////////////////////////////////////////////////////////////////////
//	This part is get matrix rotation 
//		if inverse is false you will get matrix rotation for start to target
//		if inverse is true you will get matrix rotation for target to start
//	reference http://www.chrobotics.com/library/understanding-quaternions
////////////////////////////////////////////////////////////////////////////////////////////////

	size_t QuaternionHandle::matrix_rotation( boost::numeric::ublas::matrix< double >& result 
											, bool inverse ){
		if( result.size1() != 3 || result.size2() != 3 ){
			zeabus_library::print_error(
				"zeabus_library::zeabus_rotation::matrix_rotation result wrong size matrix");
			return zeabus_library::ERROR_SIZE_MATRIX;
		}

		double ww = pow( *(this->w) , 2 );
		double xx = pow( *(this->x) , 2 );
		double yy = pow( *(this->y) , 2 );
		double zz = pow( *(this->z) , 2 );
		double xy = (*this->x) * (*this->y) * 2;
		double xz = (*this->x) * (*this->z) * 2;
		double yz = (*this->y) * (*this->z) * 2;
		double wx;
		double wy;
		double wz;
		if( ! inverse ){
			wx = (*this->w) * (*this->x) * 2;
			wy = (*this->w) * (*this->y) * 2;
			wz = (*this->w) * (*this->z) * 2;
		}else{
			wx = (*this->w) * (*this->x) * -2;
			wy = (*this->w) * (*this->y) * -2;
			wz = (*this->w) * (*this->z) * -2;
		}
		
		result( 0 , 0 ) = ww + xx - yy - zz;
		result( 0 , 1 ) = xy - wz;
		result( 0 , 2 ) = xz + wy;
		result( 1 , 0 ) = xy + wz;
		result( 1 , 1 ) = ww - xx + yy - zz;
		result( 1 , 2 ) = yz - wx;
		result( 2 , 0 ) = xz - wy;
		result( 2 , 1 ) = yz + wx;
		result( 2 , 2 ) = ww - xx - yy + zz;

		return zeabus_library::NO_ERROR;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////
//	 this part is about rotation 
//		equation is diff * q_1 = q_2 when	diff = quaternion inheratace of this object
//											q_1 = quaternion from function set_start_frame
//											q_2 = quaternion from function set_target_frame
//			That	mean q_1 is start_frame & start_quaternion 
//					meaa q_2 is target_frame * target_quaternion
//		Above equation when call by function update_ratation() < public function>
//	You can get matrix rotation by function matrix_rotation( result , inverse )	
//		normal value of inverse ins bool false when you want start to target			
////////////////////////////////////////////////////////////////////////////////////////////////

	void QuaternionHandle::update_rotation(){
		#ifdef _ROTATION_QUATERNION_

		zeabus_library::vector::multiplication( this->target_frame.matrix
												, this->start_frame.inverse_matrix 
												, this->matrix );	
		#ifdef _DEBUG_ROTATION_QUATERNION_
			zeabus_library::vector::print("Start_frame" , this->start_frame.matrix );
			zeabus_library::vector::print("Start_inverse" , this->start_frame.inverse_matrix );
			zeabus_library::vector::print("Target_frame" , this->target_frame.matrix );
			zeabus_library::vector::print("Different quaternion" , this->matrix);
			this->get_RPY( this->diff_euler[0] , this->diff_euler[1] , this->diff_euler[2] );
			printf("different quaternion roll : pitch : yaw === %8.4lf %8.4lf %8.4lf\n"
					, this->diff_euler[0] , this->diff_euler[1] , this->diff_euler[2] );
		#endif

		#endif

		#ifdef _ROTATION_EULER_

		this->start_frame.get_RPY( this->start_euler[0] 
								, this->start_euler[1] 
								, this->start_euler[2] );
		this->target_frame.get_RPY( this->target_euler[0]
								, this->target_euler[1]
								, this->target_euler[2] );
		zeabus_library::euler::find_min_diff( this->start_euler[0] 
											, this->target_euler[0] 
											, this->diff_euler[0] );
		zeabus_library::euler::find_min_diff( this->start_euler[1] 
											, this->target_euler[1] 
											, this->diff_euler[1] );
		zeabus_library::euler::find_min_diff( this->start_euler[2] 
											, this->target_euler[2] 
											, this->diff_euler[2] );
		#ifdef _DEBUG_ROTATION_EULER_
			printf("        |  roll  |  pitch  |  yaw  \n" );
			printf("  start |%8.4lf|%8.4lf|%8.4lf\n", this->start_euler[0] 
													, this->start_euler[1]
													, this->start_euler[2] );
			printf(" target |%8.4lf|%8.4lf|%8.4lf\n", this->target_euler[0] 
													, this->target_euler[1]
													, this->target_euler[2] );
			printf("  diff  |%8.4lf|%8.4lf|%8.4lf\n", this->diff_euler[0] 
													, this->diff_euler[1]
													, this->diff_euler[2] );
		#endif

		#endif
	}	
	
	// this set up part support only set up by zeabus_library::zeabus_rotation::Quaternion
	//										& zeabus_library::Point4
	//										& roll , pitch , yaw	

	void QuaternionHandle::set_start_frame( double roll , double pitch , double yaw ){
		this->start_frame.set_quaternion( roll , pitch , yaw );
		this->start_frame.update_inverse();
	}

	void QuaternionHandle::set_start_frame( 
			zeabus_library::zeabus_rotation::Quaternion quaternion ){
		this->start_frame = quaternion;
		this->start_frame.update_inverse();
	}

	void QuaternionHandle::set_start_frame( zeabus_library::Point4 data ){
		this->start_frame.set_quaternion( data );
		this->start_frame.update_inverse();
	}

	void QuaternionHandle::set_target_frame( double roll , double pitch , double yaw ){
		this->target_frame.set_quaternion( roll , pitch , yaw );
//		this->target_frame.update_inverse();
	}

	void QuaternionHandle::set_target_frame( 
			zeabus_library::zeabus_rotation::Quaternion quaternion ){
		this->target_frame = quaternion;
//		this->target_frame.update_inverse();
	}

	void QuaternionHandle::set_target_frame( zeabus_library::Point4 data ){
		this->target_frame.set_quaternion( data );
//		this->target_frame.update_inverse();
	}

}

}
