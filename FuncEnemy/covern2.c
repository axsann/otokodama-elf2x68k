/* ���E�̃J�o�[���J���Ēe�E���̂Q */
#include <xsp2lib.h>

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

#define HP_0	180

#define ENTRY_CONTROL_ADD	10	/* entry_control �ɑ����l */

#define wabs(x) ((x) >= 0 ? (short) (x) : (short) -(x))

static short EnemyMoveCoverN2 (ENEMY *);
static void EnemyTiniCoverN2 (ENEMY *);


void EnemyInitCoverN2 (ENEMY * p)
{
	p->vx = 0;
	p->vy = 2 * 65536;
	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 32;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->pt = obj_covern;
	p->seq = 0;
	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS;
	p->func_enemy_move = EnemyMoveCoverN2;
	p->func_enemy_tini = EnemyTiniCoverN2;

	entry_control += ENTRY_CONTROL_ADD;
}



static short EnemyMoveCoverN2 (ENEMY * p)
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
		if (p->vy > 16384) {
			p->vy -= 4096;
		} else {
			p->seq++;
			p->s_work = p->s_work2 = 0;
		}
		break;

	case 2:
		/* �U������ */
		switch (p->s_work) {
		case 0:	/* �J�o�[�J�� */
			p->pt++;
			if (p->pt >= obj_covern + 14) {	/* �J�o�[�J���������� */
				p->s_work++;
				p->s_work2 = p->s_work3 = 0;
				p->core_info = PALET_CORE_RED | PRIORITY_BOSS;
				SoundSetSE (SE_LOCK);
			}
			break;
		case 1:
			/* �΂ߒe */
			switch (p->s_work2++) {
#define INTERVAL1_1	14
#define SPEED_SHOT1_1	12
			case INTERVAL1_1 * 1:
				/* ���@�Ƃw���W�̍������������̂ݎ΂ߒe */
				if (wabs (player->x - p->x) < 64) {
					p->s_angle = psearch (p->x - 16, p->y) - 2;	/* ���@�̕������T�[�` */
					p->s_angle2 = psearch (p->x + 16, p->y) + 2;	/* ���@�̕������T�[�` */
					p->cwork = !0;
				} else {
					p->cwork = 0;
				}
			case INTERVAL1_1 * 2:
			case INTERVAL1_1 * 3:
			case INTERVAL1_1 * 4:
			case INTERVAL1_1 * 5:
				if (p->cwork) {
					EshotInit (ESHOT_NRG02, p->x - 16, p->y + 1, SPEED_SHOT1_1, p->s_angle, 0);
					EshotInit (ESHOT_NRG02, p->x + 16, p->y + 1, SPEED_SHOT1_1, p->s_angle2, 0);
					SoundSetSE (SE_ESHOT_M);	/* �G�V���b�g�� */
				}
				break;
			case INTERVAL1_1 * 9:
				p->s_work++;
				break;
			}
#define INTERVAL1_2	25
#define SPEED_SHOT1_2	8
			if (p->s_work3++ > INTERVAL1_2) {
				char a = psearch (p->x, p->y);	/* ���@�̕������T�[�` */
				p->s_work3 = 0;
				EshotInit (ESHOT_NRG04, p->x, p->y, SPEED_SHOT1_2, a, 0);
				EshotInit (ESHOT_NRG04, p->x, p->y, SPEED_SHOT1_2, a + 24, 0);
				EshotInit (ESHOT_NRG04, p->x, p->y, SPEED_SHOT1_2, a - 24, 0);
				SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
			}
			break;
		case 2:	/* �J�o�[���� */
			if (p->pt > obj_covern) {
				p->pt--;
			} else {
				p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS;
				SoundSetSE (SE_LOCK);
				if (SHORT_LY > 160) {
					p->seq++;
				} else {
					p->s_work++;
					p->s_work2 = 0;
				}
			}
			break;
		case 3:	/* ���΂炭�҂� */
#define INTERVAL1_3	25
			if (p->s_work2++ > INTERVAL1_3) {
				p->s_work = 0;	/* �ŏ��ɖ߂� */
			}
			break;
		}
		break;

		/* �P�� */
	case 3:
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
				EffectInit (EFFECT_POINTS_RED, POINTS_1200, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_600, p->x, p->y);
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
	p->core_x = p->x - 8;	/* �R�A�̒��S�� (-8,-12) �h�b�g����� */
	p->core_y = p->y - 12;
	xsp_set_st (&(p->core_x));

	return (0);
}



static void EnemyTiniCoverN2 (ENEMY * p)
{
	entry_control -= ENTRY_CONTROL_ADD;
}
