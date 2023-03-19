#	makefile for disent.x

CC	= gcc
CFLAGS	=
AS	= has
LD	= hlk
INC	=
LIBS	= libc.a libgnu.a
LZH	= disent100

%.o:	%.c
	$(CC) $(CFLAGS) -c $<

all:	disent.x stage01.dat stage02.dat stage03.dat stage04.dat
disent.x:	disent.o
	$(LD) $^ -o $@ -l $(LIBS)

disent.o:	disent.c ../names.h

stage01.dat:	stage01.s names.inc
	has stage01.s ; hlk stage01.o ; hcv stage01.x ; rm stage01.o stage01.x ; mv stage01.r stage01.dat
stage02.dat:	stage02.s names.inc
	has stage02.s ; hlk stage02.o ; hcv stage02.x ; rm stage02.o stage02.x ; mv stage02.r stage02.dat
stage03.dat:	stage03.s names.inc
	has stage03.s ; hlk stage03.o ; hcv stage03.x ; rm stage03.o stage03.x ; mv stage03.r stage03.dat
stage04.dat:	stage04.s names.inc
	has stage04.s ; hlk stage04.o ; hcv stage04.x ; rm stage04.o stage04.x ; mv stage04.r stage04.dat

dist:
	LHA a -t ../$(LZH)

