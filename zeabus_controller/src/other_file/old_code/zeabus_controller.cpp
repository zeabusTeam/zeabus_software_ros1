#include <ros/ros.h>
#include <ros/time.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Quaternion.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Bool.h>
#include <stdlib.h>
#include <nav_msgs/Odometry.h>
#include "PID.cpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <tf/transform_datatypes.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Vector3.h>
#include <dynamic_reconfigure/server.h>
#include <zeabus_controller/PIDConstantConfig.h>
#include <queue>
#include "PID_constant_helper.h"
#include <iostream>
#include <zeabus_controller/drive_x.h>
#include <modbus_ascii_ros/Switch.h>

#define boolToStr(a) a?"true":"false"
#define abs(a) a<0?-a:a
#define	MIN_ERROR 5.0e-2
#define epsilon 1.0e-7
#define MAX_OUT 2
#define MIN_OUT -1
double err_angle = 0;
double fix_rel_x_dist = 0;
double bouyancy = -0.0;
char axis[6][5] = {"x","y","z","r","p","y"};
//old
/*
double cmdVelK[][3] = {{0.7,0.05,0},
					{0.7,0.05,0},
					{0.7,0,0},
					{0.15,0,0.1},
					{0.15,0,0.1},
					{0.1,0,0.1},
					}; //KP,KI,KD KP only cause order of acceleration
double fixPointK[][3] = {{0.7,0.1,0.2},
					{0.7,0.1,0.2},
					{3,0.1,0.2},
					{0.4,0.1,0.08},
					{0.3,0.05,0.2},
					{0.4,0.01,0.01},
					}; // KP,KI,KD
*/


double cmdVelK[][3] = {{0.1,0.0,0.1},
					{0.2,0.0,0.2},
					{0.0,0.1,0},
					{0.0,0,0.0},
					{0.0,0,0.0},
					{0.2,0,0.2},
					}; //KP,KI,KD KP only cause order of acceleration
double fixPointK[][3] = {{0.0,0.0,0.0},
					{0.01,0.0,0.0},
					{0.5,0.0015,0.0001},
					{0.2,0.0,0.0},
					{0.6,0.0,0.0},
					{0.0,0.005,0.001},
					}; // KP,KI,KD


geometry_msgs::Pose fixPosition;
nav_msgs::Odometry previousState;
nav_msgs::Odometry currentState;
double cmd_vel[6]={0,0,0,0,0,0},position[7],vel[6],fixpos[7];
double prevPosition[6],prevVel[6];
volatile bool is_switch_on = true; //false
volatile bool isStateArrived = true;
bool isFixed[] = {false,false,true,true,true,true};
bool canFixed[] = {true,true,true,true,true,true};
bool nearZeroBeforeFix[] = {false,false,false,false,false,false};
//sk uncomment
//double fixedPosition[7] = {0,0,-1,0,0,0,1}; // x y z ? ? ? set for default fix position; //just open
//double fixedPosition[7] = {2.9,6.8,3,0,0,0,0}; // x y z ? ? ? set for default fix position; //just open
double errorPosition[6] = {0,0,0,0,0,0};
double errorVelocity[6] = {0,0,0,0,0,0};
double out[6];
SPID *pidV,*pidP;
std::queue<geometry_msgs::Quaternion> fixPositionQueue;

int normalMode = 1; // fix z roll pitch
int freeMode = 2; // free roll pitch control
int barrelRollMode = 3; // for doing barrel roll fix depth yaw
int controllerMode = normalMode;
void validateValue(double&);
void PIDConstantCallBack(zeabus_controller::PIDConstantConfig &config,uint32_t level);
void stateListenerCallBack(const nav_msgs::Odometry msg);
void fixposCallBack(const nav_msgs::Odometry msg);
void cmd_velCallBack(const geometry_msgs::Twist msg);
void fixDepthCallBack(const std_msgs::Float64 msg);
void fixYawCallBack(const std_msgs::Float64 msg);
void changeFixedState();
void setPreviousState();
bool equal(double a, double b);
bool isClose(double a, double b);
bool outOfBound(double);
void calculateError();
void setK();
double angleError(double a,double b);
geometry_msgs::Twist calculatePID();
std_msgs::Bool is_at_fix_position(double err);
std_msgs::Bool is_at_fix_orientation(double err);
void cmd_fix_positionCallBack(const geometry_msgs::Point msg);
void cmd_fix_orientationCallBack(const geometry_msgs::Quaternion msg);
void modeCallback(const std_msgs::Int16 msg);
void fixAbsDepthCallBack(const std_msgs::Float64 msg);
//void fixAbsXCallBack(const std_msgs::Float64 msg);
void fixAbsYawCallBack(const std_msgs::Float64 msg);
void fixRelYawCallBack(const std_msgs::Float64 msg);
void fixRelXCallBack(const std_msgs::Float64 msg);
void switch_callback(const modbus_ascii_ros::Switch msg);
void pushSlerp(tf::Quaternion fq);
bool is_at_fix_position_bool(double err);
bool is_at_fix_orientation_bool(double err);
bool fix_rel_x_srv_callback(zeabus_controller::drive_x::Request &req,zeabus_controller::drive_x::Response &res);


