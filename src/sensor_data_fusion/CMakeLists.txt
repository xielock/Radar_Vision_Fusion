cmake_minimum_required(VERSION  2.8.3)
project(sensor_data_fusion)

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

add_executable(Radar0DataArrange src/Radar0DataArrange.cpp src/RadarDataArrange.cpp)
target_link_libraries(Radar0DataArrange ${catkin_LIBRARIES})
add_dependencies(Radar0DataArrange ${PROJECT_NAME}_gencpp)

add_executable(Radar1DataArrange src/Radar1DataArrange.cpp src/RadarDataArrange.cpp)
target_link_libraries(Radar1DataArrange ${catkin_LIBRARIES})
add_dependencies(Radar1DataArrange ${PROJECT_NAME}_gencpp)

add_executable(RadarDataTimeFusion src/RadarDataTimeFusion.cpp src/TwoRadarTimeFusion.cpp)
target_link_libraries(RadarDataTimeFusion ${catkin_LIBRARIES})
add_dependencies(RadarDataTimeFusion ${PROJECT_NAME}_gencpp)

add_executable(RadarDataSpaceFusion src/RadarDataSpaceFusion.cpp)
target_link_libraries(RadarDataSpaceFusion ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
add_dependencies(RadarDataSpaceFusion ${PROJECT_NAME}_gencpp)

add_executable(TransOneRadarTopicName src/TransOneRadarTopicName.cpp)
target_link_libraries(TransOneRadarTopicName ${catkin_LIBRARIES})
add_dependencies(TransOneRadarTopicName ${PROJECT_NAME}_gencpp)

add_executable(TransOneCameraTopicName src/TransOneCameraTopicName.cpp)
target_link_libraries(TransOneCameraTopicName ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
add_dependencies(TransOneCameraTopicName ${PROJECT_NAME}_gencpp)

add_executable(ReceiveRadarData src/ReceiveRadarData.cpp)
target_link_libraries(ReceiveRadarData ${catkin_LIBRARIES})
add_dependencies(ReceiveRadarData ${PROJECT_NAME}_gencpp)

add_executable(ReceiveCameraData src/ReceiveCameraData.cpp)
target_link_libraries(ReceiveCameraData ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
add_dependencies(ReceiveCameraData ${PROJECT_NAME}_gencpp)

add_executable(CameraRadarTimeFusion src/CameraRadarTimeFusion.cpp)
target_link_libraries(CameraRadarTimeFusion ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
add_dependencies(CameraRadarTimeFusion ${PROJECT_NAME}_gencpp)

add_executable(CameraRadarSpaceFusion src/CameraRadarSpaceFusion.cpp)
target_link_libraries(CameraRadarSpaceFusion ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
add_dependencies(CameraRadarSpaceFusion ${PROJECT_NAME}_gencpp)


#add_executable(CameraRadarSpaceFusionRidgeDetection src/CameraRadarSpaceFusionRidgeDetection.cpp)
#target_link_libraries(CameraRadarSpaceFusionRidgeDetection ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
#add_dependencies(CameraRadarSpaceFusionRidgeDetection ${PROJECT_NAME}_gencpp)
