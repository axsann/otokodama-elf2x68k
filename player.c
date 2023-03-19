#include <stdio.h>
#include <stdlib.h>
#include <x68k/iocs.h>
#include <XSP2lib.H>

#include "otoko.h"
#include "player.h"
#include "shot.h"
#include "eshot.h"
#include "names.h"
#include "effect.h"
#include "entry.h"
#include "priority.h"
#include "txfont.h"
#include "sound.h"
#include "damage.h"


/* 自機の当たり判定 */
#define PLAYER_HIT_X	2
#define PLAYER_HIT_Y	3

#define PLAYER_SPEED	10
#define PLAYER_HIGHSPEED	25

#define PLAYER_SPEED_APPEAR	22	/* 登場時の速度 */
#define PLAYER_SPEED_LAST	25	/* オールクリア時の速度 */

#define PLAYER_MINX		(16+8)	/* X 座標最小値 */
#define PLAYER_MAXX		(16+255-8)
#define PLAYER_MINY		(16+16)	/* Y 座標最小値 */
#define PLAYER_MAXY		(16+255-12)

#define PLAYER_BANK1	8	/* 少し傾きパターンへ移行するまでの値 */
#define PLAYER_BANK2	12	/* 一番傾きパターンへ以下同文 */


#ifndef NULL
#define NULL ((void *) 0)
#endif

#define	TIME_MUTEKI	90	/* 登場時の無敵時間 */
#define	TIME_BARRIER	110	/* バリア使用時の無敵時間 */
#define TIME_HIGHSPEED	10	/* 自機高速移動までの時間 */
#define	TIME_HOLD	110	/* バリア発動までに要する時間 */
#define	TIME_BOMBER	90	/* ボンバー使用時の無敵時間 */


#define	GUAGE_X		7	/* ボンバーゲージ表示位置 */
#define	GUAGE_Y		31
#define	GUAGE_ERASE_Y		(228+16)	/* 自機がこれより下なら非表示 */
#define	GUAGE_DISP_TIME		55	/* ゲージ表示までの時間 */


static void EntryEdit (void);



/* 起動時に１度だけ呼ばれる */
void PlayerInit0 (void)
{
}


/* 残機を表示 */
void DispRest (void)
{
	short h;

#define REST_DISP_X	2
#define REST_DISP_Y	1
	TxfontCursor (REST_DISP_X, REST_DISP_Y);
	for (h = 0; h < 10; h++) {
		if (h < player->rest - 1)
			TxfontPutchar ('i');	/* 'i' に自機のマークが割り当てられてます */
		else
			TxfontPutchar (' ');
	};
}



/* プレイヤー出現時に呼ばれる（ゲーム開始時） */
void PlayerInit (void)
{
	player->info = PALET_PLAYER | PRIORITY_PLAYER;	/* 反転コード・色・優先度を表わすデータ */
	player->bank = 0;
	player->seq = PLAYER_SEQ_APPEAR;
	player->seq2 = 0;
	player->flash = 0;
	player->shot_timer = 0;
	player->shot_cont = 0;
	player->a_button = 0;
	player->b_button = 0;
	player->b_button_next = 0;
	player->bomb_type = BOMB_NON;
	player->high_speed = PLAYER_SPEED;
	player->bomb_flash_state = BG_FLASH_NON;
	player->bomb_flash_timer = 0;
	player->blaser = 0;
	player->bomb_hitcheck = BOMB_NON;
	player->bomb_hitcheck_time = 0;
	player->bomb_energy = 65536;
	player->bomb_guage_disp = 0;
	player->replay_counter = 0;
	player->game_over = GAME_PLAY;
}



