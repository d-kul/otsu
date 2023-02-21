#!/bin/bash

# requirements:
# ffmpeg (with libavformat to support pgm)
# imagemagick
# gcc (obviously with openmp)

set -o errexit

echo "compiling executable..."
g++ -fopenmp -O2 $1 -o process_gif

file="$(basename $2)"
file_noext="${file::-4}"
exec="./process_gif"
dir="$(pwd)"

echo "extracting frames..."
mkdir -p $file_noext
cents=$(identify -format "%T\n" $2)
frames=$(echo "$cents" | wc -l)
ffmpeg -i "$2" -vsync 0 "$file_noext/%03d.pgm" 2> /dev/null

echo "processing frames..."
cents_sum=0
for (( i=1; i<=frames; i++ ))
do
    name=$(printf "${file_noext}/%03d" $i)
    cents_sum=$((cents_sum + $(echo "${cents}" | sed -n "${i}p")))
    echo -ne "\r    frame no. $i of $frames..."
    ./process_gif 0 ${name}.pgm ${name}_new.pgm > /dev/null
done
echo ""

echo "combining frames..."
frame_rate=$(( ((frames * 100 + cents_sum / 2)) / cents_sum))
ffmpeg -y -framerate $frame_rate -i "$file_noext/%03d_new.pgm" "${file_noext}_new.gif" 2> /dev/null
rm -r "$file_noext/"
rm process_gif

echo "done!"
echo "file is at ${file_noext}_new.gif"
