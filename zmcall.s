*	zmcall.s	ZMUSIC ver2.x 用関数

	.include	IOCSCALL.MAC
	.include	ZMSCCALL.MAC	* ZMUSIC ver2.0 本に付属のもの

	.xdef	_ZmKeepCheck,_ZmInit,_ZmPlay,_ZmStop
	.xdef	_ZmAdpcmSE,_ZmFadeOut


_ZmKeepCheck:		* ZMUSIC が常駐しているか調べる
			* out : d0.l = 0 常駐している
			*	     < 0  〃 していない
	movem.l	d1/a0-a1,-(sp)

	suba.l	a1,a1
	IOCS	_B_SUPER
	move.l	d0,-(sp)		**

	moveq.l	#0,d1		* d1.l = 返り値
	move.l	$8c.w,a0
	subq.w	#8,a0
	cmpi.l	#'ZmuS',(a0)+
	bne	1f
	cmpi.w  #'iC',(a0)+
	beq	2f
1:	moveq.l #-1,d1
2:
	move.l	(sp)+,d0		**
	bmi	@f			* 既にスーパーだった
	movea.l	d0,a1
	IOCS	_B_SUPER
@@:

	move.l	d1,d0
	movem.l	(sp)+,d1/a0-a1
	rts



*********************************************************
_ZmInit:		* 音源／ドライバの初期化
	Z_MUSIC	m_init
	rts



*********************************************************
_ZmPlay:		* .ZMD を演奏する
			* in  :  8(a6) = データのアドレス
			*		べた読みした.ZMD のアドレス（偶数アドレス）を指定
			*	12(a6) = データのサイズ
			* out : d0.l = エラーコード
	link	a6,#0
	movem.l	d1-d2/a0-a1,-(sp)

	movea.l	8(a6),a1	* データのアドレス
	adda.w	#7,a1		* ヘッダを飛ばす
	move.l	12(a6),d1	* データのサイズ
	moveq.l	#0,d2		* 高速応答
	Z_MUSIC	play_cnv_data

	movem.l	(sp)+,d1-d2/a0-a1
	unlk	a6
	rts



*********************************************************
_ZmStop:		* 演奏を停止する
	movem.l	d1-d4,-(sp)

	moveq.l	#0,d2
	moveq.l	#0,d3
	moveq.l	#0,d4
	Z_MUSIC	m_stop

	movem.l	(sp)+,d1-d4
	rts



*********************************************************
_ZmAdpcmSE:		* ADPCM 効果音の再生
			* in  :  8(a6) = ADPCM データへのポインタ
			*	12(a6) = ADPCM データのサイズ
			*	16(a6) = 周波数*256+PAN
			*	20(a6) = 優先順位
	link	a6,#0
	movem.l	d1-d3/a0-a1,-(sp)

	movea.l	8(a6),a1
	move.l	12(a6),d2
	move.w	20+2(a6),d3
	swap.w	d3
	move.w	16+2(a6),d3
	Z_MUSIC	se_adpcm1

	movem.l	(sp)+,d1-d3/a0-a1
	unlk	a6
	rts



*********************************************************
_ZmFadeOut:		* フェードアウト
			* in  : 8(a6) = フェードアウト速度(1~85)
	link	a6,#0
	movem.l	d1-d2,-(sp)

	move.l	8(a6),d2
	Z_MUSIC	fade_out

	move.l	d1,d0
	movem.l	(sp)+,d1-d2
	unlk	a6
	rts