void init(){
	//std::cout << "asdasd" << std::endl;
	pidV = (SPID*)calloc(6,sizeof(SPID));
	pidP = (SPID*)calloc(6,sizeof(SPID));
	for(int i=0;i<6;i++){
		vel[i]=0;
		cmd_vel[i]=0;
		position[i]=0;
		fixpos[i]=0;
		pidV[i].resetPID();
		pidP[i].resetPID();
		pidP[i].setK(fixPointK[i][0],fixPointK[i][1],fixPointK[i][2]);
		pidV[i].setK(cmdVelK[i][0],cmdVelK[i][1],cmdVelK[i][2]);

	}
	fixPosition.orientation.x = 0;
	fixPosition.orientation.y = 0;
	fixPosition.orientation.z = 0;
	fixPosition.orientation.w = 1;
	std::cout << "INIT CONTROLLER" << std::endl;
	PID_constant_helper::load_file("zeabus_controller"); // TODO if ros change this node name ?
}

int main(int argc,char **argv) {
	//printf("0 Out      %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",out[0],out[1],out[2],out[3],out[4],out[5]);
	
	ros::init(argc,argv, "Controller");
	ros::NodeHandle nh;
	ros::Subscriber sub_state = nh.subscribe("/auv/state", 1000, &stateListenerCallBack);
	//ros::Subscriber sub_fixpos = nh.subscribe("/auv/fixpos", 1000, &fixposCallBack);
	ros::Subscriber sub_cmd_vel = nh.subscribe("/zeabus/cmd_vel", 1, &cmd_velCallBack);
	//ros::Subscriber sub_cmd_vel = nh.subscribe("/zeabus/cmd_vel", 1000, &cmd_velCallBack);
	ros::Subscriber sub_cmd_fix_pos = nh.subscribe("/cmd_fix_position", 1000, &cmd_fix_positionCallBack); //fix x,y,z=true
	ros::Subscriber sub_cmd_fix_orientation = nh.subscribe("/cmd_fix_orientation", 1000, &cmd_fix_orientationCallBack); //fix row,pitch.yaw=true 
	ros::Subscriber sub_controllerMode =  nh.subscribe("/zeabus_controller/mode",1000,&modeCallback);
	ros::Subscriber sub_fixAbsDepth = nh.subscribe("/fix/abs/depth", 1000, &fixAbsDepthCallBack); //fix z=true
	//ros::Subscriber sub_fixAbsX = nh.subscribe("/fix/abs/x", 1000, &fixAbsXCallBack); //fix x=true
	ros::Subscriber sub_fixRelYaw = nh.subscribe("/fix/rel/yaw", 1000, &fixRelYawCallBack); //fix yaw=true
	ros::Subscriber sub_fixAbsYaw = nh.subscribe("/fix/abs/yaw", 1000, &fixAbsYawCallBack); //fix yaw=true work normal mode
	ros::Subscriber sub_fixRelX = nh.subscribe("/fix/rel/x",1000, &fixRelXCallBack); //fix x,y=true
	ros::Subscriber sub_switch = nh.subscribe("/switch/data",100, &switch_callback); //motor
	ros::ServiceServer service = nh.advertiseService("/fix_rel_x_srv",fix_rel_x_srv_callback);
	ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel",1000);
	ros::Publisher is_at_fix_position_pub = nh.advertise<std_msgs::Bool>("/zeabbus_controller/is_at_fix_position",1000);
	ros::Publisher is_at_fix_orientation_pub = nh.advertise<std_msgs::Bool>("zeabus_controller/is_at_fix_orientation",1000);
	//just ros
	//write::Publisher is_at_fix_distance_pub = nh.advertise<std_msgs::Bool>("/zeabus_controller/is_at_fix_distance",1000);
	//ros::Publisher fixedPositionPublisher = nh.advertise<geometry_msgs::Pose>("/controller/fixed_position",10); //just open
	dynamic_reconfigure::Server<zeabus_controller::PIDConstantConfig> server;
  	dynamic_reconfigure::Server<zeabus_controller::PIDConstantConfig>::CallbackType f;
	init();
  	f = boost::bind(&PIDConstantCallBack, _1, _2);
  	server.setCallback(f);
	ros::Rate rate(50);
	while(nh.ok()) {
	//int count_round = 0;
	//while(count_round<101){
		ros::spinOnce();
		//printf("1 Out      %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",out[0],out[1],out[2],out[3],out[4],out[5]);
		
		if(!isStateArrived || !is_switch_on){ 
			if(!isStateArrived) ROS_INFO("No state");
			ROS_INFO("No state arrived or motor switch is off wait 1 sec");
			ros::Duration(1).sleep();
			continue;
		}
		//printf("2 Out      %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",out[0],out[1],out[2],out[3],out[4],out[5]);
		
		changeFixedState();
		//printf("3 Out      %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",out[0],out[1],out[2],out[3],out[4],out[5]);
		
		if(!fixPositionQueue.empty()){
			fixPosition.orientation = fixPositionQueue.front();
			fixPositionQueue.pop();
			//printf("%lf %lf %lf\n",fixPosition.orientation.x,fixPosition.orientation.y,fixPosition.orientation.z);
		}
		//printf("4 Out      %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",out[0],out[1],out[2],out[3],out[4],out[5]);
		
		rate.sleep();
		pub.publish(calculatePID());
		//printf("5 Out      %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",out[0],out[1],out[2],out[3],out[4],out[5]);
		
		is_at_fix_position_pub.publish(is_at_fix_position(0.03));
		is_at_fix_orientation_pub.publish(is_at_fix_orientation(0.8)); //0.8
		//printf("6 Out      %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",out[0],out[1],out[2],out[3],out[4],out[5]);
		
		//is_at_fix_distance_pub.pblish();
		//fixedPositionPublisher.publish(fixPosition); //just open
		//ROS_INFO("%d",is_switch_on);
		if(true){
		 printf("Vel      %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",vel[0],vel[1],vel[2],vel[3],vel[4],vel[5]);
		 printf("cmd Vel  %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",cmd_vel[0],cmd_vel[1],cmd_vel[2],cmd_vel[3],cmd_vel[4],cmd_vel[5]);
		 printf("errorV   %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",errorVelocity[0],errorVelocity[1],errorVelocity[2],errorVelocity[3],errorVelocity[4],errorVelocity[5]);
		 printf("errorP   %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",errorPosition[0],errorPosition[1],errorPosition[2],errorPosition[3],errorPosition[4],errorPosition[5]);
		 printf("isFixed    %s\t%s\t%s\t%s\t%s\t%s\n",boolToStr(isFixed[0]),boolToStr(isFixed[1]),boolToStr(isFixed[2]),boolToStr(isFixed[3]),boolToStr(isFixed[4]),boolToStr(isFixed[5]));
		 printf("Out      %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",out[0],out[1],out[2],out[3],out[4],out[5]);
		 printf("Pos      %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",position[0],position[1],position[2],position[3],position[4],position[5],position[6]);
		 printf("FixPos   %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",fixPosition.position.x
		  																	,fixPosition.position.y
		  																	,fixPosition.position.z
		  																	,fixPosition.orientation.x
		  																	,fixPosition.orientation.y
		  																	,fixPosition.orientation.z
		  																	,fixPosition.orientation.w);
		 //printf("%lf %lu\n",err_angle,fixPositionQueue.size());
		 printf("\n\n");
		}
		setK();
		//count_round++;
	}
}

