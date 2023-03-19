/* �Q�ʃ{�X���E�� (02e = 02 ear) */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"


#define SPEED_SHOT1	8	/* �P�i�K�ڐ�^�V���b�g���x */
#define SPEED_SHOT2	10	/* �Q�i�K�ڐL�т�V���b�g�̈�Ԓx���e�̑��x */

#define HP_0	70		/* �ϋv�� */



enum {
	EAR_LEFT = 0, EAR_RIGHT
};

static short EnemyMoveLBoss02E (ENEMY *);
static void EnemyTiniLBoss02E (ENEMY *);


void EnemyInitLBoss02E (ENEMY * p)
{
	p->vx = 0;
	p->vy = 2 << 16;
	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 24;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->seq = 0;
	p->m_work = 0;
	p->s_work = 0;
	p->func_enemy_move = EnemyMoveLBoss02E;
	p->func_enemy_tini = EnemyTiniLBoss02E;

	if (p->lx < 0) {
		p->cwork = EAR_LEFT;	/* �����E�� */
		p->pt = obj_lboss02 + 7;
	} else {
		p->cwork = EAR_RIGHT;
		p->pt = obj_lboss02 + 8;
	}
}



static short EnemyMoveLBoss02E (ENEMY * p)
{
	signed short offset[14][2] =	/* ���p�[�c�̍��W�I�t�Z�b�g */
	{
		{-37, -29},
		{-37, -30},
		{-37, -34},
		{-38, -38},
		{-39, -42},
		{-39, -46},
		{-40, -48},
		{-40, -51},
		{-39, -44},
		{-39, -47},
		{-40, -51},
		{-39, -45},
		{-40, -48},
		{-40, -48}
	};

	if ((p->child_kill) || (p->child_death))
		return (-1);	/* ���� */

	switch (p->seq) {
	case 0:
		if (p->m_work < 12) {
			if (p->cwork == EAR_LEFT)
				p->x = p->parent->x + offset[p->m_work][0];
			else
				p->x = p->parent->x - offset[p->m_work][0] + 6;
			p->y = p->parent->y + offset[p->m_work][1];
		}
		if (p->m_work == 7)
			SoundSetSE (SE_LOCK_M);	/* �u�������v�Ƃ������ʉ���炷 */
		if (p->m_work == 12)
			p->seq++;
		p->m_work++;
		break;
	case 1:
		if (p->cwork == EAR_LEFT) {
		    /* ���� */
			p->x = p->parent->x - 40;
			p->y = p->parent->y - 48;
		    /* ���ɍU������ */
			p->s_work++;
			if (p->s_work == 1) {
				p->s_angle = 32;
				p->s_work2 = 0;
			}
			if ((p->s_work > 6) && (p->s_work < 60)) {
				p->s_work2++;
				if (p->s_work2 > 8) {
					p->s_work2 = 0;
					EshotInit (ESHOT_NRG03, p->x - 10, p->y + 4, SPEED_SHOT1, p->s_angle + 12, 0);
					EshotInit (ESHOT_NRG03, p->x - 4, p->y + 2, SPEED_SHOT1, p->s_angle + 4, 0);
					EshotInit (ESHOT_NRG03, p->x + 4, p->y - 2, SPEED_SHOT1, p->s_angle - 4, 0);
					EshotInit (ESHOT_NRG03, p->x + 10, p->y - 4, SPEED_SHOT1, p->s_angle - 12, 0);
					p->s_angle += 12;
					SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
				}
			}
			if (p->s_work > 200)
				p->s_work = 0;
		} else {
		    /* �E�� */
			p->x = p->parent->x + 46;
			p->y = p->parent->y - 48;

		    /* ���ɍU������ */
			p->s_work++;
			if (p->s_work == 1) {
				p->s_angle = 128 - 32;
				p->s_work2 = 0;
			}
			if ((p->s_work > 6) && (p->s_work < 60)) {
				p->s_work2++;
				if (p->s_work2 > 8) {
					p->s_work2 = 0;
					EshotInit (ESHOT_NRG03, p->x - 10, p->y - 4, SPEED_SHOT1, p->s_angle + 12, 0);
					EshotInit (ESHOT_NRG03, p->x - 4, p->y - 2, SPEED_SHOT1, p->s_angle + 4, 0);
					EshotInit (ESHOT_NRG03, p->x + 4, p->y + 2, SPEED_SHOT1, p->s_angle - 4, 0);
					EshotInit (ESHOT_NRG03, p->x + 10, p->y + 4, SPEED_SHOT1, p->s_angle - 12, 0);
					p->s_angle -= 12;
					SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
				}
			}
			if (p->s_work > 200)
				p->s_work = 0;

		}
		break;
	}
    /* ���@�V���b�g�ɓ����������̏��� */
	if (p->damage) {
		SoundSetSE (SE_DAMAGE);
		p->info = PALET_DAMAGE | PRIORITY_BOSS;
		p->damage_core = 0;
		if ((p->hp -= p->damage) <= 0) {
		    /* ���� */
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			EffectInit (EFFECT_POINTS_RED, POINTS_3000, p->x, p->y);
			SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
			p->child_death = !0;	/* �������� */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 50)) {
			p->info = PALET_RED | PRIORITY_BOSS_PARTS_L;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = p->parent->work4;
		}
	}
	xobj_set_st (p);


	return (0);
}


static void EnemyTiniLBoss02E (ENEMY * p)
{
}
