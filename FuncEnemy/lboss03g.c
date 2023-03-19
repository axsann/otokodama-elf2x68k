/* ３面ボス銀玉 (03g = 03 gin) */
/* ４つ出てくる小さなボスが噴火（？）する破壊可能弾 */
#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"


#define HP_0	1		/* 耐久力 */

static short points_disp;	/* 得点を表示するか */
static short rand_work;		/* 乱数の種 */

static short EnemyMoveLBoss03G (ENEMY *);
static void EnemyTiniLBoss03G (ENEMY *);


/* ゲーム開始時に１回だけ呼ばれる */
void EnemyInitStaticLBoss03G (void)
{
	points_disp = 0;
	rand_work = 0;
}


void EnemyInitLBoss03G (ENEMY * p)
{
	signed int rand_table[8] =
	{4096, -16384, 65536, -32768, 0, -4096, 16384, 0};

	p->vx = p->parent->vx + rand_table[rand_work];
	p->vy = (-4 << 16) + p->parent->vy + rand_table[rand_work];
	if (++rand_work > 7)
		rand_work = 0;

	p->hit_px = 12;
	p->hit_py = 12;
	p->hit_sx = 12;
	p->hit_sy = 12;
	p->hit_cx = 12;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	if (p->arg == 0)
		p->info = PALET_LBOSS03 | PRIORITY_ZAKO;
	else
		p->info = PALET_LBOSS03_1 | PRIORITY_ZAKO;
	p->pt = sp_lboss03g;
	p->seq = 0;
	p->func_enemy_move = EnemyMoveLBoss03G;
	p->func_enemy_tini = EnemyTiniLBoss03G;
}



static short EnemyMoveLBoss03G (ENEMY * p)
{
    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:
		p->vy += 16384;
		if (SHORT_VY > 6)
			p->seq++;
		break;
	case 1:
	    /* 画面外に出たら消去 */
		if (SHORT_LY > 256 + 64)
			return (-1);	/* 消去 */
		break;
	}


    /* 自機ショットに当たった時の処理 */
	if (p->damage) {
		if ((p->hp -= p->damage) <= 0) {
		    /* 死んだ */
			EffectInit (EFFECT_EXPLMINI, 0, p->x, p->y);
			SoundSetSE (SE_EXPL);

			if (points_disp < 8) {
				points_disp++;
				EffectInit (EFFECT_POINTS_RED, POINTS_100, p->x, p->y);
			} else {
				EffectInit (EFFECT_POINTS, POINTS_100, p->x, p->y);
			}
			return (-1);	/* 消去 */
		}
		p->damage = 0;
	}
	xsp_set_st (p);


	return (0);
}



static void EnemyTiniLBoss03G (ENEMY * p)
{
}
