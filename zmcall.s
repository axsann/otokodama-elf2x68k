*	zmcall.s	ZMUSIC ver2.x �p�֐�

	.include	IOCSCALL.MAC
	.include	ZMSCCALL.MAC	* ZMUSIC ver2.0 �{�ɕt���̂���

	.xdef	_ZmKeepCheck,_ZmInit,_ZmPlay,_ZmStop
	.xdef	_ZmAdpcmSE,_ZmFadeOut


_ZmKeepCheck:		* ZMUSIC ���풓���Ă��邩���ׂ�
			* out : d0.l = 0 �풓���Ă���
			*	     < 0  �V ���Ă��Ȃ�
	movem.l	d1/a0-a1,-(sp)

	suba.l	a1,a1
	IOCS	_B_SUPER
	move.l	d0,-(sp)		**

	moveq.l	#0,d1		* d1.l = �Ԃ�l
	move.l	$8c.w,a0
	subq.w	#8,a0
	cmpi.l	#'ZmuS',(a0)+
	bne	1f
	cmpi.w  #'iC',(a0)+
	beq	2f
1:	moveq.l #-1,d1
2:
	move.l	(sp)+,d0		**
	bmi	@f			* ���ɃX�[�p�[������
	movea.l	d0,a1
	IOCS	_B_SUPER
@@:

	move.l	d1,d0
	movem.l	(sp)+,d1/a0-a1
	rts



*********************************************************
_ZmInit:		* �����^�h���C�o�̏�����
	Z_MUSIC	m_init
	rts



*********************************************************
_ZmPlay:		* .ZMD �����t����
			* in  :  8(a6) = �f�[�^�̃A�h���X
			*		�ׂ��ǂ݂���.ZMD �̃A�h���X�i�����A�h���X�j���w��
			*	12(a6) = �f�[�^�̃T�C�Y
			* out : d0.l = �G���[�R�[�h
	link	a6,#0
	movem.l	d1-d2/a0-a1,-(sp)

	movea.l	8(a6),a1	* �f�[�^�̃A�h���X
	adda.w	#7,a1		* �w�b�_���΂�
	move.l	12(a6),d1	* �f�[�^�̃T�C�Y
	moveq.l	#0,d2		* ��������
	Z_MUSIC	play_cnv_data

	movem.l	(sp)+,d1-d2/a0-a1
	unlk	a6
	rts



*********************************************************
_ZmStop:		* ���t���~����
	movem.l	d1-d4,-(sp)

	moveq.l	#0,d2
	moveq.l	#0,d3
	moveq.l	#0,d4
	Z_MUSIC	m_stop

	movem.l	(sp)+,d1-d4
	rts



*********************************************************
_ZmAdpcmSE:		* ADPCM ���ʉ��̍Đ�
			* in  :  8(a6) = ADPCM �f�[�^�ւ̃|�C���^
			*	12(a6) = ADPCM �f�[�^�̃T�C�Y
			*	16(a6) = ���g��*256+PAN
			*	20(a6) = �D�揇��
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
_ZmFadeOut:		* �t�F�[�h�A�E�g
			* in  : 8(a6) = �t�F�[�h�A�E�g���x(1~85)
	link	a6,#0
	movem.l	d1-d2,-(sp)

	move.l	8(a6),d2
	Z_MUSIC	fade_out

	move.l	d1,d0
	movem.l	(sp)+,d1-d2
	unlk	a6
	rts
