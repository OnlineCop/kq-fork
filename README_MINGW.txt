How to compile KQ for Windows with MinGW and MSYS
=====================================

1. MinGW
You can either use the installer (MinGW 5.0.x) or download and install manually:
- mingw runtime
- gcc-core
- binutils
- w32-api
- mingw32-make

2. MSYS
Download and install MSYS-1.0.10.exe. If you build KQ from CVS, also get msysDTK-1.0.1.exe.
Make sure that you have "mounted" /mingw in /etc/fstab

3. Lua
Download and extract lua-5.0.x.tar.gz. Edit the file "config",
change "INSTALL_ROOT= /usr/local" to "INSTALL_ROOT= /mingw"
and run "make && make install" (in MSYS).

for crosscompilation, run this:
make CC=i586-mingw32msvc-gcc RANLIB=i586-mingw32msvc-ranlib \
mingw
make INSTALL_TOP=/usr/local/i586-mingw32msvc \
'INSTALL_EXEC= $(INSTALL) -m 0755' 'INSTALL_DATA= $(INSTALL) -m 0644' \
"TO_LIB=lua51.dll" "TO_BIN=lua.exe luac.exe" \
install

4. Allegro
Download and extract all4xx.zip.
Download and extract to the MinGW root directory dx70_mgw.zip or dx80_mgw.zip.
Run "fix.sh mingw32 && MINGDIR=/mingw make && MINGDIR=/mingw make install" in the allegro directory. (in MSYS)

5. DUMB
Download and extract dumb-0.9.x.zip.
Start the regular windows shell, and run:
  >cd \path\to\dumb-0.9.x
  >set MINGDIR="c:\path\to\MinGW"
  >mingw32-make
  >mingw32-make install
If you do not want to put mingw in your PATH globally, this also works:
  >cd \path\to\dumb-0.9.x
  >PATH="c:\path\to\MinGW\bin";%PATH%
  >set MINGDIR="c:\path\to\MinGW"
  >mingw32-make CC="c:\\path\\to\\MinGW\\bin\\gcc.exe"
  >mingw32-make install

6. KQ
If you build from CVS, run "autoreconf -i".
That creates configure from configure.ac and Makefile.in from Makefile.am.
Run "./configure", then "make". "./configure --help" gives some possible options.

How to cross-compile KQ for Windows from Linux
=====================================
1) Download the KQ source. You can download the latest STABLE version from here:
http://sourceforge.net/project/showfiles.php?group_id=61344, or you can download
from CVS. Instructions for downloading from CVS are here:
http://sourceforge.net/cvs/?group_id=61344. Note that modulename is KQ.

Unpack the KQ source (unless you downloaded from CVS). These instructions assume
that your kq directory is ~/KQ. Adjust accordingly if this is not the case.

2) Download the Windows dependencies from here:
http://sourceforge.net/project/showfiles.php?group_id=61344. At the time of this
writing, the only version is 20080129. I have tested this version of kq-win32-depends
with the KQ version 20071026, and CVS as of January. Unpack the kq-win32-depends.zip
file. These instructions assumed that you unpacked directly into your home directory,
and the include directory is in ~/kq-win32-depends/include, and the lib directory is
in ~/kq-win32-depends/lib.

3) Install the development files for allegro, lua 5.0 or 5.1, and DUMB. These libraries
should be included with your Linux distribution. If you use Debian (and probably Ubuntu),
you can run this command (as root):
aptitude install liballegro4.2-dev liblua50-dev liblualib50-dev lua50 libdumb1-dev

4) Install mingw and nsis (Nullsoft Installation Scripter). If you use Debian, you can
install these programs with this command (as root):
aptitude install mingw32 nsis

5) Now edit the file ~/KQ/release/make-release.sh. You need to edit the variables WIN_INC,
WIN_LIB, and WIN_LIB_DIR, which are near the top of the file. If you unpacked
kq-win32-depends into your home directory, then these variables should read like so:
WIN_INC=-I$HOME/kq-win32-depends/include
WIN_LIB=-L$HOME/kq-win32-depends/lib
WIN_LIB_DIR=$HOME/kq-win32-depends/lib

You may want to edit the variable TMP as well.

6) Now, run:
cd ~/KQ/release/
./make-release.sh <version> win

If you don't plan to distribute this executable, name <version> whatever you want. Perhaps
your name. If you do plan to distribute, I recommend using the current date in YYYYMMDD
format.

If everything went according to plan, and it rarely does, you should have a Windows
installation executable in your TMP directory (which is /tmp by default).

TROUBLESHOOTING
===============
If you had trouble compiling, please let us know on the kqlives-main mailing list.
The address is kqlives-main@lists.sourceforge.net. You can (un)subscribe here:
http://sourceforge.net/mail/?group_id=61344
