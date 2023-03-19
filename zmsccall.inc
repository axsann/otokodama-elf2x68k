#
# Original: zmsccall.mac X68k Z-MUSIC v1.09C Copyright 1991,1992 ZENJI SOFT
#
# このファイルはzmsccall.macをモトローラ形式からMIT形式（GASフォーマット）に変換し、拡張子をincに変更したものです。
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

#ワークエリア
.equ wk_size,			256 |# 各トラックの演奏時のワークサイズ(変更不可)
.equ wk_size2,			8	|# ワークサイズが２の何乗か(変更不可)

.equ p_on_count,		$00 |# .w step time		!!! 順番を変えてはならない
.equ p_gate_time,		$02 |# .w gate time		!!!
.equ p_data_pointer,	$04 |# .l 現在のコマンドポインタ
.equ p_fo_spd,			$08 |# .b フェードアウトスピード
.equ p_ch,				$09 |# .b アサインされているチャンネル
.equ p_not_empty,		$0a |# .b トラックの生死(-1=dead/1=play end/0=alive)
.equ p_amod_step,		$0b |# .b AMのステップワーク
.equ p_mstep_tbl,		$0c |# .w 各ポイントに置けるﾓｼﾞｭﾚｰｼｮﾝステップ値($0c～$1b)
.equ p_wvpm_loop,		$0c |# .l 波形メモリループ開始アドレス
.equ p_wvpm_lpmd,		$10 |# .w 波形メモリループモード
.equ p_altp_flg,		$12 |# .b 波形メモリ反復モードフラグ
.equ p_fo_mode,			$1c |# .b fade out flag (0=off/1～127=on)	!!!
.equ p_pgm,				$1d |# .b last tone number(0-199)		!!!
.equ p_pan,				$1e |# .b last panpot(0～3)			!!!
.equ p_vol,				$1f |# .b last volume(127～0)			!!!
.equ p_mrvs_tbl,		$20 |# .b 各ポイントに置けるﾓｼﾞｭﾚｰｼｮﾝ補正値($20～$27)
.equ p_wvpm_point,		$20 |# .l 波形メモリ現在のポインタ
.equ p_wvpm_end,		$24 |# .l 波形メモリ終了アドレス
.equ p_sp_tie,			$28 |# .w MIDIのスペシャル・タイ用ワーク
.equ p_om,				$28 |# .b オペレータマスク(&b0000-&b1111)
.equ p_sync,			$29 |# .b LFOのシンクスイッチ(0=off,ne=on)
.equ p_af,				$2a |# .b AL/FB
.equ p_se_mode,			$2b |# .b se mode or not($ff=normal/0～=se mode)
.equ p_pmod_tbl,		$2c |# .w ﾓｼﾞｭﾚｰｼｮﾝ値ﾃｰﾌﾞﾙ($2c～$3b)
.equ p_total,			$3c |# .l トータルステップタイム
.equ p_fo_lvl,			$40 |# .b 出力パーセンテージ(0-128)
.equ p_note,			$42 |# .b 過去にノートオンした音階達８個($42～$49)
.equ p_extra_ch,		$4a |# .b 拡張チャンネル番号(PCM8 MODE専用0-7)
.equ p_aftc_n,			$4b |# .b ｱﾌﾀｰﾀｯﾁｼｰｹﾝｽのポインタ(0～7)
.equ p_bend_rng_f,		$4c |# .w オートベンドのレンジ(FM)
.equ p_bend_rng_m,		$4e |# .w オートベンドのレンジ(MIDI)
.equ p_detune_f,		$50 |# .w デチューン(FM用の値)	!!!順番をかえてはならない
.equ p_detune_m,		$52 |# .w デチューン(MIDI用の値)	!!!
.equ p_port_dly,		$54 |# .w ポルタメントディレイ	###順番をかえてはならない
.equ p_bend_dly,		$56 |# .w ベンドディレイ値		###
.equ p_port_work,		$58 |# .b ポルタメント用補正ワーク		!!!この3つのワークの
.equ p_port_rvs,		$59 |# .b ポルタメント用補正パラメータ	!!!順番を変えては
.equ p_port_work2,		$5a |# .w ﾎﾟﾙﾀﾒﾝﾄ/ｵｰﾄﾍﾞﾝﾄﾞ用 現在のベンド値	!!!ならない
.equ p_amod_tbl,		$5c |# .b ＡＭ値ﾃｰﾌﾞﾙ($5c～$63)
.equ p_arcc_tbl,		$5c |# .b arcc値ﾃｰﾌﾞﾙ($5c～$63)
.equ p_arvs_tbl,		$64 |# .b amod用補正値(FM)テーブル($64～$6b)
.equ p_wvam_point,		$64 |# .l 波形メモリ現在のポインタ
.equ p_wvam_end,		$68 |# .l 波形メモリ終了アドレス
.equ p_pmod_work4,		$6c |# .w ﾓｼﾞｭﾚｰｼｮﾝｽﾋﾟｰﾄﾞﾜｰｸ(FM)
.equ p_port_flg,		$6e |# .w ﾎﾟﾙﾀﾒﾝﾄｵﾝかｵﾌか(0=off/補正する方向-1 or 1) !!! 順番を
.equ p_bend_flg,		$70 |# .w ﾍﾞﾝﾄﾞがｵﾝかｵﾌか(0=off/補正する方向-1 or 1) !!! 変えちゃﾀﾞﾒ
.equ p_aftc_tbl,		$72 |# .b ｱﾌﾀｰﾀｯﾁｼｰｹﾝｽ値テーブル($72～$79)
.equ p_aftc_dly,		$7a |# .w ｱﾌﾀｰﾀｯﾁｼｰｹﾝｽﾃﾞｨﾚｲ値
.equ p_aftc_work,		$7c |# .w ｱﾌﾀｰﾀｯﾁｼｰｹﾝｽﾃﾞｨﾚｲﾜｰｸ
.equ p_astep_tbl,		$7e |# .b 各ポイントに置けるAMステップ値($7e～$85)
.equ p_wvam_loop,		$7e |# .l 波形メモリループ開始アドレス
.equ p_wvam_lpmd,		$82 |# .w 波形メモリループモード
.equ p_alta_flg,		$84 |# .b 波形メモリ反復モードフラグ
.equ p_pmod_step2,		$86 |# .w ﾓｼﾞｭﾚｰｼｮﾝｽﾃｯﾌﾟﾜｰｸ(FM)	!!!
.equ p_pmod_work,		$88 |# .w ﾓｼﾞｭﾚｰｼｮﾝﾃﾞｨﾚｲﾜｰｸ(MIDI/FM)	!!!位置も順番も
.equ p_pmod_work2,		$8a |# .w ﾓｼﾞｭﾚｰｼｮﾝﾎﾟｲﾝﾄﾜｰｸ(MIDI/FM)	!!!動かしては
.equ p_pmod_work3,		$8c |# .b ﾓｼﾞｭﾚｰｼｮﾝ用補正値ワーク(FM)	!!!ならない
.equ p_pmod_n,			$8d |# .b ﾓｼﾞｭﾚｰｼｮﾝﾃｰﾌﾞﾙﾎﾟｲﾝﾀ(MIDI/FM)!!!
.equ p_sync_wk,			$8e |# .b 強制同期コマンド用ワーク			!!!
.equ p_rpt_last,		$8f |# .b 繰り返しが最後かどうか(bit pattern)		!!!
.equ p_b_range,			$90 |# .b ベンドレンジ(初期値=12)			!!!
.equ p_arcc,			$91 |# .b ARCCのコントロールナンバー(MIDI)		!!!
.equ p_pmod_flg,		$92 |# .w ﾓｼﾞｭﾚｰｼｮﾝﾌﾗｸﾞ(FMはﾜｰﾄﾞ/MIDIはﾊﾞｲﾄ)	!!!順番をかえては
.equ p_pmod_sw,			$94 |# .b ピッチﾓｼﾞｭﾚｰｼｮﾝスイッチ(兼補正方向)	!!!ならない
.equ p_amod_sw,			$95 |# .b AMODスイッチ(0=off,ne=on)		!!!
.equ p_arcc_sw,			$95 |# .b ARCCスイッチ(0=off,ne=on)		!!!
.equ p_bend_sw,			$96 |# .b オートベンドがアクティブか(0=no/ベンド方向=yes)	!!!
.equ p_aftc_flg,		$97 |# .b ｱﾌﾀｰﾀｯﾁｼｰｹﾝｽﾌﾗｸﾞ (0=off/$ff=on)			!!!
.equ p_md_flg,			$98 |# d0 @b:ﾍﾞﾝﾄﾞ値をﾘｾｯﾄすべきかどうか(MIDI専用 0=no/1=yes)	!!!
				#d1 @m:ﾓｼﾞｭﾚｰｼｮﾝ値をﾘｾｯﾄするかしないか(MIDI専用 0=no/1=yes)
				#d2 @a:AM値をﾘｾｯﾄするかしないか(MIDI専用 0=no/1=yes)
				#d3 midi tie mode
				#d4 pmd first time? or not
				#d5 amd first time? or not
				#d6 pmd hold or not
				#d7 amd hold or not
