/*
 * pololu_packet.h
 *
 *  Created on: Nov 28, 2014
 *      Author: mahisorn
 */

#ifndef SOURCE_DIRECTORY__HG_ROS_POLOLU_INCLUDE_HG_ROS_POLOLU_POLOLU_PACKET_H_
#define SOURCE_DIRECTORY__HG_ROS_POLOLU_INCLUDE_HG_ROS_POLOLU_POLOLU_PACKET_H_

#include <utility>

namespace hg_ros_pololu
{

//Serial Servo Command
static const uint8_t COMMAND_SET_TARGET = 0x84;
static const uint8_t COMMAND_SET_MULTIPLE_TARGET = 0x9F;
static const uint8_t COMMAND_SET_SPEED = 0x87;
static const uint8_t COMMAND_SET_ACCELERATION = 0x89;
static const uint8_t COMMAND_SET_PWM = 0x8A;
static const uint8_t COMMAND_GET_POSITION = 0x90;
static const uint8_t COMMAND_GET_MOVING_STATE = 0x93;
static const uint8_t COMMAND_GET_ERROR = 0xA1;
static const uint8_t COMMAND_GO_HOME = 0xA2;

class PololuPacket : public hg_ros_serial::Packet
{
public:
  static const uint8_t SYNC = 0xAA;

  uint8_t sync_;
  uint8_t device_id_;
  uint8_t command_;
  std::vector<uint8_t> payload_;

  PololuPacket()
    : sync_(SYNC),
      device_id_(12), //Default setting for Maestro 12
      command_(0)
  {

  }

  void beginPacket(uint8_t device_id, uint8_t command)
  {
    device_id_ = device_id;
    command_ = command & 0x7F;
  }

  void append(uint8_t data)
  {
    payload_.push_back(data);
  }

  std::string toString() const
  {
    std::stringstream ss;
    ss << std::hex;
    ss << "\n======= Packet =======\n";
    ss << "Sync : " << static_cast<int>(sync_) << "\n";
    ss << "Device ID: " << static_cast<int>(device_id_) << "\n";
    ss << "Command: " << static_cast<int>(command_) << "\n";
    ss << "Payload: ";
    for (size_t s = 0; s < payload_.size(); s++)
    {
      ss << static_cast<int>(payload_[s]) << " ";
    }
    ss << "\n======================";
    return ss.str();
  }

  void getDataStream(hg_ros_serial::DataStream& buffer) const
  {
    buffer.clear();
    buffer.reserve(3 + payload_.size());
    buffer.push_back(sync_);
    buffer.push_back(device_id_);
    buffer.push_back(command_);
    for (size_t i = 0; i < payload_.size(); i++)
    {
      buffer.push_back(payload_[i]);
    }
  }
};


}



#endif /* SOURCE_DIRECTORY__HG_ROS_POLOLU_INCLUDE_HG_ROS_POLOLU_POLOLU_PACKET_H_ */
