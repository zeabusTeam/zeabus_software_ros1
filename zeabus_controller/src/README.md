	first type : use 2 file
		new_controller_2018.cpp			in src
		2018_thruster_mapper_origin.py		in script
		this use PID to control output by input error orientation into PID

		status : already


	second type : use 2 file
		force_controller.cpp 			in src
		2018_thruster_mapper_force.py 		in script
		this use error orientation into PID for file target velocity
			but I have other way to consider that I decide to will not use that
		this use error velocity to find target accelaration by PHYSICS I
		this use target accelaration and current velocity and other to find force from thruster

		status	  :	can't use
		progress  :	test file calculate
