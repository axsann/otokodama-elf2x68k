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


/* ���@�̓����蔻�� */
#define PLAYER_HIT_X	2
#define PLAYER_HIT_Y	3

#define PLAYER_SPEED	10
#define PLAYER_HIGHSPEED	25

#define PLAYER_SPEED_APPEAR	22	/* �o�ꎞ�̑��x */
#define PLAYER_SPEED_LAST	25	/* �I�[���N���A���̑��x */

#define PLAYER_MINX		(16+8)	/* X ���W�ŏ��l */
#define PLAYER_MAXX		(16+255-8)
#define PLAYER_MINY		(16+16)	/* Y ���W�ŏ��l */
#define PLAYER_MAXY		(16+255-12)

#define PLAYER_BANK1	8	/* �����X���p�^�[���ֈڍs����܂ł̒l */
#define PLAYER_BANK2	12	/* ��ԌX���p�^�[���ֈȉ����� */


#ifndef NULL
#define NULL ((void *) 0)
#endif

#define	TIME_MUTEKI	90	/* �o�ꎞ�̖��G���� */
#define	TIME_BARRIER	110	/* �o���A�g�p���̖��G���� */
#define TIME_HIGHSPEED	10	/* ���@�����ړ��܂ł̎��� */
#define	TIME_HOLD	110	/* �o���A�����܂łɗv���鎞�� */
#define	TIME_BOMBER	90	/* �{���o�[�g�p���̖��G���� */


#define	GUAGE_X		7	/* �{���o�[�Q�[�W�\���ʒu */
#define	GUAGE_Y		31
#define	GUAGE_ERASE_Y		(228+16)	/* ���@�������艺�Ȃ��\�� */
#define	GUAGE_DISP_TIME		55	/* �Q�[�W�\���܂ł̎��� */


static void EntryEdit (void);



/* �N�����ɂP�x�����Ă΂�� */
void PlayerInit0 (void)
{
}


/* �c�@��\�� */
void DispRest (void)
{
	short h;

#define REST_DISP_X	2
#define REST_DISP_Y	1
	TxfontCursor (REST_DISP_X, REST_DISP_Y);
	for (h = 0; h < 10; h++) {
		if (h < player->rest - 1)
			TxfontPutchar ('i');	/* 'i' �Ɏ��@�̃}�[�N�����蓖�Ă��Ă܂� */
		else
			TxfontPutchar (' ');
	};
}



