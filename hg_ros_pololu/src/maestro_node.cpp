/*
 * maestro_node.cpp
 *
 *  Created on: Nov 28, 2014
 *      Author: mahisorn
 */

#include <ros/ros.h>
#include <hg_ros_pololu/maestro.h>
#include "hg_ros_pololu/Pwm.h"
#include <ros/subscriber.h>

#define HOME_PWM 1500

hg_ros_pololu::Maestro g_maestro;
int g_device_id;
int g_num_channel;
std::vector<uint16_t> g_targets;
ros::Time g_last_pwm_update;
bool g_warn_num_channel;

void pwmCallback(const hg_ros_pololu::Pwm& msg)
{
  //ROS_INFO_STREAM(msg);
  if((msg.pwm.size() != g_num_channel) && g_warn_num_channel)
  {
    ROS_WARN_THROTTLE(2.0, "Number of channel in message (%d) != number of specified channel (%d)", (int)msg.pwm.size(), g_num_channel);
  }

  for(int i=0; (i < msg.pwm.size()) && (i < g_num_channel); i++)
  {
    g_targets[i] = msg.pwm[i];
  }

  if (g_maestro.isOpen())
  {
    g_maestro.setMultipleTarget(g_device_id, 0, g_targets);
    g_last_pwm_update = ros::Time::now();
  }
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "maestro");
  ros::NodeHandle nh("~");

  std::string device;
  nh.param < std::string > ("device", device, "/dev/pololu/maestro/00096161");
  nh.param < int > ("device_id", g_device_id, 12);
  nh.param < int > ("num_channel", g_num_channel, 12);
  nh.param < bool > ("warn_num_channel", g_warn_num_channel, true);

  //TODO : Add support for default home position of each channel
  std::vector<uint16_t> home_position(g_num_channel, HOME_PWM);
  g_targets = std::vector<uint16_t>(g_num_channel, HOME_PWM);

  //Command timeout -> return to home position
  bool enable_timeout;
  nh.param < bool > ("enable_timeout", enable_timeout, true);

  double timeout_period;
  nh.param < double > ("timeout_period", timeout_period, 0.5);

  bool warm_timeout;
  nh.param < bool > ("warm_timeout", warm_timeout, false);

  int baudrate;
  nh.param<int>("baudrate", baudrate, 115200);

  if (!g_maestro.openPort(device, baudrate))
  {
    ROS_FATAL("Cannot open %s", device.c_str());
    return -1;
  }

  ROS_INFO("Open Maestro on %s", device.c_str());
  ROS_INFO("    baudrate: %d", baudrate);

  ros::Subscriber pwm_sub;
  pwm_sub = nh.subscribe("/pwm", 10, pwmCallback);

  ros::Rate rate(10);

  while(ros::ok())
  {
    if(enable_timeout && ((ros::Time::now() - g_last_pwm_update).toSec() > timeout_period))
    {
      if(warm_timeout)
        ROS_WARN_THROTTLE(1.0, "Return home because not receiving PWM command for longer than %6.2f s", timeout_period);
      else
        ROS_WARN_ONCE("Return home because not receiving PWM command for longer than %6.2f s", timeout_period);
      g_maestro.setMultipleTarget(g_device_id, 0, home_position);
      g_last_pwm_update = ros::Time::now();
    }

    ros::spinOnce();
    rate.sleep();
  }

  return 0;
}

