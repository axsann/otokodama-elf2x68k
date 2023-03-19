/* �S�ʃ{�X�R�A */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../entry.h"
#include "../sound.h"
#include "../psearch.h"

#define HP_0	3000		/* �ϋv�� */
#define HP_1	1000		/* �ϋv�͂�����ȉ��ɂȂ������Q�i�K�� */

#define	TIMER_1	4000		/* ����ȏ㎞�Ԃ��o�������Q�`�Ԃ� */
#define	TIMER_2	6000		/* ����ȏ㎞�Ԃ��o�����玩�� */

#define SPEED_1		10	/* �����x */
#define SPEED_1N	5	/* �����x�𑫂��� */

#define SPEED_2		30	/* �����x */
#define SPEED_2N	15	/* �����x�𑫂��� */

#define SEQ_DEATH	5



enum {
	MOVETO_INIT = 0,	/* ������� */
	MOVETO_C2L,		/* �������獶�Ɉړ� */
	MOVETO_L2C,
	MOVETO_C2R,
	MOVETO_R2C,
};

static short EnemyMoveLBoss04C (ENEMY *);
static void EnemyTiniLBoss04C (ENEMY *);


void EnemyInitLBoss04C (ENEMY * p)
{
	p->vx = 0;
	p->vy = -8 << 16;

    /* ���蔻�� */
	p->hit_px = p->hit_py = p->hit_sx = p->hit_sy = p->hit_cx = 0;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->pt = obj_lboss04 + 9;
	p->seq = 0;
	if (standard_play)	/* �W���ݒ肩�H */
		p->core_info = PALET_CORE_RED | PRIORITY_BOSS_CORE;
	else
		p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS_CORE;

	p->func_enemy_move = EnemyMoveLBoss04C;
	p->func_enemy_tini = EnemyTiniLBoss04C;

	entry_counter_stop = !0;/* �G���g���[�J�E���^�[��~ */
}



