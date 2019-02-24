#include <ros/ros.h>
#include <ros/time.h>
#include <std_msgs/String.h>
#include <std_msgs/Header.h>
#include <sensor_msgs/CompressedImage.h>
#include <sstream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <rosbag/bag.h>
#include "std_msgs/UInt8MultiArray.h"
#include <rosbag/view.h>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

using namespace cv;

bool bag2mp4(std::string bagfile,std::string topic,std::string sfps,std::string output){
  VideoWriter vw;
  int fps = std::stoi(sfps);
  int frameStepped = 0;
  rosbag::Bag bag;
  ros::Time start;
  std::vector<std::string> topics;

  bag.open(bagfile,rosbag::bagmode::Read);

  topics.push_back(topic);
  rosbag::View view(bag,rosbag::TopicQuery(topics));

  foreach(rosbag::MessageInstance const m,view){
    sensor_msgs::CompressedImage::ConstPtr c = m.instantiate<sensor_msgs::CompressedImage>();
    std_msgs::Header h = (c->header);
    if(c != NULL){
      Mat image = imdecode(c->data,-1);
      // imshow("Eiei",image);
      // waitKey(1);
      if(!frameStepped){
        start = h.stamp;
        vw = VideoWriter(output,CV_FOURCC('F','M','P','4'),fps,image.size(),true);
      }
      frameStepped++;
      vw.write(image);
      if(frameStepped%fps==0 && (frameStepped / fps) % 30 == 0){
        std::cout << frameStepped / fps << " secs processed." << std::endl;
      }
    }
    else{
      std::cout << m.getDataType() << std::endl;
    }
  }
  // ReleaseVideoWriter(vw);
  std::cout<< "Finished " << frameStepped / fps << " secs of videos" << std::endl;
}
int main(int argc, char **argv){
  if(argc < 4){
    printf("Usage: bag2mp4 filename.bag topicname fps output.mp4\n");
    return 0;
  }
  bag2mp4(argv[1],argv[2],argv[3],argv[4]);
  return 0;
}