/* 垂直同期ごとに呼ばれる */
void PlayerMove (void)
{
	int s = -1;		/* ジョイスティックの状態 */
	char xmove_flag = 0;	/* X 方向に移動があったことのフラグ */

	switch (replay_flag) {
	case REPLAY_NON:	/* リプレイ中ではない（通常のゲーム中） */
		s = _iocs_joyget (0);
		if (player->replay_counter < REPLAY_DATA_SIZE_MAX)
			replay_data[player->replay_counter++] = s;
		break;
	case REPLAY_BEFORE:	/* 直前のゲームのリプレイ中 */
		s = replay_data[player->replay_counter++];
		break;
	case REPLAY_HIGH:	/* ハイスコアのリプレイ中 */
		s = high_score_replay_data[player->replay_counter++];
		break;
	}

	if (player->blaser)
		player->blaser--;
    /* enemy.c で参照するボンバー当たり判定関連 */
	if (player->bomb_hitcheck != BOMB_NON) {
		player->bomb_hitcheck_time--;
		if (player->bomb_hitcheck_time == 0) {
			player->bomb_hitcheck = BOMB_NON;
			player->bomb_hitcheck_time = 0;
		}
	}
	switch (player->seq) {
	case PLAYER_SEQ_ALIVE:
	case PLAYER_SEQ_MUTEKI:
	case PLAYER_SEQ_BARRIER:

		if ((player->b_button_next) && (player->shot_cont == 0)) {
			s |= !0b1000000;
			player->b_button = 0;
			player->b_button_next = 0;
		}
	    /* Ｂボタン（ショット）が押されたか？ */
		if ((s & 0b1000000) == 0) {
			if (!player->b_button) {
			    /* 前回Ｂボタンが押されていなかった時の処理 */
				player->b_button++;

				if (player->bomb_type == BOMB_NON) {
					if (player->blaser == 0) {
					    /* ショットの処理 */
#define SHOT_TIMER_ADD	6
#define SHOT_TIMER_MAX	18
						if (player->shot_cont == 0) {
							player->shot_timer += SHOT_TIMER_ADD;
							if (player->shot_timer > SHOT_TIMER_MAX)
								player->shot_timer = SHOT_TIMER_MAX;

							ShotInit (192);
							SoundSetSE (SE_SHOT);	/* プレイヤーショット音 */
							player->shot_cont = 7;
							player->shot_timer_current = player->shot_timer;
						} else {
							player->b_button_next = !0;
						}
					}
				} else {
				    /* ボム発動中にＢボタンが押された（ボム確定） */
					switch (player->bomb_type) {
					case BOMB_A:
						EffectInit (EFFECT_BOMBER, 0, player->x, player->y);
						player->bomb_flash_state = BG_FLASH_NON;
						player->bomb_flash_timer = 5;	/* 必ず奇数 */
						player->bomb_hitcheck = BOMB_A;
						player->bomb_hitcheck_time = TIME_BOMBER_HITCHECK;
						if (!eshot_erase)
							eshot_erase = ESHOT_ERASE;	/* 弾消し */
						SoundSetSE (SE_BOMB_A);
						break;
					case BOMB_B:
						EffectInit (EFFECT_BACKB, 0, player->x, player->y);
						player->bomb_flash_state = BG_FLASH_NON;
						player->bomb_flash_timer = 5;	/* 必ず奇数 */
						player->bomb_hitcheck = BOMB_B;
						player->bomb_hitcheck_time = TIME_BACK_B_HITCHECK;
						SoundSetSE (SE_BOMB_B);
						break;
					case BOMB_C:
						EffectInit (EFFECT_BLASER, 0, player->x, player->y);
						player->blaser = PLAYER_BLASER_END;
						player->bomb_hitcheck = BOMB_C;
						player->bomb_hitcheck_time = PLAYER_BLASER_END;
						SoundSetSE (SE_BOMB_C);
						break;
					default:
						break;
					}
					player->bomb_type = BOMB_NON;

				}
			} else {
			    /* 前回もＢボタンが押されていた時の処理 */
				player->b_button++;
				switch (player->seq) {
				case PLAYER_SEQ_MUTEKI:
					if (player->b_button == TIME_HIGHSPEED)
						player->high_speed = PLAYER_HIGHSPEED;	/* 高速移動モードへ */
					break;
				case PLAYER_SEQ_ALIVE:
					if (player->b_button == TIME_HIGHSPEED)
						player->high_speed = PLAYER_HIGHSPEED;	/* 高速移動モードへ */
					if (player->b_button > TIME_HOLD) {
						player->seq = PLAYER_SEQ_BARRIER;	/* バリアを張る */
						player->muteki = TIME_BARRIER;
					}
					break;
				case PLAYER_SEQ_BARRIER:
				    /* 既に無敵状態 */
					if (player->b_button > 10) {
						player->b_button = 10;
					}
				default:
					break;
				}
				if ((player->b_button > 10) && (player->shot_timer > 0))
					player->shot_timer--;
			}
		} else {
		    /* Ｂボタンが押されなかった時の処理 */
			player->b_button = 0;
			player->high_speed = PLAYER_SPEED;
			if (player->seq == PLAYER_SEQ_BARRIER)
				player->seq = PLAYER_SEQ_ALIVE;	/* バリア解除 */
			if (player->shot_timer > 0)
				player->shot_timer--;
		}

	    /* Ｂボタンを押した数フレーム後に出るショットの処理 */
		switch (player->shot_cont) {
		case 0:	/* ショットが出ていない時はボンバーのエネルギー補充 */
			if (player->high_speed != PLAYER_HIGHSPEED) {
				if (player->bomb_energy < 3 * 65536) {
					if (player->bomb_energy / 65536 < ((player->bomb_energy += (256 + 16 - player->y) / 4 + 16) / 65536))
						SoundSetSE (SE_BOMB_CHARGE);
				} else {
					player->bomb_energy = 3 * 65536;
				}
			}
			break;
		case 4:
			SoundSetSE (SE_SHOT);	/* プレイヤーショット音 */
			ShotInit (192 - (SHOT_TIMER_MAX - player->shot_timer_current) * 1);
			ShotInit (192 + (SHOT_TIMER_MAX - player->shot_timer_current) * 1);
			player->shot_cont--;
			break;
		case 1:
			SoundSetSE (SE_SHOT);	/* プレイヤーショット音 */
			ShotInit (192 - (SHOT_TIMER_MAX - player->shot_timer_current) * 2);
			ShotInit (192 + (SHOT_TIMER_MAX - player->shot_timer_current) * 2);
			player->shot_cont--;
			break;
		default:
			player->shot_cont--;
			break;
		}

	    /* Ａボタン（ボンバー）が押されたか？ */
		if ((s & 0b0100000) == 0) {
			if (!player->a_button) {	/* 前回もＡボタンが押されていたか？ */
				if (e_option)
					EntryEdit ();
				if (player->blaser == 0) {	/* ボム発動中でなければ */
					player->a_button = !0;

				    /* ボム選択 */
					switch (player->bomb_type) {
					case BOMB_NON:	/* 初回 */
						if (player->bomb_energy > 65536) {
							player->bomb_energy -= 65536;
							player->seq = PLAYER_SEQ_MUTEKI;	/* 無敵になる */
							player->muteki = TIME_BOMBER;
							player->bomb_flash_state = BG_FLASH_NON;
							player->bomb_flash_timer = 15;	/* 必ず奇数 */
							if (!eshot_erase)
								eshot_erase = ESHOT_ERASE;	/* 弾消し */
							player->bomb_type = BOMB_A;
						    /* ボム文字列 */
							EffectInit (EFFECT_BOMBSTRA, 0, player->x, player->y);
							SoundSetSE (SE_BOMB);
						}
						break;
					case BOMB_A:	/* BOMBER */
						player->bomb_type = BOMB_B;
					    /* ボム文字列 */
						EffectInit (EFFECT_BOMBSTRB, 0, player->x, player->y);
						break;
					case BOMB_B:	/* BACK-B */
						player->bomb_type = BOMB_C;
					    /* ボム文字列 */
						EffectInit (EFFECT_BOMBSTRC, 0, player->x, player->y);
						break;
					case BOMB_C:	/* LASER */
						player->bomb_type = BOMB_A;
					    /* ボム文字列 */
						EffectInit (EFFECT_BOMBSTRA, 0, player->x, player->y);
						break;
					}
				}
			}
		} else {
		    /* Ａボタンが押されなかった時の処理 */
			player->a_button = 0;
		}
	    /* ボムの処理 */
		if (player->bomb_flash_timer) {
		    /* 背景白フラッシュ処理 */
			player->bomb_flash_timer--;
			player->bomb_flash_state++;
			if (player->bomb_flash_state > BG_FLASH_NORMAL)
				player->bomb_flash_state = BG_FLASH_WHITE;
			if (player->bomb_flash_timer == 0)
				player->bomb_flash_state = BG_FLASH_NON;
		}
	    /* 自機の移動処理 */
		switch (s & 0b1111) {
		case 0b1110:	/* 上 */
			player->ly += vytable[player->high_speed][192];
			break;
		case 0b1101:	/* 下 */
			player->ly += vytable[player->high_speed][64];
			break;
		case 0b1011:	/* 左 */
			xmove_flag = 1;
			player->lx += vxtable[player->high_speed][128];
			break;
		case 0b0111:	/* 右 */
			xmove_flag = 2;
			player->lx += vxtable[player->high_speed][0];
			break;
		case 0b1010:	/* 左上 */
			xmove_flag = 1;
			player->lx += vxtable[player->high_speed][160];
			player->ly += vytable[player->high_speed][160];
			break;
		case 0b1001:	/* 左下 */
			xmove_flag = 1;
			player->lx += vxtable[player->high_speed][96];
			player->ly += vytable[player->high_speed][96];
			break;
		case 0b0110:	/* 右上 */
			xmove_flag = 2;
			player->lx += vxtable[player->high_speed][224];
			player->ly += vytable[player->high_speed][224];
			break;
		case 0b0101:	/* 右下 */
			xmove_flag = 2;
			player->lx += vxtable[player->high_speed][32];
			player->ly += vytable[player->high_speed][32];
			break;

		default:
			break;
		}


		{
			signed short plx = (*((short *) (&player->lx)));
			signed short ply = (*((short *) (&player->ly)));
			if (ply < PLAYER_MINY)
				player->ly = PLAYER_MINY << 16;
			if (ply > PLAYER_MAXY)
				player->ly = PLAYER_MAXY << 16;
			if (plx < PLAYER_MINX)
				player->lx = PLAYER_MINX << 16;
			if (plx > PLAYER_MAXX)
				player->lx = PLAYER_MAXX << 16;
		}

		switch (xmove_flag) {
		case 0:	/* X 方向の移動がなかった場合は */
		    /* 自機の傾きを元に戻す処理 */
			if (player->bank != 0) {
				if (player->bank > 0)
					player->bank--;
				else
					player->bank++;
			}
			break;
		case 1:	/* 左 */
			if (player->bank > 0) {
				player->bank = 0;
			} else {
				if (player->bank > -PLAYER_BANK2)
					player->bank--;
			}
			break;
		case 2:	/* 右 */
			if (player->bank < 0) {
				player->bank = 0;
			} else {
				if (player->bank < PLAYER_BANK2)
					player->bank++;
			}
		}

		if (player->bank == 0) {
			player->pt = obj_player + PLAYER_PT_CENTER;
		} else {
			if (player->bank > 0) {
				if (player->bank < PLAYER_BANK1)
					player->pt = obj_player + PLAYER_PT_LEFT1;
				else
					player->pt = obj_player + PLAYER_PT_LEFT2;
			} else {
				if (player->bank > -PLAYER_BANK1)
					player->pt = obj_player + PLAYER_PT_RIGHT1;
				else
					player->pt = obj_player + PLAYER_PT_RIGHT2;
			}
		}

	    /* 無敵状態の処理 */
		player->info = PALET_PLAYER | PRIORITY_PLAYER;
		if ((player->seq == PLAYER_SEQ_MUTEKI) || (player->seq == PLAYER_SEQ_BARRIER)) {
			player->muteki--;
			player->flash++;
			if (player->muteki > 55) {
			    /* バリアが沢山残っているので速いフラッシュ */
				if (player->flash > 1) {
					player->info = PALET_PLAYER_FLASH | PRIORITY_PLAYER;
					player->flash = 0;
				}
			} else {
			    /* バリアが少ししかないので遅いフラッシュ */
				if (player->flash > 5)
					player->info = PALET_PLAYER_FLASH | PRIORITY_PLAYER;
				if (player->flash > 10)
					player->flash = 0;
				if (player->muteki <= 0)
					player->seq = PLAYER_SEQ_ALIVE;	/* バリア切れ */
			}
		}
		player->x = (*((short *) (&player->lx)));
		player->y = (*((short *) (&player->ly)));
	    /* 複合スプライト登録 */
		xobj_set_st (player);
		break;

	case PLAYER_SEQ_APPEAR:
		if (player->seq2++ == 0) {
		    /* 今出現したところ */
			player->lx = (128 + 16) << 16;	/* Ｘ座標初期値 */
			player->ly = (256 + 32) << 16;	/* Ｙ座標初期値 */
			player->bank = 0;
			player->shot_timer = 0;
			player->shot_cont = 0;
			player->a_button = 0;
			player->b_button = 0;
		} else {
			player->ly += vytable[PLAYER_SPEED_APPEAR][192];
			if (player->ly <= (192 << 16)) {
				player->seq = PLAYER_SEQ_MUTEKI;
				player->seq2 = 0;
				player->muteki = TIME_MUTEKI;
			}
		}
		if (player->flash++ > 0) {
			player->info = PRIORITY_PLAYER | PALET_PLAYER_FLASH;
			player->flash = 0;
		} else {
			player->info = PRIORITY_PLAYER | PALET_PLAYER;
		}

	    /* 複合スプライト登録 */
		player->x = (*((short *) (&player->lx)));
		player->y = (*((short *) (&player->ly)));
		player->pt = obj_player + PLAYER_PT_CENTER;
		xobj_set_st (player);
		break;

	case PLAYER_SEQ_LAST:	/* オールクリア */
	    /* 自機の傾きを元に戻す処理 */
		if (player->bank != 0) {
			if (player->bank > 0)
				player->bank--;
			else
				player->bank++;
		}
		if (player->bank == 0) {
			player->pt = obj_player + PLAYER_PT_CENTER;
		} else {
			if (player->bank > 0) {
				if (player->bank < PLAYER_BANK1)
					player->pt = obj_player + PLAYER_PT_LEFT1;
				else
					player->pt = obj_player + PLAYER_PT_LEFT2;
			} else {
				if (player->bank > -PLAYER_BANK1)
					player->pt = obj_player + PLAYER_PT_RIGHT1;
				else
					player->pt = obj_player + PLAYER_PT_RIGHT2;
			}
		}
		if (player->seq2++ > 55) {
			player->seq2 = 56;
			player->ly += vytable[PLAYER_SPEED_LAST][192];
			player->info = PRIORITY_PLAYER | PALET_PLAYER;
			if (player->y < -16)
				player->game_over = GAME_COMPLETE;
		}
	    /* 複合スプライト登録 */
		player->x = (*((short *) (&player->lx)));
		player->y = (*((short *) (&player->ly)));
		player->pt = obj_player + PLAYER_PT_CENTER;
		xobj_set_st (player);
		break;

	case PLAYER_SEQ_DEAD:
	    /* 自機の傾きを元に戻す処理 */
		if (player->bank != 0) {
			if (player->bank > 0)
				player->bank--;
			else
				player->bank++;
		}
		if (player->bank == 0) {
			player->pt = obj_player + PLAYER_PT_CENTER;
		} else {
			if (player->bank > 0) {
				if (player->bank < PLAYER_BANK1)
					player->pt = obj_player + PLAYER_PT_LEFT1;
				else
					player->pt = obj_player + PLAYER_PT_LEFT2;
			} else {
				if (player->bank > -PLAYER_BANK1)
					player->pt = obj_player + PLAYER_PT_RIGHT1;
				else
					player->pt = obj_player + PLAYER_PT_RIGHT2;
			}
		}
		switch (player->seq2++) {
		case 0:
		    /* 今死んだところなら爆発パターンを出す */
			EffectInit (EFFECT_EXPLPLAYER, 0, player->x, player->y);
			SoundSetSE (SE_PLAYER_EXPL);	/* プレイヤーの爆発音 */
			player->pt = obj_player + PLAYER_PT_CENTER;
			player->bomb_type = BOMB_NON;
			player->blaser = 0;
			break;
		case 8 * 1:
		case 8 * 2:
		case 8 * 3:
		case 8 * 4:
		case 8 * 5:
			{
				signed short x, y;
				x = rndtable[(player->seq2) & 0xff ^ 0x55] - 128;
				y = rndtable[(player->seq2 + 1) & 0xff ^ 0xaa] - 128;
			    /* player->seq2 は乱数の種として使用しています */
				EffectInit (EFFECT_EXPLPLAYER, 0, player->x + x / 8, player->y + y / 8);
				SoundSetSE (SE_PLAYER_EXPL);	/* プレイヤーの爆発音 */
			}
			break;

		case 8 * 7:
		    /* 最後の「どっかーん」 */
			EffectInit (EFFECT_EXPLPLAYER, 0, player->x, player->y);
			SoundSetSE (SE_EXPL_L);
			player->x = 256 + 32;
			player->y = 256 + 32;
			break;
		case 8 * 12:
			player->seq = PLAYER_SEQ_APPEAR;
			player->bomb_energy += 65536;	/* ボンバー補充 */
			if (player->bomb_energy > 3 * 65536)
				player->bomb_energy = 3 * 65536;
			player->seq2 = 0;
			player->rest--;
			if (player->rest <= 0) {
				player->game_over = GAME_OVER;
			} else {
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* 弾消し */
			}
			DispRest ();	/* 残機数表示 */
			break;

		default:
			if (player->seq2 < 55)
				xobj_set_st (player);
			break;
		}
	}

    /* エクステンドしたか？ */
	if (extend_score < 0) {
		extend_score += EXTEND_EVERY;
		player->rest++;
		SoundSetSE (SE_EXTEND);	/* エクステンド音 */
		DispRest ();	/* 残機数表示 */
	}
    /* 各種キー入力 */
	if ((s & 0b1111) == 0) {/* [SELECT]+[START] */
		player->game_over = GAME_ABORT;
		return;
	}
	if (replay_flag) {
		s = _iocs_joyget (0);
		if (((s & 0b01000000) == 0)	/* リプレイ時にボタンが押されたら終了 */
		    ||((s & 0b1111) == 0)) {	/* [SELECT]+[START] */
			player->game_over = GAME_ABORT;
			return;
		}
	}
	if (_iocs_bitsns (0x0c) & 0b10) {
	    /* [BREAK] */
		while (_iocs_bitsns (0x0c) & 0b10);
		player->game_over = GAME_ABORT;
		return;
	}
    /* ポーズ処理 */
	if ((_iocs_bitsns (0x00) & 0b10)	/* [ESC] */
	    ||((replay_flag == REPLAY_NON) && ((s & 0b1100) == 0))	/* ゲーム中に [START] */
	    ||((replay_flag) && ((_iocs_joyget (0) & 0b1100) == 0))) {	/* リプレイ中に [START] */
		pause_flag = !0;
		while ((_iocs_bitsns (0x00) & 0b10) || ((_iocs_joyget (0) & 0b1100) == 0));
		while ((!(_iocs_bitsns (0x00) & 0b10)) && (!((_iocs_joyget (0) & 0b1100) == 0)));
		while ((_iocs_bitsns (0x00) & 0b10) || ((_iocs_joyget (0) & 0b1100) == 0));
		pause_flag = 0;
	}
    /* ボンバーゲージの表示 */
	if (player->bomb_guage_disp < GUAGE_DISP_TIME) {
	    /* 現在非表示の時 */
		if (player->y < GUAGE_ERASE_Y) {
			player->bomb_guage_disp++;
			if (player->bomb_guage_disp == GUAGE_DISP_TIME) {
			    /* ゲージを表示する処理（毎回書き換えない所） */
				TxfontCursor (GUAGE_X + 7, GUAGE_Y - 1);
				TxfontPuts ("~~~|~~~|~~~|");
				TxfontCursor (GUAGE_X, GUAGE_Y);
				TxfontPuts ("BOMBER{");
				TxfontCursor (GUAGE_X + 19, GUAGE_Y);
				TxfontPuts ("}");
			}
		} else {
			player->bomb_guage_disp = 0;
		}
	} else {
	    /* 現在表示中の時 */
		if (player->y >= GUAGE_ERASE_Y) {
			player->bomb_guage_disp = 0;
		    /* ゲージを消す処理 */
			TxfontCursor (GUAGE_X + 7, GUAGE_Y - 1);
			TxfontPuts ("            ");
			TxfontCursor (GUAGE_X, GUAGE_Y);
			TxfontPuts ("                    ");
		}
	}

	if (player->bomb_guage_disp >= GUAGE_DISP_TIME) {
		short h;
		char f = 0;
		signed int t = player->bomb_energy;
		TxfontCursor (GUAGE_X + 7, GUAGE_Y);
		for (h = 0; h < 12; h++) {
			t -= 65536 / 4;
			if (t > 0) {
				TxfontPuts ("h");
			} else {
				if (f == 0) {
					TxfontPutchar ('`' + ((t + 65536 / 4) / 2048));
					f = !0;
				} else {
					TxfontPutchar (' ');
				}
			}
		}
	}
}



