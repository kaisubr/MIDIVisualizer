#!/bin/bash -x

# audio is delayed by prerolling time.
# command: ./makevideo.sh frameRate 'outputDirPath' widthxheight audiodelay
rate=$1
path=$2
size=$3
delay=$4
echo $rate
echo $path
echo $size
echo $delay
cd "$path"
echo "Making video. Note that existing video may be overwritten."
ffmpeg -hide_banner -loglevel warning
cat *.png | ffmpeg -y -r $rate -f image2pipe -s $size -i - -vcodec libx264 -crf 25 -pix_fmt yuv420p "video_noaudio.mp4"
# ffmpeg -r $rate -f image2 -s $size -i "$path" -vcodec libx264 -crf 25 -pix_fmt yuv420p "video_noaudio.mp4"
# ffmpeg -i video_noaudio.mp4 -i *.mp3 -codec copy -shortest "video_withaudio.mp4"
ffmpeg -y -i video_noaudio.mp4 -itsoffset $delay -i *.mp3 -codec copy "video_withaudio.mp4"
