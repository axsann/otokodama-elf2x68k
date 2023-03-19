/* �Q�ʃ{�X */
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


#define SPEED_LASER		30

#define SPEED_1		3	/* �����x */
#define SPEED_1N	28	/* �����x�𑫂��� */

#define SPEED_SHOT1	8	/* �P�i�K�ڐ�^�e���x */
#define SPEED_SHOT2	10	/* �P�i�K�ڒe�̑��x */
#define SPEED_SHOT3	12	/* �P�i�K�ځE�Q�񒼐��e�̈�Ԓx�����x */
#define SPEED_SHOT4	10	/* ��Q�`�ԁE�O�p�e�̑��x */
#define SPEED_SHOT5	11	/* ��Q�`�ԁE�Ȃ��|���e�̑��x */


#define HP_0	3200		/* �ϋv�� */
#define HP_1	HP_0-700	/* ��P�`�ԁE�ϋv�͂�����ȉ��ɂȂ������y���e������ */
#define HP_2	HP_1-600	/* �V   �E�ϋv�͂�����ȉ��ɂȂ����璼���e������ */
#define HP_3	HP_2-500	/* �V   �E�ϋv�͂�����ȉ��ɂȂ������Q�`�Ԃ� */

#define TIMER_1	2500		/* ����ȏ㎞�Ԃ��o�߂������Q�`�Ԃ� */
#define TIMER_2	7000		/* ����ȏ㎞�Ԃ��o�߂�����P�� */
#define TIMER_3	5000		/* ����ȏ㎞�Ԃ��o�߂������Q�`�ԍU���J�n */

#define SPEED_3		3	/* �R�i�K�ځi�P�ށj�̉����x */
#define SPEED_3N	32	/* �R�i�K�ڂ̉����x�𑫂��� */

#define SEQ_DEATH	6



enum {
	MOVETO_INIT = 0,	/* ������� */
	MOVETO_C2L,		/* �������獶�Ɉړ� */
	MOVETO_L2C,
	MOVETO_C2R,
	MOVETO_R2C
};

enum {
	ATTACK_UPPER_CORE = 0,	/* ��̃R�A����U���i�L�т�e�j */
	ATTACK_LOWER_CORE	/* ���̃R�A����U���i�΂�܂��e�j */
};

static short EnemyMoveLBoss02 (ENEMY *);
static void EnemyTiniLBoss02 (ENEMY *);


void EnemyInitLBoss02 (ENEMY * p)
{
	p->vx = 0;
	p->vy = 4 << 16;

	/* ��P�`�Ԃ̓��蔻�� */
	p->hit_px = 32;
	p->hit_py = 16;
	p->hit_sx = 32;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->pt = obj_lboss02;
	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS_CORE;
	p->seq = 0;
	p->func_enemy_move = EnemyMoveLBoss02;
	p->func_enemy_tini = EnemyTiniLBoss02;

	p->parts1 = 0;
	p->parts2 = 0;
	p->parts1_work = 20;	/* ���������܂ł̎��ԁi =1 �ŕ����j */
	p->parts1_work2 = 0;	/* �������񕜊������� */
	p->parts2_work = 50;	/* �E�������܂ł̎��ԁi =1 �ŕ����j */
	p->parts2_work2 = 0;	/* �E�����񕜊������� */

	p->parts3 = 0;
	p->parts4 = 0;
	p->parts3_work = 0;	/* �����o���� */
	p->parts3_work2 = 0;	/* ���j�󂩂玨�����܂ł̎��� */

	entry_counter_stop = !0;/* �G���g���[�J�E���^�[��~ */

	if (p->arg == 0)
		p->work4 = PALET_LBOSS02 | PRIORITY_BOSS;
	else
		p->work4 = PALET_LBOSS02_1 | PRIORITY_BOSS;
}



