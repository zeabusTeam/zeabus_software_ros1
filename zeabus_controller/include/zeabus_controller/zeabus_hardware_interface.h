/*
 * zeabus_hardware_interface.h
 *
 *  Created on: Jun 17, 2015
 *      Author: mahisorn
 */

#ifndef ZEABUS_HARDWARE_INTERFACE_H_
#define ZEABUS_HARDWARE_INTERFACE_H_

#include <ros/ros.h>
#include <hardware_interface/robot_hw.h>
#include <zeabus_controller/zeabus_interface.h>

namespace zeabus_controller
{

class ZeabusRobot : public hardware_interface::RobotHW, public zeabus_controller::ZeabusInteface
{
public:
  ZeabusRobot(const ros::NodeHandle& nh);
  virtual ~ZeabusRobot();

  virtual PoseHandlePtr getPose()
  {
    return PoseHandlePtr(new PoseHandle(&pose_));
  }
  virtual TwistHandlePtr getTwist()
  {
    return TwistHandlePtr(new TwistHandle(&twist_));
  }
  virtual AccelerationHandlePtr getAcceleration()
  {
    return AccelerationHandlePtr(new AccelerationHandle(&acceleration_));
  }

  virtual bool getMassAndInertia(double &mass, double inertia[3]);

protected:
  ros::NodeHandle nh_;

  geometry_msgs::Pose pose_;
  geometry_msgs::Twist twist_;
  geometry_msgs::Vector3 acceleration_;

  double mass_;
  geometry_msgs::Vector3 inertia_;
};

}

#endif /* ZEABUS_HARDWARE_INTERFACE_H_ */
