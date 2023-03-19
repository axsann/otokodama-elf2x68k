/* ４面ボス左右翼 (04w = 04 wing) */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"


#define TIMER_1	9000		/* これ以上時間が経過したら自爆 */

enum {
	WING_LEFT = 0, WING_RIGHT
};

static short EnemyMoveLBoss04W (ENEMY *);
static void EnemyTiniLBoss04W (ENEMY *);


void EnemyInitLBoss04W (ENEMY * p)
{
	p->vx = 0;
	p->vy = 2 << 16;
	p->hit_px = 48;
	p->hit_py = 12;
	p->hit_sx = 48;
	p->hit_sy = 12;
	p->hit_cx = 6;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->seq = 0;
	p->m_work = 0;
	p->s_work = p->s_work2 = p->s_work3 = 0;
	p->func_enemy_move = EnemyMoveLBoss04W;
	p->func_enemy_tini = EnemyTiniLBoss04W;

	if (p->lx < 0) {
		p->cwork = WING_LEFT;	/* 左か右か */
		p->pt = obj_lboss04 + 3;
		p->hp = 4200;	/* 実は左の方がちょっと弱い */
	} else {
		p->cwork = WING_RIGHT;
		p->pt = obj_lboss04 + 4;
		p->hp = 4500;
	}
}



