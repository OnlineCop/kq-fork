#!/bin/sh

# This file is copyright Winter Knight. You may use this code without restriction.
# You may distribute this code under the MIT or GPLv2 (or later) license.

# These directory paths are hard-coded into the kq binary.
# If you change any of these directory paths, even PREFIX, you will break KQ.

PREFIX="/usr/local/"
BIN=${PREFIX}/bin
SCRIPTS=${PREFIX}/lib/kq/scripts/
DATA=${PREFIX}/share/kq/data/
MAPS=${PREFIX}/share/kq/maps/
MUSIC=${PREFIX}/share/kq/music/

echo I am about to install kq into $PREFIX

if [ `id -u` -ne 0 ]; then
	echo Warning: you are not root. You probably have to be root to install this package. 
	echo If you wish, I will try anyway.
fi

echo Press enter to continue. Ctrl-C to abort.
read

# Bin first
/usr/bin/test -d $BIN || mkdir -p $BIN
/usr/bin/install -c -m 755 bin/kq $BIN

# Scripts next
/usr/bin/test -d $SCRIPTS || mkdir -p $SCRIPTS
for i in scripts/*;
	do /usr/bin/install -c -m 644 $i $SCRIPTS; done

# Data
/usr/bin/test -d $DATA || mkdir -p $DATA
for i in data/*;
	do /usr/bin/install -c -m 644 $i $DATA; done

# Maps
/usr/bin/test -d $MAPS || mkdir -p $MAPS
for i in maps/*;
	do /usr/bin/install -c -m 644 $i $MAPS; done

# Music
/usr/bin/test -d $MUSIC || mkdir -p $MUSIC
for i in music/*;
	do /usr/bin/install -c -m 644 $i $MUSIC; done

echo Done. Run "kq" to start playing.
