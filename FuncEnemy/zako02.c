/* ��]����U�R */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"

#define PALET_MAIN	0x0600


static short points_disp;	/* ���_��\�����邩 */

static short EnemyMoveZako02 (ENEMY *);
static void EnemyTiniZako02 (ENEMY *);

enum {
	APPEAR_LEFT = 0, APPEAR_RIGHT
};



/* �Q�[���J�n���ɂP�񂾂��Ă΂�� */
void EnemyInitStaticZako02 (void)
{
	points_disp = 0;
}



void EnemyInitZako02 (ENEMY * p)
{
	/* ��Ƀv���C���[�̔��Α����猻��� */
	/* �w���W�͉�ʒ[����̑��Βl */
	if (player->x > 144) {
		p->vx = 3 * 65536;
		p->lx = -16 * 65536 - p->lx;
		p->cwork = APPEAR_LEFT;
	} else {
		p->vx = -3 * 65536;
		p->lx = (256 + 16) * 65536 + p->lx;
		p->cwork = APPEAR_RIGHT;
	}
	p->vy = -3 * 65536;
	p->pt = obj_zako02;
	p->hit_px = 8;
	p->hit_py = 12;
	p->hit_sx = 12;
	p->hit_sy = 12;
	p->damage = 0;
	p->damage_core = 0;
	p->hp = 1;
	p->seq = 0;
	p->info = PALET_MAIN | PRIORITY_ZAKO;
	p->core_pt = sp_core01 + 1;
	p->func_enemy_move = EnemyMoveZako02;
	p->func_enemy_tini = EnemyTiniZako02;
}



static short EnemyMoveZako02 (ENEMY * p)
{
	/* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;


	switch (p->seq) {
	case 0:
		p->pt++;
		p->vy += 8192;
		if (p->vy > 0) {
			p->seq++;
			p->vy = 4096;
			p->s_work = 0;	/* �V���b�g�p�J�E���^�[ */
		}
		break;

		/* ���΂炭��� */
	case 1:
		p->pt++;
#define SPEED_SHOT1_1	10
		switch (p->s_work++) {
		case 2:
			{
				char a = psearch (p->x, p->y);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1_1 - 2, a, 0);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT1_1, a, 0);
				SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
			}
			break;
		case 14:
			p->seq++;
			p->s_work = 0;
#define SPEED_1		24	/* �����x */
#define SPEED_1N	28	/* �����x�𑫂��� */
			if (p->cwork == 0)
				SubEnemyMoveToInit (p, SHORT_LX - 32, SHORT_LY + 128, SPEED_1, SPEED_1N);
			else
				SubEnemyMoveToInit (p, SHORT_LX + 32, SHORT_LY + 128, SPEED_1, SPEED_1N);
			break;
		default:
			break;
		}
		if (p->vx > 0)
			p->vx -= 8192;
		else
			p->vx += 8192;
		break;

		/* ������ */
	case 2:
		p->pt++;
		/* �ݒ�l�ɏ]���Ĉړ� */
		if (SubEnemyMoveTo (p) < 0) {
			p->seq++;
			if (p->cwork == 0)
				SubEnemyMoveToInit (p, SHORT_LX + 32, SHORT_LY + 256, SPEED_1, SPEED_1N);
			else
				SubEnemyMoveToInit (p, SHORT_LX - 32, SHORT_LY + 256, SPEED_1, SPEED_1N);
		}
		/* �U������ */
#define INTERVAL2_1	35
		switch (p->s_work++) {
		case 1 + INTERVAL2_1 * 0:
		case 1 + INTERVAL2_1 * 1:
			{
				char a = psearch (p->x, p->y);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1_1 - 2, a, 0);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT1_1, a, 0);
				SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
			}
			break;
		}
		break;

	case 3:
		p->pt++;
		if (SubEnemyMoveTo (p) < 0)
			SubEnemyMoveToInit (p, SHORT_LX, SHORT_LY + 128, SPEED_1, SPEED_1N);
		break;
	default:
		break;
	}
	if (p->pt > obj_zako02 + 15)
		p->pt -= 15;

	/* ��ʊO�ɏo������� */
	if (SHORT_LY > 256 + 32)
		return (-1);	/* ���� */


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
	if (player->y < p->y + 64)	/* ���@�Ƃ̍��W�̍�����������ΐ� */
		p->core_info = PALET_CORE_BLUE | PRIORITY_ZAKO;
	else
		p->core_info = PALET_CORE_RED | PRIORITY_ZAKO;
	xsp_set_st (&(p->core_x));

	return (0);
}



static void EnemyTiniZako02 (ENEMY * p)
{
}
