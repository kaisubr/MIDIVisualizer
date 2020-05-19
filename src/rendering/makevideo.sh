#!/bin/bash

# command: "./makevideo.sh " + frameRate + " '" + outputDirPath + "' " + width + "x" + height
rate=$1
path=$2
pathim="'"+$path+"/output_%04d.png"+"'"
size=$3
echo $rate
echo $path
echo $size
ffmpeg -r $rate -f image2 -s $size -i $pathim -vcodec libx264 -crf 25 -pix_fmt yuv420p "video_noaudio.mp4"
ffmpeg -i video_noaudio.mp4 -i audio.mp3 -codec copy -shortest "video_withaudio.mp4"
