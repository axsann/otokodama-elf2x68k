/* �R�ʃ{�X�U�R�E���̂Q (03u = 03 up) */
/* �{�X���o�Ă��Ă����ʉ�����o�ꂵ�č��̂���܂� */
#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"


#define SPEED_1		31	/* �����x */
#define SPEED_1N	10	/* �����x�𑫂��� */

#define BOSS_X	144
#define BOSS_Y	122

static short EnemyMoveLBoss03U (ENEMY *);
static void EnemyTiniLBoss03U (ENEMY *);


void EnemyInitLBoss03U (ENEMY * p)
{
	p->vx = 0;

	p->hit_px = p->hit_py = p->hit_sx = p->hit_sy = p->hit_cx = 0;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->seq = 0;
	p->pt = obj_lboss03 + 8;
	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS_PARTS_L;
	p->m_work = 0;
	p->func_enemy_move = EnemyMoveLBoss03U;
	p->func_enemy_tini = EnemyTiniLBoss03U;

	if (p->arg < 4) {
		p->info = PALET_LBOSS03 | PRIORITY_BOSS_PARTS_L;
	} else {
		p->arg -= 4;
		p->info = PALET_LBOSS03_1 | PRIORITY_BOSS_PARTS_L;
	}
	switch (p->arg) {
	case 0:
	case 1:
		p->vy = (-8 << 16);
		break;
	case 2:
	case 3:
		p->vy = (-7 << 16);
		break;
	}
}



static short EnemyMoveLBoss03U (ENEMY * p)
{
    /* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:		/* �o�꒼��A�������� */
		switch (p->arg) {
		case 0:
		case 1:
			if (SHORT_LY < 210) {
				p->vy += 16384;
				if (SHORT_VY > 0) {
					p->vy = 0;
					p->seq++;
				}
			}
			break;
		case 2:
		case 3:
			if (SHORT_LY < 244) {
				p->vy += 16384;
				if (SHORT_VY > 0) {
					p->vy = 0;
					p->seq++;
				}
			}
			break;
		}
		break;
	case 1:		/* lboss03.c �� p->seq++ �����̂�҂� */
		break;
	case 2:
		return (-1);	/* ���� */
		break;
	}


	xobj_set_st (p);


    /* �R�A�̕\�� */
	p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS;
	p->core_x = p->x - 8;	/* �R�A�̒��S�� (-8,-8) �h�b�g����� */
	p->core_y = p->y;
	xsp_set_st (&(p->core_x));


	return (0);
}



static void EnemyTiniLBoss03U (ENEMY * p)
{
}
