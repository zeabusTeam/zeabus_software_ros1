/*
 * test.cpp
 *
 *  Created on: Nov 17, 2014
 *      Author: mahisorn
 */
#include <hg_ros_serial/serial.h>

using namespace hg_ros_serial;

int main()
{
  Serial serial;
  if(serial.openPort("/dev/ttyUSB0", 921600))
  {
    std::cout << "Yeah!\n";
    serial.closePort();
  }


  return 0;
}



