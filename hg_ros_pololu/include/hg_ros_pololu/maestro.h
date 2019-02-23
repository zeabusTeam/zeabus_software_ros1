/*
 * maestro.h
 *
 *  Created on: Nov 28, 2014
 *      Author: mahisorn
 */

#ifndef SOURCE_DIRECTORY__HG_ROS_POLOLU_INCLUDE_HG_ROS_POLOLU_MAESTRO_H_
#define SOURCE_DIRECTORY__HG_ROS_POLOLU_INCLUDE_HG_ROS_POLOLU_MAESTRO_H_

#include <hg_ros_serial/serial.h>
#include <hg_ros_pololu/pololu_packet.h>

namespace hg_ros_pololu
{

static const unsigned int COMMAND_RW_TIMEOUT = 300;

class Maestro : public hg_ros_serial::Serial
{
public:
  Maestro();
  void setTarget(uint8_t device_id, uint8_t channel, uint16_t target);
  void setMultipleTarget(uint8_t device_id, uint8_t start_id, const std::vector<uint16_t>& targets);
  uint16_t getPosition(uint8_t device_id, uint8_t channel);
  uint8_t getMovingState(uint8_t device_id);
  uint16_t getError(uint8_t device_id);
  void goHome(uint8_t device_id);
  void sendPacket(const PololuPacket& p, int timeout);

private:
  PololuPacket received_packet_;
  hg_ros_serial::DataStream buffer_;
};





}



#endif /* SOURCE_DIRECTORY__HG_ROS_POLOLU_INCLUDE_HG_ROS_POLOLU_MAESTRO_H_ */