void stateListenerCallBack(const nav_msgs::Odometry msg){
	

	//printf("++++++++++++++++SAD+ASD++++++++++++++++++++++++++++++++ASAS+++++++++++++++++++++++++++++++++SAS+++++++++++++++++++++++\n");
printf("pq %lf %lf %lf %lf\n",currentState.pose.pose.orientation.x,currentState.pose.pose.orientation.y,currentState.pose.pose.orientation.z,currentState.pose.pose.orientation.w);
	currentState = msg;
	
	ROS_INFO("%d",is_switch_on);
	if(isStateArrived == false && is_switch_on){
		tf::Quaternion ini(msg.pose.pose.orientation.x,msg.pose.pose.orientation.y,msg.pose.pose.orientation.z,msg.pose.pose.orientation.w);
		tfScalar R,P,Y;
		//printf("before stateListener    %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",out[0],out[1],out[2],out[3],out[4],out[5]);
		tf::Matrix3x3(ini).getRPY(R,P,Y);
		//printf("after stateListener    %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",out[0],out[1],out[2],out[3],out[4],out[5]);
		tf::Quaternion fixY;
		fixY.setRPY(0,0,Y);
		tf::quaternionTFToMsg(fixY,fixPosition.orientation);
		fixPosition.position.x = msg.pose.pose.position.x;
		fixPosition.position.y = msg.pose.pose.position.y;
		fixPosition.position.z = msg.pose.pose.position.z;
		isStateArrived = true;
	}
	position[0] = msg.pose.pose.position.x; // position = ตำแหน่ง
	position[1] = msg.pose.pose.position.y;
	position[2] = msg.pose.pose.position.z;
	position[3] = msg.pose.pose.orientation.x; // orientation = ทิศทางชี้หุ่น
	position[4] = msg.pose.pose.orientation.y;
	position[5] = msg.pose.pose.orientation.z;
	position[6] = msg.pose.pose.orientation.w;
	vel[0] = msg.twist.twist.linear.x; //twist = ความเร็ว
	vel[1] = msg.twist.twist.linear.y;
	vel[2] = msg.twist.twist.linear.z;
	vel[3] = msg.twist.twist.angular.x;
	vel[4] = msg.twist.twist.angular.y;
	vel[5] = msg.twist.twist.angular.z;
}
/*
void fixposCallBack(const nav_msgs::Odometry msg){
	fixpos[0] = msg.fixPosition.position.x; // position = ตำแหน่ง
	fixpos[1] = msg.fixPosition.position.y;
	fixpos[2] = msg.fixPosition.position.z;
	fixpos[3] = msg.fixPosition.orientation.x; // orientation = ทิศทางชี้หุ่น
	fixpos[4] = msg.fixPosition.orientation.y;
	fixpos[5] = msg.fixPosition.orientation.z;
	fixpos[6] = msg.fixPosition.orientation.w;
}
*/

