cmake_minimum_required(VERSION 2.8.3)
project(esp-idf-test)

find_package(catkin REQUIRED COMPONENTS roslaunch
  roscpp
  rospy
  std_msgs
  serial
  sensor_msgs
)

catkin_package()

include_directories(${catkin_INCLUDE_DIRS})

install(DIRECTORY config launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})