static short EnemyMoveLBoss02 (ENEMY * p)
{
	/* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;
	p->timer++;

	switch (p->seq) {
	case 0:
		if (SHORT_LY > 48)
			p->seq++;
		break;
	case 1:		/* ���� */
		p->vy -= 8192;
		if (p->vy < 8192) {
			p->seq++;
			p->vy = 0;
			p->m_work = MOVETO_INIT;	/* �ړ����� */
			p->m_work3 = 0;	/* ��P�i�K�̒��ł̍U�����@ */
			p->s_work = 0;
			p->s_work3 = 0;	/* �����e�p */
		}
		break;

		/* ��P�`�� */
	case 2:		/* ���E�Ɉړ������e */
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
			{0, 54 + 16, 128 + 16, 256 - 54 + 16, 128 + 16};
			short m_work_table[5] =
			{MOVETO_INIT, MOVETO_L2C, MOVETO_C2R, MOVETO_R2C, MOVETO_C2L};

			p->m_work2 = !0;
			/* ���ɍ����������E�����������ֈړ� */
			dx = dx_table[p->m_work];
			p->m_work = m_work_table[p->m_work];

			/* player->y �͗����̎�Ƃ��Ďg�p���Ă��܂� */
			if (SHORT_LY < 80)
				dy = (((unsigned short) rndtable[(player->y) & 0xff]) / 4 + 32 + 16);
			else
				dy = 64;
			/* �ړ�������ݒ� */
			SubEnemyMoveToInit (p, dx, dy, SPEED_1, SPEED_1N);
		} else {
			/* �ݒ�l�ɏ]���Ĉړ� */
			if (SubEnemyMoveTo (p) < 0)
				p->m_work2 = 0;	/* �ړ������Đݒ� */
		}

		/* ���ɍU������ */
		if (p->s_work++ > 100)
			p->s_work = 0;

		switch (p->m_work3) {
		case 0:
			if (p->hp < HP_1) {
				p->m_work3++;
				EffectInit (EFFECT_EXPL, 0, p->x, p->y);
				SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
			}
			break;
		case 1:
			if (p->hp < HP_2) {
				p->m_work3++;
				EffectInit (EFFECT_EXPL, 0, p->x, p->y);
				SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
			}
			break;
		case 2:
			if (p->hp < HP_3) {
				p->m_work3++;
				EffectInit (EFFECT_EXPL, 0, p->x, p->y);
				SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
			}
			break;
		default:
			break;
		}
		if (p->timer > TIMER_1)
			p->m_work3 = 4;

		switch (p->m_work3) {
			/* case �̕��ѕ��ɒ��� */
		case 2:	/* �Q�񒼐��e */
			switch (p->s_work3++) {
			case 0:
				p->s_angle3 = psearch (p->x, p->y);	/* ���@�̕������T�[�` */
				p->s_work4 = SPEED_SHOT3;
				break;
			case 20 + 6 * 0:
			case 20 + 6 * 1:
			case 20 + 6 * 2:
			case 20 + 6 * 3:
			case 20 + 6 * 4:
			case 20 + 6 * 5:
				EshotInit (ESHOT_NRG32, SHORT_LX, SHORT_LY + 8, p->s_work4, p->s_angle3, 3);
				SoundSetSE (SE_ESHOT_M);	/* �G�V���b�g�� */
				p->s_work4 += 3;
				break;
			case 20 + 6 * 11:
				p->s_work3 = 0;
				break;
			default:
				break;
			}
			/* ������ break ���������Ƃɒ��� */
		case 1:	/* �y���e */
			if (p->s_work == 25) {
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 0, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 1, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 2, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 3, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 4, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 5, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 6, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 7, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 8, 3);
				SoundSetSE (SE_EBOMB_M);	/* �G�y���e�� */
			}
			if ((p->s_work == 76) && (p->y + 128 > player->y)) {
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 0, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 1, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 2, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 3, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 4, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 5, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 6, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 7, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 8, 3);
				SoundSetSE (SE_EBOMB_M);	/* �G�y���e�� */
			}
			/* ������ break ���������Ƃɒ��� */
		case 0:	/* �Ȃ��|���e */
			if (p->s_work == 1) {
				p->s_angle = 16;
				p->s_angle2 = 128 - 16;
				p->s_work2 = 0;
			}
			if ((p->s_work > 6) && (p->s_work < 60)) {
				p->s_work2++;
				if (p->s_work2 > 3) {
					p->s_work2 = 0;
					EshotInit (ESHOT_NRG02, SHORT_LX - 29, SHORT_LY - 23, SPEED_SHOT1, p->s_angle, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX + 29, SHORT_LY - 23, SPEED_SHOT1, p->s_angle2, 0);
					p->s_angle += 6;
					p->s_angle2 -= 6;
					SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
				}
			}
			break;
		case 3:	/* ������j�󂵂� */
		case 4:	/* ������j������ */
			{
				short arg;
				if (p->m_work3 == 3)
					arg = 0;
				else
					arg = 1;
				if (p->arg)
					arg += 2;
				EnemyInit (ENEMY_LBOSS02H, SHORT_LX, SHORT_LY, arg, 0);	/* ���𕪗� */
			}
			if (!eshot_erase)
				eshot_erase = ESHOT_ERASE;	/* �e���� */
			p->pt = obj_lboss02 + 2;	/* �{�̕��������ɂȂ� */
			p->seq++;
			p->m_work = 0;
			p->m_work2 = 0;
			p->hit_px = p->hit_py = p->hit_sx = p->hit_sy = p->hit_cx = 0;
			break;
		}
		break;


	case 3:		/* ���́I */
		switch (p->m_work) {
		case 0:
			if (SubEnemyMoveTo (p) < 0) {	/* �~�܂�܂ő҂� */
				p->m_work++;
				/* �ړ�������ݒ� */
				SubEnemyMoveToInit (p, 128 + 16, 128, SPEED_1, SPEED_1N);
			}
			break;
		case 1:
			if (SubEnemyMoveTo (p) < 0)	/* �~�܂�܂ő҂� */
				p->m_work++;
			break;
		case 2:
			if (p->m_work2 == 43)
				SoundSetSE (SE_LOCK_M);	/* �u�������v�Ƃ������ʉ���炷 */
			if (p->m_work2 == 53) {
				p->pt = obj_lboss02 + 4;	/* �{�́{��p�[�c��{�̂Ƃ��� */
				p->seq++;
				p->cwork = ATTACK_UPPER_CORE;
				p->m_work = MOVETO_INIT;	/* �ړ����� */
				p->s_work = 0;
				p->s_work2 = 0;
				p->s_work3 = -110;
				p->s_angle3 = 0;

				/* ��Q�`�Ԃ̓��蔻�� */
				p->hit_px = 64;
				p->hit_py = 16;
				p->hit_sx = 64;
				p->hit_sy = 16;
				p->hit_cx = 16;
			}
			/* ��p�[�c���̏��� */
			if (p->m_work2 < 53) {
				signed short offset_y[53] =	/* ��p�[�c�̍��W�I�t�Z�b�g */
				{
					185, 179, 172, 166, 159, 153, 147, 140, 134, 127,
					121, 115, 108, 102, 95, 89, 83, 76, 70, 63,
					57, 51, 44, 38, 31, 26, 21, 17, 14, 11,
					10, 9, 8, 7, 7, 6, 5, 4, 3, 3,
					2, 1, 0, -1, -1, 0, 1, 0, -1, -1,
					0, 1, 0
				};
				if (p->arg == 0)
					xobj_set (SHORT_LX, SHORT_LY + offset_y[p->m_work2],
						  obj_lboss02 + 3, PALET_LBOSS02 | PRIORITY_BOSS_PARTS_L);
				else
					xobj_set (SHORT_LX, SHORT_LY + offset_y[p->m_work2],
						  obj_lboss02 + 3, PALET_LBOSS02_1 | PRIORITY_BOSS_PARTS_L);
				xsp_set (SHORT_LX - 16, SHORT_LY + offset_y[p->m_work2] - 16,
					 sp_core01, PALET_CORE_BLUE | PRIORITY_BOSS_CORE);
				xsp_set (SHORT_LX, SHORT_LY + offset_y[p->m_work2] - 16,
					 sp_core01, PALET_CORE_BLUE | PRIORITY_BOSS_CORE);
			}
			p->m_work2++;
			break;
		}
		break;


		/* ��Q�`�� */
	case 4:
		if (p->timer > TIMER_2) {
			p->seq++;	/* ���Ԑ؂� */
			p->m_work = 0;
			break;
		}
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

			/* player->y �͗����̎�Ƃ��Ďg�p���Ă��܂� */
			if (SHORT_LY < 80)
				dy = (((unsigned short) rndtable[(player->y) & 0xff]) / 4 + 48 + 16);
			else
				dy = 64;
			/* �ړ�������ݒ� */
			SubEnemyMoveToInit (p, dx, dy, SPEED_1, SPEED_1N);
		} else {
			/* �ݒ�l�ɏ]���Ĉړ� */
			if (SubEnemyMoveTo (p) < 0)
				p->m_work2 = 0;	/* �ړ������Đݒ� */
		}


		/* ���p�[�c���������畜���܂ł̎��Ԃ�ݒ� */
		if ((p->parts1) && (p->parts1->child_death)) {
			p->parts1 = 0;	/* ����ȍ~�Q�Ƃ��Ȃ� */
			p->parts1_work = 90;
		}
		if ((p->parts2) && (p->parts2->child_death)) {
			p->parts2 = 0;	/* ����ȍ~�Q�Ƃ��Ȃ� */
			p->parts2_work = 70;	/* ���͉E���̕������������� */
		}
		/* ���p�[�c�����E�������� */
		if (p->parts1_work) {
			if (p->parts1_work == 1)
				p->parts1 = EnemyInit (ENEMY_LBOSS02W, -80, 0, p->parts1_work2++, p);	/* ���� */
			p->parts1_work--;
		}
		if (p->parts2_work) {
			if (p->parts2_work == 1)
				p->parts2 = EnemyInit (ENEMY_LBOSS02W, 80, 0, p->parts2_work2++, p);	/* �E�� */
			p->parts2_work--;
		}
		/* �����o���� */
		if (p->parts3_work == 0) {
			/* �܂������o�Ă��Ȃ� */
			if (p->parts1_work2 + p->parts2_work2 >= 6) {	/* �����o���� */
				p->parts3_work = !0;
				p->parts3_work2 = 60;
			}
		} else {
			if (p->parts3_work2) {
				p->parts3_work2--;
				if (p->parts3_work2 == 1) {
					p->parts3 = EnemyInit (ENEMY_LBOSS02E, 40, 0, 0, p);	/* ���� */
					p->parts4 = EnemyInit (ENEMY_LBOSS02E, -40, 0, 0, p);	/* �E�� */
				}
			} else {
				/* �����܂����邩 */
				if ((p->parts3) && (p->parts3->child_death))
					p->parts3 = 0;	/* ����ȍ~�Q�Ƃ��Ȃ� */
				if ((p->parts4) && (p->parts4->child_death))
					p->parts4 = 0;	/* ����ȍ~�Q�Ƃ��Ȃ� */
				/* ���j��ς� */
				if ((p->parts3 == 0) && (p->parts4 == 0))
					p->core_info = PALET_CORE_RED | PRIORITY_BOSS_CORE;
			}
		}

		/* ���ɍU������ */
		switch (p->s_work) {
		case 0:
			if (p->timer > TIMER_3) {
				p->s_work++;
				p->s_work2 = 0;
			}
			break;
		case 1:
			if (p->s_work2++ == 10) {
				EshotInit (ESHOT_NRG23, SHORT_LX - 16, SHORT_LY - 8, SPEED_SHOT4, 149, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX + 16, SHORT_LY - 8, SPEED_SHOT4, 234, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX, SHORT_LY + 8, SPEED_SHOT4, 64, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX - 16, SHORT_LY - 8, SPEED_SHOT4 + 4, 149, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX + 16, SHORT_LY - 8, SPEED_SHOT4 + 4, 234, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX, SHORT_LY + 8, SPEED_SHOT4 + 4, 64, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX - 16, SHORT_LY - 8, SPEED_SHOT4 + 8, 149, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX + 16, SHORT_LY - 8, SPEED_SHOT4 + 8, 234, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX, SHORT_LY + 8, SPEED_SHOT4 + 8, 64, 0);
				SoundSetSE (SE_LASER);	/* �G�V���b�g�� */
			}
			if (p->s_work2 > 100)
				p->s_work2 = 0;
			break;
		default:
			break;
		}

		/* �Ȃ��|���e */
		if (p->core_info == (PALET_CORE_RED | PRIORITY_BOSS_CORE)) {
			switch (p->s_work3++) {
			case 0:
				if (p->arg == 0) {
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT5, p->s_angle3, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT5, p->s_angle3 + 85, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT5, p->s_angle3 + 170, 0);
					p->s_angle3 += 12;
				} else {
					EshotInit (ESHOT_NRG02, SHORT_LX - 16, SHORT_LY - 8, SPEED_SHOT5, p->s_angle3, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX - 16, SHORT_LY - 8, SPEED_SHOT5, p->s_angle3 + 85, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX - 16, SHORT_LY - 8, SPEED_SHOT5, p->s_angle3 + 170, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX + 16, SHORT_LY - 8, SPEED_SHOT5, p->s_angle3, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX + 16, SHORT_LY - 8, SPEED_SHOT5, p->s_angle3 + 85, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX + 16, SHORT_LY - 8, SPEED_SHOT5, p->s_angle3 + 170, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT5, p->s_angle3, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT5, p->s_angle3 + 85, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT5, p->s_angle3 + 170, 0);
					p->s_angle3 += 12;
				}
				SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
				break;
			case 12:
				p->s_work3 = 0;
				break;
			default:
				break;
			}
		}
		break;


		/* ���Ԑ؂�P�� */
	case 5:
		switch (p->m_work) {
		case 0:
			if (SubEnemyMoveTo (p) < 0) {	/* �~�܂�܂ő҂� */
				p->m_work++;
				SubEnemyMoveToInit (p, 128 + 16, 160, SPEED_3, SPEED_3N);
			}
			break;
		case 1:	/* ��U�����܂ňړ����Ă���P�� */
			if (SubEnemyMoveTo (p) < 0) {	/* �~�܂�܂ő҂� */
				p->m_work++;
				SubEnemyMoveToInit (p, p->x, -128, SPEED_3, SPEED_3N);
			}
			break;
		case 2:
			if (SubEnemyMoveTo (p) < 0) {	/* �~�܂�܂ő҂� */
				return (-1);	/* ���� */
			}
			break;
		}
		break;

		/* ���� */
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
				if (p->m_work3 < 15) {
					signed short x, y;
					x = rndtable[(p->timer) & 0xff ^ 0x55] - 128;
					y = rndtable[(p->timer) & 0xff ^ 0xaa] - 128;
					/* p->timer �͗����̎�Ƃ��Ďg�p���Ă��܂� */
					EffectInit (EFFECT_EXPL, 0, x / 2 + p->x, y / 4 + p->y);
					SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
				}
				if (p->m_work3 > 20) {
					short h;
					for (h = 0; h < 6; h++) {
						EffectInit (EFFECT_EXPL, 0,
							    (signed short) ((rndtable[h] - 128) / 2) + p->x,
							    (signed short) ((rndtable[h + 1] - 128) / 4) + p->y + 16);
					}
					SoundSetSE (SE_EXPL_L);	/* �{�X�L���������� */
					if (p->core_info == (PALET_CORE_RED | PRIORITY_BOSS_CORE))
						EffectInit (EFFECT_POINTS_RED, POINTS_15000, p->x, p->y);
					else
						EffectInit (EFFECT_POINTS_BLUE, POINTS_7000, p->x, p->y);
					EffectInit (EFFECT_HAHEN, 0, p->x, p->y);

					return (-1);	/* ���� */
				}
			}
			break;
		}
		break;
	default:
		break;
	}
	/* ��ʊO�ɏo������� */
	if (SHORT_LY > 256 + 32)
		return (-1);	/* ���� */


	/* ���@�V���b�g�ɓ����������̏��� */
	if (p->damage) {
		if (p->damage_core) {
			p->info = PALET_RED | PRIORITY_BOSS;
			p->damage_core = 0;
		} else {
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
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 300)) {
			p->info = PALET_RED | PRIORITY_BOSS;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = p->work4;
		}
	}

	xobj_set_st (p);

	/* �R�A�̕\�� */
	p->core_x = p->x - 8;	/* �R�A�̒��S�� (-8,-8) �h�b�g����� */
	p->core_y = p->y;
	xsp_set_st (&(p->core_x));
	if (p->seq >= 4) {
		/* ��Q�`�Ԉȍ~�Ȃ�Q�ځA�R�ڂ̃R�A��\�� */
		p->core_x = p->x - 16;
		p->core_y = p->y - 16;
		xsp_set_st (&(p->core_x));
		p->core_x = p->x;
		p->core_y = p->y - 16;
		xsp_set_st (&(p->core_x));
	}
	return (0);
}



static void EnemyTiniLBoss02 (ENEMY * p)
{
	/* �p�[�c�����݂��Ă���������悤�Ɏw�� */
	if (p->parts1)
		p->parts1->child_kill = !0;
	if (p->parts2)
		p->parts2->child_kill = !0;
	if (p->parts3)
		p->parts3->child_kill = !0;
	if (p->parts4)
		p->parts4->child_kill = !0;
	entry_counter_stop = 0;	/* �G���g���[�J�E���^�[��~���� */
}
