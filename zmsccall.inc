#
# Original: zmsccall.mac X68k Z-MUSIC v1.09C Copyright 1991,1992 ZENJI SOFT
#
# ���̃t�@�C����zmsccall.mac�����g���[���`������MIT�`���iGAS�t�H�[�}�b�g�j�ɕϊ����A�g���q��inc�ɕύX�������̂ł��B
#

.nolist

.equ m_init,		$00
.equ m_alloc,		$01
.equ m_assign,		$02
.equ m_vget,		$03
.equ m_vset,		$04
.equ m_tempo,		$05
.equ m_trk,			$06
.equ m_free,		$07
.equ m_play,		$08
.equ m_stat,		$09
.equ m_stop,		$0a
.equ m_cont,		$0b
.equ m_atoi,		$0c
.equ init_all,		$0d
.equ init_stop,		$0e
#	reserve
.equ adpcm_read,	$10
.equ play_cnv_data,	$11
.equ se_play,		$12
.equ se_adpcm1,		$13
.equ se_adpcm2,		$14
.equ set_ch_mode,	$15
.equ midi_rec,		$16
.equ midi_rec_end,	$17
.equ midi_trns,		$18
.equ calc_total,	$19
.equ m_fade_out,	$1a
.equ m_vset2,		$1b
.equ send_rd_exc,	$1c
.equ send_exc,		$1d
.equ sc55_p_rsv,	$1e
.equ sc55_reverb,	$1f
.equ sc55_chorus,	$20
.equ sc55_part_parameter,	$21
.equ sc55_drum_parameter,	$22
.equ sc55_print,	$23
.equ sc55_display,	$24
.equ mt32_p_rsv,	$25
.equ mt32_reverb,	$26
.equ mt32_setup,	$27
.equ mt32_drum,		$28
.equ mt32_common,	$29
.equ mt32_partial,	$2a
.equ mt32_patch,	$2b
.equ mt32_print,	$2c
.equ u220_setup,	$2d
.equ u220_common,	$2e
.equ u220_d_setup,	$2f
.equ u220_p_setup,	$30
.equ u220_print,	$31
.equ u220_timbre,	$32
.equ u220_drum,		$33
.equ m1_midi_ch,	$34
.equ send_to_m1,	$35
.equ m1_p_setup,	$36
.equ m1_e_setup,	$37
.equ m1_print,		$38
.equ block_adpcm_data,	$39
.equ get_trk_tbl,		$3a
.equ set_loop_time,		$3b
.equ get_play_work,		$3c
.equ get_timer_mode,	$3d
.equ set_fm_master_vol,	$3e
.equ set_timer_value,	$3f
.equ release_support,	$40
.equ jump_active,		$41
.equ set_mclk,			$42
.equ picture_sync,		$43
.equ mask_channels,		$44
.equ buffer_info,		$45
.equ set_zpd_tbl,		$46
.equ set_output_level,	$47
.equ zm_status,			$50

.macro Z_MUSIC callname
		moveq.l	#\callname,%d1
		trap	#3
.endm

#//////////////////////////////////////////////////////////////////////////////

#���[�N�G���A
.equ wk_size,			256 |# �e�g���b�N�̉��t���̃��[�N�T�C�Y(�ύX�s��)
.equ wk_size2,			8	|# ���[�N�T�C�Y���Q�̉��悩(�ύX�s��)

