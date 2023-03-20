/* ���E�̃J�o�[���J���Ď΂ߒe */
#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"


#define PALET_MAIN	0x0600

static short EnemyMoveCoverN (ENEMY *);
static void EnemyTiniCoverN (ENEMY *);


void EnemyInitCoverN (ENEMY * p)
{
	/* ���E�ǂ��炩��o�������� */
	if (SHORT_LX < 144)
		p->vx = 8192;
	else
		p->vx = -8192;
	p->vy = 2 * 65536;
	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 24;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->damage = 0;
	p->flash = 0;
	p->hp = 50;
	p->pt = obj_covern;
	p->seq = 0;
	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS;
	p->func_enemy_move = EnemyMoveCoverN;
	p->func_enemy_tini = EnemyTiniCoverN;
}



static short EnemyMoveCoverN (ENEMY * p)
{
    /* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {

	case 0:
		if (p->y > 48)
			p->seq++;
		break;

	case 1:
		if (p->vy > 16384)
			p->vy -= 4096;
		else
			p->seq++;
		break;

	    /* �J�o�[���J���� */
	case 2:
		p->pt++;
		if (p->pt == obj_covern + 14) {	/* �J�o�[�J���������� */
			p->seq++;
			p->s_work2 = 0;
			p->s_work3 = 5;	/* �΂ߒe���x�����l */
			SoundSetSE (SE_LOCK);
			break;
		}
		break;

	    /* �J�o�[���J�����̂Œe������ */
	case 3:
	    /* �U������ */
		p->s_work2++;
	    /* �΂ߒe */
		switch (p->s_work2) {
#define INTERVAL2	5
		case INTERVAL2 * 1:
		case INTERVAL2 * 2:
		case INTERVAL2 * 3:
		case INTERVAL2 * 4:
		case INTERVAL2 * 5:
		case INTERVAL2 * 6:
			EshotInit (ESHOT_NRG02, p->x + 16, p->y + 1, p->s_work3, 64 - 8, 0);
			EshotInit (ESHOT_NRG02, p->x - 16, p->y + 1, p->s_work3, 64 + 8, 0);
			SoundSetSE (SE_ESHOT_M);	/* �G�V���b�g�� */
			p->s_work3 += 2;
			break;
		case INTERVAL2 * 7:
			p->seq++;
			p->s_work2 = 0;
			break;
		default:
			break;
		}
		break;

	case 4:		/* �J�o�[��߂ĐԂ��Ȃ� */
		p->s_work2++;
		if (p->s_work2 > 10) {
			if (p->pt > obj_covern) {
				p->pt--;
			} else {
				/* �܂肫������ */
				p->core_info = PALET_CORE_RED | PRIORITY_BOSS;
				SoundSetSE (SE_LOCK);
				p->seq++;
			}
		}
		break;

	    /* �P�� */
	case 5:
		if (p->vy > -1 * 65536)
			p->vy -= 4096;
	    /* ��ʊO�ɏo������� */
		if (SHORT_LY < -32)
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
				EffectInit (EFFECT_POINTS_RED, POINTS_1000, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_500, p->x, p->y);
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
			return (-1);	/* ���� */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 30)) {
			p->info = PALET_RED | PRIORITY_BOSS;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = PALET_MAIN | PRIORITY_BOSS;
		}
	}
	xobj_set_st (p);

    /* �R�A�̕\�� */
	p->core_x = p->x - 8;	/* �R�A�̒��S�� (-8,-12) �h�b�g����� */
	p->core_y = p->y - 12;
	xsp_set_st (&(p->core_x));

	return (0);
}



static void EnemyTiniCoverN (ENEMY * p)
{
}
