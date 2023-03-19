#	makefile for otoko.x

CC	= gcc
#CC	= 060loadhigh gcc
#CFLAGS	=
CFLAGS	= -O -fomit-frame-pointer -fstrength-reduce
AS	= has
LD	= hlk
INC	=
LIBS	= xsp2lib.o libc.a libgnu.a libdos.a libiocs.a pcm8afnc.o apicglib.a
LZH	= otoko100

vpath	%.c	./;FuncEnemy/;FuncEffect/;
# .c ファイルはカレント又はここで指定したディレクトリに

%.o:	%.c
	$(CC) $(CFLAGS) -c $<

all:	otoko.x maketbl.x

clean:
	rm -f *.bak *.o *.x otoko.ind otoko.dat TBL/*.TBL \
	FuncEnemy/*.bak FuncEffect/*.bak \
	DAT/*.bak DAT/*.o DAT/*.x \
	sp/*.bak

execonly:
	rm -f *.bak *.o *.c *.h *.s makefile otoko.ind \
	FuncEnemy/*.* FuncEffect/*.* \
	BG/STAGE0?.PAL SOUND/*.ZMS \
	DAT/*.c DAT/*.s DAT/*.inc DAT/*.o DAT/*.x DAT/makefile \
	SP/*.bak ; \
	rmdir FuncEnemy FuncEffect

otoko.x:	otoko.o player.o shot.o \
		enemy.o \
		boss02.o boss03.o coverf.o covern.o covern2.o \
		lboss01.o lboss01c.o \
		lboss02.o lboss02e.o lboss02h.o lboss02w.o \
		lboss03.o lboss03g.o lboss03p.o lboss03u.o lboss03z.o \
		lboss04.o lboss04c.o lboss04e.o lboss04s.o lboss04v.o lboss04w.o \
		mvlaser.o mvlaserh.o oplaser.o oplinear.o round4.o \
		searchz.o sword.o sword2.o \
		zako02.o zako031.o zako032.o zako033.o zako04.o zako041.o \
		eshot.o \
		effect.o \
		bomber.o backb.o blaser.o expl.o explzako.o explmini.o explplayer.o \
		bombstr.o hahen.o hahenmini.o pointsred.o pointsblue.o points.o \
		titspark.o titmoji.o lastlaser.o lastplayer.o \
		entry.o psearch.o \
		gvram.o txfont.o sound.o zmcall.o
#	$(LD) $^ -o $@ -l $(LIBS)
# fish.xの方は上の１行を、command.xの方は下の２行を有効にして下さい
	echo $^ > otoko.ind
	$(LD) -i otoko.ind -o $@ -l $(LIBS)

maketbl.x:	maketbl.o
	$(LD) $^ -o $@ -l $(LIBS)

otoko.o:	otoko.c otoko.h player.h shot.h enemy.h eshot.h effect.h entry.h psearch.h txfont.h gvram.h sound.h
player.o:	player.c otoko.h player.h shot.h eshot.h names.h effect.h entry.h priority.h txfont.h sound.h damage.h
shot.o:		shot.c otoko.h player.h shot.h priority.h

enemy.o:	enemy.c	otoko.h player.h shot.h enemy.h psearch.h damage.h

boss02.o:	FuncEnemy/boss02.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
boss03.o:	FuncEnemy/boss03.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
coverf.o:	FuncEnemy/coverf.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
covern.o:	FuncEnemy/covern.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
covern2.o:	FuncEnemy/covern2.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss01.o:	FuncEnemy/lboss01.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss01c.o:	FuncEnemy/lboss01c.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss02.o:	FuncEnemy/lboss02.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss02e.o:	FuncEnemy/lboss02e.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss02h.o:	FuncEnemy/lboss02h.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss02w.o:	FuncEnemy/lboss02w.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss03.o:	FuncEnemy/lboss03.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss03g.o:	FuncEnemy/lboss03g.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss03p.o:	FuncEnemy/lboss03p.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss03u.o:	FuncEnemy/lboss03u.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss03z.o:	FuncEnemy/lboss03z.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss04.o:	FuncEnemy/lboss04.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss04c.o:	FuncEnemy/lboss04c.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss04e.o:	FuncEnemy/lboss04e.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss04s.o:	FuncEnemy/lboss04s.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss04v.o:	FuncEnemy/lboss04v.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
lboss04w.o:	FuncEnemy/lboss04w.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
mvlaser.o:	FuncEnemy/mvlaser.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
mvlaserh.o:	FuncEnemy/mvlaserh.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
oplaser.o:	FuncEnemy/oplaser.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
oplinear.o:	FuncEnemy/oplinear.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
round4.o:	FuncEnemy/round4.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
searchz.o:	FuncEnemy/searchz.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
sword.o:	FuncEnemy/sword.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
sword2.o:	FuncEnemy/sword2.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
zako02.o:	FuncEnemy/zako02.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
zako031.o:	FuncEnemy/zako031.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
zako032.o:	FuncEnemy/zako032.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
zako033.o:	FuncEnemy/zako033.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
zako04.o:	FuncEnemy/zako04.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h
zako041.o:	FuncEnemy/zako041.c	otoko.h player.h enemy.h eshot.h effect.h entry.h priority.h sound.h psearch.h

eshot.o:	eshot.c	otoko.h player.h eshot.h priority.h

effect.o:	effect.c effect.h

backb.o:	FuncEffect/backb.c	otoko.h effect.h priority.h
blaser.o:	FuncEffect/blaser.c	otoko.h player.h effect.h priority.h damage.h
bomber.o:	FuncEffect/bomber.c	otoko.h effect.h priority.h
bombstr.o:	FuncEffect/bombstr.c	otoko.h effect.h player.h priority.h
expl.o:		FuncEffect/expl.c	otoko.h effect.h priority.h
explzako.o:	FuncEffect/explzako.c	otoko.h effect.h priority.h
explmini.o:	FuncEffect/explmini.c	otoko.h effect.h priority.h
explplayer.o:	FuncEffect/explplayer.c	otoko.h effect.h priority.h
hahen.o:	FuncEffect/hahen.c	otoko.h effect.h priority.h
hahenmini.o:	FuncEffect/hahenmini.c	otoko.h effect.h priority.h
points.o:	FuncEffect/points.c	otoko.h effect.h priority.h
pointsblue.o:	FuncEffect/pointsblue.c	otoko.h effect.h priority.h
pointsred.o:	FuncEffect/pointsred.c	otoko.h effect.h priority.h
titmoji.o:	FuncEffect/titmoji.c	otoko.h effect.h priority.h
titspark.o:	FuncEffect/titspark.c	otoko.h effect.h priority.h
lastlaser.o:	FuncEffect/lastlaser.c	otoko.h player.h effect.h priority.h
lastplayer.o:	FuncEffect/lastplayer.c	otoko.h player.h effect.h priority.h

entry.o:	entry.c otoko.h player.h enemy.h entry.h gvram.h sound.h
psearch.o:	psearch.c player.h psearch.h

gvram.o:	gvram.s
txfont.o:	txfont.c txfont.h
sound.o:	sound.c otoko.h zmcall.h
zmcall.o:	zmcall.s


maketbl.o:	maketbl.c


dist:
	LHA a -t ../$(LZH)

