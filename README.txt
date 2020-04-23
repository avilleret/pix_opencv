pix_opencv
==========

OpenCV bindings for Puredata/Gem.

# Building
We are now using CMake to build `pix_opencv`.
So you can build it with : 

    mkdir -p build && cd build   # create a new folder and go into
    cmake ..
    cmake --build .  # this will produce a pix_opencv.pd_darwin in the build folder

On MacOS, CMake might fail to locate Pd and Gem if don't install them in the Unix way (inside /usr/local with their respective `.pc` files in the right place). In that case, you can hint the location by filling the `PUREDATA_INCLUDE_DIRS` variable with the path to the folder where is `m_pd.h` and the `GEM_INCLUDE_DIRS` with the path to the folder of Gem headers. For example, if you have Pd 0.50-2 in `/Applications` folder and if you install Gem from deken in `~/Documents/Pd/externals/`.

	cmake .. -DPUREDATA_INCLUDE_DIRS=/Applications/Pd-0.50-2.app/Contents/Resources/src/ \ 
			 -DGEM_INCLUDE_DIRS=~/Documents/Pd/externals/Gem/include/Gem