.equ p_waon_flg,		$99 |# .b 和音かそれともシングルか(0=single/$ff=chord)	!!!
.equ p_pmod_dly,		$9a |# .w モジュレーションディレイ値(FM/MIDI)	!!!順番をかえては
.equ p_amod_dly,		$9c |# .w ＡＭディレイ値(FM)			!!!ならない
.equ p_arcc_dly,		$9c |# .w ARCCディレイ値(MIDI)		!!!ならない
.equ p_port_step,		$9e |# .w ポルタメント用加算ワーク
.equ p_d5_last,			$a0 |# .w d5.wのワーク(MIDI)
.equ p_ol1,				$a0 |# .b (OUT PUT LEVEL OP1)
.equ p_ol2,				$a1 |# .b (OUT PUT LEVEL OP2)
.equ p_ol3,				$a2 |# .b (OUT PUT LEVEL OP3)
.equ p_ol4,				$a3 |# .b (OUT PUT LEVEL OP4)
.equ p_d6_last,			$a4 |# .b d6.bのワーク(MIDI)
.equ p_cf,				$a4 |# .b (CARRIER かどうかのフラグ bit pattern:bit=1 carrier1)
.equ p_amod_step2,		$a5 |# .b AMｽﾃｯﾌﾟﾜｰｸ
.equ p_pb_add,			$a6 |# .b 休符の時にピッチ保存をするかしないか	!!!
				#d0:休符の時ベンド後のピッチを保存するか
				#d1:休符の時ピッチを保存するか