.equ p_on_count,		$00 |# .w step time		!!! ���Ԃ�ς��Ă͂Ȃ�Ȃ�
.equ p_gate_time,		$02 |# .w gate time		!!!
.equ p_data_pointer,	$04 |# .l ���݂̃R�}���h�|�C���^
.equ p_fo_spd,			$08 |# .b �t�F�[�h�A�E�g�X�s�[�h
.equ p_ch,				$09 |# .b �A�T�C������Ă���`�����l��
.equ p_not_empty,		$0a |# .b �g���b�N�̐���(-1=dead/1=play end/0=alive)
.equ p_amod_step,		$0b |# .b AM�̃X�e�b�v���[�N
.equ p_mstep_tbl,		$0c |# .w �e�|�C���g�ɒu����Ӽޭڰ��݃X�e�b�v�l($0c�`$1b)
.equ p_wvpm_loop,		$0c |# .l �g�`���������[�v�J�n�A�h���X
.equ p_wvpm_lpmd,		$10 |# .w �g�`���������[�v���[�h
.equ p_altp_flg,		$12 |# .b �g�`�������������[�h�t���O
.equ p_fo_mode,			$1c |# .b fade out flag (0=off/1�`127=on)	!!!
.equ p_pgm,				$1d |# .b last tone number(0-199)		!!!
.equ p_pan,				$1e |# .b last panpot(0�`3)			!!!
.equ p_vol,				$1f |# .b last volume(127�`0)			!!!
.equ p_mrvs_tbl,		$20 |# .b �e�|�C���g�ɒu����Ӽޭڰ��ݕ␳�l($20�`$27)
.equ p_wvpm_point,		$20 |# .l �g�`���������݂̃|�C���^
.equ p_wvpm_end,		$24 |# .l �g�`�������I���A�h���X
.equ p_sp_tie,			$28 |# .w MIDI�̃X�y�V�����E�^�C�p���[�N
.equ p_om,				$28 |# .b �I�y���[�^�}�X�N(&b0000-&b1111)
.equ p_sync,			$29 |# .b LFO�̃V���N�X�C�b�`(0=off,ne=on)
.equ p_af,				$2a |# .b AL/FB
.equ p_se_mode,			$2b |# .b se mode or not($ff=normal/0�`=se mode)
.equ p_pmod_tbl,		$2c |# .w Ӽޭڰ��ݒlð���($2c�`$3b)
.equ p_total,			$3c |# .l �g�[�^���X�e�b�v�^�C��
.equ p_fo_lvl,			$40 |# .b �o�̓p�[�Z���e�[�W(0-128)
.equ p_note,			$42 |# .b �ߋ��Ƀm�[�g�I���������K�B�W��($42�`$49)
.equ p_extra_ch,		$4a |# .b �g���`�����l���ԍ�(PCM8 MODE��p0-7)
.equ p_aftc_n,			$4b |# .b ����������ݽ�̃|�C���^(0�`7)
.equ p_bend_rng_f,		$4c |# .w �I�[�g�x���h�̃����W(FM)
.equ p_bend_rng_m,		$4e |# .w �I�[�g�x���h�̃����W(MIDI)
.equ p_detune_f,		$50 |# .w �f�`���[��(FM�p�̒l)	!!!���Ԃ������Ă͂Ȃ�Ȃ�
.equ p_detune_m,		$52 |# .w �f�`���[��(MIDI�p�̒l)	!!!
.equ p_port_dly,		$54 |# .w �|���^�����g�f�B���C	###���Ԃ������Ă͂Ȃ�Ȃ�
.equ p_bend_dly,		$56 |# .w �x���h�f�B���C�l		###
.equ p_port_work,		$58 |# .b �|���^�����g�p�␳���[�N		!!!����3�̃��[�N��
.equ p_port_rvs,		$59 |# .b �|���^�����g�p�␳�p�����[�^	!!!���Ԃ�ς��Ă�
.equ p_port_work2,		$5a |# .w �������/�������ޗp ���݂̃x���h�l	!!!�Ȃ�Ȃ�
.equ p_amod_tbl,		$5c |# .b �`�l�lð���($5c�`$63)
.equ p_arcc_tbl,		$5c |# .b arcc�lð���($5c�`$63)
.equ p_arvs_tbl,		$64 |# .b amod�p�␳�l(FM)�e�[�u��($64�`$6b)
.equ p_wvam_point,		$64 |# .l �g�`���������݂̃|�C���^
.equ p_wvam_end,		$68 |# .l �g�`�������I���A�h���X
.equ p_pmod_work4,		$6c |# .w Ӽޭڰ��ݽ�߰��ܰ�(FM)
.equ p_port_flg,		$6e |# .w ������ĵ݂��̂�(0=off/�␳�������-1 or 1) !!! ���Ԃ�
.equ p_bend_flg,		$70 |# .w ����ނ��݂��̂�(0=off/�␳�������-1 or 1) !!! �ς��������
.equ p_aftc_tbl,		$72 |# .b ����������ݽ�l�e�[�u��($72�`$79)
.equ p_aftc_dly,		$7a |# .w ����������ݽ�ިڲ�l
.equ p_aftc_work,		$7c |# .w ����������ݽ�ިڲܰ�
.equ p_astep_tbl,		$7e |# .b �e�|�C���g�ɒu����AM�X�e�b�v�l($7e�`$85)
.equ p_wvam_loop,		$7e |# .l �g�`���������[�v�J�n�A�h���X
.equ p_wvam_lpmd,		$82 |# .w �g�`���������[�v���[�h
.equ p_alta_flg,		$84 |# .b �g�`�������������[�h�t���O
.equ p_pmod_step2,		$86 |# .w Ӽޭڰ��ݽï��ܰ�(FM)	!!!
.equ p_pmod_work,		$88 |# .w Ӽޭڰ����ިڲܰ�(MIDI/FM)	!!!�ʒu�����Ԃ�
.equ p_pmod_work2,		$8a |# .w Ӽޭڰ����߲��ܰ�(MIDI/FM)	!!!�������Ă�
.equ p_pmod_work3,		$8c |# .b Ӽޭڰ��ݗp�␳�l���[�N(FM)	!!!�Ȃ�Ȃ�
.equ p_pmod_n,			$8d |# .b Ӽޭڰ���ð����߲��(MIDI/FM)!!!
.equ p_sync_wk,			$8e |# .b ���������R�}���h�p���[�N			!!!
.equ p_rpt_last,		$8f |# .b �J��Ԃ����Ōォ�ǂ���(bit pattern)		!!!
.equ p_b_range,			$90 |# .b �x���h�����W(�����l=12)			!!!
.equ p_arcc,			$91 |# .b ARCC�̃R���g���[���i���o�[(MIDI)		!!!
.equ p_pmod_flg,		$92 |# .w Ӽޭڰ����׸�(FM��ܰ��/MIDI���޲�)	!!!���Ԃ������Ă�
.equ p_pmod_sw,			$94 |# .b �s�b�`Ӽޭڰ��݃X�C�b�`(���␳����)	!!!�Ȃ�Ȃ�
.equ p_amod_sw,			$95 |# .b AMOD�X�C�b�`(0=off,ne=on)		!!!
.equ p_arcc_sw,			$95 |# .b ARCC�X�C�b�`(0=off,ne=on)		!!!
.equ p_bend_sw,			$96 |# .b �I�[�g�x���h���A�N�e�B�u��(0=no/�x���h����=yes)	!!!
.equ p_aftc_flg,		$97 |# .b ����������ݽ�׸� (0=off/$ff=on)			!!!
.equ p_md_flg,			$98 |# d0 @b:����ޒl��ؾ�Ă��ׂ����ǂ���(MIDI��p 0=no/1=yes)	!!!
				#d1 @m:Ӽޭڰ��ݒl��ؾ�Ă��邩���Ȃ���(MIDI��p 0=no/1=yes)
				#d2 @a:AM�l��ؾ�Ă��邩���Ȃ���(MIDI��p 0=no/1=yes)
				#d3 midi tie mode
				#d4 pmd first time? or not
				#d5 amd first time? or not
				#d6 pmd hold or not
				#d7 amd hold or not