static short EnemyMoveLBoss04C (ENEMY * p)
{
    /* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;
	p->timer++;

	switch (p->seq) {
	    /* ��ɏオ���Ă��� */
	case 0:
	    /* �{�̂�\�� */
		xobj_set (p->x, p->y + 8, obj_lboss04 + 7, PALET_LBOSS04 | PRIORITY_BOSS);
		if (p->y < 192)
			p->seq++;
		break;

	    /* ���� */
	case 1:
	    /* �{�̂�\�� */
		xobj_set (p->x, p->y + 8, obj_lboss04 + 7, PALET_LBOSS04 | PRIORITY_BOSS);
		if (p->vy < -16384) {
			p->vy += 16384;
		} else {
			p->seq++;
			p->vy = 0;
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			EffectInit (EFFECT_EXPL, 0, p->x + 32, p->y + 24);
			EffectInit (EFFECT_EXPL, 0, p->x - 32, p->y + 32);
			SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
			p->m_work = 0;
			p->hit_px = 24;
			p->hit_py = 16;
			p->hit_sx = 24;
			p->hit_sy = 16;
			p->hit_cx = 24;
		}
		break;

	    /* ���΂炭�҂� */
	case 2:
		if (p->m_work++ > 15) {
			p->seq++;
			p->m_work = MOVETO_INIT;	/* �ړ����� */
			p->m_work3 = 0;
			p->s_work = p->s_work2 = p->s_work3 = 0;
			p->s_angle = 0;
		}
		break;

	    /* �U���J�n */
	case 3:
	    /* �܂��ړ����� */
		if (p->m_work == MOVETO_INIT) {
		    /* ������ԂȂ� */
			if (p->x > player->x)
				p->m_work = MOVETO_C2L;	/* �ړ����� */
			else
				p->m_work = MOVETO_C2R;	/* �ړ����� */
			p->m_work2 = 0;
		}
		if (p->m_work2 == 0) {
		    /* ����Ȃ� */
			signed short dx, dy;
			signed short dx_table[5] =
			{0, 96 + 16, 128 + 16, 256 - 96 + 16, 128 + 16};
			short m_work_table[5] =
			{MOVETO_INIT, MOVETO_L2C, MOVETO_C2R, MOVETO_R2C, MOVETO_C2L};

			p->m_work2 = !0;
		    /* ���ɍ����������E�����������ֈړ� */
			dx = dx_table[p->m_work];
			p->m_work = m_work_table[p->m_work];

		    /* player->x �͗����̎�Ƃ��Ďg�p���Ă��܂� */
			dy = (((unsigned short) rndtable[(player->x) & 0xff]) / 8 - 16 + 80 + 16);
		    /* �ړ�������ݒ� */
			SubEnemyMoveToInit (p, dx, dy, SPEED_1, SPEED_1N);
		} else {
		    /* �ݒ�l�ɏ]���Ĉړ� */
			if (SubEnemyMoveTo (p) < 0)
				p->m_work2 = 0;	/* �ړ������Đݒ� */
		}
		if ((p->hp < HP_1) || (p->timer > TIMER_1)) {
			p->seq++;
			p->s_work = p->s_work2 = p->s_work3 = 0;
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
			if (!eshot_erase)
				eshot_erase = ESHOT_ERASE;	/* �e���� */
		}
	    /* ���ɍU������ */
		switch (p->s_work) {
		    /* �����܂��e */
		case 0:
#define INTERVAL0	2
#define SPEED_SHOT0		22
#define SPEED_SHOT0_1	16	/* �y���e�̑��x */
			if (p->s_work2++ > INTERVAL0) {
				p->s_work2 = 0;
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT0, p->s_angle, 4);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT0, p->s_angle + 64 * 1, 4);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT0, p->s_angle + 64 * 2, 4);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT0, p->s_angle + 64 * 3, 4);
				SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
				p->s_angle += 8;
				p->s_work3++;
				if (p->s_work3 >= 120) {
					p->s_work++;
					p->s_work3 = 0;
				}
			}
			break;

		    /* �\���ɂ�������] */
		case 1:
			if (p->s_work2++ > INTERVAL0) {
				p->s_work2 = 0;
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT0, p->s_angle, 4);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT0, p->s_angle + 64 * 1, 4);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT0, p->s_angle + 64 * 2, 4);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT0, p->s_angle + 64 * 3, 4);
				SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
				p->s_angle += 2;
				p->s_work3++;
				if (p->s_work3 >= 120) {
					p->s_work--;
					p->s_work3 = 0;
				}
			}
			break;
		}
		break;

	    /* �{���̍ŏI�i�K */
	case 4:
		if (p->m_work2 == 0) {
		    /* ����Ȃ� */
			signed short dx, dy;
			signed short dx_table[5] =
			{0, 54 + 16, 128 + 16, 256 - 54 + 16, 128 + 16};
			short m_work_table[5] =
			{MOVETO_INIT, MOVETO_L2C, MOVETO_C2R, MOVETO_R2C, MOVETO_C2L};

			p->m_work2 = !0;
		    /* ���ɍ����������E�����������ֈړ� */
			dx = dx_table[p->m_work];
			p->m_work = m_work_table[p->m_work];

		    /* player->x �͗����̎�Ƃ��Ďg�p���Ă��܂� */
			if (SHORT_LY < 80)
				dy = (((unsigned short) rndtable[(player->x) & 0xff]) / 4 + 32 + 16);
			else
				dy = 64;
		    /* �ړ�������ݒ� */
			SubEnemyMoveToInit (p, dx, dy, SPEED_2, SPEED_2N);
		} else {
		    /* �ݒ�l�ɏ]���Ĉړ� */
			if (SubEnemyMoveTo (p) < 0)
				p->m_work2 = 0;	/* �ړ������Đݒ� */
		}

		if (p->timer > TIMER_2) {	/* ���Ԑ؂� */
			p->seq = SEQ_DEATH;
			p->m_work = 0;
			p->work = 0;	/* ���Ԑ؂�̎� */
		}
	    /* ���ɍU������ */
		switch (p->s_work) {
		    /* �y���e */
		case 0:
#define INTERVAL3_0	12
#define SPEED_SHOT3_0	8	/* �y���e�̑��x */
#define SPEED_SHOT3_1	16
			if (p->s_work2++ > INTERVAL3_0) {
				short h;
				char a = 9;
				p->s_work2 = 0;
				for (h = 0; h < 8; h++) {
					EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT3_0, a += 16, 4);
					EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT3_1, a += 16, 4);
				}
				SoundSetSE (SE_EBOMB_M);
				if (p->s_work3++ > 3) {
					p->s_work++;
					p->s_work3 = 0;
				}
			}
			break;

		case 1:
