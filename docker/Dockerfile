FROM ubuntu:18.04

# first of all upgrade
RUN apt-get update -yqq && apt-get upgrade -yqq

# install build tools
RUN apt-get update -yqq && apt-get install -yqq build-essential autoconf libtool ninja-build gcc-8 g++-8 git wget

# install CMake 3.17 binary from Kitware (apt provides 3.10)
RUN wget https://github.com/Kitware/CMake/releases/download/v3.17.2/cmake-3.17.2-Linux-x86_64.sh -O /usr/local/install_cmake.sh
RUN chmod +x /usr/local/install_cmake.sh
RUN /usr/local/install_cmake.sh --skip-license
RUN rm /usr/local/install_cmake.sh

# install video capture libraries
RUN apt-get update -yqq && apt-get install -yqq libavcodec-dev libavutil-dev libavfilter-dev libavformat-dev libswresample-dev libdc1394-22-dev libv4l-dev

# install video rendering libraries
RUN apt-get update -yqq && apt-get install -yqq libsdl2-dev libsdl2-2.0-0 libglu1-mesa-dev libglu1-mesa libgles2-mesa-dev

# build OpenCV from source 
COPY scripts/build-opencv.sh /tmp/build-opencv.sh
RUN /tmp/build-opencv.sh
RUN rm /tmp/build-opencv.sh

# build puredata from source 
COPY scripts/build-puredata.sh /tmp/build-puredata.sh
RUN /tmp/build-puredata.sh
RUN rm /tmp/build-puredata.sh

# install Gem dependencies
RUN apt-get update -yqq && apt-get install -yqq libglew-dev

# build Gem from source 
COPY scripts/build-gem.sh /tmp/build-gem.sh
RUN /tmp/build-gem.sh
RUN rm /tmp/build-gem.sh