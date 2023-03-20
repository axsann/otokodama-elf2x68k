/* �S�ʃ{�X */
#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../entry.h"
#include "../sound.h"
#include "../psearch.h"

#ifdef DEBUG
#include <stdlib.h>
#include <stdio.h>
#include "../txfont.h"
#endif

#define HP_0	6000		/* �ϋv�� */
#define HP_1	4000		/* �ϋv�͂�����ȉ��ɂȂ�����J�o�[�j�� */
#define HP_2	1500		/* �ϋv�͂�����ȉ��ɂȂ�����ŏI�U�� */
#define HP_3	300		/* �ϋv�͂�����ȉ��ɂȂ����疡���@�̃��[�U�[�o�� */

#define TIMER_0	19000		/* ����ȏ㎞�Ԃ��o�߂�����J�o�[�j�� */
#define TIMER_1	24000		/* �@�@�@�@�@�V�@�@�@�@�@�@�ŏI�`�Ԃ� */
#define TIMER_2	28000		/* �@�@�@�@�@�V�@�@�@�@�@�@���� */



#define SPEED_1		3	/* �o�ꎞ�̉����x */
#define SPEED_1N	25	/* �����x�𑫂��� */

#define SPEED_2		6	/* ���ɍ~��Ă��鎞�̉����x */
#define SPEED_2N	35	/* �����x�𑫂��� */

#define SPEED_3		20	/* ���������Ă��鎞�̉����x */
#define SPEED_3N	10	/* �����x�𑫂��� */

#define SEQ_DAMAGE	5	/* p->seq ������ȍ~���ƃ_���[�W���󂯂� */
#define SEQ_DEATH	6



enum {
	MOVETO_INIT = 0,	/* ������� */
	MOVETO_C2L,		/* �������獶�Ɉړ� */
	MOVETO_L2C,
	MOVETO_C2R,
	MOVETO_R2C,
	MOVETO_DOWN,
	MOVETO_DOWN2LEFT,
	MOVETO_UP
};

static short EnemyMoveLBoss04 (ENEMY *);
static void EnemyTiniLBoss04 (ENEMY *);


void EnemyInitLBoss04 (ENEMY * p)
{
	p->vx = 0;
	p->vy = -4 * 65536;

    /* �o�����̓����蔻��i��ʉ�����o�Ă���̂Ŕ���Ȃ��j */
	p->hit_px = p->hit_py = p->hit_sx = p->hit_sy = p->hit_cx = 0;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->pt = obj_lboss04 + 10;
	p->core_pt = obj_lboss04 + 9;
	p->core_info = 0;
	p->seq = 0;
	p->m_work = 0;
	p->func_enemy_move = EnemyMoveLBoss04;
	p->func_enemy_tini = EnemyTiniLBoss04;

	p->parts1 = p->parts2 = 0;

	entry_counter_stop = !0;/* �G���g���[�J�E���^�[��~ */
}



