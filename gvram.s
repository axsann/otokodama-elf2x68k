*	gvram	グラフィック画面に背景を描く

	.include	IOCSCALL.MAC

	.text
	.even

	.xdef	_GvramInit,_GvramMove,_GvramTini
	.xref	_bg_map,_bg_sp,_stage,_bg_contrast_control
	.xref	_bg_contrast_ptr
	.xref	_stage_palet_table



GVRAM		equ	$c0_0000
GRPALET		equ	$e8_2000

BG_DRAWING	equ	1
BG_FADEIN	equ	2

BG_PALETS	equ	64	* BGで使用しているパレット数

	.text
	.even


_GvramInit:		* 各面開始時に呼ばれる
	movem.l	d1/a0-a1,-(sp)

	moveq.l	#0,d1
	move.b	_stage,d1	* d1.l = 面数

	lea.l	_bg_map,a0
	move.w	d1,d0
	mulu.w	#30+32*128*2,d0
	adda.w	d0,a0
	adda.w	#30,a0		* .MAP のヘッダを飛ばす
	move.l	a0,bg_map_work	*

	lea.l	_bg_sp,a0
	move.w	d1,d0
	mulu.w	#32*256,d0
	adda.w	d0,a0
	move.l	a0,bg_sp_work	*

			* otoko.c VdispRoutine() で参照するワークを設定
	lea.l	_stage_palet_table,a0
	move.w	d1,d0
	addq.w	#1,d0
	mulu.w	#33*BG_PALETS*2,d0
	sub.w	#BG_PALETS*2,d0
	adda.l	d0,a0
	move.l	a0,_bg_contrast_ptr	*

	move.w	#BG_DRAWING,_bg_contrast_control

	clr.w	gvram_counter
	move.l	#GVRAM,gvram_work


			* グラフィック画面のパレットをクリアする
	suba.l	a1,a1
	IOCS	_B_SUPER
	move.l	d0,-(sp)		**

	moveq.l	#0,d1
	lea.l	GRPALET,a1
	move.w	#(BG_PALETS+16)/4/2-1,d0
@@:	move.l	d1,(a1)+
	move.l	d1,(a1)+
	move.l	d1,(a1)+
	move.l	d1,(a1)+
	dbra	d0,@b

	movea.l	(sp)+,a1		**
	IOCS	_B_SUPER

	movem.l	(sp)+,d1/a0-a1
	rts



*********************************************************
_GvramMove:
	cmpi.w	#BG_DRAWING,_bg_contrast_control	* 処理の必要はある？
	bne	GvramMove_rts

	movem.l	d1/a1,-(sp)

	suba.l	a1,a1
	IOCS	_B_SUPER
	move.l	d0,-(sp)		**

	move.l	gvram_work,-(sp)
	move.l	bg_map_work,-(sp)
	bsr	putbg32
	addq.w	#8,sp

	addi.l	#32*2,bg_map_work
	move.l	d0,gvram_work

	movea.l	(sp)+,a1		**
	IOCS	_B_SUPER


	move.w	gvram_counter,d0
	addq.w	#1,d0
	cmpi.w	#128,d0		* 描画完了？
	bcs	@f
	move.w	#BG_FADEIN,_bg_contrast_control	* フェードイン開始
	moveq.l	#0,d0
@@:	move.w	d0,gvram_counter

	movem.l	(sp)+,d1/a1
GvramMove_rts:
	rts



****	****
putbg32:		* BGデータを32個（横256ドット分）グラフィック画面に描画する
			* in  : 8(a6).l = MAP データ
			*	12(a6).l = GVRAM
			* out : d0.l = 次のGVRAM
	link	a6,#0
	movem.l	d1-d4/a0-a5,-(sp)

	move.l	8(a6),a1	* a1.l = MAPデータ
	move.l	12(a6),a2	* a2.l = GVRAM

	move.l	bg_sp_work,a5	* a5.l = スプライトデータ（定数）
	moveq.l	#$f,d2		* d2.w = 定数
	moveq.l	#32-1,d4	* 
putbg32_loop:
	move.w	(a1)+,d0	* d0.w = BGデータ
	move.w	d0,d1
	andi.w	#$00ff,d0
	lsl.w	#5,d0		* BG １個当たり32バイトだから32倍
	lea.l	(a5,d0.w),a4	* a4.l = PCGパターン

	move.w	d1,d0
	lsr.w	#3,d0
	andi.w	#$01e0,d0	* d0.w = パレット
	lea.l	bg_table(pc),a3
	adda.w	d0,a3		* a3.l = bg_table にパレットを加算したもの
			* BGのVH反転フラグによって分岐
	btst.l	#15,d1
	bne	putbg32_v
	btst.l	#14,d1
	bne	putbg32_h


			* VH=00
	movea.l	a2,a0		* a0.l =
	moveq.l	#0,d0
	moveq.l	#8-1,d3
@@:
	.rept	4
	move.b	(a4)+,d0
	move.w	d0,d1

	lsr.w	#4,d0
	add.w	d0,d0
	move.w	(a3,d0.w),(a0)+

	and.w	d2,d1
	add.w	d1,d1
	move.w	(a3,d1.w),(a0)+
	.endm

	lea.l	1024-16(a0),a0
	dbra	d3,@b

	bra	putbg32_next


