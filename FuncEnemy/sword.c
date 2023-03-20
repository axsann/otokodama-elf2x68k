/* ���݂����ɒ����G */
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


#define PALET_MAIN	0x0200
#define HP_0	350

#define wabs(x) ((x) >= 0 ? (short) (x) : (short) -(x))

#define ENTRY_CONTROL_ADD	10	/* entry_control �ɑ����l */

enum {
	APPEAR_LEFT = 0, APPEAR_RIGHT
};

static short EnemyMoveSword (ENEMY *);
static void EnemyTiniSword (ENEMY *);


void EnemyInitSword (ENEMY * p)
{
	if (SHORT_LX < 144)
		p->cwork = APPEAR_LEFT;
	else
		p->cwork = APPEAR_RIGHT;
	p->vx = 0;
	p->vy = 2 * 65536;
	p->hit_px = 28;
	p->hit_py = 16;
	p->hit_sx = 48;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->pt = obj_sword;
	p->seq = 0;
	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS;
	p->func_enemy_move = EnemyMoveSword;
	p->func_enemy_tini = EnemyTiniSword;

	entry_control += ENTRY_CONTROL_ADD;
}



static short EnemyMoveSword (ENEMY * p)
{
	/* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {

	case 0:
		if (p->y > 16)
			p->seq++;
		break;

	case 1:
		if (SHORT_VY > -1)
			p->vy -= 4096;
		else
			p->seq++;
		break;

		/* �J�o�[�J�� */
	case 2:
		p->pt++;
		if (p->pt >= obj_sword + 24) {	/* �J�o�[�J���������� */
			p->seq++;
			SoundSetSE (SE_LOCK);
			p->m_work = 0;
			p->s_work = p->s_work2 = p->s_work3 = 0;
		}
		break;

	case 3:
		/* �ړ����� */
#define SPEED_3		2048	/* �w���������x */
#define SPEED_3N	12	/* �����x�𑫂��� */
#define SPEED_3S	150	/* ����𒴂����猸���J�n */
		if (p->m_work++ < SPEED_3N) {
			if (p->cwork == APPEAR_LEFT)
				p->vx += SPEED_3;
			else
				p->vx -= SPEED_3;
		}
		if ((p->m_work > SPEED_3S) && (p->m_work < SPEED_3S + SPEED_3N + 1)) {
			if (p->cwork == APPEAR_LEFT)
				p->vx -= SPEED_3;
			else
				p->vx += SPEED_3;
		}
		if (p->m_work > SPEED_3S + SPEED_3N + 1) {
			if (p->vy < 16384)
				p->vy += 4096;
		}
		/* �U������ */

		/* �ʏ�e */
#define INTERVAL3_1	14
#define SPEED_SHOT3_1	12
		switch (p->s_work++) {
		case INTERVAL3_1 * 1:
		case INTERVAL3_1 * 2:
		case INTERVAL3_1 * 3:
			EshotInit (ESHOT_NRG04, p->x, p->y, SPEED_SHOT3_1, psearch (p->x, p->y), 0);
			SoundSetSE (SE_ESHOT_M);	/* �G�V���b�g�� */
			break;
		case INTERVAL3_1 * 9:
			p->s_work = 0;
			break;
		}

		/* ���@�ɂ�����x�ߕt������U��������e */
		if ((p->s_work2 == 0) && (wabs (player->x - p->x) < 32) && (p->y + 64 < player->y)) {
			p->s_work2++;
			p->s_angle = 64 + 24;
			p->s_angle2 = 64 - 24;
		}
#define INTERVAL3_2		9
#define SPEED_SHOT3_2	8
		if (p->s_work2) {
			switch (p->s_work2++) {
			case INTERVAL3_2 * 1:
			case INTERVAL3_2 * 2:
			case INTERVAL3_2 * 3:
			case INTERVAL3_2 * 4:
			case INTERVAL3_2 * 5:
			case INTERVAL3_2 * 6:
				EshotInit (ESHOT_NRG02, p->x - 12, p->y - 8, SPEED_SHOT3_2, p->s_angle -= 6, 0);
				EshotInit (ESHOT_NRG02, p->x + 12, p->y - 8, SPEED_SHOT3_2, p->s_angle2 += 6, 0);
				SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
				break;
			case INTERVAL3_2 * 20:
				p->s_work2 = 0;
				break;
			case 0:
				break;
			}
		}
		if (SHORT_LY > 192)
			p->seq++;
		break;

		/* �P�� */
	case 4:
		if (SHORT_VY < 1)
			p->vy += 4096;
		/* ��ʊO�ɏo������� */
		if (SHORT_LY > 256 + 32 + 16)
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
			if (p->core_info == (PALET_CORE_RED | PRIORITY_BOSS))
				EffectInit (EFFECT_POINTS_RED, POINTS_2500, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_1500, p->x, p->y);
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
	p->core_x = p->x - 8;	/* �R�A�̒��S�� (-8,-8) �h�b�g����� */
	p->core_y = p->y - 8;
	if (player->y < p->y + 64)	/* ���@�Ƃ̍��W�̍�����������ΐ� */
		p->core_info = PALET_CORE_RED | PRIORITY_BOSS;
	else
		p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS;
	xsp_set_st (&(p->core_x));

	return (0);
}



static void EnemyTiniSword (ENEMY * p)
{
	entry_control -= ENTRY_CONTROL_ADD;
}
