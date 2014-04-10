CC=gcc
CFLAGS=-O3 -W -Wall
LDFLAGS=-s
LDLIBS=`allegro-config --shared` -ljgmod -llua

OBJS=kq.o combat.o draw.o effects.o enemyc.o entity.o eqpmenu.o \
eskill.o heroc.o hskill.o intrface.o itemmenu.o \
magic.o masmenu.o menu.o res.o selector.o setup.o \
sgame.o shopmenu.o \

kq: $(OBJS)
	$(LINK.o) $^ -o $@ $(LDLIBS)



