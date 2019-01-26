/*
 * maestro.cpp
 *
 *  Created on: Feb 11, 2015
 *      Author: mahisorn
 */

#include <hg_ros_pololu/maestro.h>

using namespace hg_ros_pololu;

Maestro::Maestro() :
    buffer_(16)
{

}

void Maestro::setTarget(uint8_t device_id, uint8_t channel, uint16_t target)
{
  PololuPacket p;
  p.beginPacket(device_id, COMMAND_SET_TARGET & 0x7F);
  target = target << 2;
  p.append(channel);
  p.append(target & 0x7f);
  p.append((target >> 7) & 0x7f);

  sendPacket(p, COMMAND_RW_TIMEOUT);
}

void Maestro::setMultipleTarget(uint8_t device_id, uint8_t start_id, const std::vector<uint16_t>& targets)
{
  PololuPacket p;
  p.beginPacket(device_id, COMMAND_SET_MULTIPLE_TARGET);
  p.append(targets.size());
  uint16_t target;

  p.append(start_id);

  for (int i = 0; i < targets.size(); i++)
  {
    target = targets[i];

    if (target > 2000)
    {
      target = 2000;
    }

    target = targets[i] << 2;

    p.append(target & 0x7f);
    p.append((target >> 7) & 0x7f);
  }

  //std::cout << p.toString() << std::endl;

  sendPacket(p, COMMAND_RW_TIMEOUT);
}

uint16_t Maestro::getPosition(uint8_t device_id, uint8_t channel)
{
  PololuPacket p;
  p.beginPacket(device_id, COMMAND_GET_POSITION);
  p.append(channel);

  sendPacket(p, COMMAND_RW_TIMEOUT);

  try
  {
    asyncReadBlockOfData(buffer_, 2, COMMAND_RW_TIMEOUT);
    return buffer_[0] + (buffer_[1] << 8);
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << std::endl;
    return -1;
  }
}

uint8_t Maestro::getMovingState(uint8_t device_id)
{
  PololuPacket p;
  p.beginPacket(device_id, COMMAND_GET_MOVING_STATE);

  sendPacket(p, COMMAND_RW_TIMEOUT);

  try
  {
    asyncReadBlockOfData(buffer_, 1, COMMAND_RW_TIMEOUT);
    return buffer_[0];
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << std::endl;
    return -1;
  }
}

uint16_t Maestro::getError(uint8_t device_id)
{
  PololuPacket p;
  p.beginPacket(device_id, COMMAND_GET_ERROR);

  sendPacket(p, COMMAND_RW_TIMEOUT);

  try
  {
    asyncReadBlockOfData(buffer_, 2, COMMAND_RW_TIMEOUT);
    return buffer_[0] + (buffer_[1] << 8);
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << std::endl;
    return -1;
  }
}

void Maestro::goHome(uint8_t device_id)
{
  PololuPacket p;
  p.beginPacket(device_id, COMMAND_GO_HOME);

  sendPacket(p, COMMAND_RW_TIMEOUT);
}

void Maestro::sendPacket(const PololuPacket& p, int timeout)
{
  try
  {
    writePacket(p, timeout);
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
  }
}