void cmd_velCallBack(const geometry_msgs::Twist msg){
	double cmd_vel_in[6];
	cmd_vel_in[0] = msg.linear.x;
	cmd_vel_in[1] = msg.linear.y;
	cmd_vel_in[2] = msg.linear.z;
	cmd_vel_in[3] = msg.angular.x;
	cmd_vel_in[4] = msg.angular.y;
	cmd_vel_in[5] = msg.angular.z;
	for(int i = 0;i < 6;i++){
		if(!isClose(cmd_vel_in[i],cmd_vel[i])){
			pidV[i].resetPID();
		}
		cmd_vel[i] = cmd_vel_in[i];
	}
}

void cmd_fix_positionCallBack(const geometry_msgs::Point msg){
	isFixed[0] = true;
	isFixed[1] = true;
	isFixed[2] = true;
	fixPosition.position = msg;
}

void cmd_fix_orientationCallBack(const geometry_msgs::Quaternion msg){
	isFixed[3] = true; 
	isFixed[4] = true; 
	isFixed[5] = true;

	fixPosition.orientation = msg;
}

void setPreviousState(){
	for(int i=0;i<6;i++){
		prevPosition[i] = position[i];
		prevVel[i] = position[i];
	}
}
template<typename T>
void copy(T &a,T &b,size_t n){
	while(n--)a[n]=b[n];
}
void changeFixedState(){
	double fixedPosition[7] = {fixPosition.position.x,
							fixPosition.position.y,
							fixPosition.position.z,
							fixPosition.orientation.x,
							fixPosition.orientation.y,
							fixPosition.orientation.z,
							fixPosition.orientation.w};
	// Position State
	for(int i = 0;i < 6;i++){
		// !fix -> fix
		if(equal(0.0,cmd_vel[i]) && !isFixed[i] && (isClose(vel[i],0.0) || !nearZeroBeforeFix[i]) && canFixed[i]){
			isFixed[i] = true;
			fixedPosition[i] = position[i];
			pidP[i].resetPD();
		}
		// fix -> !fix
		else if(!equal(0.0,cmd_vel[i]) && isFixed[i]){
			isFixed[i] = false;
			pidV[i].resetPD();
		}
		if(!isFixed[i]){
			fixedPosition[i] = position[i];
		}
	}

	if(controllerMode == freeMode){
		if(!isFixed[0] || !isFixed[1] || !isFixed[2]){
			isFixed[0] = false;
			isFixed[1] = false;
			isFixed[2] = false;
			//copy(fixedPosition,position,3);
			fixedPosition[0] = position[0];
			fixedPosition[1] = position[1];
			fixedPosition[2] = position[2];
		}
		if(!isFixed[3] || !isFixed[4] || !isFixed[5]){
				isFixed[3] = false;
				isFixed[4] = false;
				isFixed[5] = false;
				fixPosition = currentState.pose.pose;
		}
	}
	if(controllerMode == barrelRollMode){
		//if(!isFixed[0] || !isFixed[1] || !isFixed[2]){
			//isFixed[0] = false;
			//isFixed[1] = false;
			//isFixed[2] = false;
			//copy(fixedPosition,position,3);
			//fixedPosition[0] = position[0];
			//fixedPosition[1] = position[1];
			//fixedPosition[2] = position[2];
		//}
		//if(!isFixed[3] || !isFixed[4] || !isFixed[5]){
		//		isFixed[3] = false;
		//		//isFixed[4] = false;
		//		//isFixed[5] = false;
		//		fixPosition.orientation = currentState.pose.pose.orientation;
		//}
	}else if(controllerMode == normalMode){
		//printf("normal:%d%d%d%d%d%d -----------------------------------------------------------------------\n",isFixed[0],isFixed[1],isFixed[2],isFixed[3],isFixed[4],isFixed[5]);
		if(!isFixed[0] || !isFixed[1]){
			isFixed[0] = false;
			isFixed[1] = false;
			fixedPosition[0] = position[0];
			fixedPosition[1] = position[1];
		}
		/*
		if(isFixed[3] || isFixed[4]){
			isFixed[3] = false;
			isFixed[4] = false;
			fixedPosition[3] = position[3];
			fixedPosition[4] = position[4];
		}
		*/
	}
	if(!isFixed[3] || !isFixed[4] || !isFixed[5]){
		fixedPosition[6] = position[6];
	}
	fixPosition.position.x = fixedPosition[0];
	fixPosition.position.y = fixedPosition[1];
	fixPosition.position.z = fixedPosition[2];
	fixPosition.orientation.x = fixedPosition[3];
	fixPosition.orientation.y = fixedPosition[4];
	fixPosition.orientation.z = fixedPosition[5];
	fixPosition.orientation.w = fixedPosition[6];
}

