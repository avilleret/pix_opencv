#!/bin/bash -ex 
	
SCRIPT_FOLDER=`dirname $(readlink -f $0)`
cd ${SCRIPT_FOLDER}

git clone --depth=1 https://github.com/umlaeute/Gem.git

pushd Gem
./autogen.sh
./configure 
make -j$((`nproc` - 1))
make install

popd 
rm -rf Gem