/* プレイヤー消去時に呼ばれる（ゲームオーバー時等） */
void PlayerTini (void)
{
}



/* エントリー編集モード */
void EntryEdit (void)
{
	typedef struct {
		short x, y, pt, info;
	} CURSOR;
	CURSOR cursor[1];
	int i, s;
	char exit_flag = 0;
	static int type = 0;
	char temp_str[256];
	int name_last = sizeof (enemy_name) / sizeof (char *);

	TxfontCursor (16, 0);
	sprintf (temp_str, "ENTRY = %3hd  ", entry_counter);
	TxfontPuts (temp_str);

	TxfontCursor (16, 3);
	sprintf (temp_str, "NAME_LAST = %3hd  ", name_last);
	TxfontPuts (temp_str);

	cursor->x = player->x - 8;
	cursor->y = player->y - 8;
	cursor->pt = sp_cursor;
	cursor->info = PALET_CURSOR | PRIORITY_CURSOR;

    /* Ａボタンが離されるのを待つ */
	while ((_iocs_joyget (0) & 0b0100000) == 0);
	//while ((_iocs_joyget (0) & 0xff) != 0xff);

	do {
		s = _iocs_joyget (0);
		cursor->pt = sp_cursor;

	    /* カーソルの移動処理 */
		if (((s & 0b0001) == 0) && (cursor->y > PLAYER_MINY - 8))
		    /* 上 */
			cursor->y--;
		if (((s & 0b0010) == 0) && (cursor->y < PLAYER_MAXY - 8))
		    /* 下 */
			cursor->y++;
		if (((s & 0b0100) == 0)
		    && (cursor->x > PLAYER_MINX - 8))	/* 左 */
			cursor->x--;
		if (((s & 0b1000) == 0)
		    && (cursor->x < PLAYER_MAXX - 8))	/* 右 */
			cursor->x++;

	    /* Ｂボタンが押されたか？ */
		if ((s & 0b1000000) == 0) {
			while ((_iocs_joyget (0) & 0b1000000) == 0);
			TxfontCursor (16, 1);
			sprintf (temp_str, "%s", enemy_name[type]);
			TxfontPuts (temp_str);
			do {
				while ((s = (_iocs_joyget (0) & 0xff)) == 0xff);

				if (((s & 0b0001) == 0) && (type > 0))	/* 上 */
					type--;
				if (((s & 0b0010) == 0) && (type < name_last - 1))	/* 下 */
					type++;
				TxfontCursor (16, 1);
				TxfontPuts ("                ");
				TxfontCursor (16, 1);
				sprintf (temp_str, "%s", enemy_name[type]);
				TxfontPuts (temp_str);

			    /* Ｂボタンが押されたか？ */
				if ((s & 0b1000000) == 0) {
					short t;
				    /* エントリーを１つづつ後ろにずらす */
					for (i = ENTRY_MAX; i > entry_counter; i--)
						stage_data[stage][i] = stage_data[stage][i - 1];
					stage_data[stage][entry_counter].type = type;
					stage_data[stage][entry_counter].x = cursor->x;
					stage_data[stage][entry_counter].y = cursor->y;
					t = stage_data[stage][entry_counter].timer;
					stage_data[stage][entry_counter].timer = entry_timer;
					stage_data[stage][entry_counter + 1].timer = t - entry_timer;

					cursor->pt = sp_cursor + 1;
					xsp_set_st (cursor);
					xsp_out ();
					exit_flag = !0;
				}
			    /* Ａボタンが押されたか？ */
				if ((s & 0b0100000) == 0) {
					exit_flag = !0;
				}
				while ((_iocs_joyget (0) & 0xff) != 0xff);
			} while (!exit_flag);
			TxfontCursor (16, 1);
			TxfontPuts ("                ");
			exit_flag = 0;
		}
	    /* Ａボタンが押されたか？ */
		if ((s & 0b0100000) == 0) {
			exit_flag = !0;
		}
		xsp_set_st (cursor);
		xsp_out ();
	} while (!exit_flag);
	while ((_iocs_joyget (0) & 0xff) != 0xff);
}
