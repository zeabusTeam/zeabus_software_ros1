/*
 * handle.h
 *
 *  Created on: Jun 10, 2015
 *      Author: mahisorn
 */

#ifndef ZEABUS_CONTROLLER_HANDLE_H_
#define ZEABUS_CONTROLLER_HANDLE_H_

#include <hardware_interface/internal/hardware_resource_manager.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Wrench.h>



namespace zeabus_controller
{

using namespace hardware_interface;
using namespace geometry_msgs;

template <class Derived, typename T>
class Handle_
{
public:
  typedef T ValueType;
  typedef Handle_<Derived, T> Base;

  Handle_(const std::string& name) : name_(name), value_(0) { }
  Handle_(const std::string& name, const ValueType *source) : name_(name), value_(source) { }

  virtual ~Handle_() {}
  virtual const std::string& getName() const { return name_; }

  Derived& operator=(const ValueType *source)
  {
    value_ = source;
    return static_cast<Derived &>(*this);
  }

  const ValueType *get() const { return value_; }
  const ValueType &operator*() const { return *value_; }

protected:
  std::string name_;
  const ValueType* value_;
};


class CommandHandle
{
public:
  CommandHandle()  { }
  CommandHandle(const std::string& name) : name_(name)  { }
  virtual ~CommandHandle() {}

  virtual const std::string& getName() const { return name_; }

  template <typename T> T* ownData(T* data) { data_.reset(data); return data; }

protected:
  std::string name_;
  boost::shared_ptr<void> data_;
};

template <class Derived, typename T, class Parent=CommandHandle>
class CommandHandle_ : public Parent
{
public:
  typedef T ValueType;
  typedef CommandHandle_<Derived, T, Parent> Base;

  CommandHandle_(const std::string& name, ValueType* command)
    : Parent(name), command_(command)
  {
      if (!command_)
      {
        throw HardwareInterfaceException("Cannot create handle '" + this->getName() + "'. Command data pointer is null.");
      }
  }

  void setCommand(const ValueType& command) {assert(command_); *command_ = command; }
  void getCommand(ValueType& command) {assert(command_); command = *command_; }
  const ValueType *get() const { return command_; }
  const ValueType &operator*() const { return *command_; }

protected:
  ValueType* command_;
};



/**
 * Pose
 */
class PoseHandle : public Handle_<PoseHandle, Pose>
{
public:
  using Base::operator=;

  PoseHandle() : Base("pose") { }
  PoseHandle(const Pose* pose) : Base("pose", pose) { }

  virtual ~PoseHandle() { }

  const ValueType& pose() const { return *get(); }

  void getEulerRPY(double &roll, double &pitch, double &yaw) const
  {
    const Quaternion::_w_type& w = pose().orientation.w;
    const Quaternion::_x_type& x = pose().orientation.x;
    const Quaternion::_y_type& y = pose().orientation.y;
    const Quaternion::_z_type& z = pose().orientation.z;
    double ww = w*w;
    double xx = x*x;
    double yy = y*y;
    double zz = z*z;

    double wx = w*x;
    double wy = w*y;
    double wz = w*z;

    double xy = x*y;
    double xz = x*z;

    double yz = y*z;

    roll  =  atan2(2.*yz + 2.*wx, zz - yy - xx + ww);
    pitch = -asin(2.*xz - 2.*wy);
    yaw   =  atan2(2.*xy + 2.*wz, xx + ww - zz - yy);

//    roll  =  atan2(2.*y*z + 2.*w*x, z*z - y*y - x*x + w*w);
//    pitch = -asin(2.*x*z - 2.*w*y);
//    yaw   =  atan2(2.*x*y + 2.*w*z, x*x + w*w - z*z - y*y);
  }

  double getYaw() const
  {
    const Quaternion::_w_type& w = pose().orientation.w;
    const Quaternion::_x_type& x = pose().orientation.x;
    const Quaternion::_y_type& y = pose().orientation.y;
    const Quaternion::_z_type& z = pose().orientation.z;
    return atan2(2.*x*y + 2.*w*z, x*x + w*w - z*z - y*y);
  }

  Vector3 toBody(const Vector3& nav) const
  {
    const Quaternion::_w_type& w = pose().orientation.w;
    const Quaternion::_x_type& x = pose().orientation.x;
    const Quaternion::_y_type& y = pose().orientation.y;
    const Quaternion::_z_type& z = pose().orientation.z;
    Vector3 body;
    double ww = w*w;
    double xx = x*x;
    double yy = y*y;
    double zz = z*z;

    double wx = w*x;
    double wy = w*y;
    double wz = w*z;

    double xy = x*y;
    double xz = x*z;

    double yz = y*z;

    body.x = (ww+xx-yy-zz) * nav.x + (2.*xy + 2.*wz) * nav.y + (2.*xz - 2.*wy) * nav.z;
    body.y = (2.*xy - 2.*wz) * nav.x + (ww-xx+yy-zz) * nav.y + (2.*yz + 2.*wx) * nav.z;
    body.z = (2.*xz + 2.*wy) * nav.x + (2.*yz - 2.*wx) * nav.y + (ww-xx-yy+zz) * nav.z;

//    body.x = (w*w+x*x-y*y-z*z) * nav.x + (2.*x*y + 2.*w*z) * nav.y + (2.*x*z - 2.*w*y) * nav.z;
//    body.y = (2.*x*y - 2.*w*z) * nav.x + (w*w-x*x+y*y-z*z) * nav.y + (2.*y*z + 2.*w*x) * nav.z;
//    body.z = (2.*x*z + 2.*w*y) * nav.x + (2.*y*z - 2.*w*x) * nav.y + (w*w-x*x-y*y+z*z) * nav.z;
    return body;
  }

