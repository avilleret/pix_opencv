#!/bin/bash -ex 
	
SCRIPT_FOLDER=`dirname $(readlink -f $0)`
cd ${SCRIPT_FOLDER}

git clone --depth=1 --branch 0.50-2 https://github.com/pure-data/pure-data.git

pushd pure-data
./autogen.sh
./configure 
make -j$((`nproc` - 1))
make install

popd 
rm -rf pure-data