putbg32_v:
	btst.l	#14,d1
	bne	putbg32_vh

			* VH=10
	lea.l	1024*7(a2),a0	* a0.l = 書き込むGVRAM
	moveq.l	#0,d0
	moveq.l	#8-1,d3
@@:
	.rept	4
	move.b	(a4)+,d0
	move.w	d0,d1

	lsr.w	#4,d0
	add.w	d0,d0
	move.w	(a3,d0.w),(a0)+

	and.w	d2,d1
	add.w	d1,d1
	move.w	(a3,d1.w),(a0)+
	.endm

	lea.l	-1024-16(a0),a0
	dbra	d3,@b

	bra	putbg32_next


putbg32_h:		* VH=01
	lea.l	16(a2),a0	* a0.l = 書き込むGVRAM
	moveq.l	#0,d0
	moveq.l	#8-1,d3
@@:
	.rept	4
	move.b	(a4)+,d0
	move.w	d0,d1

	lsr.w	#4,d0
	add.w	d0,d0
	move.w	(a3,d0.w),-(a0)

	and.w	d2,d1
	add.w	d1,d1
	move.w	(a3,d1.w),-(a0)

	.endm

	lea.l	1024+16(a0),a0
	dbra	d3,@b

	bra	putbg32_next


putbg32_vh:		* VH=11
	lea.l	1024*7+16(a2),a0	* a0.l = 書き込むGVRAM
	moveq.l	#0,d0
	moveq.l	#8-1,d3
@@:
	.rept	4
	move.b	(a4)+,d0
	move.w	d0,d1

	lsr.w	#4,d0
	add.w	d0,d0
	move.w	(a3,d0.w),-(a0)

	and.w	d2,d1
	add.w	d1,d1
	move.w	(a3,d1.w),-(a0)
	.endm

	lea.l	-1024+16(a0),a0
	dbra	d3,@b


putbg32_next:
	lea.l	16(a2),a2
	dbra	d4,putbg32_loop

	lea.l	1024*8-512(a2),a2

	move.l	a2,d0
	movem.l	(sp)+,d1-d4/a0-a5
	unlk	a6
	rts


*********************************************************
_GvramTini:
	moveq.l	#0,d0
	rts


*********************************************************
bg_table:		* ゼロプライオリティ対策のためこういうテーブルが必要
	.dc.w	$00,$01,$02,$03,$04,$05,$06,$07,$08,$09,$0a,$0b,$0c,$0d,$0e,$0f
	.dc.w	$00,$11,$12,$13,$14,$15,$16,$17,$18,$19,$1a,$1b,$1c,$1d,$1e,$1f
	.dc.w	$00,$21,$22,$23,$24,$25,$26,$27,$28,$29,$2a,$2b,$2c,$2d,$2e,$2f
	.dc.w	$00,$31,$32,$33,$34,$35,$36,$37,$38,$39,$3a,$3b,$3c,$3d,$3e,$3f
	.dc.w	$00,$41,$42,$43,$44,$45,$46,$47,$48,$49,$4a,$4b,$4c,$4d,$4e,$4f
	.dc.w	$00,$51,$52,$53,$54,$55,$56,$57,$58,$59,$5a,$5b,$5c,$5d,$5e,$5f
	.dc.w	$00,$61,$62,$63,$64,$65,$66,$67,$68,$69,$6a,$6b,$6c,$6d,$6e,$6f
	.dc.w	$00,$71,$72,$73,$74,$75,$76,$77,$78,$79,$7a,$7b,$7c,$7d,$7e,$7f
	.dc.w	$00,$81,$82,$83,$84,$85,$86,$87,$88,$89,$8a,$8b,$8c,$8d,$8e,$8f
	.dc.w	$00,$91,$92,$93,$94,$95,$96,$97,$98,$99,$9a,$9b,$9c,$9d,$9e,$9f
	.dc.w	$00,$a1,$a2,$a3,$a4,$a5,$a6,$a7,$a8,$a9,$aa,$ab,$ac,$ad,$ae,$af
	.dc.w	$00,$b1,$b2,$b3,$b4,$b5,$b6,$b7,$b8,$b9,$ba,$bb,$bc,$bd,$be,$bf
	.dc.w	$00,$c1,$c2,$c3,$c4,$c5,$c6,$c7,$c8,$c9,$ca,$cb,$cc,$cd,$ce,$cf
	.dc.w	$00,$d1,$d2,$d3,$d4,$d5,$d6,$d7,$d8,$d9,$da,$db,$dc,$dd,$de,$df
	.dc.w	$00,$e1,$e2,$e3,$e4,$e5,$e6,$e7,$e8,$e9,$ea,$eb,$ec,$ed,$ee,$ef
	.dc.w	$00,$f1,$f2,$f3,$f4,$f5,$f6,$f7,$f8,$f9,$fa,$fb,$fc,$fd,$fe,$ff
	.even


	.bss
	.even
bg_map_work:
	.ds.l	1
bg_sp_work:
	.ds.l	1
bg_pal_work:
	.ds.l	1
gvram_counter:
	.ds.w	1
gvram_work:
	.ds.l	1