.equ p_waon_flg,		$99 |# .b �a��������Ƃ��V���O����(0=single/$ff=chord)	!!!
.equ p_pmod_dly,		$9a |# .w ���W�����[�V�����f�B���C�l(FM/MIDI)	!!!���Ԃ������Ă�
.equ p_amod_dly,		$9c |# .w �`�l�f�B���C�l(FM)			!!!�Ȃ�Ȃ�
.equ p_arcc_dly,		$9c |# .w ARCC�f�B���C�l(MIDI)		!!!�Ȃ�Ȃ�
.equ p_port_step,		$9e |# .w �|���^�����g�p���Z���[�N
.equ p_d5_last,			$a0 |# .w d5.w�̃��[�N(MIDI)
.equ p_ol1,				$a0 |# .b (OUT PUT LEVEL OP1)
.equ p_ol2,				$a1 |# .b (OUT PUT LEVEL OP2)
.equ p_ol3,				$a2 |# .b (OUT PUT LEVEL OP3)
.equ p_ol4,				$a3 |# .b (OUT PUT LEVEL OP4)
.equ p_d6_last,			$a4 |# .b d6.b�̃��[�N(MIDI)
.equ p_cf,				$a4 |# .b (CARRIER ���ǂ����̃t���O bit pattern:bit=1 carrier1)
.equ p_amod_step2,		$a5 |# .b AM�ï��ܰ�
.equ p_pb_add,			$a6 |# .b �x���̎��Ƀs�b�`�ۑ������邩���Ȃ���	!!!
				#d0:�x���̎��x���h��̃s�b�`��ۑ����邩
				#d1:�x���̎��s�b�`��ۑ����邩
