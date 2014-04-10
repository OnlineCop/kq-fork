# Makefile to compile all .lua into .lob
# They are preprocessed first into .plua and then compiled.  This
# two-step process allows luac to give better error messages, though
# the line numbers will refer to the versions in pscripts rather than
# the original scripts

CPP=cpp -P -undef

.plua.lob:
	luac -o $@ $<

.lua.plua: $(top_srcdir)/include/itemdefs.h
	$(CPP) -include $(top_srcdir)/include/itemdefs.h $< > $@

KQ.app:
	install -C $(cscripts_DATA) ../$@/Contents/Resources/scripts

cscripts_DATA = bridge.lob bridge2.lob camp.lob cave1.lob cave2.lob cave3a.lob \
	cave3b.lob cave4.lob cave5.lob cave6a.lob cave6b.lob coliseum.lob cult.lob \
	dville.lob estate.lob esteria.lob fort.lob global.lob goblin.lob \
	grotto.lob grotto2.lob guild.lob init.lob main.lob manor.lob pass.lob \
	shrine.lob starting.lob sunarin.lob temple1.lob temple2.lob tower.lob \
	town1.lob town2.lob town3.lob town4.lob town5.lob town6.lob town7.lob \
	town8.lob tunnel.lob

MOSTLYCLEANFILES = $(cscripts_DATA)
cscriptsdir = $(pkglibdir)/scripts

EXTRA_DIST = bridge.lua bridge2.lua camp.lua cave1.lua cave2.lua cave3a.lua \
	cave3b.lua cave4.lua cave5.lua cave6a.lua cave6b.lua coliseum.lua cult.lua \
	dville.lua estate.lua esteria.lua fort.lua global.lua goblin.lua \
	grotto.lua grotto2.lua guild.lua init.lua main.lua manor.lua pass.lua \
	shrine.lua starting.lua sunarin.lua temple1.lua temple2.lua tower.lua \
	town1.lua town2.lua town3.lua town4.lua town5.lua town6.lua town7.lua \
	town8.lua tunnel.lua
