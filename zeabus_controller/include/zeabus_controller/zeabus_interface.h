/*
 * zeabus_interface.h
 *
 *  Created on: Jun 9, 2015
 *      Author: mahisorn
 */

#ifndef ZEABUS_INTERFACE_H_
#define ZEABUS_INTERFACE_H_

#include <ros/ros.h>
#include <hardware_interface/robot_hw.h>
#include <zeabus_controller/handle.h>

#include <map>

namespace zeabus_controller
{

using namespace hardware_interface;

class ZeabusInteface : public HardwareInterface
{
public:
  ZeabusInteface() { }
  virtual ~ZeabusInteface() { }

  virtual PoseHandlePtr getPose() { return PoseHandlePtr(); }
  virtual TwistHandlePtr getTwist() { return TwistHandlePtr(); }
  virtual AccelerationHandlePtr getAcceleration() { return AccelerationHandlePtr(); }

  virtual void setWrenchCommand(const WrenchCommandHandlePtr& ptr) { wrench_output_ = ptr; }
  virtual void getWrenchCommand(WrenchCommandHandlePtr& ptr) { ptr = wrench_output_; }

  virtual bool getMassAndInertia(double &mass, double inertia[3]) { return false; }



protected:
  //std::map<std::string, CommandHandlePtr> inputs_;
  //std::map<std::string, CommandHandlePtr> outputs_;
  WrenchCommandHandlePtr wrench_output_;

};

}
#endif /* ZEABUS_INTERFACE_H_ */
