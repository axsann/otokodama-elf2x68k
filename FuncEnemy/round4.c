/* ��]�S���H */
#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"
#include "../entry.h"

#define PALET_MAIN	0x0800

#define HP_0	180
#define SHOT_SPEED		7

static short EnemyMoveRound4 (ENEMY *);
static void EnemyTiniRound4 (ENEMY *);

enum {
	APPEAR_LEFT = 0, APPEAR_RIGHT
};

#define ENTRY_CONTROL_ADD	10	/* entry_control �ɑ����l */


void EnemyInitRound4 (ENEMY * p)
{
	if (SHORT_LX < 144) {
		p->cwork = APPEAR_LEFT;
		p->vx = 5 * 65536;
	} else {
		p->cwork = APPEAR_RIGHT;
		p->vx = -5 * 65536;
	}
	p->vy = -(3 << 16);
	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 24;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->pt = obj_round4 + 25;
	p->seq = 0;
	p->work2 = 0;
	p->m_work3 = 0;		/* ��]�p�i8+8bit �Œ菬���_�j */
	p->m_work4 = 2 << 8;	/* ��]�p�̑��x */

	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_RED | PRIORITY_BOSS;
	p->func_enemy_move = EnemyMoveRound4;
	p->func_enemy_tini = EnemyTiniRound4;

	entry_control += ENTRY_CONTROL_ADD;
}



static short EnemyMoveRound4 (ENEMY * p)
{
    /* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:
	    /* �ړ����� */
		if (p->cwork == APPEAR_LEFT)
			if (p->vx > 0)
				p->vx -= 4096;
			else
				p->vx = 0;
		else if (p->vx < 0)
			p->vx += 4096;
		else
			p->vx = 0;
		if (SHORT_VY < 0)
			p->vy += 4096;

	    /* ��]���� */
		p->m_work3 += p->m_work4;
		if (p->m_work4 > 64)
			p->m_work4 -= 4;
		p->pt = obj_round4 + 25 + *(char *) (&p->m_work3);
		if (p->m_work3 > (8 << 8)) {
			p->m_work3 -= (8 << 8);
			p->pt = obj_round4 + 25;
			if (p->m_work4 < 192) {
			    /* ��]���������ĂقƂ�ǒ�~�����玟�̒i�K�ֈڍs */
				p->seq++;
				p->vx = 0;
				p->pt = obj_round4;
			}
		}
		break;

	case 1:		/* �H���J�� */
		p->pt++;
		if (p->pt == obj_round4 + 13)
			SoundSetSE (SE_LOCK);	/* �u�������v�Ƃ������ʉ���炷 */

		if (p->pt > obj_round4 + 23) {
			p->seq++;
			p->s_work = p->s_work2 = p->s_work3 = 0;
			p->s_angle = 64;
			p->m_work3 = 0;	/* ��]�p�i8+8bit�Œ菬���_�j */
			p->m_work4 = 16;	/* ��]�p�����x */
			if (p->x > player->x)	/* ���̎��_�Ŏ��@���T�[�` */
				p->cwork2 = !0;
			else
				p->cwork2 = 0;
		}
		break;

	case 2:		/* �J������Ԃŉ�] */
	    /* �ړ����� */
		if (p->vy < 65536)
			p->vy += 4096;

	    /* ��]���� */
		p->m_work3 += p->m_work4;
		if (p->m_work4 < 256)
			p->m_work4 += 4;
		else
			p->m_work4 = 256;
		if (p->m_work3 > (8 << 8))
			p->m_work3 -= (8 << 8);
		p->pt = obj_round4 + 25 + 9 + *(char *) (&p->m_work3);

	    /* �U������ */
		switch (p->s_work) {
		case 0:
			if (p->s_work2++ > 20) {
				p->s_work++;
				p->s_work2 = 0;
			}
			break;

		case 1:
#define INTERVAL2_1	7
#define SPEED_SHOT2	8
			if (p->s_work2++ > INTERVAL2_1) {
				char a = p->s_angle;
				p->s_work2 = 0;
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT2, a+64*0, 2);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT2, a+64*1, 2);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT2, a+64*2, 2);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT2, a+64*3, 2);
				SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
				p->s_angle+=12;
				if (p->s_work3++ == 8)
					p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS;
				if (p->s_work3 > 10)
					p->seq++;
			}
			break;
		}
		break;

	case 3:		/* �㏸���ē����� */
	    /* �ړ����� */
		if (p->vy > -65536)
			p->vy -= 4096;

	    /* ��]���� */
		p->m_work3 += p->m_work4;
		if (p->m_work4 < 256)
			p->m_work4 += 4;
		else
			p->m_work4 = 256;
		if (p->m_work3 > (8 << 8))
			p->m_work3 -= (8 << 8);
		p->pt = obj_round4 + 25 + 9 + *(char *) (&p->m_work3);
		break;
	}

    /* ��ʊO�ɏo������� */
	if (SHORT_LY < -48)
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
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
			if (p->core_info == (PALET_CORE_RED | PRIORITY_BOSS))
				EffectInit (EFFECT_POINTS_RED, POINTS_1200, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_700, p->x, p->y);
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);

			return (-1);	/* ���� */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 80)) {
			p->info = PALET_RED | PRIORITY_BOSS;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = PALET_MAIN | PRIORITY_BOSS;
		}
	}
	xobj_set_st (p);

    /* �R�A�̕\�� */
	p->core_x = p->x - 8;	/* �R�A�̒��S�� (-8,-8) �h�b�g����� */
	p->core_y = p->y - 8;
	xsp_set_st (&(p->core_x));

	return (0);
}
static void EnemyTiniRound4 (ENEMY * p)
{
	entry_control -= ENTRY_CONTROL_ADD;
}
