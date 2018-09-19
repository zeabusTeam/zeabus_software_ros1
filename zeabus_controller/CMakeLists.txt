cmake_minimum_required(VERSION 2.8.3)

project(zeabus_controller)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS roslib roscpp rospy dynamic_reconfigure geometry_msgs sensor_msgs nav_msgs tf genmsg actionlib_msgs actionlib message_generation std_msgs)

## System dependencies are found with CMake's conventions
find_package(Boost REQUIRED COMPONENTS system thread)

## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()


generate_dynamic_reconfigure_options(
  cfg/PIDConstant.cfg
  cfg/forcePIDConstant.cfg
  cfg/OffSetConstant.cfg
)

add_message_files(
  FILES
  control_force.msg
  orientation.msg
  point_xy.msg
)

add_service_files(
  DIRECTORY srv
  FILES
  drive_x.srv
  control_mode.srv
  message_service.srv
  ok_position.srv
  target_service.srv
  navigation_srv.srv
  fix_abs_xy.srv
  fix_rel_xy.srv
  fix_abs_depth.srv
  fix_abs_yaw.srv
  fix_abs_x.srv
  fix_abs_y.srv
  change_mode.srv
)
#add_action_files(DIRECTORY action FILES BarrelRoll.action)
generate_messages(DEPENDENCIES std_msgs)

include_directories(include ${catkin_INCLUDE_DIRS})

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if you package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
    INCLUDE_DIRS include
    LIBRARIES zeabus_controller
    CATKIN_DEPENDS roslib roscpp dynamic_reconfigure geometry_msgs sensor_msgs nav_msgs std_msgs  tf message_runtime
    DEPENDS
)

###########
## Build ##
###########

add_library(zeabus_controller src/offset_control/offset_controller_main.cpp)
#add_library(zeabus_controller src/linear_control/linear_controller_main.cpp)
#add_library(zeabus_controller src/recover_control/recover_controller_main.cpp)
#add_library(zeabus_controller src/assign_close.cpp)
#add_library(zeabus_controller src/manage_control.cpp)

#add_library( zeabus_controller ) 

##################add_executable(zeabus_controller src/other_file/old_code/zeabus_controller.cpp)
##############add_dependencies(zeabus_controller ${PROJECT_NAME}_gencfg zeabus_controller_gencpp)
####################################target_link_libraries(zeabus_controller ${catkin_LIBRARIES})

add_executable(pwm_test src/other_file/test_code/loop_pwm_test.cpp)
add_executable(origin_control src/other_file/not_use_code/new_controller_2018.cpp)
add_executable(test_force src/other_file/not_use_code/calculate_force.cpp)
add_dependencies(pwm_test ${PROJECT_NAME}_gencfg zeabus_controller_gencpp)
add_dependencies(origin_control ${PROJECT_NAME}_gencfg zeabus_controller_gencpp)
add_dependencies(test_force ${PROJECT_NAME}_gencfg zeabus_controller_gencpp)
target_link_libraries(pwm_test ${catkin_LIBRARIES})
target_link_libraries(origin_control ${catkin_LIBRARIES})
target_link_libraries(test_force ${catkin_LIBRARIES})

#add_executable(sim_controller src/sim_controller.cpp)
#add_dependencies(sim_controller ${PROJECT_NAME}_gencfg zeabus_controller_gencpp)
#target_link_libraries(sim_controller ${catkin_LIBRARIES})

add_executable(control src/manage_control.cpp)
add_dependencies(control ${PROJECT_NAME}_gencfg zeabus_controller_gencpp)
target_link_libraries(control zeabus_controller ${catkin_LIBRARIES})

add_executable(offset_control src/offset_control/offset_controller_main.cpp)
add_dependencies(offset_control ${PROJECT_NAME}_gencfg zeabus_controller_gencpp)
target_link_libraries(offset_control zeabus_controller ${catkin_LIBRARIES})

add_executable(linear_control src/linear_control/linear_controller_main.cpp)
add_dependencies(linear_control ${PROJECT_NAME}_gencfg zeabus_controller_gencpp)
target_link_libraries(linear_control zeabus_controller ${catkin_LIBRARIES})

add_executable(recover_control src/recover_control/recover_controller_main.cpp)
add_dependencies(recover_control ${PROJECT_NAME}_gencfg zeabus_controller_gencpp)
target_link_libraries(recover_control zeabus_controller ${catkin_LIBRARIES})

add_executable(assign_close src/assign_close.cpp)
add_dependencies(assign_close ${PROJECT_NAME}_gencfg zeabus_controller_gencpp)
target_link_libraries(assign_close zeabus_controller ${catkin_LIBRARIES})

# add_executable(TranformFrameBroadcaster src/tf-broadcaster.cpp)
# target_link_libraries(TranformFrameBroadcaster ${catkin_LIBRARIES})

# add_executable(TranformFrameListener src/tf-listener.cpp)
# target_link_libraries(TranformFrameListener ${catkin_LIBRARIES})

# add_library(hector_quadrotor_motor_controller src/motor_controller.cpp)
# target_link_libraries(hector_quadrotor_motor_controller hector_quadrotor_controller)

#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executables and/or libraries for installation
# install(TARGETS hector_quadrotor_controller hector_quadrotor_pose_controller hector_quadrotor_twist_controller hector_quadrotor_motor_controller
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(DIRECTORY launch params DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})
# install(FILES
#   plugin.xml
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

