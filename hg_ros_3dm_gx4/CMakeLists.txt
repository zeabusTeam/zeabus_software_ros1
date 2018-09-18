cmake_minimum_required(VERSION 2.8.3)

project(hg_ros_3dm_gx4)

find_package(catkin REQUIRED COMPONENTS roscpp hg_ros_serial sensor_msgs geometry_msgs)


catkin_package()

include_directories(
    include    
    ${catkin_INCLUDE_DIRS}
)

add_executable(3dm_gx4_test src/3dm_gx4_test.cpp src/3dm_gx4.cpp)
target_link_libraries(3dm_gx4_test  
  ${catkin_LIBRARIES}
  ${Boost_LIBRARIES}
)

add_executable(3dm_gx4_node src/3dm_gx4_node.cpp src/3dm_gx4.cpp)
target_link_libraries(3dm_gx4_node
  ${catkin_LIBRARIES}
  ${Boost_LIBRARIES}
)