/* �v���C���[�o�����ɌĂ΂��i�Q�[���J�n���j */
void PlayerInit (void)
{
	player->info = PALET_PLAYER | PRIORITY_PLAYER;	/* ���]�R�[�h�E�F�E�D��x��\�킷�f�[�^ */
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



/* �����������ƂɌĂ΂�� */
void PlayerMove (void)
{
	int s = -1;		/* �W���C�X�e�B�b�N�̏�� */
	char xmove_flag = 0;	/* X �����Ɉړ������������Ƃ̃t���O */

	switch (replay_flag) {
	case REPLAY_NON:	/* ���v���C���ł͂Ȃ��i�ʏ�̃Q�[�����j */
		s = _iocs_joyget (0);
		if (player->replay_counter < REPLAY_DATA_SIZE_MAX)
			replay_data[player->replay_counter++] = s;
		break;
	case REPLAY_BEFORE:	/* ���O�̃Q�[���̃��v���C�� */
		s = replay_data[player->replay_counter++];
		break;
	case REPLAY_HIGH:	/* �n�C�X�R�A�̃��v���C�� */
		s = high_score_replay_data[player->replay_counter++];
		break;
	}

	if (player->blaser)
		player->blaser--;
    /* enemy.c �ŎQ�Ƃ���{���o�[�����蔻��֘A */
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
	    /* �a�{�^���i�V���b�g�j�������ꂽ���H */
		if ((s & 0b1000000) == 0) {
			if (!player->b_button) {
			    /* �O��a�{�^����������Ă��Ȃ��������̏��� */
				player->b_button++;

				if (player->bomb_type == BOMB_NON) {
					if (player->blaser == 0) {
					    /* �V���b�g�̏��� */
#define SHOT_TIMER_ADD	6
#define SHOT_TIMER_MAX	18
						if (player->shot_cont == 0) {
							player->shot_timer += SHOT_TIMER_ADD;
							if (player->shot_timer > SHOT_TIMER_MAX)
								player->shot_timer = SHOT_TIMER_MAX;

							ShotInit (192);
							SoundSetSE (SE_SHOT);	/* �v���C���[�V���b�g�� */
							player->shot_cont = 7;
							player->shot_timer_current = player->shot_timer;
						} else {
							player->b_button_next = !0;
						}
					}
				} else {
				    /* �{���������ɂa�{�^���������ꂽ�i�{���m��j */
					switch (player->bomb_type) {
					case BOMB_A:
						EffectInit (EFFECT_BOMBER, 0, player->x, player->y);
						player->bomb_flash_state = BG_FLASH_NON;
						player->bomb_flash_timer = 5;	/* �K��� */
						player->bomb_hitcheck = BOMB_A;
						player->bomb_hitcheck_time = TIME_BOMBER_HITCHECK;
						if (!eshot_erase)
							eshot_erase = ESHOT_ERASE;	/* �e���� */
						SoundSetSE (SE_BOMB_A);
						break;
					case BOMB_B:
						EffectInit (EFFECT_BACKB, 0, player->x, player->y);
						player->bomb_flash_state = BG_FLASH_NON;
						player->bomb_flash_timer = 5;	/* �K��� */
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
			    /* �O����a�{�^����������Ă������̏��� */
				player->b_button++;
				switch (player->seq) {
				case PLAYER_SEQ_MUTEKI:
					if (player->b_button == TIME_HIGHSPEED)
						player->high_speed = PLAYER_HIGHSPEED;	/* �����ړ����[�h�� */
					break;
				case PLAYER_SEQ_ALIVE:
					if (player->b_button == TIME_HIGHSPEED)
						player->high_speed = PLAYER_HIGHSPEED;	/* �����ړ����[�h�� */
					if (player->b_button > TIME_HOLD) {
						player->seq = PLAYER_SEQ_BARRIER;	/* �o���A�𒣂� */
						player->muteki = TIME_BARRIER;
					}
					break;
				case PLAYER_SEQ_BARRIER:
				    /* ���ɖ��G��� */
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
		    /* �a�{�^����������Ȃ��������̏��� */
			player->b_button = 0;
			player->high_speed = PLAYER_SPEED;
			if (player->seq == PLAYER_SEQ_BARRIER)
				player->seq = PLAYER_SEQ_ALIVE;	/* �o���A���� */
			if (player->shot_timer > 0)
				player->shot_timer--;
		}

	    /* �a�{�^�������������t���[����ɏo��V���b�g�̏��� */
		switch (player->shot_cont) {
		case 0:	/* �V���b�g���o�Ă��Ȃ����̓{���o�[�̃G�l���M�[��[ */
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
			SoundSetSE (SE_SHOT);	/* �v���C���[�V���b�g�� */
			ShotInit (192 - (SHOT_TIMER_MAX - player->shot_timer_current) * 1);
			ShotInit (192 + (SHOT_TIMER_MAX - player->shot_timer_current) * 1);
			player->shot_cont--;
			break;
		case 1:
			SoundSetSE (SE_SHOT);	/* �v���C���[�V���b�g�� */
			ShotInit (192 - (SHOT_TIMER_MAX - player->shot_timer_current) * 2);
			ShotInit (192 + (SHOT_TIMER_MAX - player->shot_timer_current) * 2);
			player->shot_cont--;
			break;
		default:
			player->shot_cont--;
			break;
		}

	    /* �`�{�^���i�{���o�[�j�������ꂽ���H */
		if ((s & 0b0100000) == 0) {
			if (!player->a_button) {	/* �O����`�{�^����������Ă������H */
				if (e_option)
					EntryEdit ();
				if (player->blaser == 0) {	/* �{���������łȂ���� */
					player->a_button = !0;

				    /* �{���I�� */
					switch (player->bomb_type) {
					case BOMB_NON:	/* ���� */
						if (player->bomb_energy > 65536) {
							player->bomb_energy -= 65536;
							player->seq = PLAYER_SEQ_MUTEKI;	/* ���G�ɂȂ� */
							player->muteki = TIME_BOMBER;
							player->bomb_flash_state = BG_FLASH_NON;
							player->bomb_flash_timer = 15;	/* �K��� */
							if (!eshot_erase)
								eshot_erase = ESHOT_ERASE;	/* �e���� */
							player->bomb_type = BOMB_A;
						    /* �{�������� */
							EffectInit (EFFECT_BOMBSTRA, 0, player->x, player->y);
							SoundSetSE (SE_BOMB);
						}
						break;
					case BOMB_A:	/* BOMBER */
						player->bomb_type = BOMB_B;
					    /* �{�������� */
						EffectInit (EFFECT_BOMBSTRB, 0, player->x, player->y);
						break;
					case BOMB_B:	/* BACK-B */
						player->bomb_type = BOMB_C;
					    /* �{�������� */
						EffectInit (EFFECT_BOMBSTRC, 0, player->x, player->y);
						break;
					case BOMB_C:	/* LASER */
						player->bomb_type = BOMB_A;
					    /* �{�������� */
						EffectInit (EFFECT_BOMBSTRA, 0, player->x, player->y);
						break;
					}
				}
			}
		} else {
		    /* �`�{�^����������Ȃ��������̏��� */
			player->a_button = 0;
		}
	    /* �{���̏��� */
		if (player->bomb_flash_timer) {
		    /* �w�i���t���b�V������ */
			player->bomb_flash_timer--;
			player->bomb_flash_state++;
			if (player->bomb_flash_state > BG_FLASH_NORMAL)
				player->bomb_flash_state = BG_FLASH_WHITE;
			if (player->bomb_flash_timer == 0)
				player->bomb_flash_state = BG_FLASH_NON;
		}
	    /* ���@�̈ړ����� */
		switch (s & 0b1111) {
		case 0b1110:	/* �� */
			player->ly += vytable[player->high_speed][192];
			break;
		case 0b1101:	/* �� */
			player->ly += vytable[player->high_speed][64];
			break;
		case 0b1011:	/* �� */
			xmove_flag = 1;
			player->lx += vxtable[player->high_speed][128];
			break;
		case 0b0111:	/* �E */
			xmove_flag = 2;
			player->lx += vxtable[player->high_speed][0];
			break;
		case 0b1010:	/* ���� */
			xmove_flag = 1;
			player->lx += vxtable[player->high_speed][160];
			player->ly += vytable[player->high_speed][160];
			break;
		case 0b1001:	/* ���� */
			xmove_flag = 1;
			player->lx += vxtable[player->high_speed][96];
			player->ly += vytable[player->high_speed][96];
			break;
		case 0b0110:	/* �E�� */
			xmove_flag = 2;
			player->lx += vxtable[player->high_speed][224];
			player->ly += vytable[player->high_speed][224];
			break;
		case 0b0101:	/* �E�� */
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
		case 0:	/* X �����̈ړ����Ȃ������ꍇ�� */
		    /* ���@�̌X�������ɖ߂����� */
			if (player->bank != 0) {
				if (player->bank > 0)
					player->bank--;
				else
					player->bank++;
			}
			break;
		case 1:	/* �� */
			if (player->bank > 0) {
				player->bank = 0;
			} else {
				if (player->bank > -PLAYER_BANK2)
					player->bank--;
			}
			break;
		case 2:	/* �E */
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

	    /* ���G��Ԃ̏��� */
		player->info = PALET_PLAYER | PRIORITY_PLAYER;
		if ((player->seq == PLAYER_SEQ_MUTEKI) || (player->seq == PLAYER_SEQ_BARRIER)) {
			player->muteki--;
			player->flash++;
			if (player->muteki > 55) {
			    /* �o���A����R�c���Ă���̂ő����t���b�V�� */
				if (player->flash > 1) {
					player->info = PALET_PLAYER_FLASH | PRIORITY_PLAYER;
					player->flash = 0;
				}
			} else {
			    /* �o���A�����������Ȃ��̂Œx���t���b�V�� */
				if (player->flash > 5)
					player->info = PALET_PLAYER_FLASH | PRIORITY_PLAYER;
				if (player->flash > 10)
					player->flash = 0;
				if (player->muteki <= 0)
					player->seq = PLAYER_SEQ_ALIVE;	/* �o���A�؂� */
			}
		}
		player->x = (*((short *) (&player->lx)));
		player->y = (*((short *) (&player->ly)));
	    /* �����X�v���C�g�o�^ */
		xobj_set_st (player);
		break;

	case PLAYER_SEQ_APPEAR:
		if (player->seq2++ == 0) {
		    /* ���o�������Ƃ��� */
			player->lx = (128 + 16) << 16;	/* �w���W�����l */
			player->ly = (256 + 32) << 16;	/* �x���W�����l */
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

	    /* �����X�v���C�g�o�^ */
		player->x = (*((short *) (&player->lx)));
		player->y = (*((short *) (&player->ly)));
		player->pt = obj_player + PLAYER_PT_CENTER;
		xobj_set_st (player);
		break;

	case PLAYER_SEQ_LAST:	/* �I�[���N���A */
	    /* ���@�̌X�������ɖ߂����� */
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
	    /* �����X�v���C�g�o�^ */
		player->x = (*((short *) (&player->lx)));
		player->y = (*((short *) (&player->ly)));
		player->pt = obj_player + PLAYER_PT_CENTER;
		xobj_set_st (player);
		break;

	case PLAYER_SEQ_DEAD:
	    /* ���@�̌X�������ɖ߂����� */
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
		    /* �����񂾂Ƃ���Ȃ甚���p�^�[�����o�� */
			EffectInit (EFFECT_EXPLPLAYER, 0, player->x, player->y);
			SoundSetSE (SE_PLAYER_EXPL);	/* �v���C���[�̔����� */
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
			    /* player->seq2 �͗����̎�Ƃ��Ďg�p���Ă��܂� */
				EffectInit (EFFECT_EXPLPLAYER, 0, player->x + x / 8, player->y + y / 8);
				SoundSetSE (SE_PLAYER_EXPL);	/* �v���C���[�̔����� */
			}
			break;

		case 8 * 7:
		    /* �Ō�́u�ǂ����[��v */
			EffectInit (EFFECT_EXPLPLAYER, 0, player->x, player->y);
			SoundSetSE (SE_EXPL_L);
			player->x = 256 + 32;
			player->y = 256 + 32;
			break;
		case 8 * 12:
			player->seq = PLAYER_SEQ_APPEAR;
			player->bomb_energy += 65536;	/* �{���o�[��[ */
			if (player->bomb_energy > 3 * 65536)
				player->bomb_energy = 3 * 65536;
			player->seq2 = 0;
			player->rest--;
			if (player->rest <= 0) {
				player->game_over = GAME_OVER;
			} else {
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* �e���� */
			}
			DispRest ();	/* �c�@���\�� */
			break;

		default:
			if (player->seq2 < 55)
				xobj_set_st (player);
			break;
		}
	}

    /* �G�N�X�e���h�������H */
	if (extend_score < 0) {
		extend_score += EXTEND_EVERY;
		player->rest++;
		SoundSetSE (SE_EXTEND);	/* �G�N�X�e���h�� */
		DispRest ();	/* �c�@���\�� */
	}
    /* �e��L�[���� */
	if ((s & 0b1111) == 0) {/* [SELECT]+[START] */
		player->game_over = GAME_ABORT;
		return;
	}
	if (replay_flag) {
		s = _iocs_joyget (0);
		if (((s & 0b01000000) == 0)	/* ���v���C���Ƀ{�^���������ꂽ��I�� */
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
    /* �|�[�Y���� */
	if ((_iocs_bitsns (0x00) & 0b10)	/* [ESC] */
	    ||((replay_flag == REPLAY_NON) && ((s & 0b1100) == 0))	/* �Q�[������ [START] */
	    ||((replay_flag) && ((_iocs_joyget (0) & 0b1100) == 0))) {	/* ���v���C���� [START] */
		pause_flag = !0;
		while ((_iocs_bitsns (0x00) & 0b10) || ((_iocs_joyget (0) & 0b1100) == 0));
		while ((!(_iocs_bitsns (0x00) & 0b10)) && (!((_iocs_joyget (0) & 0b1100) == 0)));
		while ((_iocs_bitsns (0x00) & 0b10) || ((_iocs_joyget (0) & 0b1100) == 0));
		pause_flag = 0;
	}
    /* �{���o�[�Q�[�W�̕\�� */
	if (player->bomb_guage_disp < GUAGE_DISP_TIME) {
	    /* ���ݔ�\���̎� */
		if (player->y < GUAGE_ERASE_Y) {
			player->bomb_guage_disp++;
			if (player->bomb_guage_disp == GUAGE_DISP_TIME) {
			    /* �Q�[�W��\�����鏈���i���񏑂������Ȃ����j */
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
	    /* ���ݕ\�����̎� */
		if (player->y >= GUAGE_ERASE_Y) {
			player->bomb_guage_disp = 0;
		    /* �Q�[�W���������� */
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



/* �v���C���[�������ɌĂ΂��i�Q�[���I�[�o�[�����j */
void PlayerTini (void)
{
}



/* �G���g���[�ҏW���[�h */
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

    /* �`�{�^�����������̂�҂� */
	while ((_iocs_joyget (0) & 0b0100000) == 0);
	//while ((_iocs_joyget (0) & 0xff) != 0xff);

	do {
		s = _iocs_joyget (0);
		cursor->pt = sp_cursor;

	    /* �J�[�\���̈ړ����� */
		if (((s & 0b0001) == 0) && (cursor->y > PLAYER_MINY - 8))
		    /* �� */
			cursor->y--;
		if (((s & 0b0010) == 0) && (cursor->y < PLAYER_MAXY - 8))
		    /* �� */
			cursor->y++;
		if (((s & 0b0100) == 0)
		    && (cursor->x > PLAYER_MINX - 8))	/* �� */
			cursor->x--;
		if (((s & 0b1000) == 0)
		    && (cursor->x < PLAYER_MAXX - 8))	/* �E */
			cursor->x++;

	    /* �a�{�^���������ꂽ���H */
		if ((s & 0b1000000) == 0) {
			while ((_iocs_joyget (0) & 0b1000000) == 0);
			TxfontCursor (16, 1);
			sprintf (temp_str, "%s", enemy_name[type]);
			TxfontPuts (temp_str);
			do {
				while ((s = (_iocs_joyget (0) & 0xff)) == 0xff);

				if (((s & 0b0001) == 0) && (type > 0))	/* �� */
					type--;
				if (((s & 0b0010) == 0) && (type < name_last - 1))	/* �� */
					type++;
				TxfontCursor (16, 1);
				TxfontPuts ("                ");
				TxfontCursor (16, 1);
				sprintf (temp_str, "%s", enemy_name[type]);
				TxfontPuts (temp_str);

			    /* �a�{�^���������ꂽ���H */
				if ((s & 0b1000000) == 0) {
					short t;
				    /* �G���g���[���P�Â��ɂ��炷 */
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
			    /* �`�{�^���������ꂽ���H */
				if ((s & 0b0100000) == 0) {
					exit_flag = !0;
				}
				while ((_iocs_joyget (0) & 0xff) != 0xff);
			} while (!exit_flag);
			TxfontCursor (16, 1);
			TxfontPuts ("                ");
			exit_flag = 0;
		}
	    /* �`�{�^���������ꂽ���H */
		if ((s & 0b0100000) == 0) {
			exit_flag = !0;
		}
		xsp_set_st (cursor);
		xsp_out ();
	} while (!exit_flag);
	while ((_iocs_joyget (0) & 0xff) != 0xff);
}
