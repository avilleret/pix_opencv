#!/bin/bash -ex 

if [[ "$OSTYPE" == "darwin"* ]]; then
  SCRIPT_FOLDER=`dirname $(greadlink -f $0)`
  INSTALL_FOLDER="${SCRIPT_FOLDER}/../../../3rdparty/opencv-install"
  rm -rf ${INSTALL_FOLDER}
  CMAKE_OPTS="-DCMAKE_INSTALL_PREFIX=${INSTALL_FOLDER}"
else
  SCRIPT_FOLDER=`dirname $(readlink -f $0)`
  CMAKE_OPTS="-DWITH_V4L=ON"
fi

cd ${SCRIPT_FOLDER}

BUILD_FOLDER=build-opencv

OPENCV_BRANCH=4.3.0

if [[ -e opencv ]]; then
  pushd opencv
  CURRENT_BRANCH=`git describe --always`
  if [[ ${CURRENT_BRANCH} != ${OPENCV_BRANCH} ]]; then
    if [[ ! `git tag | grep ${OPENCV_BRANCH}` ]]; then
      git fetch origin ${OPENCV_BRANCH}:${OPENCV_BRANCH} || true
    fi
    git checkout ${OPENCV_BRANCH}
  fi
  popd
else
  git clone --depth=1 --branch ${OPENCV_BRANCH} https://github.com/opencv/opencv.git
fi

if [[ -e opencv_contrib ]]; then
  pushd opencv_contrib
  CURRENT_BRANCH=`git describe --always`
  if [[ ${CURRENT_BRANCH} != ${OPENCV_BRANCH} ]]; then
    if [[ ! `git tag | grep ${OPENCV_BRANCH}` ]]; then
      git fetch origin ${OPENCV_BRANCH}:${OPENCV_BRANCH} || true
    fi
  git checkout ${OPENCV_BRANCH}
  fi
  popd
else
  git clone --depth=1 --branch ${OPENCV_BRANCH} https://github.com/opencv/opencv_contrib.git
fi

mkdir -p ${BUILD_FOLDER} && cd ${BUILD_FOLDER}

cmake "${PWD}/../opencv" -GNinja \
      -DCMAKE_BUILD_TYPE=RELEASE \
      -DWITH_TBB=ON \
      -DWITH_QT=OFF \
      -DWITH_OPENGL=OFF \
      -DWITH_NVCUVID=OFF \
      -DWITH_VTK=OFF \
      -DBUILD_EXAMPLES=OFF \
      -DOPENCV_EXTRA_MODULES_PATH="${PWD}/../opencv_contrib/modules" \
      -DBUILD_LIST=calib3d,core,features2d,highgui,imgcodecs,imgproc,objdetect,photo,video,videoio,bgsegm \
      $CMAKE_OPTS 

cmake --build .
cmake --build . --target install

cd ..
rm -rf opencv opencv_contrib ${BUILD_FOLDER}
