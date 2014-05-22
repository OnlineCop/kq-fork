# -*- makefile -*-
# make the map editor
# makefile by PH
DISK_H=../include/structs.h ../include/disk.h

LDFLAGS += -lalleg

CFLAGS+=-I../include

all: mapdraw$(EXE_SUFFIX) mapdump$(EXE_SUFFIX) mapdiff$(EXE_SUFFIX)

mapdraw$(EXE_SUFFIX): bounds.o markers.o disk.o mapdata.o mapedit.o mapent.o \
	mapfile.o mapshared.o mapstructs.o
	$(LINK.o) $^ -o $@ $(LDFLAGS)

disk.o: ../src/disk.cpp $(DISK_H)
	$(COMPILE.c)  $< -o $@

bounds.o: ../src/bounds.cpp $(DISK_H)
	$(COMPILE.c)  $< -o $@

markers.o: ../src/markers.cpp $(DISK_H)
	$(COMPILE.c)  $< -o $@

mapedit.o: mapedit.cpp mapdraw.h $(DISK_H)
mapfile.o: mapfile.cpp mapdraw.h $(DISK_H)
mapshared.o: mapshared.cpp mapdraw.h $(DISK_H)
mapent.o:  mapent.cpp  mapdraw.h $(DISK_H)
mapdata.o: mapdata.cpp mapdraw.h $(DISK_H)


mapdump$(EXE_SUFFIX): mapdump.o bounds.o markers.o mapshared.o mapdata.o disk.o
	$(LINK.o) $^ -o $@ $(LDFLAGS)

mapdiff$(EXE_SUFFIX): mapdiff.o bounds.o markers.o disk.o
	$(LINK.o) $^ -o $@ $(LDFLAGS)

mapdump.o: mapdump.cpp mapdraw.h $(DISK_H)
mapdiff.o: mapdiff.cpp mapdraw.h $(DISK_H)