static short EnemyMoveLBoss04W (ENEMY * p)
{
	if ((p->child_kill) || (p->child_death))
		return (-1);	/* 消去 */
	p->timer++;
	if (p->timer > TIMER_1){
		EffectInit (EFFECT_EXPL, 0, p->x, p->y);
		SoundSetSE (SE_EXPL_M);	/* 中ボス爆発音 */
		p->child_death = !0;	/* 消去準備 */
	}

	if (p->cwork == WING_LEFT) {
		p->x = p->parent->x - 31;
		p->y = p->parent->y + 16;
	} else {
		p->x = p->parent->x + 31;
		p->y = p->parent->y + 16;
	}

    /* p->s_work は lboss04.c が ++ する */
	switch (p->s_work) {

	case 0:		/* なぎ倒し弾 */
		p->s_work2++;
#define INTERVAL0	4
#define SPEED_SHOT0_0	16	/* 遅いなぎ倒し弾の速度 */
#define SPEED_SHOT0_1	24	/* 速い　　　〃　　　　 */
		if (p->s_work2 > INTERVAL0) {
			p->s_work2 = 0;
			p->s_work3++;
			if (p->s_work3 == 1) {
				if (p->cwork == WING_LEFT) {
					p->s_angle = 64 + 96;
					p->s_angle2 = 64 + 32;
				} else {
					p->s_angle = 64 - 96;
					p->s_angle2 = 64 - 32;
				}
			}
		    /* 下に降りてきている時は攻撃を一切しない */
			if (p->parent->parent->m_work < 11) {
				if (p->s_work3 < 28) {
					if (p->cwork == WING_LEFT) {
						EshotInit (ESHOT_NRG02, p->x + 32 - 8, p->y + 16 - 8, SPEED_SHOT0_0, p->s_angle, 2);
						p->s_angle -= 5;
					} else {
						EshotInit (ESHOT_NRG02, p->x - 32 + 8, p->y + 16 - 8, SPEED_SHOT0_0, p->s_angle, 2);
						p->s_angle += 5;
					}
					SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
				}
				if ((p->s_work3 > 8)&&(p->s_work3 < 16)) {
					if (p->cwork == WING_LEFT) {
						EshotInit (ESHOT_NRG24N, p->x + 32 - 8, p->y + 16 - 8, SPEED_SHOT0_1, p->s_angle2, 2);
						p->s_angle2 -= 8;
					} else {
						EshotInit (ESHOT_NRG24N, p->x - 32 + 8, p->y + 16 - 8, SPEED_SHOT0_1, p->s_angle2, 2);
						p->s_angle2 += 8;
					}
					SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
				}
			}
			if (p->s_work3 >= 32)
				p->s_work3 = 0;
		}
		break;

	case 1:		/* 直線弾 */
		p->s_work2++;
		switch (p->s_work2) {
#define INTERVAL1	12
#define SPEED_SHOT1_0	20	/* 天帝ショット速度 */
#define SPEED_SHOT1_1	16	/* 直線弾速度 */
		case 8 + INTERVAL1 * 0:
			p->s_angle = psearch (p->x, p->y);	/* 自機の方向をサーチ */
		case 8 + INTERVAL1 * 1:
		case 8 + INTERVAL1 * 2:
		case 8 + INTERVAL1 * 3:
		case 8 + INTERVAL1 * 4:
		case 8 + INTERVAL1 * 5:
		    /* 下に降りてきている時は攻撃を一切しない */
			if (p->parent->parent->m_work < 11) {
				if (p->cwork == WING_LEFT) {
					EshotInit (ESHOT_NRG03, p->x + 32 - 8, p->y + 16 - 8, SPEED_SHOT1_0 + 0, p->s_angle - 8 * 2, 2);
					EshotInit (ESHOT_NRG04, p->x + 16 - 8, p->y + 16 - 4, SPEED_SHOT1_0 + 2, p->s_angle - 8 * 1, 2);
					EshotInit (ESHOT_NRG05, p->x + 0 - 8, p->y + 16 + 0, SPEED_SHOT1_0 + 4, p->s_angle + 8 * 0, 2);
					EshotInit (ESHOT_NRG06, p->x - 16 - 8, p->y + 16 + 4, SPEED_SHOT1_0 + 6, p->s_angle + 8 * 1, 2);
				} else {
					EshotInit (ESHOT_NRG03, p->x - 32 + 8, p->y + 16 - 8, SPEED_SHOT1_0 + 0, p->s_angle + 8 * 2, 2);
					EshotInit (ESHOT_NRG04, p->x - 16 + 8, p->y + 16 - 4, SPEED_SHOT1_0 + 2, p->s_angle + 8 * 1, 2);
					EshotInit (ESHOT_NRG05, p->x + 0 + 8, p->y + 16 + 0, SPEED_SHOT1_0 + 4, p->s_angle + 8 * 0, 2);
					EshotInit (ESHOT_NRG06, p->x + 16 + 8, p->y + 16 + 4, SPEED_SHOT1_0 + 6, p->s_angle - 8 * 1, 2);
				}
				SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
			}
			break;
		case 8 + INTERVAL1 * 7:
		case 8 + INTERVAL1 * 8:
		case 8 + INTERVAL1 * 9:
		    /* 下に降りてきている時は攻撃を一切しない */
			if (p->parent->parent->m_work < 11) {
				p->s_angle = psearch (p->x, p->y);	/* 自機の方向をサーチ */
				if (p->cwork == WING_LEFT) {
					EshotInit (ESHOT_NRG24N, p->x - 8, p->y + 16, SPEED_SHOT1_1, p->s_angle, 2);
				} else {
					EshotInit (ESHOT_NRG24N, p->x + 8, p->y + 16, SPEED_SHOT1_1, p->s_angle, 2);
				}
				SoundSetSE (SE_ESHOT_M);	/* 敵ショット音（中） */
			}
			break;
		case 8 + INTERVAL1 * 10:
			p->s_work2 = 0;
			break;
		default:
			break;
		}
		break;
	case 2:		/* ２列直線弾 */
		p->s_work2++;
#define INTERVAL2	25
#define SPEED_SHOT2_1	10
		switch (p->s_work2) {
		case 8 + INTERVAL2 * 0:
		case 8 + INTERVAL2 * 1:
		case 8 + INTERVAL2 * 2:
		case 8 + INTERVAL2 * 3:
		case 8 + INTERVAL2 * 4:
		case 8 + INTERVAL2 * 5:
		    /* 下に降りてきている時は攻撃を一切しない */
			if (p->parent->parent->m_work < 11) {
				if (p->cwork == WING_LEFT) {
					EshotInit (ESHOT_NRG22, p->x + 80 - 8, p->y + 16 - 16, SPEED_SHOT2_1 + 4, 64, 2);
					EshotInit (ESHOT_NRG22, p->x + 60 - 8, p->y + 16 - 12, SPEED_SHOT2_1 + 8, 64, 2);
					EshotInit (ESHOT_NRG22, p->x + 40 - 8, p->y + 16 - 8, SPEED_SHOT2_1 + 4, 64, 2);
					EshotInit (ESHOT_NRG22, p->x + 20 - 8, p->y + 16 - 4, SPEED_SHOT2_1 + 0, 64, 2);
					EshotInit (ESHOT_NRG22, p->x + 0 - 8, p->y + 16 + 0, SPEED_SHOT2_1 + 4, 64, 2);
					EshotInit (ESHOT_NRG22, p->x - 20 - 8, p->y + 16 + 4, SPEED_SHOT2_1 + 8, 64, 2);
					EshotInit (ESHOT_NRG22, p->x - 40 - 8, p->y + 16 + 8, SPEED_SHOT2_1 + 4, 64, 2);
				} else {
					EshotInit (ESHOT_NRG22, p->x - 80 + 8, p->y + 16 - 16, SPEED_SHOT2_1 + 4, 64, 2);
					EshotInit (ESHOT_NRG22, p->x - 60 + 8, p->y + 16 - 12, SPEED_SHOT2_1 + 8, 64, 2);
					EshotInit (ESHOT_NRG22, p->x - 40 + 8, p->y + 16 - 8, SPEED_SHOT2_1 + 4, 64, 2);
					EshotInit (ESHOT_NRG22, p->x - 20 + 8, p->y + 16 - 4, SPEED_SHOT2_1 + 0, 64, 2);
					EshotInit (ESHOT_NRG22, p->x + 0 + 8, p->y + 16 + 0, SPEED_SHOT2_1 + 4, 64, 2);
					EshotInit (ESHOT_NRG22, p->x + 20 + 8, p->y + 16 + 4, SPEED_SHOT2_1 + 8, 64, 2);
					EshotInit (ESHOT_NRG22, p->x + 40 + 8, p->y + 16 + 8, SPEED_SHOT2_1 + 4, 64, 2);
				}
				SoundSetSE (SE_ESHOT_M);	/* 敵ショット音（中） */
			}
			break;
		case 8 + INTERVAL2 * 10:
			p->s_work2 = 0;
			break;
		}
		break;

	default:
		p->s_work = 0;
		break;
	}


    /* 自機ショットに当たった時の処理 */
	if (p->damage) {
		SoundSetSE (SE_DAMAGE);
		p->info = PALET_DAMAGE | PRIORITY_BOSS_PARTS + 1;
		p->damage_core = 0;
		if ((p->hp -= p->damage) <= 0) {
		    /* 死んだ */
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			EffectInit (EFFECT_POINTS_RED, POINTS_7000, p->x, p->y);
			SoundSetSE (SE_EXPL_M);	/* 中ボス爆発音 */
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
			p->child_death = !0;	/* 消去準備 */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 300)) {
			p->info = PALET_RED | PRIORITY_BOSS_PARTS + 1;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = PALET_LBOSS04 | PRIORITY_BOSS_PARTS + 1;
		}
	}
	xobj_set_st (p);


	return (0);
}



static void EnemyTiniLBoss04W (ENEMY * p)
{
}
