SUBDIRS = intl po scripts
bin_PROGRAMS = kq mapdraw mapdiff mapdump $(MAPDRAW2)
EXTRA_PROGRAMS = mapdraw2

kq_SOURCES = include/bounds.h include/combat.h include/console.h \
	include/credits.h include/disk.h include/draw.h include/effects.h \
	include/enemyc.h include/entity.h include/enums.h include/eqpmenu.h \
	include/eskill.h include/fade.h include/gettext.h include/heroc.h \
	include/hskill.h include/intrface.h include/itemdefs.h include/itemmenu.h \
	include/kq.h include/kqsnd.h include/magic.h include/markers.h \
	include/masmenu.h include/menu.h include/movement.h include/mpcx.h \
	include/music.h include/platform.h include/res.h include/scrnshot.h \
	include/selector.h include/setup.h include/sgame.h include/shopmenu.h \
	include/skills.h include/ssprites.h include/structs.h include/timing.h \
	src/bounds.c src/combat.c src/console.c src/credits.c src/disk.c \
	src/draw.c src/effects.c src/enemyc.c src/entity.c src/eqpmenu.c \
	src/eskill.c src/fade.c src/heroc.c src/hskill.c src/intrface.c \
	src/itemmenu.c src/kq.c src/magic.c src/markers.c src/masmenu.c src/menu.c \
	src/movement.c src/music.c src/res.c src/scrnshot.c src/selector.c \
	src/setup.c src/sgame.c src/shopmenu.c src/timing.c

EXTRA_kq_SOURCES = src/beos.c src/dos.c src/osx.m src/unix.c src/win.c \
	src/winres.rc
kq_LDADD = $(kq_extra_files) $(ALLEGRO_LIBS) $(LIBINTL)
kq_DEPENDENCIES = $(kq_extra_files)

mapdraw_SOURCES = include/disk.h include/structs.h maps/mapdata.c \
	maps/mapdraw.h maps/mapedit.c maps/mapent.c maps/mapfile.c \
	maps/mapshared.c maps/mapstructs.c src/bounds.c src/disk.c src/markers.c
mapdraw_LDADD = $(ALLEGRO_LIBS) $(LIBINTL)

mapdraw2_SOURCES = include/disk.h include/structs.h maps/mainwindow.c \
	maps/mapdraw.h maps/mapdraw2.c maps/mapdraw2.h maps/mapedit2.c \
	maps/mapedit2.h maps/mapshared.c src/bounds.c src/disk.c src/markers.c
mapdraw2_LDADD = $(GTK_LIBS) $(ALLEGRO_LIBS) $(LIBINTL)
mapdraw2_CPPFLAGS = $(AM_CPPFLAGS) $(GTK_CFLAGS)

mapdiff_SOURCES = include/bounds.h include/disk.h include/structs.h \
	maps/mapdiff.c maps/mapdraw.h src/bounds.c src/disk.c src/markers.c
mapdiff_LDADD = $(ALLEGRO_LIBS) $(LIBINTL)

mapdump_SOURCES = include/bounds.h include/disk.h include/structs.h \
	maps/mapdata.c maps/mapdump.c maps/mapshared.c src/bounds.c src/disk.c \
	src/markers.c
mapdump_LDADD = $(ALLEGRO_LIBS) $(LIBINTL)

APPR=KQ.app/Contents/Resources
KQ.app: kq
	install -d $(APPR)/data $(APPR)/music $(APPR)/scripts $(APPR)/maps
	install -d KQ.app/Contents/MacOS
	install -C $(top_srcdir)/Mac/Info.plist KQ.app/Contents
	install -C kq KQ.app/Contents/MacOS
	install -C $(addprefix $(top_srcdir)/,$(dist_gfx_DATA)) $(APPR)/data
	install -C $(addprefix $(top_srcdir)/,$(dist_map_DATA)) $(APPR)/maps
	install -C $(addprefix $(top_srcdir)/,$(dist_music_DATA)) $(APPR)/music
	install -C $(top_srcdir)/Mac/KQ-icon.icns $(APPR)/KQ.icns
	$(MAKE) -C scripts $@
	touch KQ.app

winres.o: winres.rc
	windres $< -o $@

dist_map_DATA = maps/bridge.map maps/bridge2.map maps/camp.map maps/cave1.map \
	maps/cave2.map maps/cave3a.map maps/cave3b.map maps/cave4.map \
	maps/cave5.map maps/cave6a.map maps/cave6b.map maps/coliseum.map \
	maps/cult.map maps/dville.map maps/estate.map maps/fort.map \
	maps/goblin.map maps/grotto.map maps/grotto2.map maps/guild.map \
	maps/main.map maps/manor.map maps/pass.map maps/starting.map \
	maps/sunarin.map maps/temple1.map maps/temple2.map maps/tower.map \
	maps/town1.map maps/town2.map maps/town3.map maps/town4.map maps/town5.map \
	maps/town6.map maps/town7.map maps/town8.map

mapdir = $(pkgdatadir)/maps

EXTRA_DIST = config.rpath Mac/Info.plist Mac/KQ-icon.icns \
	maps/mapdraw2.glade other/kq.ico other/luaxgettext

dist_gfx_DATA = data/allstat.mon data/hero.kq data/kqsnd.dat \
	data/mpcx.dat data/resabil.mon data/ssprites.dat data/tileset.kq

gfxdir = $(pkgdatadir)/data

dist_music_DATA = music/aa_arofl.xm music/comeback.mod music/dag_4.xm \
	music/enfero.xm music/eranasp.mod music/exp.s3m music/hm.s3m \
	music/infanita.mod music/into61.s3m music/land.mod music/oxford.s3m \
	music/rain.s3m music/rend5.s3m music/toroia.s3m music/town.mod \
	music/walk.s3m music/waterw.xm

musicdir = $(pkgdatadir)/music

dist_man_MANS = debian/kq.6

AM_CPPFLAGS = $(WARNING_CFLAGS) -I$(top_srcdir)/include \
	-DKQ_BIN=\"$(bindir)/kq$(EXEEXT)\" \
	-DKQ_DATA=\"$(pkgdatadir)\" \
	-DKQ_LIB=\"$(pkglibdir)\" \
	-DKQ_LOCALE=\"$(localedir)\" \
	-Iintl -I$(top_srcdir)/intl

if BUNDLE
all-local: KQ.app
endif BUNDLE

ACLOCAL_AMFLAGS = -I m4
