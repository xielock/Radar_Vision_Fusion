#1.cmake verson，指定cmake版本,自己的版本比这个高就可以
cmake_minimum_required(VERSION 3.2)

#补.设置编译选项
add_definitions(-std=c++11)

#2.project name，指定项目的名称，一般和项目的文件夹名称对应，就是最后生成的可执行文件名称
project(yolo_detection)#一定要与#7中的名字一致,好像test是保留字，不能用

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_BUILD_TYPE Relaese)
set(CMAKE_CXX_STANDARD  11)
set(GPU_ARCHS 61)
set(TENSORRT_ROOT /home/xielock/下载/TensorRT-7.0.0.11)
#补.添加opencv库
message("here we go")
find_package(catkin REQUIRED COMPONENTS roscpp std_msgs rospy message_generation  sensor_msgs cv_bridge image_transport)
find_package(OpenCV 3.2 REQUIRED)#注意对应版本

add_message_files(
  FILES
  data.msg
  yolo_single.msg
  origin_and_result_yolo.msg
)


generate_messages(
  DEPENDENCIES
  std_msgs
  sensor_msgs
)

include_directories(OpenCV 3.2 REQUIRED)
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

catkin_package(
#  INCLUDE_DIRS include
CATKIN_DEPENDS  roscpp rospy std_msgs message_runtime#message_runtime不是个包，但自定义消息类型时要加
#  DEPENDS system_lib
)

#3.head file path，头文件目录，所有.h文件放在include文件夹下
INCLUDE_DIRECTORIES(include ${catkin_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

#4.source directory，源文件目录，DIR_SRCS代表目录下所有源文件，所有.cpp文件放在src文件夹下
AUX_SOURCE_DIRECTORY(src DIR_SRCS)

#5.set environment variable，设置环境变量，编译用到的源文件全部都要放到这里，否则编译能够通过，但是执行的时候会出现各种问题，比如"symbol lookup error xxxxx , 
SET(TEST_MATH ${DIR_SRCS})
#居然只有先链接catkin再链接opencv才行 同时必须有空格  呵呵

add_subdirectory(src)
add_subdirectory(onnx-tensorrt)
add_subdirectory(example)



