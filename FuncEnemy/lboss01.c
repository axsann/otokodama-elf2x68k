/* �P�ʃ{�X */
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

#define SPEED_SHOT1	8	/* �P�i�K�ڐ�^�V���b�g���x */
#define SPEED_SHOT2	10	/* �Q�i�K�ڐL�т�V���b�g�̈�Ԓx���e�̑��x */
#define SPEED_SHOT3	12	/* �Q�i�K�ڂȂ��|���e�̑��x */

#define HP_0	3000		/* �ϋv�� */
#define HP_1	2300		/* �ϋv�͂�����ȉ��ɂȂ�����Q�i�K�ڂ� */
#define HP_2	800		/* �ϋv�͂�����ȉ��ɂȂ������y���e */
#define TIMER_1	1500		/* ����ȏ㎞�Ԃ��o�߂�����Q�i�K�ڂ� */
#define TIMER_2	3500		/* ����ȏ㎞�Ԃ��o�߂�����P�� */

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
static short EnemyMoveLBoss01 (ENEMY *);
static void EnemyTiniLBoss01 (ENEMY *);


void EnemyInitLBoss01 (ENEMY * p)
{
	p->vx = 0;
	p->vy = 2 << 16;
	p->hit_px = 54;
	p->hit_py = 16;
	p->hit_sx = 54;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->pt = obj_lboss01 + 1;
	p->core_pt = sp_core01;
	p->seq = 0;
	p->func_enemy_move = EnemyMoveLBoss01;
	p->func_enemy_tini = EnemyTiniLBoss01;

	if (p->arg == 0)
		p->work4 = PALET_LBOSS01 | PRIORITY_BOSS;
	else
		p->work4 = PALET_LBOSS01_1 | PRIORITY_BOSS;

	entry_counter_stop = !0;/* �G���g���[�J�E���^�[��~ */
}



static short EnemyMoveLBoss01 (ENEMY * p)
{
	/* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;
	p->timer++;

	switch (p->seq) {
	case 0:
		if (SHORT_LY > 64)
			p->seq++;
		break;
	case 1:		/* ���� */
		p->vy -= 8192;
		if (p->vy < 8192) {
			p->seq++;
			p->vy = 0;
			p->m_work = MOVETO_INIT;	/* �ړ����� */
			p->s_work = 0;
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
		p->s_work++;
		if (p->s_work == 1) {
			p->s_angle = 16;
			p->s_angle2 = 128 - 16;
			p->s_work2 = 0;
		}
		if ((p->s_work > 6) && (p->s_work < 60)) {
			p->s_work2++;
			if (p->s_work2 > 3) {
				p->s_work2 = 0;
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1, p->s_angle, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1, p->s_angle2, 3);
				p->s_angle += 6;
				p->s_angle2 -= 6;
				SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
			}
		}
		if (p->s_work > 100)
			p->s_work = 0;

		/* ������x�_���[�W���󂯂� or ���Ԑ؂� */
		if ((p->hp < HP_1) || (p->timer > TIMER_1)) {
			/* �J�o�[�j�� */
			if (p->hp < HP_1)
				p->parts1 = EnemyInit (ENEMY_LBOSS01C, SHORT_LX, SHORT_LY, 0 + p->arg * 2, p);	/* �J�o�[�𕪗� */
			else
				p->parts1 = EnemyInit (ENEMY_LBOSS01C, SHORT_LX, SHORT_LY, 1 + p->arg * 2, p);	/* �J�o�[�𕪗� */
			if (!eshot_erase)
				eshot_erase = ESHOT_ERASE;	/* �e���� */
			p->pt++;
			p->seq++;
			p->m_work = 0;
			p->m_work2 = 0;
			p->hit_px = p->hit_sx = p->hit_py = p->hit_sy = 0;
		}
		break;
	case 3:		/* �A�j���[�V�����J�n�I */
		p->m_work2++;
		switch (p->m_work) {
		case 0:
			if (SubEnemyMoveTo (p) < 0)	/* �~�܂�܂ő҂� */
				p->m_work++;
			break;
		case 1:
			if (p->m_work2 < 100)
				break;
			if (p->pt < obj_lboss01 + 50) {
				p->pt++;
			} else {
				p->seq++;
				p->cwork = ATTACK_UPPER_CORE;
				p->m_work = MOVETO_INIT;	/* �ړ����� */
				p->s_work = 0;
				p->s_work2 = 0;
				p->s_work3 = 0;	/* �Ȃ��|���e�p */
				p->s_work4 = 0;	/* �y���e�p */
				p->s_angle3 = 0;	/* �Ȃ��|���e�p */
				p->hit_px = p->hit_sx = 70;
				p->hit_py = p->hit_sy = 16;
			}
			if ((p->pt == obj_lboss01 + 9) || (p->pt == obj_lboss01 + 30) || (p->pt == obj_lboss01 + 46))
				SoundSetSE (SE_LOCK_M);	/* �u�������v�Ƃ������ʉ���炷 */
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
			{0, 32 + 16, 128 + 16, 256 - 32 + 16, 128 + 16};
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
		if (p->cwork == ATTACK_UPPER_CORE) {
			/* ��̃R�A����U���i�L�т�e�j */
			p->s_work++;
			switch (p->s_work) {
			case 1:
				p->s_angle = psearch (p->x, p->y);	/* ���@�̕������T�[�` */
				if (p->x > player->x) {
					p->s_work2 = 0;
					p->s_angle -= 32;
				} else {
					p->s_work2 = !0;
					p->s_angle += 32;
				}
				break;
			case 10 + 20 * 0:
			case 10 + 20 * 1:
			case 10 + 20 * 2:
			case 10 + 20 * 3:
			case 10 + 20 * 4:
			case 10 + 20 * 5:
			case 10 + 20 * 6:
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY + 16, SPEED_SHOT2 + 3 * 0, p->s_angle, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY + 16, SPEED_SHOT2 + 3 * 1, p->s_angle, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY + 16, SPEED_SHOT2 + 3 * 2, p->s_angle, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY + 16, SPEED_SHOT2 + 3 * 3, p->s_angle, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY + 16, SPEED_SHOT2 + 3 * 4, p->s_angle, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY + 16, SPEED_SHOT2 + 3 * 5, p->s_angle, 3);
				SoundSetSE (SE_LASER);	/* �G�V���b�g�� */
				if (p->s_work2 == 0)
					p->s_angle += 8;
				else
					p->s_angle -= 8;
				break;
			case 10 + 20 * 9:
				p->cwork = ATTACK_LOWER_CORE;
				p->s_work = 0;
				p->s_work2 = 0;
				break;
			default:
				break;
			}
		} else {
			/* ���̃R�A����U���i���e�j */
			p->s_work++;
			if (p->s_work < 400) {
				if (p->s_work2++ > 1) {
					p->s_work2 = 0;
					EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY, 7, rndtable[p->angle++] & 0xfc, 8);
					EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY, 7, rndtable[p->angle++] & 0xfc, 8);
					SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
				}
			}
			if (p->s_work == 480) {
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* �e���� */
			}
			if (p->s_work > 500) {
				p->cwork = ATTACK_UPPER_CORE;
				p->s_work = 0;
				p->s_work2 = 0;
			}
		}
		/* �Ȃ��|���e */
		if (p->hp < HP_2) {
			switch (p->s_work3++) {
			case 0:
				EffectInit (EFFECT_EXPL, 0, p->x, p->y);
				SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
				break;
			case 1:
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT3, p->s_angle3, 0);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT3, p->s_angle3 + 128, 0);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, SPEED_SHOT3, p->s_angle3 + 64, 0);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, SPEED_SHOT3, p->s_angle3 + 192, 0);
				SoundSetSE (SE_ESHOT);
				p->s_angle3 += 16;
				break;
			case 11:
				p->s_work3 = 1;
				break;
			default:
				break;
			}
		}
		/* �{�X���b�V���� */
