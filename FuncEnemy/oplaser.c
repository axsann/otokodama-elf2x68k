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

#define PALET_MAIN	0x0600

#define HP_0		150

static short EnemyMoveOpLaser (ENEMY *);
static void EnemyTiniOpLaser (ENEMY *);

#define wabs(x) ((x) >= 0 ? (short) (x) : (short) -(x))

#define ENTRY_CONTROL_ADD	10	/* entry_control �ɑ����l */


void EnemyInitOpLaser (ENEMY * p)
{
	p->vx = 0;
	p->vy = 1 * 65536;
	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 24;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->damage = p->damage_core = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->pt = obj_oplaser;
	p->core_pt = sp_core01;
	p->seq = 0;
	p->func_enemy_move = EnemyMoveOpLaser;
	p->func_enemy_tini = EnemyTiniOpLaser;

	entry_control += ENTRY_CONTROL_ADD;
}



static short EnemyMoveOpLaser (ENEMY * p)
{
	/* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:
		if (SHORT_LY > 32)
			p->seq++;
		break;
	case 1:
		p->vy -= 2048;
		if (p->vy <= 16384)
			p->seq++;
		break;
	case 2:
		if (p->pt == obj_oplaser + 27)
			SoundSetSE (SE_LOCK);	/* �u�������v�Ƃ������ʉ���炷 */
		if (p->pt < obj_oplaser + 35) {
			p->pt++;
		} else {
			p->seq++;
			p->s_work = p->s_work2 = 0;
		}
		break;

		/* ���[�U�[���� */
	case 3:
#define INTERVAL_LASER	5
#define SPEED_LASER		24
		switch (p->s_work++) {
		case 4 + INTERVAL_LASER * 1:
			SoundSetSE (SE_LASER);	/* ���[�U�[���ʉ���炷 */
		case 4 + INTERVAL_LASER * 2:
		case 4 + INTERVAL_LASER * 3:
		case 4 + INTERVAL_LASER * 4:
			EshotInit (ESHOT_LASER01, SHORT_LX - 18, SHORT_LY + 20, SPEED_LASER, 64, 0);
			EshotInit (ESHOT_LASER01, SHORT_LX + 18, SHORT_LY + 20, SPEED_LASER, 64, 0);
			break;
		case 4 + INTERVAL_LASER * 10:
			p->s_work = 0;
			if (p->s_work2++ > 2) {
				p->seq++;
				p->s_work = p->s_work2 = 0;
			}
		default:
			break;
		}
		break;

	case 4:
#if	1
#define INTERVAL4_1		75
#define SPEED_SHOT4_1	10
		if (p->s_work2++ > INTERVAL4_1) {
			if (((p->y + 48) < player->y) && (wabs (p->x - player->x < 48))) {
				char a = psearch (p->x, p->y);	/* ���@�̕������T�[�` */
				p->s_work2 = 0;
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT4_1, a - 18, 2);
				//EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT4_1 - 4, a, 2);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT4_1, a, 2);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT4_1, a + 18, 2);
				SoundSetSE (SE_ESHOT_M);
			}
		}
#endif

		if (SHORT_LY > 196) {
			p->vy += 4096;
			if (SHORT_LY >= 1)
				p->seq++;
		}
		break;

		/* �P�� */
	case 5:
		/* ��ʊO�ɏo������� */
		if (SHORT_LY > 256 + 32)
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
				EffectInit (EFFECT_POINTS_RED, POINTS_2500, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_1200, p->x, p->y);
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



static void EnemyTiniOpLaser (ENEMY * p)
{
	entry_control -= ENTRY_CONTROL_ADD;
}