void calculateError(){

	// Calculate Orientation error

	tf::Quaternion fq(fixPosition.orientation.x,
                    fixPosition.orientation.y,
                    fixPosition.orientation.z,
                    fixPosition.orientation.w);
		
  	tf::Quaternion pq(currentState.pose.pose.orientation.x,
                    currentState.pose.pose.orientation.y,
                    currentState.pose.pose.orientation.z,	
                    currentState.pose.pose.orientation.w);

	//printf("pq %lf %lf %lf %lf\n",currentState.pose.pose.orientation.x,currentState.pose.pose.orientation.y,currentState.pose.pose.orientation.z,currentState.pose.pose.orientation.w);
	tf::Quaternion p =  fq*pq.inverse();
//printf("+++++++++++++++++++++++++++++++**********************************************&W*EDSHSADJKAAAAAAAAAAAAAAAAAAAAAAAAAAA++++++++++++++++++++++++\n");
	//printf("%d\n",sizeof(pq)/sizeof(pq[0]));
	//printf("%lf %lf %lf\n",fq[0],pq[0],p[0]);
	//printf("iiiiiiiiii inverse p errorP   %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",errorPosition[0],errorPosition[1],errorPosition[2],errorPosition[3],errorPosition[4],errorPosition[5]);
	//tf::Quaternion nq(p.getAxis().getX()
	//				,p.getAxis().getY()
	//				,p.getAxis().getZ()
	//				,0.0);
	//double L = nq.getAngle();
	//nq.normalize();

	pq = pq.inverse() * p * pq;
	//printf("----------------------------------------------------------------------------------------------------------------------------------------------line 439 result "); //%i \n",sizeof(pq[0]));
	//for(int count_x = 0; count_x < 4; count_x++){
		//for(int count_y = 0; count_y <8; count_y ++){
		//	printf("%.4f\t",pq[count_x][count_y]);
		//}
	//	printf("%.4f\n",pq[count_x]);
	//}

	//printf("iiiiiiiiii inverse pq errorP   %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",errorPosition[0],errorPosition[1],errorPosition[2],errorPosition[3],errorPosition[4],errorPosition[5]);
	geometry_msgs::Quaternion neww;
	tf::quaternionTFToMsg(pq,neww);
	//printf("nnnnnnnnnn new quaternionTFToMsg errorP   %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",errorPosition[0],errorPosition[1],errorPosition[2],errorPosition[3],errorPosition[4],errorPosition[5]);
	tfScalar R,P,Y;
	//printf("%lf %lf %lf\n",R,P,Y);	
	tf::Matrix3x3(pq[0],pq[1],pq[2],pq[3],1,1,currentState.pose.pose.position.x,currentState.pose.pose.position.y,currentState.pose.pose.position.z).getRPY(R,P,Y);
	//tf::Matrix3x3(pq).getRPY(R, P, Y);
	//printf("gggggggggggggetRPY errorP   %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",errorPosition[0],errorPosition[1],errorPosition[2],errorPosition[3],errorPosition[4],errorPosition[5]);
	//printf("RPY get %f %f %f \n",R,P,Y);
	errorPosition[3] = R;
	errorPosition[4] = P;
	errorPosition[5] = Y;
	if(R != R){
		printf("=====================================================================================================++");
		printf("%lf %lf %lf\n",R,P,Y);
	}
	//printf("gggggggggggg getRPY after errorP   %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",errorPosition[0],errorPosition[1],errorPosition[2],errorPosition[3],errorPosition[4],errorPosition[5]);
	//printf("RPY %lf %lf %lf\n",R,P,Y);

	// END OF CALCUALTION


	// Calcualate position error
	tf::Quaternion Pin(fixPosition.position.x - currentState.pose.pose.position.x
						,fixPosition.position.y - currentState.pose.pose.position.y
						,fixPosition.position.z - currentState.pose.pose.position.z
						,0);
	if(controllerMode == barrelRollMode){
		tf::Quaternion Pin1(0,0,fixPosition.position.z - currentState.pose.pose.position.z,0);
		Pin = Pin1;	
	}
	double Len = Pin.length();
	if(!equal(Len,0)){
	Pin.normalize();
	tf::Quaternion Q(-currentState.pose.pose.orientation.x
					,-currentState.pose.pose.orientation.y
					,-currentState.pose.pose.orientation.z
					,currentState.pose.pose.orientation.w);
	Q = Q * Pin * Q.inverse();
	geometry_msgs::Quaternion errorP;
	tf::quaternionTFToMsg(Q,errorP);
	errorPosition[0] = errorP.x*Len;
	errorPosition[1] = errorP.y*Len;
	errorPosition[2] = errorP.z*Len;
	}
	else{

	errorPosition[0] = 0;
	errorPosition[1] = 0;
	errorPosition[2] = 0;
	}
	//END of calculation

	for(int i = 0;i < 6;i++){
		if(i>=3){
			if(fabs(errorPosition[i])>M_PI){
				if(errorPosition[i]>0)
					errorPosition[i]-=2*M_PI;
				else
					errorPosition[i]+=2*M_PI;
			}
		}
		errorVelocity[i] = cmd_vel[i] - vel[i];
	}
}