.equ p_vset_flg,		$a7 |# .b �{�����[�����Z�b�g�t���O(FM)		!!!
.equ p_arcc_rst,		$a8 |# .b ARCC�̃��Z�b�g�o�����[(default:0)		!!!
.equ p_arcc_def,		$a9 |# .b ARCC�f�t�H���g�l(default:127)		!!!
.equ p_coda_ptr,		$aa |# .l [coda]�̂���ʒu
.equ p_pointer,			$ae |# .l [segno]�̂���ʒu
.equ p_do_loop_ptr,		$b2 |# .l [do]�̂���ʒu
.equ p_pmod_work5,		$b6 |# .w �ï����т�1/8(FM)
.equ p_pmod_work6,		$b8 |# .w �ï����т�1/8���[�N(FM)
.equ p_amod_flg,		$ba |# .b ARCC�׸�(FM)			!!!���Ԃ�
.equ p_arcc_flg,		$ba |# .b ARCC�׸�(MIDI)			!!!���Ԃ�
.equ p_aftc_sw,			$bb |# .b ����������ݽ�̽���(0=off/$ff=on)	!!!�ς��Ă�
.equ p_dumper,			$bc |# .b dumper on or off (0=off/$ff=on)	!!!�Ȃ�Ȃ�
.equ p_tie_flg,			$bd |# .b �^�C��������(0=no/ff=yes)		!!!
.equ p_pmod_dpt,		$be |# .w �߯�Ӽޭڰ������߽(FM)			!!!
.equ p_seq_flag,		$c0 |# .b []�R�}���h�n�̏����t���O�r�b�g�p�^�[��	!!!
				#d0:[d.c.]�������������Ƃ����邩(0=no/1=yes)
				#d1:[fine]���������ׂ����ǂ���(0=no/1=yes)
				#d2:[coda]���ȑO�ɐݒ肵�����Ƃ����邩(0=no/1=yes)
				#d3:[segno]�����邩�Ȃ����̃t���O(0=no/1=yes)
				#d4:[d.s.]�������������Ƃ����邩(0=no/1=yes)
				#d6:key off bit
				#d7:key on bit
