#!/bin/bash
#
#	Script to make .pd_darwins and place them in ~/Documents/Pd/externals
#	questions to: fdch.github.io
#
#	Usage: 
#		You need to install pix_opencv [optionally Facetracker] first:
#		$ cd pix_opencv
#		$ sh mac/make_darwin.sh
#

ROOT=$(pwd)
SRCDIR=$ROOT/src

TARGET=pix_opencv
TARGET_PATH=~/Documents/Pd/externals
TARGET_DIR=$TARGET_PATH/$TARGET

#	Where .o reside
LIBDIR=$SRCDIR/.libs
FTLIB=$ROOT/Facetracker/src/lib

CXX=g++
EXTENSION=pd_darwin


LIBS=" -ldl -lz -lpthread -g -O2 "

#	Dependencies
DEPS=(
		pd
		Gem
		opencv
)


#	Array to store .o names / basenames
BINARY=()
FTOBJ=''

FACETRACKER_INSTRUCTIONS="\
To compile Facetraker, do this:
	$ cd $TARGET 
	$ git submodule init
	$ git submodule update
	$ cd Facetracker
	$ make
Otherwise, it will be skipped."


function get_dependencies()
{
	for i in ${DEPS[@]}
	do
		LIBS+=" `pkg-config --libs $i`"
	done
}

function make_target_dir()
{
	if [[ ! -d $TARGET_DIR ]]
	then
		echo "Creating $TARGET_DIR to place binaries..."
		mkdir $TARGET_DIR || echo "... could not create. Exiting." && exit
	fi
}

function get_objects()
{
	local cnt=0
	cd $SRCDIR
	for i in *.o
	do
		if [[ -f $i ]]
		then
			if [[ "${TARGET}.o" != "$i" ]]
			then
				BINARY[$cnt]="`basename $i .o | cut -f2 -d- `.${EXTENSION}"
				((cnt++))
				BINARY[$cnt]="$i"
				((cnt++))
			fi
		fi
	done
	cd $ROOT
	if [[ ! -d $FTLIB ]]
	then
		echo "Facetracker was not initialized"
		echo "$FACETRACKER_INSTRUCTIONS"
	else
		for i in $FTLIB/*.o
		do
			if [[ -f $i ]]
			then
				FTOBJ+=" $i"
			else
				echo "Skiping: $i"
			fi
		done
	fi
}

function check_multiple_compilation()
{
	local i

	for ((i=0; i<${#BINARY[@]}; i+=2))
	do
		n=$((i+1))
		echo "$CXX -o ${BINARY[$i]} ${BINARY[$n]} $FTOBJ $LIBS"
	done

}

function check_single_compilation()
{
	local i
	local obj=""

	for ((i=0; i<${#BINARY[@]}; i+=2))
	do
		obj+=" $LIBDIR/${BINARY[$((i+1))]}"
	done

	echo "$CXX -o $TARGET.${EXTENSION} $obj $FTOBJ $LIBS"

}

function make_binaries()
{
	local i
	cd $SRCDIR
	for ((i=0; i<${#BINARY[@]}; i+=2))
	do
		n=$((i+1))
		echo "$CXX -o ${BINARY[$i]} ${BINARY[$n]} $FTOBJ $LIBS"
		$CXX -o ${BINARY[$i]} ${BINARY[$n]} $LIBS
	done
	cd $ROOT
}





function make_one()
{
	local src=`basename "$1" .o`
	local obj=pix_opencv_la-$src.o
	local tar=$src.pd_darwin
	cd $SRCDIR
	if [[ ! -f "$src.cc" ]]
	then
		echo "$src.cc does not exist. Exiting."
	else
		echo "Compiling $src ..."
		$CXX -DPACKAGE_NAME=\"$TARGET\" -DPACKAGE_TARNAME=\"$TARGET\" -DPACKAGE_VERSION=\"0.4\" -DPACKAGE_STRING=\"$TARGET\ 0.4\" -DPACKAGE_BUGREPORT=\"antoine.villeret@gmail.com\" -DPACKAGE_URL=\"\" -DPACKAGE=\"$TARGET\" -DVERSION=\"0.4\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DLT_OBJDIR=\".libs/\" -DHAVE_BGSUB=1 -DHAVE_CLAHE=1 -DHAVE_LIBOPENCV_IMGPROC=1 -DHAVE_LIBOPENCV_OBJDETECT=1 -DHAVE_LIBOPENCV_CALIB3D=1 -DHAVE_LIBOPENCV_VIDEO=1 `pkg-config --cflags {pd,Gem}` -g -O2 -MT $obj -MD -MP -MF .deps/$src.Tpo -c -o $obj $src.cc
		echo "Linking $src ..."
		echo "$CXX -o $tar $obj $FTOBJ $LIBS"
		$CXX -o $tar $obj $LIBS
		echo "Installing $src ..."
		ln -f $tar $TARGET_DIR
	fi
	cd $ROOT
}

function make_single_binary()
{
	local i
	local obj=""

	for ((i=0; i<${#BINARY[@]}; i+=2))
	do
		obj+=" $LIBDIR/${BINARY[$((i+1))]}"
	done

	cd $SRCDIR
	echo "$CXX -o $TARGET.${EXTENSION} $obj $FTOBJ $LIBS"
	$CXX -o $TARGET.${EXTENSION} $obj $LIBS
	cd $ROOT
}

function check_firstone()
{
	# try with only one
	cd $SRCDIR
	echo "$CXX -o ${BINARY[2]} ${BINARY[3]} $FTOBJ $LIBS"
	$CXX -o ${BINARY[2]} ${BINARY[3]} $FTOBJ $LIBS
	cd $ROOT
}

function link_lib()
{
	echo "Linking the following files to $TARGET_DIR:"
	for i in $SRCDIR/*.${EXTENSION}
	do
		if [[ ! -f $i ]]
		then
			echo "$i not found, there was a problem"
		else
			if [[ ! -f $TARGET_DIR/$i ]]
			then
				basename "$i"
				ln -f "$i" $TARGET_DIR
			fi
		fi
	done
}

if [ `uname` != "Darwin" ]
then
	echo "Only for macos. Exiting."
	exit 1
elif [[ ! -d $LIBDIR ]]
then
	echo "You need to compile $TARGET first, as in:
	$ cd $TARGET
	$ sh autogen.sh
	$ sh configure --with-pd=/usr/local/include/pd --with-gem=/usr/local/include/Gem
	$ make
	$ make install libdir=$TARGET_PATH"
	echo "Optionally, you can install Facetracker before compiling $TARGET."
	echo "$FACETRACKER_INSTRUCTIONS"
	exit 1
else
	get_dependencies
	make_target_dir
	if [[ $1 ]]
	then
		echo "making $1"
		make_one "$1"
		exit
	else
		get_objects
		make_binaries
		# link_lib
		#	Copy missing helpfile to target dir
		cp $ROOT/${TARGET}-help.pd $TARGET_DIR/${TARGET}-help.pd 
		#	Copy <model> directory to target dir (for Facetracker)
		rsync -aP $ROOT/model $SRCDIR/*.pd_darwin $TARGET_DIR
		echo "Try running the following:
		$ pd -lib Gem:pix_opencv -open $TARGET_DIR/${TARGET}-help.pd"
	fi
fi
