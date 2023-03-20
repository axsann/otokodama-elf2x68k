/* �R�ʂœo�ꂷ��j�󂷂���y���e�𔭎˂���U�R */
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

#define SPEED_1		8	/* ���x */
#define	SPEED_2		12	/* �Q�i�K�ڂ̑��x */


static short points_disp;	/* ���_��\�����邩 */

static short EnemyMoveZako031 (ENEMY *);
static void EnemyTiniZako031 (ENEMY *);


/* �Q�[���J�n���ɂP�񂾂��Ă΂�� */
void EnemyInitStaticZako031 (void)
{
	points_disp = 0;
}



void EnemyInitZako031 (ENEMY * p)
{
	if (player->x < 144)
		p->lx = 144 * 65536 + p->lx;
	else
		p->lx = 144 * 65536 - p->lx;
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
	p->s_work = p->s_work2 = p->s_work3 = 0;
	p->pt = obj_zako03 + 1;
	p->info = PALET_MAIN | PRIORITY_ZAKO;
	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_BLUE | PRIORITY_ZAKO;

	p->func_enemy_move = EnemyMoveZako031;
	p->func_enemy_tini = EnemyTiniZako031;
}



static short EnemyMoveZako031 (ENEMY * p)
{
    /* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:
		if (p->m_work++ > 30) {
			p->seq++;
		}
		break;
	case 1:
		break;
	}

    /* �U���J�n */
	switch (p->s_work) {

	case 0:		/* 3-Way �V���b�g */
#define INTERVAL1_0		24
#define SPEED_SHOT1_0	10
		if (p->s_work2++ > INTERVAL1_0) {
			p->s_work2 = 0;
			switch (p->s_work3++) {
			    /* case �̏��Ԃɒ��� */
			case 1:
			    /* ������ break ���Ȃ����Ƃɒ��� */
			case 0:
				{
					char a = psearch (SHORT_LX, SHORT_LY);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1_0, a - 8, 0);
					EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT1_0 + 2, a, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1_0, a + 8, 0);
				}
				SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
				break;
			case 2:
			default:
				p->s_work++;
				p->s_work2 = p->s_work3 = 0;
				p->core_info = PALET_CORE_RED | PRIORITY_ZAKO;	/* �Ԃ��Ȃ� */
				break;
			}
		}
		break;

	case 1:		/* 2-Way �V���b�g */
#define INTERVAL1_1		15
#define SPEED_SHOT1_1	10
		if (p->s_work2++ > INTERVAL1_1) {
			p->s_work2 = 0;
			if (p->s_work3++ < 2) {
				char a = psearch (SHORT_LX, SHORT_LY);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1_1, a - 8, 0);
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


    /* ��ʊO�ɏo������� */
    /* �i��ʍ��E����o������Ə㉺����o��������P�񂸂̔�r�ōs���Ă���j */
	if ((p->seq >= 1)
	    && (((unsigned short) p->x > 256 + 32) || ((unsigned short) p->y > 256 + 32)))
		return (-1);	/* ���� */

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
				EffectInit (EFFECT_POINTS_BLUE, POINTS_200, p->x, p->y);
		} else {
			if (p->core_info == (PALET_CORE_RED | PRIORITY_ZAKO))
				EffectInit (EFFECT_POINTS, POINTS_500, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS, POINTS_200, p->x, p->y);
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



static void EnemyTiniZako031 (ENEMY * p)
{
	short h;
	unsigned char a = 0;
#define SPEED_SHOT2_3 12
	for (h = 0; h < 16; h++)
		EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY, SPEED_SHOT2_3, a += 16, 0);
}