static short EnemyMoveLBoss04 (ENEMY * p)
{
    /* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;
	p->timer++;

#ifdef DEBUG
	{
		char temp_str[64];
		sprintf (temp_str, "TIMER %05hd", p->timer);
		TxfontCursor (20, 3);
		TxfontPuts (temp_str);
		sprintf (temp_str, "   HP %05hd", p->hp);
		TxfontCursor (20, 4);
		TxfontPuts (temp_str);
	}
#endif

	switch (p->seq) {
	case 0:
		if (SHORT_LY < 222) {
			p->seq++;
		}
		break;

	    /* ���� */
	case 1:
		p->vy += 4096;
		if (p->vy > 16384) {
			p->seq++;
		}
		break;

	    /* �����L���� */
	case 2:
		p->pt++;
		if (p->pt == obj_lboss04 + 30)
			SoundSetSE (SE_LOCK_L);	/* �u�������v�Ƃ������ʉ���炷 */
		if (p->pt == obj_lboss04 + 55)
			SoundSetSE (SE_LOCK_M);	/* �u�������v�Ƃ������ʉ���炷 */
		if (p->pt >= obj_lboss04 + 60) {
			p->vy -= 2048;
			if (p->vy < 0)
				p->vy = 0;
		}
		if (p->pt >= obj_lboss04 + 73) {
			p->seq++;
		    /* ��P�`�Ԃ̓��蔻�� */
			p->hit_px = 24;
			p->hit_py = 8;
			p->hit_sx = 24;
			p->hit_sy = 8;
			p->hit_cx = 24;
			p->m_work = MOVETO_INIT;	/* �ړ����� */
			p->m_work3 = 0;	/* ��or����j�󂵂��� */
			p->s_work = 0;
			p->s_work3 = 0;	/* �����e�p */
			p->pt = obj_lboss04;
			p->parts1 = EnemyInit (ENEMY_LBOSS04S, -32, 0, 0, p);	/* ���� */
			p->parts2 = EnemyInit (ENEMY_LBOSS04S, 32, 0, 0, p);	/* �E�� */
		}
		break;

	    /* ��P�`�� */
	case 3:		/* ���E�Ɉړ� */
	    /* �܂��ړ����� */
		if (p->m_work == MOVETO_INIT) {
		    /* ������ԂȂ� */
			if (p->x > player->x)
				p->m_work = 3;	/* �ړ����� */
			else
				p->m_work = 1;	/* �ړ����� */
			p->m_work2 = 0;
		}
		if (p->m_work2 == 0) {
		    /* ����Ȃ� */
			signed short dx, dy;
			signed short dx_table[5] =
			{0, 96 + 16, 128 + 16, 256 - 96 + 16, 128 + 16};
			short m_work_list[15] =
			{MOVETO_L2C, MOVETO_C2R, MOVETO_R2C, MOVETO_C2L,
			 MOVETO_L2C, MOVETO_C2R, MOVETO_R2C, MOVETO_C2L,
			 MOVETO_L2C, MOVETO_C2R, MOVETO_R2C,
			 MOVETO_DOWN, MOVETO_DOWN2LEFT, MOVETO_UP, MOVETO_C2L,};
			unsigned char speed;
			signed short count;

			p->m_work2 = !0;
		    /* ���ɍ����������E�����������ֈړ� */
			switch (p->m_work) {
			case 11:	/* ���ɍ~��Ă��� */
				p->work = p->x;
				p->work2 = p->y;
				speed = SPEED_2;
				count = SPEED_2N;
				dx = 144;
				dy = 256 + 16;
				p->m_work++;
				break;
			case 12:	/* ���ɔ����� */
				speed = SPEED_1;
				count = SPEED_1N;
				dx = 64;
				dy = p->y - 32;
				p->m_work++;
				break;
			case 13:	/* ��ɏオ�� */
				speed = SPEED_2;
				count = SPEED_2N;
				dx = p->work;
				dy = p->work2;
				if ((p->parts1) && (p->parts1->parts1)) {
					p->parts1->parts1->s_work++;
					p->parts1->parts1->s_work2 = 0;
					p->parts1->parts1->s_work3 = 0;
				}
				if ((p->parts2) && (p->parts2->parts1)) {
					p->parts2->parts1->s_work++;
					p->parts2->parts1->s_work2 = 0;
					p->parts2->parts1->s_work3 = 0;
				}
				p->m_work++;
				break;
			case 10:
				if (p->m_work3)	/* �����Ȃ���΂������ɍ~��Ȃ� */
					p->m_work = 14;
			    /* ������ break ���Ȃ����ɒ��� */
			default:
				speed = SPEED_1;
				count = SPEED_1N;
				dx = dx_table[m_work_list[p->m_work]];
			    /* player->x �͗����̎�Ƃ��Ďg�p���Ă��܂� */
				if (SHORT_LY < 80)
					dy = (((unsigned short) rndtable[(player->x) & 0xff]) / 4 + 32 + 16);
				else
					dy = 64;
				if (p->m_work < 14)
					p->m_work++;
				else
					p->m_work = 0;
				break;
			}

		    /* �ړ�������ݒ� */
			SubEnemyMoveToInit (p, dx, dy, speed, count);
		} else {
		    /* �ݒ�l�ɏ]���Ĉړ� */
			if (SubEnemyMoveTo (p) < 0)
				p->m_work2 = 0;	/* �ړ������Đݒ� */
		}

	    /* ���p�[�c�����������H */
		if ((p->parts1) && (p->parts1->child_death))
			p->parts1 = 0;	/* ����ȍ~�Q�Ƃ��Ȃ� */
		if ((p->parts2) && (p->parts2->child_death))
			p->parts2 = 0;	/* ����ȍ~�Q�Ƃ��Ȃ� */

		if (((p->parts1 == 0)	/* �������Ȃ� */
		     ||((p->parts1) && (p->parts1->parts1 == 0)))	/* �܂��͍������Ȃ� */
		    &&		/* �Ȃ����� */
		    ((p->parts2 == 0)	/* �E�����Ȃ� */
		     ||((p->parts2) && (p->parts2->parts1 == 0)))) {	/* �܂��͉E�����Ȃ� */
			p->m_work3 = !0;
		}
	    /* �������Ȃ��ꍇ */
		if ((p->parts1 == 0) && (p->parts2 == 0)) {
			p->seq++;
			p->m_work = 0;
			SubEnemyMoveToAbort (p);
		}
		break;

	case 4:
	    /* �ݒ�l�ɏ]���Ĉړ� */
		if (SubEnemyMoveTo (p) < 0) {
			if (p->m_work == 0) {
			    /* �ړ�������ݒ� */
				SubEnemyMoveToInit (p, 144, 96, SPEED_1, SPEED_1N);
				p->m_work++;
			} else {
				p->seq++;
				p->m_work = MOVETO_INIT;	/* �ړ����� */
				p->s_work = p->s_work2 = 0;
				p->parts1_work = 30;	/* ���������܂ł̎��ԁi =0 �ŕ����j */
				p->parts1_work2 = 12;	/* �����\�� */
				p->parts2_work = 60;	/* �E�������܂ł̎��ԁi =0 �ŕ����j */
				p->parts2_work2 = 12;	/* �����\�� */
				//p->parts1_work2 = 1;	/* �����\�� */
				//p->parts2_work2 = 1;	/* �����\�� */
			}
		}
		break;


	    /* ��Q�`�� */
	case 5:		/* �������鎨�������� */
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
			{0, 72 + 16, 128 + 16, 256 - 72 + 16, 128 + 16};
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
			SubEnemyMoveToInit (p, dx, dy, SPEED_3, SPEED_3N);
		} else {
		    /* �ݒ�l�ɏ]���Ĉړ� */
			if (SubEnemyMoveTo (p) < 0)
				p->m_work2 = 0;	/* �ړ������Đݒ� */
		}

	    /* �����������畜���܂ł̎��Ԃ�ݒ� */
		if ((p->parts1) && (p->parts1->child_death)) {
			p->parts1 = 0;	/* ����ȍ~�Q�Ƃ��Ȃ� */
			p->parts1_work = 110;
		}
		if ((p->parts2) && (p->parts2->child_death)) {
			p->parts2 = 0;	/* ����ȍ~�Q�Ƃ��Ȃ� */
			p->parts2_work = 80;	/* ���͉E���̕������������� */
		}
	    /* �������E�������� */
		if ((p->parts1_work) && (p->parts1_work2)) {
			p->parts1_work--;
			if (p->parts1_work == 0)
				p->parts1 = EnemyInit (ENEMY_LBOSS04E, -16, 0, p->parts1_work2--, p);	/* ���� */
		}
		if ((p->parts2_work) && (p->parts2_work2)) {
			p->parts2_work--;
			if (p->parts2_work == 0)
				p->parts2 = EnemyInit (ENEMY_LBOSS04E, 16, 0, p->parts2_work2--, p);	/* �E�� */
		}
	    /* ���ɍU������ */
		switch (p->s_work) {
#define S_WORK_LAST	6
		case 0:
			if ((p->hp < HP_1) || (p->timer > TIMER_0)
			    || (((p->parts1_work2 == 0) && (p->parts1 == 0))
				&& ((p->parts2_work2 == 0) && (p->parts2 == 0)))) {
				p->s_work++;
				p->s_work2 = 0;
			}
			break;

		    /* ���΂炭�҂� */
		case 1:
			p->s_work2++;
			if (p->s_work2 >= 55) {
				p->s_work++;
			    /* �j�󂵂����̂ݓ��_���� */
				if (p->hp < HP_1)
					p->parts3 = EnemyInit (ENEMY_LBOSS04V, p->x, p->y, 0, p);	/* �J�o�[�𕪗� */
				else
					p->parts3 = EnemyInit (ENEMY_LBOSS04V, p->x, p->y, 1, p);	/* �J�o�[�𕪗� */
				p->hp = HP_1;	/* �����ŋ����I�ɑϋv�͂� HP_1 �� */
				EffectInit (EFFECT_EXPL, 0, p->x, p->y);
				SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* �e���� */
				p->pt = obj_lboss04 + 7;
				if (standard_play)	/* �W���ݒ肩�H */
					p->core_info = PALET_CORE_RED | PRIORITY_BOSS_CORE;
				else
					p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS_CORE;
			}
			break;

		    /* �J�o�[��������܂ő҂� */
		case 2:
			if ((p->parts3) && (p->parts3->child_death)) {
				p->parts3 = 0;	/* ����ȍ~�Q�Ƃ��Ȃ� */
				p->s_work++;
				p->s_work2 = p->s_work3 = p->s_work4 = 0;
			    /* ���蔻�� */
				p->hit_px = 40;
				p->hit_py = 16;
				p->hit_sx = 40;
				p->hit_sy = 16;
				p->hit_cx = 24;
			}
			break;

		    /* ���E����Ȃ��|���e */
		case 3:
			p->s_work2++;
#define INTERVAL1	6
#define SPEED_SHOT1	15	/* ���E����Ȃ��|���e�̑��x */
#define INTERVAL1_1	9
#define SPEED_SHOT1_1	23	/* ���@��_���Ă���P���e�̑��x */
			if (p->s_work2 > INTERVAL1) {
				p->s_work2 = 0;
				p->s_work3++;
				if (p->s_work3 == 1) {
					p->s_angle = 64 + 48;
					p->s_angle2 = 64 - 48;
				}
				EshotInit (ESHOT_NRG04, p->x - 16, p->y, SPEED_SHOT1, p->s_angle, 0);
				EshotInit (ESHOT_NRG04, p->x + 16, p->y, SPEED_SHOT1, p->s_angle2, 0);
				p->s_angle -= 12;
				p->s_angle2 += 12;
				SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */

				if (p->s_work3 >= 80) {
					if (!eshot_erase)
						eshot_erase = ESHOT_ERASE;	/* �e���� */
					p->s_work++;	/* ���̍U���� */
					p->s_work2 = p->s_work3 = p->angle = 0;
				}
			}
			p->s_work4++;
			if (p->s_work4 > INTERVAL1_1) {
				p->s_work4 = 0;
				EshotInit (ESHOT_NRG02, p->x, p->y - 12, SPEED_SHOT1_1, psearch (p->x, p->y), 5);
				SoundSetSE (SE_ESHOT_M);	/* �G�V���b�g�� */
			}
			if (p->hp < HP_2) {
				p->s_work = S_WORK_LAST;
				p->s_work2 = p->s_work3 = 0;
			}
			break;

		    /* �Q�ӏ�����΂�܂��e */
		case 4:
			p->s_work2++;
#define SPEED_SHOT3		12
#define SPEED_SHOT3_1	18
#define SPEED_SHOT3_2	10
			switch (p->s_work2) {
			case 1:
				EshotInit (ESHOT_NRG02, p->x - 16, p->y, SPEED_SHOT3, rndtable[p->angle++] & 0xfe, 4);
				EshotInit (ESHOT_NRG02, p->x + 16, p->y, SPEED_SHOT3, rndtable[p->angle++] & 0xfe, 4);
				EshotInit (ESHOT_NRG24N, p->x, p->y - 12, SPEED_SHOT3_1, rndtable[p->angle++] & 0xfe, 4);
				SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
				break;
			case 2:
				EshotInit (ESHOT_NRG03, p->x - 16, p->y, SPEED_SHOT3_1, rndtable[p->angle++] & 0xfe, 4);
				EshotInit (ESHOT_NRG03, p->x + 16, p->y, SPEED_SHOT3_1, rndtable[p->angle++] & 0xfe, 4);
				EshotInit (ESHOT_NRG24N, p->x, p->y - 12, SPEED_SHOT3_1, rndtable[p->angle++] & 0xfe, 4);
				EshotInit (ESHOT_NRG34N, p->x, p->y - 12, SPEED_SHOT3_2, rndtable[p->angle++] & 0xfe, 4);
				break;
			case 3:
				p->s_work2 = 0;
				break;
			}
			p->s_work3++;
			if (p->s_work3 > 350) {
				p->s_work++;
				p->s_work2 = p->s_work3 = p->s_work4 = 0;
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* �e���� */
			}
			if (p->hp < HP_2) {
				p->s_work = S_WORK_LAST;
				p->s_work2 = p->s_work3 = 0;
			}
			break;

		    /* �y���e�A�� */
		case 5:
			p->s_work2++;
			{
				unsigned char a;
				short h;
#define SPEED_SHOT5_1	18
#define SPEED_SHOT5_2	16
#define INTERVAL5	13
				switch (p->s_work2) {
				case INTERVAL5 * 1:
					a = 0;
					for (h = 0; h < 16; h++)
						EshotInit (ESHOT_NRG03, p->x, p->y - 10 - 8, SPEED_SHOT5_1, a += 16, 3);
					SoundSetSE (SE_EBOMB_M);
					break;
				case INTERVAL5 * 1 + 1:
					a = 8;
					for (h = 0; h < 16; h++)
						EshotInit (ESHOT_NRG03, p->x, p->y - 10 - 8, SPEED_SHOT5_2, a += 16, 4);
					break;
				case INTERVAL5 * 2:
					a = 0;
					for (h = 0; h < 16; h++)
						EshotInit (ESHOT_NRG02, p->x - 12, p->y - 8, SPEED_SHOT5_2, a += 16, 3);
					SoundSetSE (SE_EBOMB_M);
					break;
				case INTERVAL5 * 2 + 1:
					a = 8;
					for (h = 0; h < 16; h++)
						EshotInit (ESHOT_NRG04, p->x - 12, p->y - 8, SPEED_SHOT5_2, a += 16, 4);
					break;
				case INTERVAL5 * 3:
					a = 0;
					for (h = 0; h < 16; h++)
						EshotInit (ESHOT_NRG02, p->x + 12, p->y - 8, SPEED_SHOT5_2, a += 16, 3);
					SoundSetSE (SE_EBOMB_M);
					break;
				case INTERVAL5 * 3 + 1:
					a = 8;
					for (h = 0; h < 16; h++)
						EshotInit (ESHOT_NRG04, p->x + 12, p->y - 8, SPEED_SHOT5_1, a += 16, 4);
					break;
				case INTERVAL5 * 4:
					p->s_work2 = 0;
					break;
				default:
					break;
				}
			}
			p->s_work3++;
			if (p->s_work3 > 350) {
				p->s_work -= 2;
				p->s_work2 = p->s_work3 = p->s_work4 = 0;
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* �e���� */
			}
			if ((p->hp < HP_2) || (p->timer > TIMER_1)) {
				p->s_work = S_WORK_LAST;
				p->s_work2 = p->s_work3 = 0;
			}
			break;

		    /* ���X�{�X�ŏI�U�� */
		case S_WORK_LAST:
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
			if (!eshot_erase)
				eshot_erase = ESHOT_ERASE;	/* �e���� */
			p->s_work++;
			p->s_work2 = p->s_work3 = 0;
			break;

		case S_WORK_LAST + 1:
			{
				short h;
				char a;
				p->s_work2++;
				switch (p->s_work2) {
#define INTERVAL6	5
#define SPEED_SHOT6	25
#define SPEED_SHOT6_1	15
				case INTERVAL6 * 1:
				case INTERVAL6 * 2:
				case INTERVAL6 * 3:
				case INTERVAL6 * 4:
				case INTERVAL6 * 5:
				case INTERVAL6 * 6:
					a = 0;
					for (h = 0; h < 16; h++)
						EshotInit (ESHOT_NRG02, p->x, p->y - 8, SPEED_SHOT6, a += 16, 4);
					SoundSetSE (SE_ESHOT_M);	/* �G�V���b�g�� */
					break;
				case INTERVAL6 * 7:
				case INTERVAL6 * 8:
				case INTERVAL6 * 9:
				case INTERVAL6 * 10:
				case INTERVAL6 * 11:
				case INTERVAL6 * 12:
					a = 8;
					for (h = 0; h < 16; h++)
						EshotInit (ESHOT_NRG03, p->x, p->y - 8, SPEED_SHOT6, a += 16, 4);
					SoundSetSE (SE_ESHOT_M);	/* �G�V���b�g�� */
					break;
				case INTERVAL6 * 13:
					a = psearch (p->x, p->y);	/* ���@�̕������T�[�` */
					for (h = 0; h < 16; h++)
						EshotInit (ESHOT_NRG35, p->x, p->y - 8, SPEED_SHOT6_1, a += 16, 4);
					p->s_work2 = 0;
					break;
				}
			}

			if ((p->hp < HP_3) || (p->timer > TIMER_2)) {
				p->damage += 5;
				p->s_work3++;
				switch (p->s_work3) {
				case 1:
					EffectInit (EFFECT_LASTLASER, 0, p->x, p->y);
					SoundSetSE (SE_BOMB_C);	/* �{���o�[���i���[�U�[�j */
					break;
				case 15:
					EffectInit (EFFECT_LASTLASER, 0, p->x + 20, p->y);
					SoundSetSE (SE_BOMB_C);	/* �{���o�[���i���[�U�[�j */
					if (!eshot_erase)
						eshot_erase = ESHOT_ERASE;	/* �e���� */
					break;
				case 40:
					EffectInit (EFFECT_LASTLASER, 0, p->x - 35, p->y);
					SoundSetSE (SE_BOMB_C);	/* �{���o�[���i���[�U�[�j */
					if (!eshot_erase)
						eshot_erase = ESHOT_ERASE;	/* �e���� */
					break;
				case 50:
					EffectInit (EFFECT_LASTLASER, 0, p->x + 4, p->y);
					SoundSetSE (SE_BOMB_C);	/* �{���o�[���i���[�U�[�j */
					if (!eshot_erase)
						eshot_erase = ESHOT_ERASE;	/* �e���� */
					break;
				case 75:
					EffectInit (EFFECT_LASTLASER, 0, p->x - 48, p->y);
					SoundSetSE (SE_BOMB_C);	/* �{���o�[���i���[�U�[�j */
					if (!eshot_erase)
						eshot_erase = ESHOT_ERASE;	/* �e���� */
					if (p->hp > 0)
						p->hp = 0;	/* �Ƃɂ������� */
					break;
				}
			}
			break;

		default:
			break;
		}		/* end of switch(p->s_work) */
		break;

	case SEQ_DEATH:
		switch (p->m_work) {
		case 0:
			if (SubEnemyMoveToAbort (p) < 0) {	/* �~�܂�܂ő҂� */
				p->m_work++;
				p->m_work2 = p->m_work3 = 0;
				SubEnemyMoveToInit (p, p->x, 256 + 64, SPEED_3, SPEED_3N);
			    /* �����蔻��𖳂��� */
				p->hit_px = p->hit_py = p->hit_sx = p->hit_sy = p->hit_cx = 0;
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* �e���� */
			}
			break;
		case 1:	/* �������Ȃ��牺�ɗ����� */
			if (SubEnemyMoveTo (p) < 0)
				p->m_work++;

			if (p->m_work2++ > 3) {
				signed short x, y;
				p->m_work2 = 0;
				x = rndtable[(p->timer) & 0xff ^ 0x55] - 128;
				y = rndtable[(p->timer) & 0xff ^ 0xaa] - 128;
			    /* p->timer �͗����̎�Ƃ��Ďg�p���Ă��܂� */
				EffectInit (EFFECT_EXPL, 0, x / 2 + p->x, y / 4 + p->y);
				SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
			}
			p->m_work3++;
#define	TIMER_LASTPLAYER	55
			switch (p->m_work3) {
			case TIMER_LASTPLAYER + 1:
				EffectInit (EFFECT_LASTPLAYER, 0, 144, 256 + 16);
				break;
			case TIMER_LASTPLAYER + 15:
				EffectInit (EFFECT_LASTPLAYER, 0, 144 + 20, 256 + 32);
				break;
			case TIMER_LASTPLAYER + 40:
				EffectInit (EFFECT_LASTPLAYER, 0, 144 - 35, 256 + 48);
				break;
			case TIMER_LASTPLAYER + 50:
				EffectInit (EFFECT_LASTPLAYER, 0, 144 + 4, 256 + 64);
				break;
			case TIMER_LASTPLAYER + 75:
				EffectInit (EFFECT_LASTPLAYER, 0, 144 - 48, 256 + 56);
				break;
			default:
				break;
			}

			break;
		case 2:
			return (-1);	/* ���� */
			break;
		}
		break;

	default:
		break;
	}			/* end of switch(p->seq) */


    /* ���@�V���b�g�ɓ����������̏��� */
	if (p->damage) {
		if (p->seq >= SEQ_DAMAGE) {
			if (p->damage_core) {
				SoundSetSE (SE_CORE_DAMAGE);
				p->info = PALET_RED | PRIORITY_BOSS;
				p->damage_core = 0;
			} else {
				SoundSetSE (SE_DAMAGE);
				p->info = PALET_DAMAGE | PRIORITY_BOSS;
			}
			if ((p->hp -= p->damage) <= 0) {
			    /* ���� */
				p->seq = SEQ_DEATH;
				p->m_work = 0;
			}
			p->damage = 0;
			p->flash = TIMER_FLASH_DAMAGE;
		} else {
			SoundSetSE (SE_NO_DAMAGE);
			p->damage = 0;
		}
	} else {
		if ((p->flash++ > TIMER_FLASH_RED)
		    && ((p->hp < 500) || ((p->seq == 5) && (p->hp < HP_1 + 500) && (p->s_work == 0)))) {
			p->info = PALET_RED | PRIORITY_BOSS;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = PALET_LBOSS04 | PRIORITY_BOSS;
		}
	}

	xobj_set_st (p);

    /* �R�A�̕\�� */
	if (p->core_info) {
		p->core_x = p->x;
		p->core_y = p->y - 8;
		xobj_set_st (&(p->core_x));
	}
	return (0);
}



static void EnemyTiniLBoss04 (ENEMY * p)
{
    /* �p�[�c�����݂��Ă���������悤�Ɏw�� */
	if (p->parts1)
		p->parts1->child_kill = !0;
	if (p->parts2)
		p->parts2->child_kill = !0;
	if (p->parts3)
		p->parts3->child_kill = !0;
	entry_counter_stop = 0;	/* �G���g���[�J�E���^�[��~���� */
}
