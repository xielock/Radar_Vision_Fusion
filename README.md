# Sensor_Data_Fusion
## introduction

This is a program that fuses mmWave radar and camera  information. At the same time, YOLOv4 is used for target detection.  The program focus on the spatio-temporal alignment of camera and mmWave radar, and radar target points are mapped into camera images. The detection part is not  completed, the method used now is simple. 
## Requirements

- Linux Ubuntu (tested on version 16.04 and 18.04)
- CUDA 9.0
- cuDNN 7.6.3
- ROS Kinetic or ROS  Melodic
- TensorRT 5.0.1
- OpenCV 3.3.1

>  If you use Jetson Nano or TX2, JetPack can get all the required above, and my installation version is JetPack3.3

## installation

create a ROS workspace and copy the repo to the workspace

``` bash
#compile header file first
catkin_make -DCATKIN_WHITELIST_PACKAGES='mymsg'
#then
catkin_make -DCATKIN_WHITELIST_PACKAGES=''
#
source devel/setup.bash
```

in src/ folder, **ui_start.sh** can be used through

```bash
#remind 'chmod a+x ui_start.sh' to give execution permission
./ui_start.sh 
```

in root folder, **start_1radar_1camera.sh** can open all ROS nodes, the use of script is same as **ui_start.sh**

## Usage&Outputs

in **src/yolo_detection/** folder, you need to create a model folder, and put your detection weights file here.       

GUI:
![GUI](https://github.com/xielock/Radar_Vision_Fusion/blob/master/images/image-20230121171446893.png)
 rqt_graph result:
> The last detection node is missing in the image, because my notebook does not have CUDA, cuDNN installed
![rqt](https://github.com/xielock/Radar_Vision_Fusion/blob/master/images/rosgraph.png)
space fusion result, the radar points are mapped into images:
![space fusion](https://github.com/xielock/Radar_Vision_Fusion/blob/master/images/1111.gif)
detection result sample:
![results](https://github.com/xielock/Radar_Vision_Fusion/blob/master/images/image-20230121171446893.png)

> I can't remotely connect to my desktop, so the above is mostly from memory, some parts may be inaccurate, I will continue to improve when I get back to school.