#define INTERVAL3_1	12
#define SPEED_SHOT3_11	8	/* �y���e�̑��x */
#define SPEED_SHOT3_12	12
#define SPEED_SHOT3_13	16
			if (p->s_work2++ > INTERVAL0) {
				short h;
				p->s_work2 = 0;
				for (h = 0; h < 4; h++) {
					EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT3_11, rndtable[p->angle++] & 0xfe, 4);
					EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT3_12, rndtable[p->angle++] & 0xfe, 4);
					EshotInit (ESHOT_NRG24N, p->x, p->y, SPEED_SHOT3_13, rndtable[p->angle++] & 0xfe, 4);
				}
				SoundSetSE (SE_EBOMB_L);
				if (p->s_work3++ > 3) {
					p->s_work--;
					p->s_work3 = 0;
				}
			}
			break;
		}
		break;


	case SEQ_DEATH:
		switch (p->m_work) {
		case 0:
			if (SubEnemyMoveToAbort (p) < 0) {	/* �~�܂�܂ő҂� */
				p->m_work++;
				p->m_work2 = 0;
				p->m_work3 = 0;
			    /* �����蔻��𖳂��� */
				p->hit_px = p->hit_py = p->hit_sx = p->hit_sy = p->hit_cx = 0;
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* �e���� */
			}
			break;
		case 1:
			if (p->m_work2++ > 3) {
				p->m_work2 = 0;
				p->m_work3++;
				if (p->m_work3 < 35) {
					signed short x, y;
					x = rndtable[(p->timer) & 0xff ^ 0x55] / 4 - 32;
					y = rndtable[(p->timer) & 0xff ^ 0xaa] / 4 - 32;
				    /* p->timer �͗����̎�Ƃ��Ďg�p���Ă��܂� */
					EffectInit (EFFECT_EXPL, 0, p->x + x, p->y + y);
					SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
				}
				if (p->m_work3 > 40) {
					short h;
					for (h = 0; h < 6; h++) {
						signed short x, y;
					    /* p->timer �͗����̎�Ƃ��Ďg�p���Ă��܂� */
						x = rndtable[(p->timer + h) & 0xff ^ 0x55] - 128;
						y = rndtable[(p->timer - h) & 0xff ^ 0xaa] - 128;
						EffectInit (EFFECT_EXPL, 0, p->x + x / 4, p->y + y / 4);
					}
					SoundSetSE (SE_EXPL_L);	/* �{�X�L���������� */
					if (p->work) {	/* �����Ɠ|�������H */
						if (p->core_info == (PALET_CORE_RED | PRIORITY_BOSS_CORE))
							EffectInit (EFFECT_POINTS_RED, POINTS_10000, p->x, p->y);
						else
							EffectInit (EFFECT_POINTS_BLUE, POINTS_100, p->x, p->y);
					}
					player->seq = PLAYER_SEQ_LAST;	/* player.c �Q�� */
					player->seq2 = 0;
					return (-1);	/* ���� */
				}
			}
			break;
		}
		break;

	default:
		break;
	}


    /* ���@�V���b�g�ɓ����������̏��� */
	if (p->damage) {
		SoundSetSE (SE_CORE_DAMAGE);
		p->info = PALET_RED | PRIORITY_BOSS;
		p->damage_core = 0;
		if ((p->hp -= p->damage) <= 0) {
		    /* ���� */
			p->seq = SEQ_DEATH;
			p->m_work = 0;
			p->work = !0;	/* �����Ɠ|������ */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 700)) {
			p->info = PALET_RED | PRIORITY_BOSS;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = p->core_info;
		}
	}
	xobj_set_st (p);


	return (0);
}



static void EnemyTiniLBoss04C (ENEMY * p)
{
	entry_counter_stop = 0;	/* �G���g���[�J�E���^�[��~���� */
}
