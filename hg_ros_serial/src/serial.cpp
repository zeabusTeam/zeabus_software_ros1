/*
 * serial.cpp
 *
 *  Created on: Nov 17, 2014
 *      Author: mahisorn
 */

#include <hg_ros_serial/serial.h>

using namespace hg_ros_serial;

Serial::Serial() :
    io_(), port_(io_), timer_(io_), byte_transferred_(0), io_state_(IO_DONE)
{

}

Serial::~Serial()
{

}

bool Serial::openPort(std::string port, unsigned int baud_rate,
                      boost::asio::serial_port_base::parity opt_parity,
                      boost::asio::serial_port_base::character_size opt_csize,
                      boost::asio::serial_port_base::flow_control opt_flow,
                      boost::asio::serial_port_base::stop_bits opt_stop)
{
  if (isOpen())
    closePort();

  try
  {
    port_.open(port);
    port_.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    port_.set_option(opt_parity);
    port_.set_option(opt_csize);
    port_.set_option(opt_flow);
    port_.set_option(opt_stop);

  }
  catch (boost::system::system_error& e)
  {
    std::cout << "Error: " << e.what() << std::endl;
    return false;
  }
  return true;
}

void Serial::closePort()
{
  port_.close();
}

bool Serial::isOpen()
{
  return port_.is_open();
}

void Serial::changeBaudRate(unsigned int baud_rate)
{
  try
  {
    port_.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
  }
  catch (boost::system::system_error& e)
  {
    std::cout << "Error: " << e.what() << std::endl;
  }
}

//TODO change to async_write
void Serial::writePacket(const hg_ros_serial::Packet& packet, int timeout)
{
  DataStream buffer;
  packet.getDataStream(buffer);
  writeData(buffer, timeout);
}

void Serial::writeString(const std::string& message)
{
  boost::system::error_code ignored_error;
  boost::asio::write(port_, boost::asio::buffer(message), boost::asio::transfer_all(), ignored_error);
}

//TODO make it async and timeout supported
std::string Serial::readLine()
{
  std::string line;
  char c;
  for (;;)
  {
    boost::asio::read(port_, boost::asio::buffer(&c, 1));
    switch (c)
    {
      case '\r':
        break;
      case '\n':
        return line;
      default:
        line += c;
    }
  }
  return line;
}

void Serial::writeData(const DataStream& data, int timeout)
{
  using namespace boost::chrono;

  high_resolution_clock::time_point tstart = high_resolution_clock::now();
  high_resolution_clock::time_point tstop = tstart + milliseconds(timeout);

  size_t written = 0;
  while (written < data.size())
  {
    const ssize_t amt = boost::asio::write(port_, boost::asio::buffer(&data[written], data.size()));
    if (amt > 0)
    {
      written += amt;
    }
    else if (amt < 0)
    {
      if (errno == EAGAIN || errno == EINTR)
      {
        //  blocked or interrupted - try again until timeout
      }
      else
      {
        throw IOError("I/O error while writing");
      }
    }

    if (tstop < high_resolution_clock::now())
    {
      throw TimeoutError(true, timeout);
    }
  }
}

void Serial::readHandle(const boost::system::error_code& ec, const size_t bytes_transferred)
{
  if (!ec)
  {
    io_state_ = IO_DONE;
    byte_transferred_ = bytes_transferred;
    return;
  }

  if (ec.value() == 125)
    return;

  io_state_ = IO_ERROR;
}

void Serial::timerHandle(const boost::system::error_code& ec)
{
  if (!ec && (io_state_ == IO_IN_PROCESS))
  {
    io_state_ = IO_TIMEOUT;
  }
}

//asynchronous read fixed amount of data
void Serial::asyncReadBlockOfData(DataStream& data, size_t size, int timeout)
{
  if (data.size() < size)
  {
    data.resize(size);
  }
  boost::asio::async_read(
      port_,
      boost::asio::buffer(data, size),
      boost::bind(&Serial::readHandle, this, boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));

  timer_.expires_from_now(boost::posix_time::milliseconds(timeout));
  timer_.async_wait(boost::bind(&Serial::timerHandle, this, boost::asio::placeholders::error));

  io_state_ = IO_IN_PROCESS;
  byte_transferred_ = 0;

  while (true)
  {
    io_.run_one();
    switch (io_state_)
    {
      case IO_IN_PROCESS:
        continue;
      case IO_DONE:
        timer_.cancel();
        return;
      case IO_TIMEOUT:
        port_.cancel();
        throw TimeoutError(false, timeout);
      case IO_ERROR:
      default:
        timer_.cancel();
        port_.cancel();
        throw IOError("I/O error while reading");
    }
  }
}