#define INTERVAL4_3	30
#define SPEED_SHOT4_3	22
		if (p->arg) {
			if (p->s_work4++ > INTERVAL4_3) {
				short h;
				char a = 0;
				for (h = 0; h < 16; h++)
					EshotInit (ESHOT_NRG24N, SHORT_LX, SHORT_LY + 8, SPEED_SHOT4_3, a += 16, 3);
				SoundSetSE (SE_EBOMB_M);	/* �G�y���e�� */
				p->s_work4 = 0;
			}
		}
		break;

		/* ���Ԑ؂�P�� */
	case 5:
		switch (p->m_work) {
		case 0:
			if (SubEnemyMoveTo (p) < 0) {	/* �~�܂�܂ő҂� */
				p->m_work++;
				SubEnemyMoveToInit (p, p->x, -128, SPEED_3, SPEED_3N);
			}
			break;
		case 1:
			if (SubEnemyMoveTo (p) < 0) {	/* �~�܂�܂ő҂� */
				entry_counter_stop = 0;	/* �G���g���[�J�E���^�[��~���� */
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
					EffectInit (EFFECT_EXPL, 0, x / 2 + p->x, y / 4 + p->y + 16);
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
						EffectInit (EFFECT_POINTS_RED, POINTS_10000, p->x, p->y);
					else
						EffectInit (EFFECT_POINTS_BLUE, POINTS_5000, p->x, p->y);
					EffectInit (EFFECT_HAHEN, 0, p->x, p->y);

					entry_counter_stop = 0;	/* �G���g���[�J�E���^�[��~���� */
					return (-1);	/* ���� */
				}
			}
			break;
		}
	default:
		break;
	}

	/* ��ʊO�ɏo������� */
	if (SHORT_LY > 256 + 32)
		return (-1);	/* ���� */


	/* ���@�V���b�g�ɓ����������̏��� */
	if (p->damage) {
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
	p->core_y = p->y - 8;
	if (p->seq != SEQ_DEATH) {	/* ���񂾂�R�A��Ԃ͕ω������Ȃ� */
		if (player->y < p->y + 96)	/* ���@�Ƃ̍��W�̍�����������ΐ� */
			p->core_info = PALET_CORE_RED | PRIORITY_BOSS_CORE;
		else
			p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS_CORE;
	}
	xsp_set_st (&(p->core_x));
	/* �R�A�Q�� */
	p->core_x = p->x - 8;	/* �R�A�̒��S�� (-8,+8) �h�b�g����� */
	p->core_y = p->y + 8;
	xsp_set_st (&(p->core_x));

	return (0);
}


static void EnemyTiniLBoss01 (ENEMY * p)
{
}
