/* ２面ボス左右翼 (02w = 02 wing) */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"


#define SPEED_SHOT1	12	/* １段階目扇型ショット速度 */
#define SPEED_SHOT2	10	/* ２段階目伸びるショットの一番遅い弾の速度 */


enum {
	WING_LEFT = 0, WING_RIGHT
};

enum {
	ATTACK_UPPER_CORE = 0,	/* 上のコアから攻撃（伸びる弾） */
	ATTACK_LOWER_CORE	/* 下のコアから攻撃（ばらまき弾） */
};

static short EnemyMoveLBoss02W (ENEMY *);
static void EnemyTiniLBoss02W (ENEMY *);


void EnemyInitLBoss02W (ENEMY * p)
{
    /* 攻撃する回数（復活する後とに回数が増える） */
	short shot_num_table[8] =
	{3, 4, 5, 6, 8, 10, 12, 14};

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
	p->m_work = 0;
	p->s_work = 0;
	p->func_enemy_move = EnemyMoveLBoss02W;
	p->func_enemy_tini = EnemyTiniLBoss02W;

	if (p->lx < 0) {
		p->cwork = WING_LEFT;	/* 左か右か */
		p->cwork2 = p->ly;	/* 難易度（何回復活したか・ly を流用） */
		p->pt = obj_lboss02 + 5;
		p->hp = 300;
	} else {
		p->cwork = WING_RIGHT;
		p->cwork2 = p->ly;
		p->pt = obj_lboss02 + 6;
		p->hp = 250;	/* 実は右の方がちょっと弱い */
	}
	if (p->arg > 8)
		p->arg = 8;
	p->s_work3 = 0;
	p->s_work4 = shot_num_table[p->arg];
}



static short EnemyMoveLBoss02W (ENEMY * p)
{
	signed short offset[14][2] =	/* 翼パーツの座標オフセット */
	{
		{-44, -25},
		{-47, -22},
		{-54, -17},
		{-62, -12},
		{-69, -7},
		{-76, -2},
		{-80, 0},
		{-86, 4},
		{-74, -3},
		{-78, -1},
		{-86, 4},
		{-75, -2},
		{-80, 0},
		{-80, 0}
	};

	if ((p->child_kill) || (p->child_death))
		return (-1);	/* 消去 */

	switch (p->seq) {
	case 0:
		if (p->m_work < 12) {
			if (p->cwork == WING_LEFT)
				p->x = p->parent->x + offset[p->m_work][0];
			else
				p->x = p->parent->x - offset[p->m_work][0];
			p->y = p->parent->y + offset[p->m_work][1];
		}
		if (p->m_work == 7)
			SoundSetSE (SE_LOCK_M);	/* 「がちゃん」という効果音を鳴らす */
		if (p->m_work == 12)
			p->seq++;
		p->m_work++;
		break;
	case 1:
		if (p->cwork == WING_LEFT)
			p->x = p->parent->x - 80;
		else
			p->x = p->parent->x + 80;
		p->y = p->parent->y;

	    /* 次に攻撃処理 */
		p->s_work++;
		if (p->s_work == 1) {
			if (p->cwork == WING_LEFT)
				p->s_angle = 16;
			else
				p->s_angle = 128 - 16;
			p->s_work2 = 0;
		}
		p->s_work2++;
		if (p->s_work2 > 8) {
			p->s_work2 = 0;
			if (p->s_work3++ < p->s_work4) {
				if (p->cwork == WING_LEFT) {
					EshotInit (ESHOT_NRG03, p->x - 10, p->y + 4, SPEED_SHOT1, p->s_angle, 0);
					EshotInit (ESHOT_NRG03, p->x + 4, p->y - 2, SPEED_SHOT1, p->s_angle, 0);
					p->s_angle += 12;
				} else {
					EshotInit (ESHOT_NRG03, p->x + 10, p->y + 4, SPEED_SHOT1, p->s_angle, 0);
					EshotInit (ESHOT_NRG03, p->x - 4, p->y - 2, SPEED_SHOT1, p->s_angle, 0);
					p->s_angle -= 12;
				}
				SoundSetSE (SE_ESHOT_M);	/* 敵ショット音（中） */
			}
		}
	    /* ４回復活以降の伸びる弾 */
		if (p->arg >= 4) {
			char a = psearch (p->x, p->y);
			switch (p->s_work) {
			case 20:
				if (p->cwork == WING_LEFT) {
					EshotInit (ESHOT_NRG24N, p->x - 10, p->y + 4, SPEED_SHOT2, a, 0);
					EshotInit (ESHOT_NRG24N, p->x + 4, p->y - 2, SPEED_SHOT2, a, 0);
					EshotInit (ESHOT_NRG24N, p->x - 10, p->y + 4, SPEED_SHOT2 + 4, a, 0);
					EshotInit (ESHOT_NRG24N, p->x + 4, p->y - 2, SPEED_SHOT2 + 4, a, 0);
					EshotInit (ESHOT_NRG24N, p->x - 10, p->y + 4, SPEED_SHOT2 + 8, a, 0);
					EshotInit (ESHOT_NRG24N, p->x + 4, p->y - 2, SPEED_SHOT2 + 8, a, 0);
					EshotInit (ESHOT_NRG24N, p->x - 10, p->y + 4, SPEED_SHOT2 + 12, a, 0);
					EshotInit (ESHOT_NRG24N, p->x + 4, p->y - 2, SPEED_SHOT2 + 12, a, 0);
				} else {
					EshotInit (ESHOT_NRG24N, p->x + 10, p->y + 4, SPEED_SHOT2, a, 0);
					EshotInit (ESHOT_NRG24N, p->x - 4, p->y - 2, SPEED_SHOT2, a, 0);
					EshotInit (ESHOT_NRG24N, p->x + 10, p->y + 4, SPEED_SHOT2 + 4, a, 0);
					EshotInit (ESHOT_NRG24N, p->x - 4, p->y - 2, SPEED_SHOT2 + 4, a, 0);
					EshotInit (ESHOT_NRG24N, p->x + 10, p->y + 4, SPEED_SHOT2 + 8, a, 0);
					EshotInit (ESHOT_NRG24N, p->x - 4, p->y - 2, SPEED_SHOT2 + 8, a, 0);
					EshotInit (ESHOT_NRG24N, p->x + 10, p->y + 4, SPEED_SHOT2 + 12, a, 0);
					EshotInit (ESHOT_NRG24N, p->x - 4, p->y - 2, SPEED_SHOT2 + 12, a, 0);
				}
				SoundSetSE (SE_EBOMB_M);
				break;
			default:
				break;
			}
		}
		if (p->s_work > 200) {
			p->s_work = 0;
			p->s_work3 = 0;
		}
		break;
	}


    /* 自機ショットに当たった時の処理 */
	if (p->damage) {
		SoundSetSE (SE_DAMAGE);
		p->info = PALET_RED | PRIORITY_BOSS_PARTS_L;
		p->damage_core = 0;
		if ((p->hp -= p->damage) <= 0) {
		    /* 死んだ */
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			EffectInit (EFFECT_POINTS_RED, POINTS_4000, p->x, p->y);
			SoundSetSE (SE_EXPL_M);	/* 中ボス爆発音 */
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
			p->child_death = !0;	/* 消去準備 */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 100)) {
			p->info = PALET_RED | PRIORITY_BOSS_PARTS_L;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = p->parent->work4;
		}
	}
	xobj_set_st (p);


	return (0);
}



static void EnemyTiniLBoss02W (ENEMY * p)
{
}
