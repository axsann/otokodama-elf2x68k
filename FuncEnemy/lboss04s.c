/* ４面ボス左右肩 (04s = 04 sholder) */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"


#define TIMER_1	10000		/* これ以上時間が経過したら自爆 */

#define SPEED_SHOT1	12	/* １段階目扇型ショット速度 */
#define SPEED_SHOT2	10	/* ２段階目伸びるショットの一番遅い弾の速度 */


enum {
	WING_LEFT = 0, WING_RIGHT
};

static short EnemyMoveLBoss04S (ENEMY *);
static void EnemyTiniLBoss04S (ENEMY *);


void EnemyInitLBoss04S (ENEMY * p)
{
	p->vx = 0;
	p->vy = 2 << 16;
	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 24;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->seq = 0;
	p->func_enemy_move = EnemyMoveLBoss04S;
	p->func_enemy_tini = EnemyTiniLBoss04S;

	if (p->lx < 0) {
		p->cwork = WING_LEFT;	/* 左か右か */
		p->pt = obj_lboss04 + 1;
		p->hp = 800;
		p->parts1 = EnemyInit (ENEMY_LBOSS04W, -16, 0, 0, p);	/* 左翼 */
	} else {
		p->cwork = WING_RIGHT;
		p->pt = obj_lboss04 + 2;
		p->hp = 600;	/* 実は右の方がちょっと弱い */
		p->parts1 = EnemyInit (ENEMY_LBOSS04W, 16, 0, 0, p);	/* 右翼 */
	}
}



static short EnemyMoveLBoss04S (ENEMY * p)
{
	if ((p->child_kill) || (p->child_death))
		return (-1);	/* 消去 */
	p->timer++;
	if (p->timer > TIMER_1) {
		EffectInit (EFFECT_EXPL, 0, p->x, p->y);
		SoundSetSE (SE_EXPL_M);	/* 中ボス爆発音 */
		p->child_death = !0;	/* 消去準備 */
	}
	if (p->cwork == WING_LEFT) {
		p->x = p->parent->x - 47;
		p->y = p->parent->y - 31;
	} else {
		p->x = p->parent->x + 47;
		p->y = p->parent->y - 31;
	}
	switch (p->seq) {
	case 0:
	    /* 翼パーツが消えたか？ */
		if ((p->parts1) && (p->parts1->child_death)) {
			p->parts1 = 0;	/* これ以降参照しない */
			p->seq++;
			p->m_work = 110;
		}
		break;
	case 1:		/* しばらく待つ */
		if (p->m_work-- <= 0) {
			p->seq++;
			p->s_work = p->s_work2 = p->s_work3 = 0;
		}
		break;

	case 2:		/* 攻撃開始 */
		switch (p->s_work) {

		case 0:
			p->s_work2++;
			switch (p->s_work2) {
#define INTERVAL0	12
#define SPEED_SHOT0_1	10
			case 8 + INTERVAL0 * 0:
				p->s_angle = psearch (p->x, p->y);	/* 自機の方向をサーチ */
				p->s_work4 = SPEED_SHOT0_1;
			case 8 + INTERVAL0 * 1:
			case 8 + INTERVAL0 * 2:
			case 8 + INTERVAL0 * 3:
			    /* 下に降りてきている時は攻撃をしない */
				if (p->parent->m_work < 11) {
					EshotInit (ESHOT_NRG03, p->x, p->y, p->s_work4, p->s_angle - 12, 2);
					EshotInit (ESHOT_NRG04, p->x, p->y, p->s_work4, p->s_angle, 2);
					EshotInit (ESHOT_NRG03, p->x, p->y, p->s_work4, p->s_angle + 12, 2);
					SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
					p->s_work4 += 4;
				}
				break;
			case 8 + INTERVAL0 * 6:
				p->s_work2 = 0;
				if (p->s_work3++ > 3) {
					p->s_work++;
					p->s_work3 = 0;
				}
				break;
			default:
				break;
			}
			break;

		case 1:	/* 炸裂弾 */
			p->s_work2++;
#define INTERVAL1	20
#define SPEED_SHOT1_1	13
#define SPEED_SHOT1_2	21
			if ((p->s_work2 == INTERVAL1) && (p->parent->m_work < 11)) {
				unsigned char a;
				short h;
				a = p->s_angle = psearch (p->x, p->y);	/* 自機の方向をサーチ */
				for (h = 0; h < 16; h++) {
					EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT1_1, a, 4);
					a += 16;
				}
				SoundSetSE (SE_EBOMB_M);
			}
			if ((p->s_work2 == INTERVAL1 + 1) && (p->parent->m_work < 11)) {
				unsigned char a = p->s_angle + 8;
				short h;
				for (h = 0; h < 16; h++) {
					EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT1_1, a, 2);
					a += 16;
				}
			}
			if ((p->s_work2 == INTERVAL1 * 2) && (p->parent->m_work < 11)) {
				unsigned char a = psearch (p->x, p->y);	/* 自機の方向をサーチ */
				short h;
				for (h = 0; h < 16; h++) {
					EshotInit (ESHOT_NRG34N, p->x, p->y, SPEED_SHOT1_2, a, 2);
					a += 16;
				}
				SoundSetSE (SE_EBOMB_L);
			}
			if (p->s_work2 == INTERVAL1 * 3) {
				p->s_work2 = 0;
				if (p->s_work3++ > 3) {
					p->s_work++;
					p->s_work3 = 0;
				}
			}
			break;
		default:
			p->s_work = 0;
			break;
		}
		break;
	}


    /* 自機ショットに当たった時の処理 */
	if (p->damage) {
		SoundSetSE (SE_DAMAGE);
		p->info = PALET_DAMAGE | PRIORITY_BOSS_PARTS;
		p->damage_core = 0;
		if ((p->hp -= p->damage) <= 0) {
		    /* 死んだ */
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			EffectInit (EFFECT_POINTS_RED, POINTS_1000, p->x, p->y);
			SoundSetSE (SE_EXPL_M);	/* 中ボス爆発音 */
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
			p->child_death = !0;	/* 消去準備 */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 200)) {
			p->info = PALET_RED | PRIORITY_BOSS_PARTS;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = PALET_LBOSS04 | PRIORITY_BOSS_PARTS;
		}
	}
	xobj_set_st (p);

	return (0);
}



static void EnemyTiniLBoss04S (ENEMY * p)
{
    /* パーツが存在していたら消すように指示 */
	if (p->parts1)
		p->parts1->child_kill = !0;
}
