cmake_minimum_required(VERSION  2.8.3)
project(sensor_data_reading)

add_definitions(-std=c++11)
find_package(catkin REQUIRED COMPONENTS roscpp std_msgs rospy mymsg message_generation cv_bridge image_transport sensor_msgs)

find_package(OpenCV 3.2 REQUIRED)
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

catkin_package(
CATKIN_DEPENDS  roscpp rospy std_msgs message_runtime#message_runtime不是个包，但自定义消息类型时要加
LIBRARIES mymsg
#  DEPENDS system_lib
)

include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(Radar0DataReading src/Radar0DataReading.cpp src/socket_can.cpp src/radar.cpp)
target_link_libraries(Radar0DataReading ${catkin_LIBRARIES})
add_dependencies(Radar0DataReading ${PROJECT_NAME}_gencpp)

add_executable(Radar1DataReading src/Radar1DataReading.cpp src/socket_can.cpp src/radar.cpp)
target_link_libraries(Radar1DataReading ${catkin_LIBRARIES})
add_dependencies(Radar1DataReading ${PROJECT_NAME}_gencpp)

add_executable(Camera0DataReading src/Camera0DataReading.cpp src/camera.cpp)
target_link_libraries(Camera0DataReading ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
add_dependencies(Camera0DataReading ${PROJECT_NAME}_gencpp)

add_executable(Camera1DataReading src/Camera1DataReading.cpp src/camera.cpp)
target_link_libraries(Camera1DataReading ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
add_dependencies(Camera1DataReading ${PROJECT_NAME}_gencpp)