geometry_msgs::Twist calculatePID(){
	geometry_msgs::Twist t;
	calculateError();
	//double out[6];
	double max = 0;
	for(int i = 0;i<6;i++){
		//printf("============ pidV ==============\n");
		//printf("pidV before errorVelocity  %lf %lf %lf %lf %lf %lf\n", pidV[0],pidV[1],pidV[2],pidV[3],pidV[4],pidV[5]);

		out[i] = pidV[i].pid(errorVelocity[i]);
		//printf("cmd_vel: %.2f %.2f %.2f %.2f %.2f %.2f\n", cmd_vel[0],cmd_vel[1],cmd_vel[2],cmd_vel[3],cmd_vel[4],cmd_vel[5]);
		//printf("vel: %.2f %.2f %.2f %.2f %.2f %.2f\n", vel[0],vel[1],vel[2],vel[3],vel[4],vel[5]);
		//printf("errorVelocity[%d]: %lf\n",i,errorVelocity[i]);
		//printf("pidV after errorVelocity: %.10f %.10f %.10f %.10f %.10f %.10f\n", pidV[0],pidV[1],pidV[2],pidV[3],pidV[4],pidV[5]);
		//printf("8 Out      %lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",out[0],out[1],out[2],out[3],out[4],out[5]);

		if(isFixed[i]){
			out[i] = pidP[i].pid(errorPosition[i]);
			//printf("-----errorPosition[%d]: %lf\n",i,errorPosition[i]);
			//printf("9 Out      %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",out[0],out[1],out[2],out[3],out[4],out[5]);
		}
		 if(i == 0 || (fabs(out[i]) > max)){
			 max = fabs(out[i]);
			 //printf("10 Out      %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",out[0],out[1],out[2],out[3],out[4],out[5]);
		 }
		 if(fabs(out[i])>1){
			 out[i]/=fabs(out[i]);
			 //printf("11 Out      %.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n",out[0],out[1],out[2],out[3],out[4],out[5]);
		}
		//out[i]/=2.0;
		//validateValue(out[i]);
	}
	//to limit the output in the same ratio
	// if(max > 1){

	// 	for(int i=0 ;i<6;i++){
	// 		out[i]/=max;
	// 	}
	// }
	// dont allow Controller to stabilize both rotation and position
	// if((fabs(errorPosition[3])+fabs(errorPosition[4])+fabs(errorPosition[5])) > 0.1) {
	// 	printf("!!!TOO MUCH rotation error!!! %.2lf\n",abs(errorPosition[3])+abs(errorPosition[4])+abs(errorPosition[5]));
	// 	out[0] = 0;
	// 	out[1] = 0;
	// 	out[2] = 0;
	// }
	t.linear.x = out[0];
	t.linear.y = out[1];
	t.linear.z = out[2];
	t.angular.x = out[3];
	t.angular.y = out[4];
	t.angular.z = out[5];	
	// add offset force (bouyancy)
	tf::Quaternion Pin(0,0,bouyancy,0);
	double Len = Pin.length();
	if(!equal(Len,0)){
	Pin.normalize();
	tf::Quaternion Q(-currentState.pose.pose.orientation.x
					,-currentState.pose.pose.orientation.y
					,-currentState.pose.pose.orientation.z
					,currentState.pose.pose.orientation.w)
;	Q = Q * Pin * Q.inverse();
	geometry_msgs::Quaternion errorP;
	tf::quaternionTFToMsg(Q,errorP);
	t.linear.x+= errorP.x*Len;
	t.linear.y+= errorP.y*Len;
	t.linear.z+= errorP.z*Len;
	}
	// end offset force

	return t;
}

