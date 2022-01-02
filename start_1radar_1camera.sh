#!/bin/bash

{
 gnome-terminal -x bash -c "cd ../../;roscore;exec bash"
}

sleep 3s

{
 gnome-terminal	--tab -e 'bash -c "cd ../../;source devel/setup.bash;rosrun sensor_data_reading Radar0DataReading;exec bash"'\
		--tab -e 'bash -c "cd ../../;source devel/setup.bash;rosrun sensor_data_fusion Radar0DataArrange;exec bash"'\
		--tab -e 'bash -c "cd ../../;source devel/setup.bash;rosrun sensor_data_fusion TransOneRadarTopicName;exec bash"'\
		--tab -e 'bash -c "cd ../../;source devel/setup.bash;rosrun sensor_data_reading Camera0DataReading;exec bash"'\
		--tab -e 'bash -c "cd ../../;source devel/setup.bash;rosrun sensor_data_fusion TransOneCameraTopicName;exec bash"'\
		--tab -e 'bash -c "cd ../../;source devel/setup.bash;rosrun sensor_data_fusion CameraRadarTimeFusion;exec bash"'\
		--tab -e 'bash -c "cd ../../;source devel/setup.bash;rosrun sensor_data_fusion CameraRadarSpaceFusion;exec bash"'\
		--tab -e 'bash -c "cd ../../;source devel/setup.bash;rosrun xie runDet;exec bash"'
}