.equ p_vset_flg,		$a7 |# .b ボリュームリセットフラグ(FM)		!!!
.equ p_arcc_rst,		$a8 |# .b ARCCのリセットバリュー(default:0)		!!!
.equ p_arcc_def,		$a9 |# .b ARCCデフォルト値(default:127)		!!!
.equ p_coda_ptr,		$aa |# .l [coda]のある位置
.equ p_pointer,			$ae |# .l [segno]のある位置
.equ p_do_loop_ptr,		$b2 |# .l [do]のある位置
.equ p_pmod_work5,		$b6 |# .w ｽﾃｯﾌﾟﾀｲﾑの1/8(FM)
.equ p_pmod_work6,		$b8 |# .w ｽﾃｯﾌﾟﾀｲﾑの1/8ワーク(FM)
.equ p_amod_flg,		$ba |# .b ARCCﾌﾗｸﾞ(FM)			!!!順番を
.equ p_arcc_flg,		$ba |# .b ARCCﾌﾗｸﾞ(MIDI)			!!!順番を
.equ p_aftc_sw,			$bb |# .b ｱﾌﾀｰﾀｯﾁｼｰｹﾝｽのｽｲｯﾁ(0=off/$ff=on)	!!!変えては
.equ p_dumper,			$bc |# .b dumper on or off (0=off/$ff=on)	!!!ならない
.equ p_tie_flg,			$bd |# .b タイだったか(0=no/ff=yes)		!!!
.equ p_pmod_dpt,		$be |# .w ﾋﾟｯﾁﾓｼﾞｭﾚｰｼｮﾝﾃﾞﾌﾟｽ(FM)			!!!
.equ p_seq_flag,		$c0 |# .b []コマンド系の処理フラグビットパターン	!!!
				#d0:[d.c.]処理をしたことがあるか(0=no/1=yes)
				#d1:[fine]処理をすべきかどうか(0=no/1=yes)
				#d2:[coda]を以前に設定したことがあるか(0=no/1=yes)
				#d3:[segno]があるかないかのフラグ(0=no/1=yes)
				#d4:[d.s.]処理をしたことがあるか(0=no/1=yes)
				#d6:key off bit
				#d7:key on bit
