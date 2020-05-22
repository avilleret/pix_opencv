#!/bin/bash -ex

SCRIPT_FOLDER=`dirname $(readlink -f $0)`
REPO_ROOT="${SCRIPT_FOLDER}/.."
BUILD_FOLDER="${REPO_ROOT}/build" 

function cleanup {
  if [ -d ${BUILD_FOLDER} ]; then
    echo "Removing ${BUILD_FOLDER}"
    rm  -r ${BUILD_FOLDER}
  fi

  if [ -d  ${REPO_ROOT}/3rdparty/NDI_SDK_for_Linux ]; then
    echo "Removing extracted files"
    rm -rf ${REPO_ROOT}/3rdparty/NDI_SDK_for_Linux 
  fi
}

trap cleanup EXIT

pushd ${REPO_ROOT}
if [ -z $CI_COMMIT_TAG ]
then
  TIMESTAMP=`date +%Y.%m.%d-%H.%M.%S`
  HASH=`git describe --always || echo NO_HASH`
  VERSION=${TIMESTAMP}-${HASH}
else
  VERSION=$CI_COMMIT_TAG
fi
echo "Building Version: ${VERSION}"
popd

PIX_OPENCV_ARCHIVE=${REPO_ROOT}/pix_opencv-${VERSION}.tar.gz

mkdir -p ${BUILD_FOLDER} && cd ${BUILD_FOLDER}

cmake ${REPO_ROOT} -GNinja \
  -DCMAKE_C_COMPILER=/usr/bin/gcc-8 \
  -DCMAKE_CXX_COMPILER=/usr/bin/g++-8 \
  -DCMAKE_INSTALL_PREFIX=/usr

cmake --build .
cmake --build . --target install

if [ -d package ]; then
  pushd package
  tar cazf ${PIX_OPENCV_ARCHIVE} *
  popd

  if [ ! -z $CI_COMMIT_TAG ]
  then
    set +x # do not show FTP_* variable on command line
    curl -T "${PIX_OPENCV_ARCHIVE}" ftp://${FTP_USER}:${FTP_PASSWORD}@${FTP_URL}/releases/
  fi
fi