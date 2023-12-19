all: blahaj.cuddleimg keymap/de keymap/us fonts/Bm437_IBM_VGA_8x16.cuddlefont fonts/ter-u16n.cuddlefont

%.cuddleimg: %.png
	make -C ../util/png2cuddleimg
	../util/png2cuddleimg/png2cuddleimg $< > $@

fonts/%.cuddlefont: fonts/otb/%.otb
	make -C ../util/ttf2cuddlefont
	../util/ttf2cuddlefont/ttf2cuddlefont $< > $@

keymap/%:
	make -C ../util/dump2cuddlekeys
	doas loadkeys -C/dev/tty1 $*
	dumpkeys -C/dev/tty1 | ../util/dump2cuddlekeys/dump2cuddlekeys > $@