.equ p_do_loop_flag,	$c1 |# .b [do]���ȑO�ɐݒ肳��Ă��邩/���[�v��	!!!
.equ p_pmod_spd,		$c2 |# .w �o�l�̂P�^�S����	!!!
.equ p_amod_spd,		$c4 |# .w �`�l�̂P�^�S����	!!!
.equ p_total_olp,		$c6 |# .l ٰ�ߊO��İ�ٽï�߲�
.equ p_pmod_step,		$ca |# .w Ӽޭڰ��ݗp���Z���[�N
.equ p_tie_pmod,		$cc |# .b tie�̓r���Ńp�����[�^�`�F���W���s��ꂽ���ǂ���	!!!
.equ p_tie_bend,		$cd |# .b (0=no,$ff=yes)					!!!
.equ p_tie_amod,		$ce |# .b							!!!
.equ p_tie_arcc,		$ce |# .b							!!!
.equ p_tie_aftc,		$cf |# .b							!!!
.equ p_pan2,			$d0 |# .b �p���|�b�g(FM/MIDI L 0�`M64�`127 R)	!!!
.equ p_non_off,			$d1 |# .b �L�[�I�t�������[�h(0=no,$ff=yes)	!!!
.equ p_frq,				$d2 |# .b ADPCM�̎��g��(0-6)			!!!
.equ p_velo,			$d3 |# .b velocity(0-127)			!!!
.equ p_amod_work4,		$d4 |# .w Ӽޭڰ��ݽ�߰��ܰ�(FM)
.equ p_pmod_rvs,		$d6 |# .b ���W�����[�V�����p�␳���Ұ�
.equ p_waon_dly,		$d7 |# .b �a���p�f�B���C�l
.equ p_waon_work,		$d8 |# .b �a���p�f�B���C���[�N
.equ p_waon_num,		$d9 |# .b ���Ԗڃm�[�m�[�g���L�[�I������̂�(minus=end)
.equ p_note_last,		$d9 |# .b �m�[�g�̈ꎞ�ޔ�(MIDI)�����ɂ͋N���蓾�Ȃ�������S
.equ p_rpt_cnt,			$da |# .b repeat count work($da�`$e1)
.equ p_maker,			$e2 |# .b Ұ��ID(MIDI)
.equ p_device,			$e3 |# .b ���޲�ID(MIDI)
.equ p_module,			$e4 |# .b Ӽޭ��ID(MIDI)
.equ p_last_aft,		$e5 |# .b �O��̃A�t�^�[�^�b�`�l
.equ p_amod_work,		$e6 |# .w AMOD�ިڲܰ�(FM)		!!!
.equ p_arcc_work,		$e6 |# .w ARCC�ިڲܰ�(MIDI)		!!!
.equ p_arcc_work2,		$e8 |# .b ARCC�߲��ܰ�(MIDI)		!!!
.equ p_amod_work2,		$e8 |# .b AMOD�߲��ܰ�(FM)		!!!
.equ p_amod_work3,		$e9 |# .b Ӽޭڰ��ݗp�␳�l���[�N(FM)	!!!
.equ p_amod_work7,		$ea |# .b �m�R�M���g��p���[�N!!!
.equ p_amod_n,			$eb |# .b AMð����߲��(FM)	!!!
.equ p_arcc_n,			$eb |# .b ARCCð����߲��(MIDI)!!!
.equ p_arcc_work5,		$ec |# .w �ï����т�1/8(FM)
.equ p_amod_work5,		$ec |# .w �ï����т�1/8(FM)
.equ p_arcc_work6,		$ee |# .w �ï����т�1/8���[�N(FM)
.equ p_amod_work6,		$ee |# .w �ï����т�1/8���[�N(FM)
.equ p_pmod_wf,			$f0 |# .b �\�t�g�k�e�n(�o�l)�̔g�`�^�C�v(FM:-1,0,1)	!!!
.equ p_amod_dpt,		$f1 |# .b FM����AMD�f�v�X				!!!
.equ p_amod_wf,			$f2 |# .b �\�t�g�k�e�n(�`�l)�̔g�`�^�C�v(FM:-1,0,1)	!!!
.equ p_dmp_n,			$f3 |# .b FM�����p�_���p�[�������[�N			!!!
.equ p_pmod_omt,		$f4 |# .b 1/8-PMOD�̏ȗ��r�b�g�p�^�[��			!!!
.equ p_arcc_omt,		$f5 |# .b 1/8-ARCC�̏ȗ��r�b�g�p�^�[��
.equ p_amod_omt,		$f5 |# .b 1/8-AMOD�̏ȗ��r�b�g�p�^�[��			!!!
.equ p_pmod_mode,		$f6 |# .b MIDI���W�����[�V�����̌`��(-1:normal/0:FM/1:MIDI)	!!!
.equ p_arcc_mode,		$f7 |# .b MIDI ARCC�̌`��(-1:normal/1�`127:extended mode)	!!!
.equ p_pmod_chain,		$f8 |# .b PM�̐ڑ��t���O
.equ p_amod_chain,		$f9 |# .b AM�̐ڑ��t���O
.equ p_jump_flg,		$fa |# .b [!]�R�}���h���[�N($00=normal,$ff=jumping,$01=done)	 !!!
.equ p_waon_mark,		$fb |# .b ������ق̃p�����[�^��ݒ肵����(0=not yet,1=done)	 !!!
.equ p_marker,			$fc |# .w ̪��ޱ�Ď��Ɏg�p (p_maker(a5)=se track or not,+1=flg)!!!
.equ p_amod_rvs,		$fe |# .b amod�p�␳�l(FM)
.equ p_ne_buff,			$ff |# .b p_not_empty�̈ꎞ�ޔ��ꏊ(se mode ��p���[�N)
.equ p_user,			$ff |# .b ���[�U�[�ėp���[�N

