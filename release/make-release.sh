#!/bin/sh

# This script is beta. It may not work on your system.
# This script has several requirements. They are not all listed here:
# bash (maybe. not tested with other shells.)
# gcc, make, and autotools
# gnu utils, such as date, find, grep, tar, and gzip
# autopackage gcc (apgcc)
# Making Windows binaries requires:
  # mingw32
  # the windows binaries for lua50 and allegro4.2
  # the windows binaries for libdumb (not included upstream)
  # nsis (Nullsoft Installation Scripter)

TMPDIR="/tmp"
VERSION="$1"	# Do not run suid or sgid without fixing this line.
					# Special characters, such as spaces and semi-colons must be
					# stripped out, or else the user can execute
					# arbitrary commands.

if [ -z "$VERSION" ]; then
	VERSION=`date +%Y%m%d`
fi

cd ..
ROOT=`pwd`
# Set these variables to the directories where your extra windows includes and libaries are (for allegro, libdumb, and lua)
# mingw can usually find windows.h and such automatically. You probably do not need to include those directories here.
WIN_INC=-I$HOME/kq-win32-depends/include
WIN_LIB=-L$HOME/kq-win32-depends/lib
WIN_LIB_DIR=$HOME/kq-win32-depends/lib

# ===============

cd "$TMPDIR"
test -d kq-*-$VERSION && echo "Please remove ${TMPDIR}/kq-'*'-$VERSION and re-run script" && exit
cp -a "$ROOT" ./kq-src-$VERSION
cd kq-src-$VERSION

# Clean unneeded files
rm -rf `find -type d | grep CVS`
rm `find -type f | grep .cvsignore`
rm `find -type f | grep .cvswrappers`

# Build and package source
build_source ()
{
autoreconf -i
rm *.o kq mapd* scripts/*.lob scripts/pscripts/*
rmdir scripts/pscripts
cd ..
tar -cf kq-src-$VERSION.tar kq-src-$VERSION
gzip -9 kq-src-$VERSION.tar
}

# Build and package linux binaries (KQ game)
build_linux_binary ()
{
cd kq-src-$VERSION
make clean
./configure CC="apgcc"
cd scripts
make
cd ..
make kq
strip kq

cd ..
mkdir kq-linuxbin-$VERSION
cd kq-linuxbin-$VERSION

mkdir bin
cp ../kq-src-$VERSION/kq bin
mkdir data
cp ../kq-src-$VERSION/data/* data
mkdir maps
cp ../kq-src-$VERSION/maps/*.map maps
mkdir music
cp ../kq-src-$VERSION/music/* music
mkdir scripts
cp ../kq-src-$VERSION/scripts/*.lob scripts

cp ../kq-src-$VERSION/COPYING .
cp ../kq-src-$VERSION/release/linux-bin-README ./README
cp ../kq-src-$VERSION/release/linux-bin-install.sh ./install.sh

cd ..
tar -cf kq-linuxbin-$VERSION.tar kq-linuxbin-$VERSION
gzip -9 kq-linuxbin-$VERSION.tar
}

# Build and package linux binaries (map editor et al)
build_linux_mapeditor ()
{
cd kq-src-$VERSION
make clean
./configure
make mapdraw
make mapdraw2
make mapdump
make mapdiff

cd ..
mkdir kq-mapeditor-$VERSION
cd kq-mapeditor-$VERSION

cp ../kq-src-$VERSION/mapdraw .
cp ../kq-src-$VERSION/mapdraw2 .
cp ../kq-src-$VERSION/mapdump .
cp ../kq-src-$VERSION/mapdiff .
cp ../kq-src-$VERSION/maps/*.pcx .
cp ../kq-src-$VERSION/maps/mapdraw2.glade .

cp ../kq-src-$VERSION/COPYING .
# should copy other files, such as README and install.sh

cd ..
tar -cf kq-mapeditor-$VERSION.tar kq-mapeditor-$VERSION
gzip -9 kq-mapeditor-$VERSION.tar
}


# Build and package windows binaries (KQ game)
# This assumes that allegro, libdumb, and lua are already compiled and available.
build_win32_binary ()
{
cd kq-src-$VERSION
make clean
./configure --host=i686-mingw32
sed 's/define HAVE_SYS_SELECT_H 1/undef HAVE_SYS_SELECT_H/' config.h >tmp.txt
mv tmp.txt config.h

cd scripts
make
cd ..

make kq CC="i586-mingw32msvc-cc" OBJC="i586-mingw32msvc-cc" LINK="i586-mingw32msvc-cc" INCLUDES="$WIN_INC" LDFLAGS="$WIN_LIB" LIBS="-llua50 -lintl"

cd ..
mkdir kq-win32bin-$VERSION
cd kq-win32bin-$VERSION

cp ../kq-src-$VERSION/kq ./KQ.exe
mkdir data
cp ../kq-src-$VERSION/data/* data
mkdir maps
cp ../kq-src-$VERSION/maps/*.map maps
mkdir music
cp ../kq-src-$VERSION/music/* music
mkdir scripts
cp ../kq-src-$VERSION/scripts/*.lob scripts

cp ../kq-src-$VERSION/release/win32-bin-README ./README
cp ../kq-src-$VERSION/COPYING ./GPL
cp ../kq-src-$VERSION/release/win32-bin-COPYING ./COPYING


cp ../kq-src-$VERSION/release/kq.nsi .

cp $WIN_LIB_DIR/alleg42.dll .
cp $WIN_LIB_DIR/lua50.dll .
cp $WIN_LIB_DIR/intl.dll .

makensis "-XOutFile kq-win32bin-installer-$VERSION.exe" kq.nsi
mv kq-win32bin-installer-$VERSION.exe ..
}

# Build and package windows binaries (map editor et al)


# Build and package Mac binaries (KQ game)


# Build and package Mac binaries (map editor et al)


# Clean-up
cleanup_after ()
{
cd "$TMPDIR"
rm -rf kq-*-$VERSION
}


# Start here!

LIN=0
WIN=0
LIN_MAP=0

while [ -n "$1" ]
do
	if [ "$1" = "lin" ]; then
		LIN=1
	elif [ "$1" = "win" ]; then
		WIN=1
	elif [ "$1" = "lin-map" ]; then
		LIN_MAP=1
	fi
	shift
done

build_source
if [ $LIN -eq 1 ]; then
	build_linux_binary
fi

if [ $WIN -eq 1 ]; then
	build_win32_binary
fi

if [ $LIN_MAP -eq 1 ]; then
	build_linux_mapeditor
fi
