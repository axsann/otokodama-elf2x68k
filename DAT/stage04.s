*	stage04.s

	.include	names.inc

	.data
	.even
stage04:
		* type,x,y,arg,ctrl,time
	.dc.w	ENEMY_START,0,0,0,999,110		* 0
	.dc.w	ENEMY_MUSIC,0,0,MUSIC_STAGE4,999,110

.if	1
			* SWORD 登場
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12

	.dc.w	ENEMY_SWORD,220,-64,0,999,220
	.dc.w	ENEMY_SWORD,64,-64,0,5,1
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,120

	.dc.w	ENEMY_SWORD,128,-64,0,999,220
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12

	.dc.w	ENEMY_SWORD,228,-64,0,999,220
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12

	.dc.w	ENEMY_SWORD,160,-48,0,15,120
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12

	.dc.w	ENEMY_SWORD,108,-48,0,15,120
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12

	.dc.w	ENEMY_SWORD,48,-48,0,15,120
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12

	.dc.w	ENEMY_SWORD,96,-48,0,15,120
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12

	.dc.w	ENEMY_SWORD,160,-48,0,5,120
	.dc.w	ENEMY_SEARCHZ,32,-16,1,15,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,15,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,15,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,15,12
	.dc.w	ENEMY_SEARCHZ,32,-16,1,15,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,5,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,5,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,5,120

	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,32,-16,1,999,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,999,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,999,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,999,120

	.dc.w	ENEMY_SEARCHZ,32,-16,1,5,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,5,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,5,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,5,12
	.dc.w	ENEMY_SEARCHZ,32,-16,1,5,12
	.dc.w	ENEMY_SEARCHZ,64,0,1,5,12
	.dc.w	ENEMY_SEARCHZ,72,-32,1,5,12
	.dc.w	ENEMY_SEARCHZ,80,0,1,5,120
.endif

	.dc.w	ENEMY_MUSIC_FADEOUT,0,0,64,999,120

.if	1
			* ボスラッシュ
	.dc.w	ENEMY_MUSIC,0,0,MUSIC_BOSS,999,12
	.dc.w	ENEMY_LBOSS01,144,-32,1,999,60
	.dc.w	ENEMY_LBOSS02,144,-32,1,999,60
	.dc.w	ENEMY_LBOSS03,144,-32,1,0,220
.endif