bool outOfBound(double x){
	return x < MIN_OUT || x > MAX_OUT || x!=x;
}
void validateValue(double& x){
	if(x != x){
		printf("%lf ->",x);
		x = 0;
		printf("%lf\n", x);
	}
	else if(x > MAX_OUT){
		printf("%lf ->", x);
		x = MAX_OUT;
		printf("%lf\n", x);
	}
	else if(x < MIN_OUT){
		printf("%lf ->", x);
		x = MIN_OUT;
		printf("%lf\n", x);
	}
}
double Ang(double a){
	if(a>0)
		return -M_PI-(M_PI-a);
	return M_PI+(a+M_PI);
}
bool equal(double a, double b){ // for double error
    return fabs(a - b) < epsilon;
}

bool isClose(double a,double b){ // for sensor error
	return fabs(a - b) < MIN_ERROR;
}

void PIDConstantCallBack(zeabus_controller::PIDConstantConfig &config,uint32_t level){
	ROS_INFO("!!!--K changed--!!!");

	cmdVelK[0][0] = config.KPVx;
	cmdVelK[0][1] = config.KIVx;
	cmdVelK[0][2] = config.KDVx;

	cmdVelK[1][0] = config.KPVy;
	cmdVelK[1][1] = config.KIVy;
	cmdVelK[1][2] = config.KDVy;

	cmdVelK[2][0] = config.KPVz;
	cmdVelK[2][1] = config.KIVz;
	cmdVelK[2][2] = config.KDVz;

	cmdVelK[3][0] = config.KPVroll;
	cmdVelK[3][1] = config.KIVroll;
	cmdVelK[3][2] = config.KDVroll;

	cmdVelK[4][0] = config.KPVpitch;
	cmdVelK[4][1] = config.KIVpitch;
	cmdVelK[4][2] = config.KDVpitch;

	cmdVelK[5][0] = config.KPVyaw;
	cmdVelK[5][1] = config.KIVyaw;
	cmdVelK[5][2] = config.KDVyaw;

	fixPointK[0][0] = config.KPPx;
	fixPointK[0][1] = config.KIPx;
	fixPointK[0][2] = config.KDPx;

	fixPointK[1][0] = config.KPPy;
	fixPointK[1][1] = config.KIPy;
	fixPointK[1][2] = config.KDPy;

	fixPointK[2][0] = config.KPPz;
	fixPointK[2][1] = config.KIPz;
	fixPointK[2][2] = config.KDPz;

	fixPointK[3][0] = config.KPProll;
	fixPointK[3][1] = config.KIProll;
	fixPointK[3][2] = config.KDProll;

	fixPointK[4][0] = config.KPPpitch;
	fixPointK[4][1] = config.KIPpitch;
	fixPointK[4][2] = config.KDPpitch;

	fixPointK[5][0] = config.KPPyaw;
	fixPointK[5][1] = config.KIPyaw;
	fixPointK[5][2] = config.KDPyaw;

	PID_constant_helper::dump_file(cmdVelK,fixPointK);
}
void fixAbsDepthCallBack(const std_msgs::Float64 msg){
	isFixed[2] = true;
	cmd_vel[2] = 0;
	pidP[2].resetPD();
	fixPosition.position.z = msg.data;
}

void fixAbsYawCallBack(const std_msgs::Float64 msg){
	isFixed[5] = true;
	pidP[5].resetPD();

	// only work on Normal Mode
	// current quaternion -> msg (Absolute)
	//fixPosition.orientation = tf::createQuaternionMsgFromYaw(msg.data);
	tf::Quaternion fq;
	fq.setRPY(0,0,msg.data);
	pushSlerp(fq);
}

void pushSlerp(tf::Quaternion fq){
	while(!fixPositionQueue.empty())
		fixPositionQueue.pop();
	tf::Quaternion pq(fixPosition.orientation.x
					,fixPosition.orientation.y
					,fixPosition.orientation.z
					,fixPosition.orientation.w);
	//tf::Quaternion dq = fq * pq.inverse();
	double interval = fabs(pq.angleShortestPath(fq));
	interval = interval / M_PI * 300.0;
	for(double i=1;i<=interval;i++){
		geometry_msgs::Quaternion q;
		quaternionTFToMsg(pq.slerp(fq,i/interval),q);
		fixPositionQueue.push(q);
	}
	geometry_msgs::Quaternion q;
	quaternionTFToMsg(pq.slerp(fq,1),q);
	fixPositionQueue.push(q);
}

void fixRelYawCallBack(const std_msgs::Float64 msg){
	isFixed[5] = true;
	cmd_vel[5] = 0;
	pidP[5].resetPD();

	// To rotate current quaternion -> current quaternion + msg (Relative)
	tf::Quaternion currentOrientation(fixPosition.orientation.x
					,fixPosition.orientation.y
					,fixPosition.orientation.z
					,fixPosition.orientation.w);
	tf::Quaternion rotationQuaternion;
	rotationQuaternion.setRPY(0,0,msg.data);
	pushSlerp(currentOrientation*rotationQuaternion);
	// tf::quaternionTFToMsg(currentOrientation*rotationQuaternion,fixPosition.orientation);
}

