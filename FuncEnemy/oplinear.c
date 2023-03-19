/* �J���Ē����e */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../entry.h"

#define PALET_MAIN		0x0400

#define SPEED_1		8	/* ���ړ��̉����x */
#define SPEED_1N	18	/* ���ړ��̉����x�𑫂��� */
#define SPEED_2		12	/* �P�ގ��̉����x */
#define SPEED_2N	24	/* �P�ގ��̉����x�𑫂��� */

#define SPEED_NRG	8	/* �G�e�̑��x�i�x�����j */
#define SPEED_NRG2	16	/* �G�e�̑��x�i�������j */

#define ENTRY_CONTROL_ADD	10	/* entry_control �ɑ����l */


enum {
	MOVETO_L = 0,
	MOVETO_R,
};

static short EnemyMoveOpLinear (ENEMY *);
static void EnemyTiniOpLinear (ENEMY *);


void EnemyInitOpLinear (ENEMY * p)
{
	p->vx = 0;
	p->vy = 8 * 65536;
	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 24;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->damage = p->damage_core = 0;
	p->flash = 0;
	p->hp = 240;
	p->pt = obj_oplaser;
	p->core_pt = sp_core01;
	p->seq = 0;
	p->func_enemy_move = EnemyMoveOpLinear;
	p->func_enemy_tini = EnemyTiniOpLinear;
	if (SHORT_LX < 128 + 16)
		p->cwork = 0;
	else
		p->cwork = !0;

	entry_control += ENTRY_CONTROL_ADD;
}



static short EnemyMoveOpLinear (ENEMY * p)
{
    /* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:
		p->vy -= 16384;
		if (p->vy < 0) {
			p->seq++;
			p->vy = 0;
		}
		break;
	case 1:		/* �J�� */
		if (p->pt == obj_oplaser + 27)
			SoundSetSE (SE_LOCK);	/* �u�������v�Ƃ������ʉ���炷 */
		if (p->pt < obj_oplaser + 35) {
			p->pt++;
		} else {
			p->seq++;
			p->s_work = 0;
			if (p->cwork == 0) {
				SubEnemyMoveToInit (p, SHORT_LX + 128, SHORT_LY - 16, SPEED_1, SPEED_1N);
				p->m_work = MOVETO_L;	/* ���̈ړ����� */
			} else {
				SubEnemyMoveToInit (p, SHORT_LX - 128, SHORT_LY - 16, SPEED_1, SPEED_1N);
				p->m_work = MOVETO_R;
			}
			p->m_work2 = 0;
			p->m_work3 = 0;
		}
		break;
	case 2:		/* �U���J�n */
		if (SubEnemyMoveTo (p) < 0) {
			short m_work_table[2] =
			{MOVETO_R, MOVETO_L};
			signed short dx_table[2] =
			{-64, 64};
			signed short dy_table[5] =
			{-16, 8, 16, -24, 16};
			p->m_work2++;
			if (p->m_work2 > 8) {
				p->seq++;	/* �P�� */
				SubEnemyMoveToInit (p, SHORT_LX, -64, SPEED_2, SPEED_2N);
			} else {
				SubEnemyMoveToInit (p, SHORT_LX + dx_table[p->m_work], SHORT_LY + dy_table[p->m_work3], SPEED_1, SPEED_1N);
				p->m_work = m_work_table[p->m_work];	/* ���̈ړ����� */
				p->m_work3++;
				if (p->m_work3 > 4)
					p->m_work3 = 0;
			}
		}
		switch (p->s_work++) {
#define	TIME_SHOT_INTERVAL	30	/* �V���b�g�����Ԋu */
		case 4 + TIME_SHOT_INTERVAL * 0:
		case 4 + TIME_SHOT_INTERVAL * 1:
		case 4 + TIME_SHOT_INTERVAL * 2:
		case 4 + TIME_SHOT_INTERVAL * 3:
			EshotInit (ESHOT_LASER01, SHORT_LX - 18, SHORT_LY + 24, SPEED_NRG, 64, 0);
			EshotInit (ESHOT_LASER01, SHORT_LX + 18, SHORT_LY + 24, SPEED_NRG, 64, 0);
			EshotInit (ESHOT_LASER01, SHORT_LX - 18, SHORT_LY + 24, SPEED_NRG2, 64, 0);
			EshotInit (ESHOT_LASER01, SHORT_LX + 18, SHORT_LY + 24, SPEED_NRG2, 64, 0);
			SoundSetSE (SE_LASER);
			break;
		case 4 + TIME_SHOT_INTERVAL * 5:
			p->s_work = 0;
			break;
		default:
			break;
		}
		break;

	case 3:		/* �P�� */
		if (SubEnemyMoveTo (p) < 0)
			SubEnemyMoveToInit (p, SHORT_LX, -64, SPEED_2, SPEED_2N);
	    /* ��ʊO�ɏo������� */
		if (SHORT_LY < -64)
			return (-1);	/* ���� */
		break;
	}

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
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
			if ((p->core_info == (PALET_CORE_RED | PRIORITY_BOSS))
			    || (p->core_info == (PALET_CORE_RED | PRIORITY_BOSS_CORE)))
				EffectInit (EFFECT_POINTS_RED, POINTS_2000, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_1200, p->x, p->y);
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
			return (-1);	/* ���� */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 100)) {
			p->info = PALET_RED | PRIORITY_BOSS;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = PALET_MAIN | PRIORITY_BOSS;
		}
	}
	xobj_set_st (p);

    /* �R�A�̕\�� */
	{
		short priority;

		if (p->pt > obj_oplaser + 30)
			priority = PRIORITY_BOSS;
		else
			priority = PRIORITY_BOSS_CORE;
		p->core_x = p->x - 8;	/* �R�A�̒��S�� (-8,-8) �h�b�g����� */
		p->core_y = p->y - 8;
		if (player->y < p->y + 64)	/* ���@�Ƃ̍��W�̍�����������ΐ� */
			p->core_info = PALET_CORE_RED | priority;
		else
			p->core_info = PALET_CORE_BLUE | priority;
		xsp_set_st (&(p->core_x));
	}
	return (0);
}



static void EnemyTiniOpLinear (ENEMY * p)
{
	entry_control -= ENTRY_CONTROL_ADD;
}
