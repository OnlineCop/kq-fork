# -*- makefile -*-
# make the map editor
# makefile by PH
DISK_H=../include/structs.h ../include/disk.h

LDFLAGS :=
LDFLAGS += -lalld

# CFLAGS+=-I../include

all: mapdraw$(EXE_SUFFIX) mapdump$(EXE_SUFFIX) mapdiff$(EXE_SUFFIX)

mapdraw$(EXE_SUFFIX): bounds.o disk.o mapdata.o mapedit.o mapent.o \
	mapfile.o mapshared.o mapstructs.o
	$(LINK.o) $^ -o $@ $(LDFLAGS)

disk.o: ../src/disk.c $(DISK_H)
	$(COMPILE.c)  $< -o $@

bounds.o: ../src/bounds.c $(DISK_H)
	$(COMPILE.c)  $< -o $@

mapedit.o: mapedit.c mapdraw.h $(DISK_H)
mapfile.o: mapfile.c mapdraw.h $(DISK_H)
mapshared.o: mapshared.c mapdraw.h $(DISK_H)
mapent.o:  mapent.c  mapdraw.h $(DISK_H)
mapdata.o: mapdata.c mapdraw.h $(DISK_H)


mapdump$(EXE_SUFFIX): mapdump.o mapshared.o mapdata.o disk.o
	$(LINK.o) $^ -o $@ $(LDFLAGS)

mapdiff$(EXE_SUFFIX): mapdiff.o disk.o
	$(LINK.o) $^ -o $@ $(LDFLAGS)

mapdump.o: mapdump.c mapdraw.h $(DISK_H)
mapdiff.o: mapdiff.c mapdraw.h $(DISK_H)