.if	1
			* ザコラッシュ
	.rept	5
	.dc.w	ENEMY_ZAKO033,32*0+16,-32,0,16,15
	.dc.w	ENEMY_ZAKO033,32*3+16,-32,0,16,15
	.dc.w	ENEMY_ZAKO033,32*4+16,-32,0,16,15
	.dc.w	ENEMY_ZAKO033,32*1+16,-32,0,16,15
	.dc.w	ENEMY_ZAKO033,32*5+16,-32,0,16,15
	.dc.w	ENEMY_ZAKO033,32*6+16,-32,0,16,15
	.dc.w	ENEMY_ZAKO033,32*7+16,-32,0,16,15
	.dc.w	ENEMY_ZAKO033,32*2+16,-32,0,16,15

	.dc.w	ENEMY_ZAKO033,32*6+16,-32,0,16,15
	.dc.w	ENEMY_ZAKO033,32*4+16,-32,0,16,15
	.dc.w	ENEMY_ZAKO033,32*1+16,-32,0,16,15
	.dc.w	ENEMY_ZAKO033,32*7+16,-32,0,16,15
	.dc.w	ENEMY_ZAKO033,32*0+16,-32,0,16,15
	.dc.w	ENEMY_ZAKO033,32*5+16,-32,0,16,15
	.dc.w	ENEMY_ZAKO033,32*2+16,-32,0,16,15
	.dc.w	ENEMY_ZAKO033,32*3+16,-32,0,16,15
	.endm

	.dc.w	ENEMY_ZAKO033,32*0+16,-32,0,16,25
	.dc.w	ENEMY_ZAKO033,32*3+16,-32,0,16,25
	.dc.w	ENEMY_ZAKO033,32*4+16,-32,0,16,25
	.dc.w	ENEMY_ZAKO033,32*1+16,-32,0,16,25
	.dc.w	ENEMY_ZAKO033,32*5+16,-32,0,16,35
	.dc.w	ENEMY_ZAKO033,32*6+16,-32,0,16,35
	.dc.w	ENEMY_ZAKO033,32*7+16,-32,0,16,35
	.dc.w	ENEMY_ZAKO033,32*2+16,-32,0,16,35

			* Ｘ座標は zako032.c 内で決まる
	.rept	6
	.dc.w	ENEMY_ZAKO032,-1,40,0,16,15
	.dc.w	ENEMY_ZAKO032,16,-32,0,16,15
	.dc.w	ENEMY_ZAKO032,-1,24,0,16,15
	.dc.w	ENEMY_ZAKO032,32,-32,0,16,15
	.dc.w	ENEMY_ZAKO032,-1,48,0,16,15
	.dc.w	ENEMY_ZAKO032,80,-32,0,16,15
	.dc.w	ENEMY_ZAKO032,-1,56,0,16,15
	.dc.w	ENEMY_ZAKO032,48,-32,0,16,15
	.dc.w	ENEMY_ZAKO032,-1,32,0,16,15
	.dc.w	ENEMY_ZAKO032,32,-32,0,16,15
	.dc.w	ENEMY_ZAKO032,-1,16,0,16,15
	.dc.w	ENEMY_ZAKO032,112,-32,0,16,15
	.dc.w	ENEMY_ZAKO032,-1,40,0,16,15
	.dc.w	ENEMY_ZAKO032,16,-32,0,16,15
	.dc.w	ENEMY_ZAKO032,-1,24,0,16,15
	.dc.w	ENEMY_ZAKO032,32,-32,0,16,15
	.dc.w	ENEMY_ZAKO032,-1,48,0,16,15
	.dc.w	ENEMY_ZAKO032,80,-32,0,16,15
	.dc.w	ENEMY_ZAKO032,-1,56,0,16,15
	.dc.w	ENEMY_ZAKO032,48,-32,0,16,15
	.dc.w	ENEMY_ZAKO032,-1,32,0,16,15
	.dc.w	ENEMY_ZAKO032,32,-32,0,16,15
	.dc.w	ENEMY_ZAKO032,-1,16,0,16,15
	.dc.w	ENEMY_ZAKO032,112,-32,0,16,15
	.endm

	.dc.w	ENEMY_ZAKO032,112,-32,0,16,25
	.dc.w	ENEMY_ZAKO032,-1,40,0,16,25
	.dc.w	ENEMY_ZAKO032,16,-32,0,5,25
	.dc.w	ENEMY_ZAKO032,-1,24,0,5,35
	.dc.w	ENEMY_ZAKO032,32,-32,0,3,35
	.dc.w	ENEMY_ZAKO032,-1,48,0,3,35
	.dc.w	ENEMY_ZAKO032,80,-32,0,3,220
.endif

	.dc.w	ENEMY_MUSIC_FADEOUT,0,0,64,999,160

.if	1
			* ラスボス出現
	.dc.w	ENEMY_MUSIC,0,0,MUSIC_LBOSS,999,145
	.dc.w	ENEMY_LBOSS04,144,256+64,0,999,60
	.dc.w	ENEMY_MUSIC_FADEOUT,0,0,64,999,160
.endif
			* ボスコア出現
	.dc.w	ENEMY_MUSIC,0,0,MUSIC_LBOSS,999,1
	.dc.w	ENEMY_LBOSS04C,144,256+48,0,999,1
	.dc.w	ENEMY_MUSIC_FADEOUT,0,0,64,999,55		* 60
	.dc.w	ENEMY_END,0,0,0,999,32
	.dc.w	ENEMY_NON,0,0,0,999,32767
	.dc.w	ENEMY_NON,0,0,0,999,32767
	.dc.w	ENEMY_NON,0,0,0,999,32767
