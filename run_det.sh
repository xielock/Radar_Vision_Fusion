#!/bin/bash

{
 gnome-terminal -x bash -c "source devel/setup.bash;rosrun xie runDet;exec bash"
}


