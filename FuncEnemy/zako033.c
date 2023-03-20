/* �S�ʂœo�ꂷ��~�܂��Č����Ă���Γ�����Ȃ��U�R */
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

#define SPEED_1		4	/* ���x */
#define	SPEED_2		6	/* �Q�i�K�ڂ̑��x */



static short points_disp;	/* ���_��\�����邩 */

static short EnemyMoveZako033 (ENEMY *);
static void EnemyTiniZako033 (ENEMY *);


/* �Q�[���J�n���ɂP�񂾂��Ă΂�� */
void EnemyInitStaticZako033 (void)
{
	points_disp = 0;
}



void EnemyInitZako033 (ENEMY * p)
{
	p->angle = psearch (SHORT_LX, SHORT_LY);
	p->vx = vxtable[SPEED_1][p->angle];
	p->vy = vytable[SPEED_1][p->angle];
	p->pt = obj_zako02;
	p->hit_px = 12;
	p->hit_py = 12;
	p->hit_sx = 12;
	p->hit_sy = 12;
	p->hit_cx = 8;
	p->damage = p->damage_core = 0;
	p->hp = 1;
	p->seq = 0;
	p->m_work = 0;
	p->pt = obj_zako03 + 2;
	p->info = PALET_MAIN | PRIORITY_ZAKO;
	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_BLUE | PRIORITY_ZAKO;

	p->func_enemy_move = EnemyMoveZako033;
	p->func_enemy_tini = EnemyTiniZako033;
}



static short EnemyMoveZako033 (ENEMY * p)
{
	/* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:
		if (p->m_work++ > 30) {
			p->seq++;
			p->s_work = p->s_work2 = p->s_work3 = 0;
		}
		break;

		/* �U���J�n */
	case 1:
		switch (p->s_work) {

		case 0:	/* 2-Way �V���b�g */
#define INTERVAL1_0		16
#define SPEED_SHOT1_0	12
			if (p->s_work2++ > INTERVAL1_0) {
				p->s_work2 = 0;
				if (p->s_work3++ < 3) {
					char a = psearch (SHORT_LX, SHORT_LY);
					EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT1_0, a - 8, 0);
					EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT1_0, a + 8, 0);
					SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
				} else {
					p->s_work++;
					p->s_work3 = 0;
					p->core_info = PALET_CORE_RED | PRIORITY_ZAKO;	/* �Ԃ��Ȃ� */
				}
			}
			break;

		case 1:	/* 3-Way �V���b�g */
#define INTERVAL1_1		15
#define SPEED_SHOT1_1	18
			if (p->s_work2++ > INTERVAL1_1) {
				p->s_work2 = 0;
				if (p->s_work3++ < 2) {
					char a = psearch (SHORT_LX, SHORT_LY);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1_1, a - 8, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1_1, a, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1_1, a + 8, 0);
					SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
				} else {
					p->seq++;
					p->vx = vxtable[SPEED_2][p->angle];
					p->vy = vytable[SPEED_2][p->angle];
				}
			}
			break;
		}
		break;

	case 2:		/* ������ */
		/* ��ʊO�ɏo������� */
		/* �i��ʍ��E����o������Ə㉺����o��������P�񂸂̔�r�ōs���Ă���j */
		if (((unsigned short) p->x > 256 + 32) || ((unsigned short) p->y > 256 + 32))
			return (-1);	/* ���� */
		break;

	default:
		break;
	}

	/* ���@�V���b�g�ɓ����������̏��� */
	/* �U�R�͈ꌂ�Ŏ��ʂ̂ŃR�A�]�X�͍l�����Ȃ��ėǂ� */
	if (p->damage) {
		EffectInit (EFFECT_EXPLZAKO, 0, p->x, p->y);
		SoundSetSE (SE_EXPL);
		if (points_disp < ZAKO_SCORE_DISP) {
			points_disp++;
			if (p->core_info == (PALET_CORE_RED | PRIORITY_ZAKO))
				EffectInit (EFFECT_POINTS_RED, POINTS_500, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_100, p->x, p->y);
		} else {
			if (p->core_info == (PALET_CORE_RED | PRIORITY_ZAKO))
				EffectInit (EFFECT_POINTS, POINTS_500, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS, POINTS_100, p->x, p->y);
		}
		return (-1);	/* ���� */
	}
	xobj_set_st (p);

	/* �R�A�̕\�� */
	p->core_x = p->x - 8;
	p->core_y = p->y - 8;
	xsp_set_st (&(p->core_x));

	return (0);
}



static void EnemyTiniZako033 (ENEMY * p)
{
}
