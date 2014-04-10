# -*- makefile -*- ############################################################
#                                                                             #
#  This is the BeOS  Makefile for compiling this package. Check also the      #
#  base makefile for more options.                                            #
#                                                                             #
#  ReyBrujo, 2002 (modified from DJGPP version by Peter Hull)                 #
#                                                                             #
#  23-Sep-2002:                                                               #
#     (RB) Added PREPROCESSOR variable.                                       #
#                                                                             #
#  20030912 New makefile for BeOS                                             #
###############################################################################



#                                                                             #
#  Set the compiler and linker. Usually, they are both gcc, gpp or gxx. By    #
#  default, we will compile a C project. If you want to compile C++ sources,  #
#  set COMPILER and LINKER to $(GPP), and the COMPILER_FLAGS to CXXFLAGS.     #
#                                                                             #
COMPILER       := $(GCC)
PREPROCESSOR   := $(GPP) -P -undef
LINKER         := $(GCC)
COMPILER_FLAGS := $(CFLAGS)
OBJDIR         := $(OBJ_DIR)/beos
EXE_SUFFIX     :=
EXEC           := $(BIN_DIR)/$(ID_SHORT)

###############################################################################
#                                                                             #
#                        END OF CONFIGURATION SECTION                         #
#                                                                             #
###############################################################################

# Unix uses allegro-config rather than -lalleg (or whatever)
ifdef DEBUG
ALLEGRO = `allegro-config --static debug`
else
ifdef PROFILE
ALLEGRO  = `allegro-config --static  profile`
else
ALLEGRO  = `allegro-config --static release`
endif
endif

#                                                                             #
#  Convert all libraries needed so that GCC can understand them.              #
#                                                                             #
CFLAGS   += -s -O3 -march=pentium -mcpu=pentium -malign-functions=2 -malign-jumps=2 -malign-loops=2 -ffast-math -funroll-loops -fomit-frame-pointer -fno-pic -I/boot/develop/headers/posix -I/boot/develop/headers/gnu -I/boot/home/config/include -I/boot/home/config/include/allegro
LDFLAGS += $(addprefix -l, $(LIBRARY)) $(ALLEGRO)
FILES+=beos.c

