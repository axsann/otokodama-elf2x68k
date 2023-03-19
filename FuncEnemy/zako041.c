/* �U�R�S�̎ア��i�U�����Ă��Ȃ��j */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"

#define PALET_MAIN	0x0300


static short points_disp;	/* ���_��\�����邩 */

static short EnemyMoveZako041 (ENEMY *);
static void EnemyTiniZako041 (ENEMY *);


/* �Q�[���J�n���ɂP�񂾂��Ă΂�� */
void EnemyInitStaticZako041 (void)
{
	points_disp = 0;
}


void EnemyInitZako041 (ENEMY * p)
{
#define	SPEED_0		25
	{
		signed short x;	/* �ړI�n�� x */
		char a;
		if (SHORT_LX < 128 + 16)
			x = player->x - 32;	/* ��������o�ꂵ���Ȃ玩�@�̏������� */
		else
			x = player->x + 32;

		/* �ړI�n����ʊO�ɂȂ�Ȃ��悤�� */
		if (x < 32 + 16)
			x = 32 + 16;
		if (x > 256 - 32 + 16)
			x = 256 - 32 + 16;
		a = p->angle = psearch (x, SHORT_LY);
		p->vx = vxtable[SPEED_0][a];
		p->vy = vytable[SPEED_0][a];
	}
	p->hit_px = 8;
	p->hit_py = 12;
	p->hit_sx = 12;
	p->hit_sy = 12;
	p->hit_cx = 4;
	p->m_work = 0;
	p->damage = p->damage_core = 0;
	p->hp = 1;
	p->seq = 0;

	p->info = PALET_MAIN | PRIORITY_ZAKO;
	p->core_pt = sp_core01 + 1;
	p->core_info = PALET_CORE_BLUE | PRIORITY_ZAKO;
	p->func_enemy_move = EnemyMoveZako041;
	p->func_enemy_tini = EnemyTiniZako041;
}



static short EnemyMoveZako041 (ENEMY * p)
{
    /* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:		/* �܂��˂�����ł��� */
	    /* ������x�i�񂾂����@�ɋߕt�����猸�� */
		if ((p->m_work++ > 45) || (p->y > player->y - 32)) {
			p->seq++;
			p->m_work = 0;
			p->m_work2 = 0;
		}
		break;
	case 1:		/* ���� */
#define	SPEED_1	0
		if (p->m_work2++ > 5) {
			p->m_work2 = 0;
			p->vx -= vxtable[SPEED_1][p->angle];
			p->vy -= vytable[SPEED_1][p->angle];
			p->vy += 4096;
			if (p->m_work++ > 4) {
				p->seq++;
				//p->vx = p->vy = 0;
				p->core_info = PALET_CORE_RED | PRIORITY_ZAKO;	/* �R�A�Ԃ��Ȃ� */
			}
		}
		break;

	    /* �P�� */
	case 2:
		if (SHORT_VY > -3)
			p->vy -= 4096;

		if ((p->angle > 192 - 4) && (p->angle < 192 + 4)) {
			p->angle = 192;
		} else {
		    /* ������� */
			if ((unsigned char) (p->angle - 192) > (unsigned char) 128)
				p->angle += 4;
			else
				p->angle -= 4;
		}
		p->pt = obj_zako04 + ((p->angle) >> 3);
	    /* ��ʊO�ɏo������� */
		if (p->y < -32)
			return (-1);	/* ���� */
		break;
	default:
		break;
	}
	p->pt = obj_zako04 + ((p->angle) >> 3);


    /* ���@�V���b�g�ɓ����������̏��� */
    /* �U�R�͈ꌂ�Ŏ��ʂ̂ŃR�A�]�X�͍l�����Ȃ��ėǂ� */
	if (p->damage) {
		EffectInit (EFFECT_EXPLZAKO, 0, p->x, p->y);
		SoundSetSE (SE_EXPL);
		if (points_disp < ZAKO_SCORE_DISP) {
			points_disp++;
			if (p->core_info == (PALET_CORE_RED | PRIORITY_ZAKO))
				EffectInit (EFFECT_POINTS_RED, POINTS_200, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_100, p->x, p->y);
		} else {
			if (p->core_info == (PALET_CORE_RED | PRIORITY_ZAKO))
				EffectInit (EFFECT_POINTS, POINTS_200, p->x, p->y);
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



static void EnemyTiniZako041 (ENEMY * p)
{
}
