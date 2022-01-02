cmake_minimum_required(VERSION  2.8.3)
project(mymsg)

add_definitions(-std=c++11)
find_package(catkin REQUIRED COMPONENTS roscpp std_msgs rospy message_generation cv_bridge image_transport sensor_msgs)

add_message_files(
  FILES
  RadarData.msg
  RadarDataPerSec.msg
  RadarDataTimeFusion.msg
  CameraDataPerSec.msg
  CameraRadarTimeFusion.msg
  SingleBox.msg 
  CameraRadarSpaceFusion.msg
  origin_and_result.msg
)


generate_messages(
  DEPENDENCIES
  std_msgs
  sensor_msgs
)

catkin_package(
CATKIN_DEPENDS  roscpp rospy std_msgs message_runtime#message_runtime不是个包，但自定义消息类型时要加
#  DEPENDS system_lib
)

include_directories(${catkin_INCLUDE_DIRS})

add_executable(assist_build src/main.cpp)
target_link_libraries(assist_build ${catkin_LIBRARIES})

#先编译这一句，这样别的功能包才能引用其中定义的消息类型
#catkin_make -DCATKIN_WHITELIST_PACKAGES='mymsg'
#然后恢复编译所有包
#catkin_make -DCATKIN_WHITELIST_PACKAGES=''