#//////////////////////////////////////////////////////////////////////////////
#	�y-�l�t�r�h�b�֌W
#
.macro zm_picture_sync_on
			movem.l	%d0-%d2,-(%sp)
			moveq	#0,%d2
			Z_MUSIC	picture_sync
			moveq	#-1,%d2
			Z_MUSIC	picture_sync
			movem.l	(%sp)+,%d0-%d2
.endm

.macro zm_picture_sync_off
			movem.l	%d0-%d2,-(%sp)
			moveq	#0,%d2
			Z_MUSIC	picture_sync
			movem.l	(%sp)+,%d0-%d2
.endm

.macro zm_fade_out	time
		movem.l	%d0-%d2,-(%sp)
		moveq	\time,%d2
		Z_MUSIC	m_fade_out
		movem.l	(%sp)+,%d0-%d2
.endm

#�Pch���ʉ����m�����o��
.macro MONO_SE1CH	se_no,prw_levelandwait
		.local	MONO_SE1CH_SKIP
		movem.l	%d0,-(%sp)
		move.l	\prw_levelandwait,%d0
		cmp.w	fm_se_prwlevel,%d0
		bmi	MONO_SE1CH_SKIP
		move.l	%d0,fm_se_prwwait
		moveq	\se_no,%d0
		jsr	fm_se_play_1ch
MONO_SE1CH_SKIP:
		movem.l	(%sp)+,%d0
.endm

#�Qch���ʉ����m�����o��
.macro MONO_SE2CH	se_no,prw_levelandwait
		.local	MONO_SE2CH_SKIP
		movem.l	%d0,-(%sp)
		move.l	\prw_levelandwait,%d0
		cmp.w	fm_se_prwlevel,%d0
		bmi	MONO_SE2CH_SKIP
		move.l	%d0,fm_se_prwwait
		moveq	\se_no,%d0
		jsr	fm_se_play_2ch
MONO_SE2CH_SKIP:
		movem.l	(%sp)+,%d0
.endm

#reg��para���傫���ꍇP2(R),�������ꍇP1(L)��se_no�̌��ʉ��𔭐����܂����
.macro STEREO_SE1CH	reg,para,se_no,prw_levelandwait
		.local	STE_SE001
		.local	STEREO_SE1CH_SKIP
		movem.l	%d0-%d1,-(%sp)
		move.l	\prw_levelandwait,%d1
		cmp.w	fm_se_prwlevel,%d1
		bmi	STEREO_SE1CH_SKIP
		move.l	%d1,fm_se_prwwait
		move.l	#$b2,%d1
		cmpi.w	\para,\reg
		bpl	STE_SE001
		subq.w	#1,%d1
STE_SE001:
		moveq	\se_no,%d0
		jsr	fm_se_play_stereo1ch
STEREO_SE1CH_SKIP:
		movem.l	(%sp)+,%d0-%d1
.endm

.macro STEREO_SE1CH_C	se_no,prw_levelandwait
		.local	STEREO_SE1CH_C_SKIP
		movem.l	%d0-%d1,-(%sp)
		move.l	\prw_levelandwait,%d0
		cmp.w	fm_se_prwlevel,%d0
		bmi	STEREO_SE1CH_C_SKIP
		move.l	%d0,fm_se_prwwait
		move.l	#$b3,%d1
		moveq	\se_no,%d0
		jsr	fm_se_play_stereo1ch
STEREO_SE1CH_C_SKIP:
		movem.l	(%sp)+,%d0-%d1
.endm

#reg��para���傫���ꍇP2(R),�������ꍇP1(L)��se_no�̌��ʉ��𔭐����܂����
.macro STEREO_SE2CH	reg,para,se_no,prw_levelandwait
		.local	STE_SE201
		.local	STEREO_SE2CH_SKIP
		movem.l	%d0-%d1,-(%sp)
		move.l	prw_levelandwait,%d1
		cmp.w	fm_se_prwlevel,%d1
		bmi	STEREO_SE2CH_SKIP
		move.l	%d1,fm_se_prwwait
		moveq	se_no,%d1
		cmpi.w	para,reg
		bpl	STE_SE201
		subq.w	#1,%d1
STE_SE201:
		move.l	%d1,%d0
		jsr	fm_se_play_2ch
STEREO_SE2CH_SKIP:
		movem.l	(%sp)+,%d0-%d1
.endm


.list