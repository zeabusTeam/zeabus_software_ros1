/*
 * 3dm_gx4.h
 *
 *  Created on: Nov 17, 2014
 *      Author: mahisorn
 */

#ifndef SOURCE_DIRECTORY__HG_3DM_GX4_INCLUDE_HG_3DM_GX4_3DM_GX4_H_
#define SOURCE_DIRECTORY__HG_3DM_GX4_INCLUDE_HG_3DM_GX4_3DM_GX4_H_

#include <boost/function.hpp>
#include <hg_ros_serial/serial.h>
#include <bitset>
#include "../hg_ros_3dm_gx4/mip.h"

namespace hg_3dm_gx4
{

static const unsigned int COMMAND_RW_TIMEOUT = 2000;

class Hg3dmGx4 : public hg_ros_serial::Serial
{
public:
  Hg3dmGx4()
  {

  }

  virtual ~Hg3dmGx4()
  {

  }

//Base commands
  void ping();
  void idle();
  void resume();
  void updateGpsTime();
  void getDeviceInformation();
  void getDeviceDescriptorSets();
  bool runDeviceBuildInTest();
  void reset();

  void selectBaudRate(unsigned int baud);
  void setCFSettings();

  void setIMUDataRate(unsigned int decimation, const std::bitset<IMUData::NUM_IMU_DATA>& sources);
  void setGPSDataRate(unsigned int decimation, const std::bitset<GPSData::NUM_GPS_DATA>& sources);
  void setEFDataRate(unsigned int decimation, const std::bitset<EFData::NUM_EF_DATA>& sources);

  void selectDataStream(const std::bitset<3>& streams);

  void initializeFilterWithMagneto();
  void setInitialAttitude(float roll, float pitch, float yaw);
  void setInitialHeading(float heading);

  //const MIP& receivedPacket() { return received_packet_; }

  void receiveDataStream();

  void setIMUDataCallback(const boost::function<void(const IMUData &)> & cb)
  {
    imu_data_callback_ = cb;
  }

  void setGPSDataCallback(const boost::function<void(const GPSData &)> & cb)
  {
    gps_data_callback_ = cb;
  }

  void setEFDataCallback(const boost::function<void(const EFData &)> & cb)
  {
    ef_data_callback_ = cb;
  }


protected:
  void sendPacket(const MIP& p, int timeout);
  void sendAndReceivePacket(const MIP& p);



  void processPacket();
  void processIMUPacket();
  void processGPSPacket();
  void processEFPacket();
  void processRespondPacket();




  MIP received_packet_;


  boost::function<void(const IMUData &)> imu_data_callback_;
  boost::function<void(const GPSData &)> gps_data_callback_;
  boost::function<void(const EFData &)> ef_data_callback_;

};


}


#endif /* SOURCE_DIRECTORY__HG_3DM_GX4_INCLUDE_HG_3DM_GX4_3DM_GX4_H_ */
