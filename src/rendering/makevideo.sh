#!/bin/bash

# command: ./makevideo.sh frameRate 'outputDirPath' widthxheight
rate=$1
path=$2
size=$3
echo $rate
echo $path
echo $size
cd "$path"
cat *.png | ffmpeg -r $rate -f image2pipe -s $size -i - -vcodec libx264 -crf 25 -pix_fmt yuv420p "video_noaudio.mp4"
# ffmpeg -r $rate -f image2 -s $size -i "$path" -vcodec libx264 -crf 25 -pix_fmt yuv420p "video_noaudio.mp4"
ffmpeg -i video_noaudio.mp4 -i audio.mp3 -codec copy -shortest "video_withaudio.mp4"
