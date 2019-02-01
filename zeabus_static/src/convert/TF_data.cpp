/*
	File name			:	TF_data.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , FEB 01
	Date last modified	:	2019 , ??? ??
	Purpose				:	
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintain

	Namespace			:	zeabus_static::convert
*/

#include	<zeabus_static/convert/TF_data.h>

namespace zeabus_static{

namespace convert{

	tf::Vector3 tf_Vector3( geometry_msgs::Vector3& data ){
		return tf::Vector3( data.x , data.y , data.z );
	}

	tf::Quaternion tf_Quaternion( geometry_msgs::Quaternion& data){
		return tf::Quaternion( data.x , data.y , data.z , data.z );
	}

}

}
