# Check for Objective-C compiler.
# For now we only handle the GNU compiler.

# Copyright (C) 2004 Sir Raorn <raorn@altlinux.ru>

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

AC_DEFUN([AM_PROG_OBJC],[
AC_CHECK_PROGS(OBJC, $CC gcc, gcc)
test -z "$OBJC" && AC_MSG_ERROR([no acceptable gcc found in \$PATH])
if test "x${OBJCFLAGS-unset}" = xunset; then
   OBJCFLAGS="-g -O2"
fi
AC_SUBST(OBJCFLAGS)
_AM_IF_OPTION([no-dependencies],, [_AM_DEPENDENCIES(OBJC)])
])
