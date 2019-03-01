#!/bin/bash
echo "sh running"
echo $1 $2 $3

if [ -d ~/catkin_ws/src/src_code ]; then
    dir="src_code"
elif [ -d ~/catkin_ws/src/zeabus_software ]; then
    dir="zeabus_software"
else
    dir="None"
fi

if ! [ -d ~/catkin_ws/src/$dir/zeabus_vision/params/$1 ];then
    mkdir ~/catkin_ws/src/$dir/zeabus_vision/params/$1
fi

if ! [ -f ~/catkin_ws/src/$dir/zeabus_vision/params/$1/color_$3_$2.yaml ];then
    echo "color_"$3":" > ~/catkin_ws/src/$dir/zeabus_vision/params/$1/color_$3_$2.yaml
    cat ~/catkin_ws/src/$dir/zeabus_vision/params/template/color_template.yaml >> ~/catkin_ws/src/$dir/zeabus_vision/params/$1/color_$3_$2.yaml
fi