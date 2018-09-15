/*
 * seria#l.h
 *
 *  Created on: Nov 17, 2014
 *      Author: mahisorn
 */

#ifndef SOURCE_DIRECTORY__HG_ROS_SERIAL_INCLUDE_HG_ROS_SERIAL_SERIAL_H_
#define SOURCE_DIRECTORY__HG_ROS_SERIAL_INCLUDE_HG_ROS_SERIAL_SERIAL_H_

#include <boost/asio.hpp>
#include <boost/chrono.hpp>
#include <boost/bind.hpp>

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ //  will fail outside of gcc/clang
#define HOST_LITTLE_ENDIAN
#else
#define HOST_BIG_ENDIAN
#endif

#define u8(x) static_cast<uint8_t>((x))
#define u16(x) static_cast<uint16_t>((x))
#define u32(x) static_cast<uint32_t>((x))

namespace hg_ros_serial
{

typedef std::vector<uint8_t> DataStream;

typedef union { struct { uint8_t d0; uint8_t d1; }; uint16_t d; } Data16;

class TimeoutError : public std::runtime_error
{
public:
  TimeoutError(bool is_write, unsigned int time)
    : std::runtime_error(generateString(is_write, time))
  {

  }
private:
  std::string generateString(bool is_write, unsigned int time)
  {
    std::stringstream ss;
    ss << "Timed out while " << ((is_write) ? "writing" : "reading") << ". ";
    ss << "Time out limit is " << time << "ms.";
    return ss.str();
  }
};

class IOError : public std::runtime_error
{
public:
  IOError(const std::string& description)
    : std::runtime_error(description)
  {

  }
};

class Packet
{
public:
  Packet() { }
  virtual ~Packet() { }

  virtual std::string toString() const = 0;
  virtual void getDataStream(hg_ros_serial::DataStream& buffer) const = 0;
};

class PacketError : public std::runtime_error
{
public:
  PacketError(const Packet &p, uint8_t code)
    : std::runtime_error(generateString(p, code))
  {

  }

private:
  std::string generateString(const Packet &p, uint8_t code)
  {
    std::stringstream ss;
    ss << p.toString();
    return ss.str();
  }
};

class Serial : private boost::noncopyable
{
public:


  enum
  {
    IO_IN_PROCESS,
    IO_DONE,
    IO_ERROR,
    IO_TIMEOUT,
  };

  Serial();

  virtual ~Serial();

  bool openPort(std::string port,
                unsigned int baud_rate,
                boost::asio::serial_port_base::parity opt_parity = boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none),
                boost::asio::serial_port_base::character_size opt_csize = boost::asio::serial_port_base::character_size(8),
                boost::asio::serial_port_base::flow_control opt_flow = boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none),
                boost::asio::serial_port_base::stop_bits opt_stop = boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));

  void closePort();

  bool isOpen();

  void changeBaudRate(unsigned int baud_rate);

  //TODO change to async_write
  void writePacket(const hg_ros_serial::Packet& packet, int timeout);

  void writeString(const std::string& message);

  //TODO make it async and timeout supported
  std::string readLine();

  void writeData(const DataStream& data, int timeout);

  virtual void readHandle(const boost::system::error_code& ec, const size_t bytes_transferred);

  virtual void timerHandle(const boost::system::error_code& ec);

  //asynchronous read fixed amount of data
  void asyncReadBlockOfData(DataStream& data, size_t size, int timeout);

protected:
  boost::asio::io_service io_;
  boost::asio::serial_port port_;
  boost::asio::deadline_timer timer_;
  int byte_transferred_;
  int io_state_;
};

}



#endif /* SOURCE_DIRECTORY__HG_ROS_SERIAL_INCLUDE_HG_ROS_SERIAL_SERIAL_H_ */