  Vector3 fromBody(const Vector3& body) const
  {
    const Quaternion::_w_type& w = pose().orientation.w;
    const Quaternion::_x_type& x = pose().orientation.x;
    const Quaternion::_y_type& y = pose().orientation.y;
    const Quaternion::_z_type& z = pose().orientation.z;
    Vector3 nav;
    double ww = w*w;
    double xx = x*x;
    double yy = y*y;
    double zz = z*z;

    double wx = w*x;
    double wy = w*y;
    double wz = w*z;

    double xy = x*y;
    double xz = x*z;

    double yz = y*z;

    nav.x = (ww+xx-yy-zz) * body.x + (2.*xy - 2.*wz) * body.y + (2.*xz + 2.*wy) * body.z;
    nav.y = (2.*xy + 2.*wz) * body.x + (ww-xx+yy-zz) * body.y + (2.*yz - 2.*wx) * body.z;
    nav.z = (2.*xz - 2.*wy) * body.x + (2.*yz + 2.*wx) * body.y + (ww-xx-yy+zz) * body.z;

//    nav.x = (w*w+x*x-y*y-z*z) * body.x + (2.*x*y - 2.*w*z) * body.y + (2.*x*z + 2.*w*y) * body.z;
//    nav.y = (2.*x*y + 2.*w*z) * body.x + (w*w-x*x+y*y-z*z) * body.y + (2.*y*z - 2.*w*x) * body.z;
//    nav.z = (2.*x*z - 2.*w*y) * body.x + (2.*y*z + 2.*w*x) * body.y + (w*w-x*x-y*y+z*z) * body.z;
    return nav;
  }
};
class PoseInterface : public HardwareResourceManager<PoseHandle> {};
typedef boost::shared_ptr<PoseHandle> PoseHandlePtr;

/*
class PoseCommandHandle : public CommandHandle<PoseCommandHandle, Pose, PoseHandle>
{
public:
  PoseCommandHandle(const PoseHandle& handle, Pose* command)
    : Base(handle, command)
  { }
  virtual ~PoseCommandHandle() { }
};
class PoseCommandInterface : public HardwareResourceManager<PoseCommandHandle> {};
*/

/**
 * Twist
 */
class TwistHandle : public Handle_<TwistHandle, Twist>
{
public:
  using Base::operator=;

  TwistHandle() : Base("twist") { }
  TwistHandle(const Twist* twist) : Base("twist", twist) { }

  virtual ~TwistHandle() { }

  const ValueType& twist() const { return *get(); }
};
class TwistInterface : public HardwareResourceManager<TwistHandle> {};
typedef boost::shared_ptr<TwistHandle> TwistHandlePtr;

/*
class TwistCommandHandle : public CommandHandle<TwistCommandHandle, Twist, TwistHandle>
{
public:
  TwistCommandHandle(const TwistHandle& handle, Twist* command)
    : Base(handle, command)
  { }
  virtual ~TwistCommandHandle() { }
};
class TwistCommandInterface : public HardwareResourceManager<TwistCommandHandle> {};

/**
 * Acceleration
 */
class AccelerationHandle : public Handle_<AccelerationHandle, Vector3>
{
public:
  using Base::operator=;

  AccelerationHandle() : Base("acceleration") { }
  AccelerationHandle(const Vector3* acceleration) : Base("acceleration", acceleration) { }

  virtual ~AccelerationHandle() { }

  const ValueType& acceleration() const { return *get(); }
};
class AccelerationInterface : public HardwareResourceManager<AccelerationHandle> {};
typedef boost::shared_ptr<AccelerationHandle> AccelerationHandlePtr;

/*
class AccelerationCommandHandle : public CommandHandle<AccelerationCommandHandle, Vector3, AccelerationHandle>
{
public:
  AccelerationCommandHandle(const AccelerationHandle& handle, Vector3* command)
    : Base(handle, command)
  { }
  virtual ~AccelerationCommandHandle() { }
};
class AccelerationCommandInterface : public HardwareResourceManager<AccelerationCommandHandle> {};
*/

/**
 * Wrench
 */
class WrenchHandle : public Handle_<WrenchHandle, Wrench>
{
public:
  using Base::operator=;

  WrenchHandle() : Base("wrench") { }
  WrenchHandle(const Wrench* wrench) : Base("wrench", wrench) { }

  virtual ~WrenchHandle() { }

  const ValueType& wrench() const { return *get(); }
};
class WrenchInterface : public HardwareResourceManager<WrenchHandle> {};
typedef boost::shared_ptr<WrenchHandle> WrenchHandlePtr;


class WrenchCommandHandle : public CommandHandle_<WrenchCommandHandle, Wrench, CommandHandle>
{
public:
  WrenchCommandHandle(Wrench* command)
    : Base("wrench_command", command)
  { }
  virtual ~WrenchCommandHandle() { }

  ValueType& wrench() { return *command_; }
};
//class WrenchCommandInterface : public HardwareResourceManager<WrenchCommandHandle> {};
typedef boost::shared_ptr<WrenchCommandHandle> WrenchCommandHandlePtr;


}


#endif /* ZEABUS_CONTROLLER_HANDLE_H_ */