.equ p_do_loop_flag,	$c1 |# .b [do]が以前に設定されているか/ループ回数	!!!
.equ p_pmod_spd,		$c2 |# .w ＰＭの１／４周期	!!!
.equ p_amod_spd,		$c4 |# .w ＡＭの１／４周期	!!!
.equ p_total_olp,		$c6 |# .l ﾙｰﾌﾟ外のﾄｰﾀﾙｽﾃｯﾌﾟｲﾑ
.equ p_pmod_step,		$ca |# .w ﾓｼﾞｭﾚｰｼｮﾝ用加算ワーク
.equ p_tie_pmod,		$cc |# .b tieの途中でパラメータチェンジが行われたかどうか	!!!
.equ p_tie_bend,		$cd |# .b (0=no,$ff=yes)					!!!
.equ p_tie_amod,		$ce |# .b							!!!
.equ p_tie_arcc,		$ce |# .b							!!!
.equ p_tie_aftc,		$cf |# .b							!!!
.equ p_pan2,			$d0 |# .b パンポット(FM/MIDI L 0～M64～127 R)	!!!
.equ p_non_off,			$d1 |# .b キーオフ無しモード(0=no,$ff=yes)	!!!
.equ p_frq,				$d2 |# .b ADPCMの周波数(0-6)			!!!
.equ p_velo,			$d3 |# .b velocity(0-127)			!!!
.equ p_amod_work4,		$d4 |# .w ﾓｼﾞｭﾚｰｼｮﾝｽﾋﾟｰﾄﾞﾜｰｸ(FM)
.equ p_pmod_rvs,		$d6 |# .b モジュレーション用補正ﾊﾟﾗﾒｰﾀ
.equ p_waon_dly,		$d7 |# .b 和音用ディレイ値
.equ p_waon_work,		$d8 |# .b 和音用ディレイワーク
.equ p_waon_num,		$d9 |# .b 何番目ノーノートをキーオンするのか(minus=end)
.equ p_note_last,		$d9 |# .b ノートの一時退避(MIDI)同時には起こり得ないから安心
.equ p_rpt_cnt,			$da |# .b repeat count work($da～$e1)
.equ p_maker,			$e2 |# .b ﾒｰｶｰID(MIDI)
.equ p_device,			$e3 |# .b ﾃﾞﾊﾞｲｽID(MIDI)
.equ p_module,			$e4 |# .b ﾓｼﾞｭｰﾙID(MIDI)
.equ p_last_aft,		$e5 |# .b 前回のアフタータッチ値
.equ p_amod_work,		$e6 |# .w AMODﾃﾞｨﾚｲﾜｰｸ(FM)		!!!
.equ p_arcc_work,		$e6 |# .w ARCCﾃﾞｨﾚｲﾜｰｸ(MIDI)		!!!
.equ p_arcc_work2,		$e8 |# .b ARCCﾎﾟｲﾝﾄﾜｰｸ(MIDI)		!!!
.equ p_amod_work2,		$e8 |# .b AMODﾎﾟｲﾝﾄﾜｰｸ(FM)		!!!
.equ p_amod_work3,		$e9 |# .b ﾓｼﾞｭﾚｰｼｮﾝ用補正値ワーク(FM)	!!!
.equ p_amod_work7,		$ea |# .b ノコギリ波専用ワーク!!!
.equ p_amod_n,			$eb |# .b AMﾃｰﾌﾞﾙﾎﾟｲﾝﾀ(FM)	!!!
.equ p_arcc_n,			$eb |# .b ARCCﾃｰﾌﾞﾙﾎﾟｲﾝﾀ(MIDI)!!!
.equ p_arcc_work5,		$ec |# .w ｽﾃｯﾌﾟﾀｲﾑの1/8(FM)
.equ p_amod_work5,		$ec |# .w ｽﾃｯﾌﾟﾀｲﾑの1/8(FM)
.equ p_arcc_work6,		$ee |# .w ｽﾃｯﾌﾟﾀｲﾑの1/8ワーク(FM)
.equ p_amod_work6,		$ee |# .w ｽﾃｯﾌﾟﾀｲﾑの1/8ワーク(FM)
.equ p_pmod_wf,			$f0 |# .b ソフトＬＦＯ(ＰＭ)の波形タイプ(FM:-1,0,1)	!!!
.equ p_amod_dpt,		$f1 |# .b FM音源AMDデプス				!!!
.equ p_amod_wf,			$f2 |# .b ソフトＬＦＯ(ＡＭ)の波形タイプ(FM:-1,0,1)	!!!
.equ p_dmp_n,			$f3 |# .b FM音源用ダンパー処理ワーク			!!!
.equ p_pmod_omt,		$f4 |# .b 1/8-PMODの省略ビットパターン			!!!
.equ p_arcc_omt,		$f5 |# .b 1/8-ARCCの省略ビットパターン
.equ p_amod_omt,		$f5 |# .b 1/8-AMODの省略ビットパターン			!!!
.equ p_pmod_mode,		$f6 |# .b MIDIモジュレーションの形式(-1:normal/0:FM/1:MIDI)	!!!
.equ p_arcc_mode,		$f7 |# .b MIDI ARCCの形式(-1:normal/1～127:extended mode)	!!!
.equ p_pmod_chain,		$f8 |# .b PMの接続フラグ
.equ p_amod_chain,		$f9 |# .b AMの接続フラグ
.equ p_jump_flg,		$fa |# .b [!]コマンドワーク($00=normal,$ff=jumping,$01=done)	 !!!
.equ p_waon_mark,		$fb |# .b 主ﾁｬﾝﾈﾙのパラメータを設定したか(0=not yet,1=done)	 !!!
.equ p_marker,			$fc |# .w ﾌｪｰﾄﾞｱｳﾄ時に使用 (p_maker(a5)=se track or not,+1=flg)!!!
.equ p_amod_rvs,		$fe |# .b amod用補正値(FM)
.equ p_ne_buff,			$ff |# .b p_not_emptyの一時退避場所(se mode 専用ワーク)
.equ p_user,			$ff |# .b ユーザー汎用ワーク

#//////////////////////////////////////////////////////////////////////////////
#	Ｚ-ＭＵＳＩＣ関係
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

#１ch効果音モノラル出力
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

#２ch効果音モノラル出力
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

#regがparaより大きい場合P2(R),小さい場合P1(L)でse_noの効果音を発声します･･･
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

#regがparaより大きい場合P2(R),小さい場合P1(L)でse_noの効果音を発声します･･･
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
