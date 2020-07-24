#!/bin/sh

cd ~/opengl_workspace/realtime_render/

if [ ! -d "./build/" ];then
  echo "[INFO]>>> 创建build文件夹"
  mkdir ./build
else
  echo "[INFO]>>> 清空build下内容"
  rm -rf build/*
fi

cd ./build

cmake .. && make
mv realtrender .. && cd ..
./realtrender

