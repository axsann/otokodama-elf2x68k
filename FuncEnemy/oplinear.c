/* 開いて直線弾 */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../entry.h"

#define PALET_MAIN		0x0400

#define SPEED_1		8	/* 横移動の加速度 */
#define SPEED_1N	18	/* 横移動の加速度を足す回数 */
#define SPEED_2		12	/* 撤退時の加速度 */
#define SPEED_2N	24	/* 撤退時の加速度を足す回数 */

#define SPEED_NRG	8	/* 敵弾の速度（遅い方） */
#define SPEED_NRG2	16	/* 敵弾の速度（速い方） */

#define ENTRY_CONTROL_ADD	10	/* entry_control に足す値 */


enum {
	MOVETO_L = 0,
	MOVETO_R,
};

static short EnemyMoveOpLinear (ENEMY *);
static void EnemyTiniOpLinear (ENEMY *);


void EnemyInitOpLinear (ENEMY * p)
{
	p->vx = 0;
	p->vy = 8 * 65536;
	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 24;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->damage = p->damage_core = 0;
	p->flash = 0;
	p->hp = 240;
	p->pt = obj_oplaser;
	p->core_pt = sp_core01;
	p->seq = 0;
	p->func_enemy_move = EnemyMoveOpLinear;
	p->func_enemy_tini = EnemyTiniOpLinear;
	if (SHORT_LX < 128 + 16)
		p->cwork = 0;
	else
		p->cwork = !0;

	entry_control += ENTRY_CONTROL_ADD;
}



static short EnemyMoveOpLinear (ENEMY * p)
{
    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:
		p->vy -= 16384;
		if (p->vy < 0) {
			p->seq++;
			p->vy = 0;
		}
		break;
	case 1:		/* 開く */
		if (p->pt == obj_oplaser + 27)
			SoundSetSE (SE_LOCK);	/* 「がちゃん」という効果音を鳴らす */
		if (p->pt < obj_oplaser + 35) {
			p->pt++;
		} else {
			p->seq++;
			p->s_work = 0;
			if (p->cwork == 0) {
				SubEnemyMoveToInit (p, SHORT_LX + 128, SHORT_LY - 16, SPEED_1, SPEED_1N);
				p->m_work = MOVETO_L;	/* 次の移動方向 */
			} else {
				SubEnemyMoveToInit (p, SHORT_LX - 128, SHORT_LY - 16, SPEED_1, SPEED_1N);
				p->m_work = MOVETO_R;
			}
			p->m_work2 = 0;
			p->m_work3 = 0;
		}
		break;
	case 2:		/* 攻撃開始 */
		if (SubEnemyMoveTo (p) < 0) {
			short m_work_table[2] =
			{MOVETO_R, MOVETO_L};
			signed short dx_table[2] =
			{-64, 64};
			signed short dy_table[5] =
			{-16, 8, 16, -24, 16};
			p->m_work2++;
			if (p->m_work2 > 8) {
				p->seq++;	/* 撤退 */
				SubEnemyMoveToInit (p, SHORT_LX, -64, SPEED_2, SPEED_2N);
			} else {
				SubEnemyMoveToInit (p, SHORT_LX + dx_table[p->m_work], SHORT_LY + dy_table[p->m_work3], SPEED_1, SPEED_1N);
				p->m_work = m_work_table[p->m_work];	/* 次の移動方向 */
				p->m_work3++;
				if (p->m_work3 > 4)
					p->m_work3 = 0;
			}
		}
		switch (p->s_work++) {
#define	TIME_SHOT_INTERVAL	30	/* ショットを撃つ間隔 */
		case 4 + TIME_SHOT_INTERVAL * 0:
		case 4 + TIME_SHOT_INTERVAL * 1:
		case 4 + TIME_SHOT_INTERVAL * 2:
		case 4 + TIME_SHOT_INTERVAL * 3:
			EshotInit (ESHOT_LASER01, SHORT_LX - 18, SHORT_LY + 24, SPEED_NRG, 64, 0);
			EshotInit (ESHOT_LASER01, SHORT_LX + 18, SHORT_LY + 24, SPEED_NRG, 64, 0);
			EshotInit (ESHOT_LASER01, SHORT_LX - 18, SHORT_LY + 24, SPEED_NRG2, 64, 0);
			EshotInit (ESHOT_LASER01, SHORT_LX + 18, SHORT_LY + 24, SPEED_NRG2, 64, 0);
			SoundSetSE (SE_LASER);
			break;
		case 4 + TIME_SHOT_INTERVAL * 5:
			p->s_work = 0;
			break;
		default:
			break;
		}
		break;

	case 3:		/* 撤退 */
		if (SubEnemyMoveTo (p) < 0)
			SubEnemyMoveToInit (p, SHORT_LX, -64, SPEED_2, SPEED_2N);
	    /* 画面外に出たら消去 */
		if (SHORT_LY < -64)
			return (-1);	/* 消去 */
		break;
	}

    /* 自機ショットに当たった時の処理 */
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
			SoundSetSE (SE_EXPL_M);	/* 中ボス爆発音 */
			if ((p->core_info == (PALET_CORE_RED | PRIORITY_BOSS))
			    || (p->core_info == (PALET_CORE_RED | PRIORITY_BOSS_CORE)))
				EffectInit (EFFECT_POINTS_RED, POINTS_2000, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_1200, p->x, p->y);
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
			return (-1);	/* 消去 */
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

    /* コアの表示 */
	{
		short priority;

		if (p->pt > obj_oplaser + 30)
			priority = PRIORITY_BOSS;
		else
			priority = PRIORITY_BOSS_CORE;
		p->core_x = p->x - 8;	/* コアの中心は (-8,-8) ドットずれる */
		p->core_y = p->y - 8;
		if (player->y < p->y + 64)	/* 自機との座標の差が小さければ赤 */
			p->core_info = PALET_CORE_RED | priority;
		else
			p->core_info = PALET_CORE_BLUE | priority;
		xsp_set_st (&(p->core_x));
	}
	return (0);
}



static void EnemyTiniOpLinear (ENEMY * p)
{
	entry_control -= ENTRY_CONTROL_ADD;
}