void fixRelXCallBack(const std_msgs::Float64 msg){
	//if(!is_at_fix_position_bool(0.03)){
	//	return;
	//}
	std::cout << "fix rel x->" << msg.data << std::endl;
	cmd_vel[0] = 0;
	cmd_vel[1] = 0;
	isFixed[0] = true;
	isFixed[1] = true;
	tf::Quaternion q(fixPosition.orientation.x,fixPosition.orientation.y,fixPosition.orientation.z,fixPosition.orientation.w);
	tfScalar R,P,Y;
	tf::Matrix3x3(q).getRPY(R,P,Y);
	fixPosition.position.x += msg.data * cos(Y);
	fixPosition.position.y += msg.data * sin(Y);
	printf("%lf %lf\n ",msg.data * cos(Y),msg.data * sin(Y));
}

void fixRelYCallBack(const std_msgs::Float64 msg){
	isFixed[0] = true;
	isFixed[1] = true;
	tf::Quaternion q(fixPosition.orientation.x,fixPosition.orientation.y,fixPosition.orientation.z,fixPosition.orientation.w);
	tfScalar R,P,Y;
	tf::Matrix3x3(q).getRPY(R,P,Y);
	fixPosition.position.x += msg.data * cos(Y+M_PI/2);
	fixPosition.position.y += msg.data * sin(Y+M_PI/2);
}
void setK(){
	for(int i=0;i<6;i++){
		pidP[i].setK(fixPointK[i][0],fixPointK[i][1],fixPointK[i][2]);
		pidV[i].setK(cmdVelK[i][0],cmdVelK[i][1],cmdVelK[i][2]);
	}
}
void modeCallback(const std_msgs::Int16 msg){
	if(msg.data == 1){
		controllerMode = 1;
		//TODO set rotation
	}
	else if(msg.data == 2){
		controllerMode = 2;
	}
	else if(msg.data == 3){
		controllerMode = 3;
	}
	else{
		ROS_WARN("Controller got wrong mode code");
	}
}

bool is_at_fix_position_bool(double err){
	return (((position[0]-fixPosition.position.x)*(position[0]-fixPosition.position.x)+
                (position[1]-fixPosition.position.y)*(position[1]-fixPosition.position.y)+
                (position[2]-fixPosition.position.z)*(position[2]-fixPosition.position.z)) < err);
}

bool is_at_fix_orientation_bool(double err){
	tf::Quaternion pq(currentState.pose.pose.orientation.x
			,currentState.pose.pose.orientation.y
			,currentState.pose.pose.orientation.z
			,currentState.pose.pose.orientation.w);
	tf::Quaternion fq(fixPosition.orientation.x
			,fixPosition.orientation.y
                        ,fixPosition.orientation.z
                       	,fixPosition.orientation.w);
        //tf::Quaternion dq = fq * pq.inverse();
        double angle = fabs(pq.angleShortestPath(fq));
	err_angle = angle;
	return angle>err? false : true;
}

std_msgs::Bool is_at_fix_orientation(double err){
	std_msgs::Bool result;
	result.data = is_at_fix_orientation_bool(err);
	return result;
}

std_msgs::Bool is_at_fix_position(double err){
	std_msgs::Bool result;
	result.data = (((position[0]-fixPosition.position.x)*(position[0]-fixPosition.position.x)+
		(position[1]-fixPosition.position.y)*(position[1]-fixPosition.position.y)+
		(position[2]-fixPosition.position.z)*(position[2]-fixPosition.position.z)) < err);
	return result;
}

bool fix_rel_x_srv_callback(zeabus_controller::drive_x::Request &req,zeabus_controller::drive_x::Response &res){
	//std::cout << "fix rel x->" << msg.data << std::endl;
	cmd_vel[0] = 0;
	cmd_vel[1] = 0;
	isFixed[0] = true;
	isFixed[1] = true;
	tf::Quaternion q(fixPosition.orientation.x,fixPosition.orientation.y,fixPosition.orientation.z,fixPosition.orientation.w);
	tfScalar R,P,Y;
	tf::Matrix3x3(q).getRPY(R,P,Y);
	fixPosition.position.x += req.x * cos(Y);
	fixPosition.position.y += req.x * sin(Y);
	fixPosition.position.x += req.y * cos(Y+M_PI/2);
	fixPosition.position.y += req.y * sin(Y+M_PI/2);
	//printf("%lf %lf\n ",msg.data * cos(Y),msg.data * sin(Y));
	res.success = true;
	return true;
}

void switch_callback(const modbus_ascii_ros::Switch msg){
	is_switch_on =  msg.motor_switch;
	is_switch_on = true;
}
